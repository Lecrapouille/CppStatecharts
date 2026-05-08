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

#pragma once

#include "Statechart/Context.hpp"
#include "Statechart/DelayQueue.hpp"
#include "Statechart/EventQueueEntry.hpp"
#include "Statechart/StatechartException.hpp"

#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace statechart {

/**
 * @brief Top-level statechart, owner of every @c State and @c Transition
 *        created in its scope.
 *
 * The arena ownership model lets the framework hand out raw pointers to
 * client code while keeping a single, deterministic destruction order:
 * destroying the @c Statechart frees every state, transition, and queued
 * entry in one shot.
 *
 * Use @c create<T>(...) and @c createTransition(...) factory methods to
 * spawn objects; passing a parent context is mandatory for non-root
 * states.
 */
class Statechart: public Context
{
public:

    /** @brief Library version, mirrors the Java @c VERSION constant. */
    static constexpr std::string_view VERSION = "1.2.0";

    /**
     * @brief Creates a new statechart.
     *
     * @param p_name              Name of the statechart, also used as the
     *                            base name for worker threads.
     * @param p_threads           Maximum number of worker threads (clamped
     *                            to at least 2 for async + timeout).
     * @param p_makeDaemonThreads Kept for API parity with the Java
     *                            implementation; unused on C++ POSIX
     *                            threads which always exit with the
     *                            process.
     */
    Statechart(std::string p_name, int p_threads, bool p_makeDaemonThreads);

    ~Statechart() override;

    Statechart(const Statechart&) = delete;
    Statechart& operator=(const Statechart&) = delete;
    Statechart(Statechart&&) = delete;
    Statechart& operator=(Statechart&&) = delete;

    /**
     * @brief Allocates a new state of type @p T inside this statechart.
     *
     * The newly-created state is registered in the internal name lookup
     * table when @p T derives from @c State and a name is available.
     *
     * @return A non-owning pointer to the created state. The statechart
     *         keeps sole ownership.
     */
    template <typename T, typename... Args>
    T* create(std::string p_name, Context* p_parent, Args&&... p_args)
    {
        static_assert(std::is_base_of_v<State, T>,
                      "T must derive from statechart::State");
        if (p_name.empty())
        {
            throw StatechartException("Parameter name cannot be null");
        }
        if (p_parent == nullptr)
        {
            throw StatechartException("Parameter parent cannot be null");
        }
        if (m_states.find(p_name) != m_states.end())
        {
            throw StatechartException(
                "State name <" + p_name +
                "> already used! Please define a unique name.");
        }
        auto owned = std::make_unique<T>(
            p_name, p_parent, this, std::forward<Args>(p_args)...);
        T* raw = owned.get();
        m_states.emplace(std::move(p_name), raw);
        m_ownedStates.push_back(std::move(owned));
        return raw;
    }

    /**
     * @brief Creates and registers a transition, perfect-forwarding the
     *        constructor arguments.
     */
    template <typename... Args>
    Transition* createTransition(Args&&... p_args)
    {
        auto owned =
            std::make_unique<Transition>(std::forward<Args>(p_args)...);
        Transition* raw = owned.get();
        m_ownedTransitions.push_back(std::move(owned));
        return raw;
    }

    /**
     * @brief Creates and registers an @c Event subclass owned by this
     *        statechart.
     *
     * Convenience factory that lets test code allocate trigger events
     * inline while keeping their lifetime tied to the statechart.
     */
    template <typename E, typename... Args>
    E* createEvent(Args&&... p_args)
    {
        static_assert(std::is_base_of_v<Event, E>,
                      "E must derive from statechart::Event");
        auto owned = std::make_unique<E>(std::forward<Args>(p_args)...);
        E* raw = owned.get();
        m_ownedEvents.push_back(std::move(owned));
        return raw;
    }

    /**
     * @brief Creates and registers an internal (self) transition.
     */
    template <typename... Args>
    InternalTransition* createInternalTransition(Args&&... p_args);

    /**
     * @brief Looks up a state by name.
     *
     * @throws StatechartException if no state has the given name.
     */
    State* getStateByName(const std::string& p_stateName) const;

    /**
     * @brief Initializes the metadata and dispatches the initial step.
     */
    bool start(Metadata& p_data);

    /** @copydoc start(Metadata&) */
    bool start(Metadata& p_data, Parameter& p_param);

    /**
     * @brief Restores @p p_data so that @p p_state is the active state on
     *        return, without running entry actions on the parents.
     *
     * @return @c false if the statechart is already running on @p p_data.
     */
    bool restoreState(State* p_state, Metadata& p_data, Parameter& p_param);

    /** @brief Synchronously dispatches @p p_event into the statechart. */
    bool dispatch(Metadata& p_data, Event* p_event);

    /** @brief Synchronously dispatches @p p_event with a custom parameter. */
    bool dispatch(Metadata& p_data, Event* p_event, Parameter& p_param);

    /**
     * @brief Initializes the statechart and triggers an initial async
     *        dispatch.
     */
    void startAsynchron(Metadata& p_data);
    void startAsynchron(Metadata& p_data, std::shared_ptr<Parameter> p_param);

    /** @brief Schedules @p p_event for asynchronous dispatch. */
    void dispatchAsynchron(Metadata& p_data, std::shared_ptr<Event> p_event);
    void dispatchAsynchron(Metadata& p_data,
                           std::shared_ptr<Event> p_event,
                           std::shared_ptr<Parameter> p_param);

    /**
     * @brief Stops the worker threads. Idempotent.
     */
    void shutdown();

    /**
     * @brief Schedules a timeout entry. Used by @c State::activate().
     *
     * @return A non-owning pointer to the entry, valid until the entry
     *         either fires or is cancelled.
     */
    EventQueueEntry* enqueueTimeout(State* p_state,
                                    Metadata* p_data,
                                    TimeoutEvent* p_event,
                                    Parameter* p_param,
                                    std::int64_t p_timeoutMs);

    /** @brief Cancels a previously-scheduled timeout entry. */
    void cancelTimeout(EventQueueEntry* p_entry);

    /** @brief Returns @c true if the threadpool is shut down. */
    bool isShutdown() const;

    /**
     * @brief Sets the maximum number of trigger-less transitions that
     *        @c dispatch() is allowed to fire in a single invocation.
     *
     * Trigger-less transitions are transitions without event/guard (or
     * whose guard is permanently true). They are evaluated in a loop after
     * the user-supplied event has been processed. If that loop runs more
     * than @p p_max times an @c InfiniteLoopException is thrown, which
     * usually signals a modelling mistake (a cycle of trigger-less
     * transitions).
     *
     * The default value is 1000.
     *
     * @param p_max Maximum number of consecutive trigger-less steps. Must
     *              be > 0.
     */
    void setInfiniteLoopThreshold(std::size_t p_max)
    {
        m_infiniteLoopThreshold = p_max == 0 ? 1 : p_max;
    }

    /** @brief Returns the configured infinite-loop detection threshold. */
    std::size_t infiniteLoopThreshold() const
    {
        return m_infiniteLoopThreshold;
    }

private:

    void timeoutLoop();

private:

    std::vector<std::unique_ptr<State>> m_ownedStates;
    std::vector<std::unique_ptr<Transition>> m_ownedTransitions;
    std::vector<std::unique_ptr<Event>> m_ownedEvents;
    std::unordered_map<std::string, State*> m_states;

    std::unique_ptr<class ThreadPool> m_threadpool;
    DelayQueue<std::shared_ptr<EventQueueEntry>> m_timeoutQueue;
    std::vector<std::shared_ptr<EventQueueEntry>>
        m_pendingEntries; // keeps async entries alive
    mutable std::mutex m_pendingMutex;
    std::thread m_timeoutThread;
    std::atomic<bool> m_running{false};

    /// @brief Max trigger-less transitions before throwing
    ///        @c InfiniteLoopException. See @c setInfiniteLoopThreshold().
    std::size_t m_infiniteLoopThreshold = 1000;

    friend class State;
    friend class EventQueueEntry;
};

} // namespace statechart

#include "Statechart/InternalTransition.hpp"

namespace statechart {

template <typename... Args>
inline InternalTransition*
Statechart::createInternalTransition(Args&&... p_args)
{
    auto owned =
        std::make_unique<InternalTransition>(std::forward<Args>(p_args)...);
    InternalTransition* raw = owned.get();
    m_ownedTransitions.push_back(std::move(owned));
    return raw;
}

} // namespace statechart
