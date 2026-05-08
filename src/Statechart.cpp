/*
 * UML statechart framework (https://github.com/Lecrapouille/CppStatecharts)
 * Copyright (c) 2026 Quentin Quadrat (lecrapouille@gmail.com)
 *
 * Based on the Java UML statechart framework
 * (https://github.com/klangfarbe/UML-Statechart-Framework-for-Java)
 * Copyright (C) 2006-2013 Christian Mocek (christian.mocek@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "CppStatecharts/Statechart.hpp"

#include "CppStatecharts/Event.hpp"
#include "CppStatecharts/EventQueueEntry.hpp"
#include "CppStatecharts/InternalTransition.hpp"
#include "CppStatecharts/Metadata.hpp"
#include "CppStatecharts/Parameter.hpp"
#include "CppStatecharts/PseudoState.hpp"
#include "CppStatecharts/StateRuntimedata.hpp"
#include "CppStatecharts/StatechartException.hpp"
#include "CppStatecharts/ThreadPool.hpp"
#include "CppStatecharts/TimeoutEvent.hpp"
#include "CppStatecharts/Transition.hpp"

#include <chrono>
#include <utility>

namespace statechart {

Statechart::Statechart(std::string p_name,
                       int p_threads,
                       bool /*p_makeDaemonThreads*/)
    : Context(std::move(p_name), nullptr, nullptr)
{
    // The Statechart is its own root: register the back-pointer so child
    // states can navigate up.
    m_statechart = this;

    // The Java reference implementation uses a ThreadPoolExecutor with
    // corePoolSize=1 over an unbounded LinkedBlockingQueue, so async events
    // are guaranteed to be processed in submission order on a single worker.
    // We mirror that semantic with a one-thread pool. The @p p_threads
    // parameter is kept for API parity but only acts as a sanity floor.
    (void)p_threads;
    m_threadpool = std::make_unique<ThreadPool>(static_cast<std::size_t>(1));
    m_running.store(true, std::memory_order_release);
    m_timeoutThread = std::thread([this] { timeoutLoop(); });
}

Statechart::~Statechart()
{
    shutdown();
}

void Statechart::shutdown()
{
    if (!m_running.exchange(false))
    {
        return;
    }
    m_timeoutQueue.clear();
    m_timeoutQueue.stop();
    if (m_threadpool)
    {
        m_threadpool->shutdown();
    }
    if (m_timeoutThread.joinable())
    {
        m_timeoutThread.join();
    }
    if (m_threadpool)
    {
        m_threadpool->awaitTermination(std::chrono::milliseconds{60'000});
    }
    {
        std::lock_guard<std::mutex> lk(m_pendingMutex);
        m_pendingEntries.clear();
    }
}

State* Statechart::getStateByName(const std::string& p_stateName) const
{
    auto it = m_states.find(p_stateName);
    if (it == m_states.end())
    {
        throw StatechartException("Could not find state <" + p_stateName +
                                  ">.");
    }
    return it->second;
}

bool Statechart::start(Metadata& p_data)
{
    Parameter empty;
    return start(p_data, empty);
}

bool Statechart::start(Metadata& p_data, Parameter& p_param)
{
    p_data.reset();
    p_data.activate(this);
    if (m_startState != nullptr)
    {
        p_data.activate(m_startState);
    }
    return dispatch(p_data, nullptr, p_param);
}

bool Statechart::restoreState(State* p_state,
                              Metadata& p_data,
                              Parameter& p_param)
{
    if (p_data.isActive(this))
    {
        return false;
    }
    p_data.reset();

    std::vector<State*> path;
    State* parent = p_state;
    while (parent != nullptr)
    {
        path.insert(path.begin(), parent);
        parent = parent->context();
    }
    for (State* s : path)
    {
        s->activate(p_data, p_param);
    }
    return true;
}

bool Statechart::dispatch(Metadata& p_data, Event* p_event)
{
    Parameter empty;
    return dispatch(p_data, p_event, empty);
}

bool Statechart::dispatch(Metadata& p_data, Event* p_event, Parameter& p_param)
{
    bool rc = false;
    std::lock_guard<std::recursive_mutex> lk(p_data.mutex());

    StateRuntimedata* selfData = p_data.getData(this);
    if (selfData == nullptr)
    {
        return false;
    }
    State* current = selfData->currentState;
    if (current != nullptr)
    {
        rc = current->dispatch(p_data, p_event, p_param);
    }

    // Trigger-less transitions (no event, no guard, or guard always-true)
    // are handled by feeding @c nullptr to dispatch() until no transition
    // fires anymore. A cycle of such transitions creates an infinite loop;
    // we cap the number of consecutive steps and throw a dedicated
    // exception so the modelling mistake is reported instead of hanging.
    std::size_t steps = 0;
    while (true)
    {
        StateRuntimedata* d = p_data.getData(this);
        if (d == nullptr)
        {
            break;
        }
        State* c = d->currentState;
        if (c == nullptr)
        {
            break;
        }
        if (!c->dispatch(p_data, nullptr, p_param))
        {
            break;
        }
        if (++steps >= m_infiniteLoopThreshold)
        {
            throw InfiniteLoopException(
                "Statechart <" + name() + ">: more than " +
                std::to_string(m_infiniteLoopThreshold) +
                " consecutive trigger-less transitions; the model very "
                "likely contains a cycle of unguarded transitions. Use "
                "setInfiniteLoopThreshold() to raise the limit if this is "
                "intentional.");
        }
    }
    return rc;
}

void Statechart::startAsynchron(Metadata& p_data)
{
    startAsynchron(p_data, std::make_shared<Parameter>());
}

void Statechart::startAsynchron(Metadata& p_data,
                                std::shared_ptr<Parameter> p_param)
{
    p_data.reset();
    p_data.activate(this);
    if (m_startState != nullptr)
    {
        p_data.activate(m_startState);
    }
    dispatchAsynchron(p_data, nullptr, std::move(p_param));
}

void Statechart::dispatchAsynchron(Metadata& p_data,
                                   std::shared_ptr<Event> p_event)
{
    dispatchAsynchron(
        p_data, std::move(p_event), std::make_shared<Parameter>());
}

void Statechart::dispatchAsynchron(Metadata& p_data,
                                   std::shared_ptr<Event> p_event,
                                   std::shared_ptr<Parameter> p_param)
{
    if (!m_threadpool || m_threadpool->isShutdown())
    {
        return;
    }

    if (const auto* timeout = dynamic_cast<TimeoutEvent*>(p_event.get()))
    {
        auto entry = std::make_shared<EventQueueEntry>(this,
                                                       this,
                                                       &p_data,
                                                       p_event,
                                                       std::move(p_param),
                                                       timeout->timeout());
        {
            std::lock_guard<std::mutex> lk(m_pendingMutex);
            m_pendingEntries.push_back(entry);
        }
        m_timeoutQueue.add(entry);
    }
    else
    {
        auto entry = std::make_shared<EventQueueEntry>(
            this, this, &p_data, std::move(p_event), std::move(p_param), 0);
        {
            std::lock_guard<std::mutex> lk(m_pendingMutex);
            m_pendingEntries.push_back(entry);
        }
        std::shared_ptr<EventQueueEntry> captured = entry;
        m_threadpool->execute([captured]() { captured->run(); });
    }
}

EventQueueEntry* Statechart::enqueueTimeout(State* p_state,
                                            Metadata* p_data,
                                            TimeoutEvent* p_event,
                                            Parameter* p_param,
                                            std::int64_t p_timeoutMs)
{
    if (!m_running.load(std::memory_order_acquire))
    {
        return nullptr;
    }
    // Wrap raw pointers with non-owning aliasing shared_ptrs so the queue
    // entry can hold them with the same shared_ptr<Event>/Parameter shape
    // as the async path.
    std::shared_ptr<Event> evShared(std::shared_ptr<Event>{}, p_event);
    std::shared_ptr<Parameter> paramShared(std::shared_ptr<Parameter>{},
                                           p_param);

    auto entry = std::make_shared<EventQueueEntry>(this,
                                                   p_state,
                                                   p_data,
                                                   std::move(evShared),
                                                   std::move(paramShared),
                                                   p_timeoutMs);
    {
        std::lock_guard<std::mutex> lk(m_pendingMutex);
        m_pendingEntries.push_back(entry);
    }
    m_timeoutQueue.add(entry);
    return entry.get();
}

void Statechart::cancelTimeout(EventQueueEntry* p_entry)
{
    if (p_entry == nullptr)
    {
        return;
    }
    p_entry->markInvalid();
    std::lock_guard<std::mutex> lk(m_pendingMutex);
    for (auto it = m_pendingEntries.begin(); it != m_pendingEntries.end(); ++it)
    {
        if (it->get() == p_entry)
        {
            m_timeoutQueue.remove(*it);
            m_pendingEntries.erase(it);
            return;
        }
    }
}

bool Statechart::isShutdown() const
{
    return !m_threadpool || m_threadpool->isShutdown();
}

void Statechart::timeoutLoop()
{
    while (m_running.load(std::memory_order_acquire))
    {
        auto opt = m_timeoutQueue.take();
        if (!opt.has_value())
        {
            return;
        }
        std::shared_ptr<EventQueueEntry> entry = std::move(opt.value());
        if (!entry || entry->invalid())
        {
            continue;
        }
        if (!m_threadpool || m_threadpool->isShutdown())
        {
            return;
        }
        std::shared_ptr<EventQueueEntry> captured = entry;
        m_threadpool->execute([captured]() { captured->run(); });
    }
}

} // namespace statechart
