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

#include "Statechart/Transition.hpp"

#include "Statechart/ConcurrentState.hpp"
#include "Statechart/Context.hpp"
#include "Statechart/Event.hpp"
#include "Statechart/Metadata.hpp"
#include "Statechart/Parameter.hpp"
#include "Statechart/PseudoState.hpp"
#include "Statechart/State.hpp"
#include "Statechart/StateRuntimedata.hpp"
#include "Statechart/Statechart.hpp"

#include <utility>

namespace statechart {

Transition::Transition(State* p_start, State* p_end)
{
    init(p_start, p_end);
}

Transition::Transition(State* p_start, State* p_end, Event* p_event)
    : m_event(p_event)
{
    init(p_start, p_end);
}

Transition::Transition(State* p_start, State* p_end, Guard p_guard)
    : m_guard(std::move(p_guard))
{
    init(p_start, p_end);
}

Transition::Transition(State* p_start, State* p_end, Action p_action)
    : m_action(std::move(p_action))
{
    init(p_start, p_end);
}

Transition::Transition(State* p_start,
                       State* p_end,
                       Event* p_event,
                       Guard p_guard)
    : m_event(p_event), m_guard(std::move(p_guard))
{
    init(p_start, p_end);
}

Transition::Transition(State* p_start,
                       State* p_end,
                       Event* p_event,
                       Action p_action)
    : m_event(p_event), m_action(std::move(p_action))
{
    init(p_start, p_end);
}

Transition::Transition(State* p_start,
                       State* p_end,
                       Guard p_guard,
                       Action p_action)
    : m_guard(std::move(p_guard)), m_action(std::move(p_action))
{
    init(p_start, p_end);
}

Transition::Transition(State* p_start,
                       State* p_end,
                       Event* p_event,
                       Guard p_guard,
                       Action p_action)
    : m_event(p_event),
      m_guard(std::move(p_guard)),
      m_action(std::move(p_action))
{
    init(p_start, p_end);
}

bool Transition::execute(Event* p_event, Metadata& p_data, Parameter& p_param)
{
    if (m_event != nullptr && !m_event->equals(p_event, p_data, p_param))
    {
        return false;
    }
    if (m_event != nullptr && p_event == nullptr)
    {
        return false;
    }
    if (!allowed(p_data, p_param))
    {
        return false;
    }

    for (State* s : m_deactivate)
    {
        if (s != nullptr)
        {
            s->deactivate(p_data, p_param);
        }
    }

    if (m_action)
    {
        m_action(p_data, p_param);
    }

    for (std::size_t i = 0; i < m_activate.size(); ++i)
    {
        // Implicit activation of a concurrent state: register the next state
        // (a region) in the concurrent state's stateset so that automatic
        // region activation skips it.
        if (i + 1 < m_activate.size())
        {
            auto* concurrent = dynamic_cast<ConcurrentState*>(m_activate[i]);
            if (concurrent != nullptr)
            {
                StateRuntimedata* cd = p_data.createRuntimedata(concurrent);
                State* region = m_activate[i + 1];
                bool already = false;
                for (State* s : cd->stateset)
                {
                    if (s == region)
                    {
                        already = true;
                        break;
                    }
                }
                if (!already)
                {
                    cd->stateset.push_back(region);
                }
            }
        }
        if (m_activate[i] != nullptr)
        {
            m_activate[i]->activate(p_data, p_param);
        }
    }
    return true;
}

bool Transition::allowed(Metadata& p_data, Parameter& p_param)
{
    if (m_guard && !m_guard(p_data, p_param))
    {
        return false;
    }
    if (m_activate.empty())
    {
        return true;
    }
    if (auto* pseudo = dynamic_cast<PseudoState*>(m_activate.back()))
    {
        return pseudo->lookup(p_data, p_param);
    }
    return true;
}

void Transition::init(State* p_start, State* p_end)
{
    Transition::calculateStateSet(p_start, p_end, m_deactivate, m_activate);
    if (p_start != nullptr)
    {
        p_start->addTransition(this);
    }

    // For join states we need to remember which transitions feed into them
    // so that lookup() can verify all sources are active before firing.
    if (auto* pseudo = dynamic_cast<PseudoState*>(p_end))
    {
        if (pseudo->type() == PseudoStateType::Join)
        {
            pseudo->addIncomingTransition(this);
        }
    }
}

void Transition::calculateStateSet(State* p_start,
                                   State* p_end,
                                   std::vector<State*>& p_deactivate,
                                   std::vector<State*>& p_activate)
{
    std::vector<State*> a;
    std::vector<State*> d;

    // Walk up from start to root, building the deactivate path top-down.
    State* s = p_start;
    while (s != nullptr)
    {
        d.insert(d.begin(), s);
        Context* ctx = s->context();
        if (ctx != nullptr && dynamic_cast<Statechart*>(ctx) == nullptr)
        {
            s = ctx;
        }
        else
        {
            s = nullptr;
        }
    }

    // Walk up from end to root, building the activate path top-down.
    State* e = p_end;
    while (e != nullptr)
    {
        a.insert(a.begin(), e);
        Context* ctx = e->context();
        if (ctx != nullptr && dynamic_cast<Statechart*>(ctx) == nullptr)
        {
            e = ctx;
        }
        else
        {
            e = nullptr;
        }
    }

    const std::size_t minSize = (a.size() < d.size()) ? a.size() : d.size();
    std::size_t lca = (minSize == 0) ? 0 : minSize - 1;

    if (p_start != p_end)
    {
        for (lca = 0; lca < minSize; ++lca)
        {
            if (a[lca] != d[lca])
            {
                break;
            }
        }
    }

    for (std::size_t j = lca; j < d.size(); ++j)
    {
        p_deactivate.insert(p_deactivate.begin(), d[j]);
    }
    for (std::size_t j = lca; j < a.size(); ++j)
    {
        p_activate.push_back(a[j]);
    }
}

} // namespace statechart
