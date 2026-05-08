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

#include "Statechart/State.hpp"

#include "Statechart/Context.hpp"
#include "Statechart/Event.hpp"
#include "Statechart/EventQueueEntry.hpp"
#include "Statechart/Metadata.hpp"
#include "Statechart/Parameter.hpp"
#include "Statechart/StateRuntimedata.hpp"
#include "Statechart/Statechart.hpp"
#include "Statechart/TimeoutEvent.hpp"
#include "Statechart/Transition.hpp"

#include <utility>

namespace statechart {

State::State(std::string p_name,
             Context* p_parent,
             Statechart* p_chart,
             Action p_entryAction,
             Action p_doAction,
             Action p_exitAction)
    : m_entryAction(std::move(p_entryAction)),
      m_doAction(std::move(p_doAction)),
      m_exitAction(std::move(p_exitAction)),
      m_context(p_parent),
      m_statechart(p_chart),
      m_name(std::move(p_name))
{
}

void State::setEntryAction(Action p_action)
{
    m_entryAction = std::move(p_action);
}

void State::setDoAction(Action p_action)
{
    m_doAction = std::move(p_action);
}

void State::setExitAction(Action p_action)
{
    m_exitAction = std::move(p_action);
}

void State::setName(std::string p_name)
{
    m_name = std::move(p_name);
}

void State::addTransition(Transition* p_transition)
{
    if (p_transition == nullptr)
    {
        return;
    }
    // Guarded transitions are checked first so that the most specific path
    // wins when multiple transitions share the same trigger.
    if (p_transition->hasGuard())
    {
        m_transitions.insert(m_transitions.begin(), p_transition);
    }
    else
    {
        m_transitions.push_back(p_transition);
    }
}

bool State::activate(Metadata& p_data, Parameter& p_param)
{
    if (p_data.isActive(this))
    {
        return false;
    }

    p_data.activate(this);

    // Schedule timeout transitions associated with this state.
    if (m_statechart != nullptr)
    {
        for (const Transition* t : m_transitions)
        {
            auto* timeout = dynamic_cast<TimeoutEvent*>(t->event());
            if (timeout != nullptr)
            {
                EventQueueEntry* entry = m_statechart->enqueueTimeout(
                    this, &p_data, timeout, &p_param, timeout->timeout());
                StateRuntimedata* runtimedata = p_data.getData(this);
                if (runtimedata != nullptr && entry != nullptr)
                {
                    runtimedata->timeoutEvents.push_back(entry);
                }
            }
        }
    }

    if (m_entryAction)
    {
        m_entryAction(p_data, p_param);
    }
    if (m_doAction)
    {
        m_doAction(p_data, p_param);
    }
    return true;
}

void State::deactivate(Metadata& p_data, Parameter& p_param)
{
    if (!p_data.isActive(this))
    {
        return;
    }

    const StateRuntimedata* runtimedata = p_data.getData(this);
    if (runtimedata != nullptr && m_statechart != nullptr)
    {
        for (EventQueueEntry* entry : runtimedata->timeoutEvents)
        {
            if (entry != nullptr)
            {
                entry->markInvalid();
            }
            m_statechart->cancelTimeout(entry);
        }
    }

    p_data.deactivate(this);

    if (m_exitAction)
    {
        m_exitAction(p_data, p_param);
    }
}

bool State::dispatch(Metadata& p_data, Event* p_event, Parameter& p_param)
{
    for (Transition* t : m_transitions)
    {
        if (t->execute(p_event, p_data, p_param))
        {
            return true;
        }
    }
    return false;
}

} // namespace statechart
