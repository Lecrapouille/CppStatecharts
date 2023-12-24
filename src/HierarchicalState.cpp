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

#include "Statechart/HierarchicalState.hpp"

#include "Statechart/ConcurrentState.hpp"
#include "Statechart/FinalState.hpp"
#include "Statechart/Metadata.hpp"
#include "Statechart/Parameter.hpp"
#include "Statechart/PseudoState.hpp"
#include "Statechart/StateRuntimedata.hpp"
#include "Statechart/Transition.hpp"

namespace statechart {

HierarchicalState::HierarchicalState(std::string p_name,
                                     Context* p_parent,
                                     Statechart* p_chart,
                                     Action p_entryAction,
                                     Action p_doAction,
                                     Action p_exitAction)
    : Context(std::move(p_name),
              p_parent,
              p_chart,
              std::move(p_entryAction),
              std::move(p_doAction),
              std::move(p_exitAction))
{
    if (auto* concurrent = dynamic_cast<ConcurrentState*>(p_parent))
    {
        concurrent->addRegion(this);
    }
}

void HierarchicalState::deactivate(Metadata& p_data, Parameter& p_param)
{
    if (!p_data.isActive(this))
    {
        return;
    }

    StateRuntimedata* statedata = p_data.getData(this);
    if (statedata == nullptr)
    {
        State::deactivate(p_data, p_param);
        return;
    }

    // Save history before deactivating substates, mirroring the Java behaviour.
    if (m_history != nullptr && statedata->currentState != m_startState &&
        statedata->currentState != m_history &&
        dynamic_cast<FinalState*>(statedata->currentState) == nullptr)
    {
        m_history->storeHistory(p_data);
    }

    if (statedata->currentState != nullptr)
    {
        statedata->currentState->deactivate(p_data, p_param);
    }
    statedata->currentState = nullptr;

    State::deactivate(p_data, p_param);
}

bool HierarchicalState::dispatch(Metadata& p_data,
                                 Event* p_event,
                                 Parameter& p_param)
{
    if (!p_data.isActive(this))
    {
        return false;
    }

    StateRuntimedata* statedata = p_data.getData(this);

    // First entry into a hierarchical state: enter the start state if any.
    if (statedata != nullptr && statedata->currentState == nullptr &&
        m_startState != nullptr)
    {
        p_data.activate(m_startState);
        statedata = p_data.getData(this);
        if (statedata != nullptr && statedata->currentState != nullptr)
        {
            statedata->currentState->activate(p_data, p_param);
        }
    }

    bool handled = false;
    if (statedata != nullptr && statedata->currentState != nullptr)
    {
        handled = statedata->currentState->dispatch(p_data, p_event, p_param);
    }

    // The substate dispatch may have deactivated this state and freed our
    // runtime data, so re-evaluate before reading any cached pointer.
    if (!p_data.isActive(this))
    {
        return handled;
    }
    statedata = p_data.getData(this);
    State* current = (statedata != nullptr) ? statedata->currentState : nullptr;
    const bool currentIsFinal = dynamic_cast<FinalState*>(current) != nullptr;
    if (handled && !currentIsFinal)
    {
        return handled;
    }

    // No substate handled the event: try this state's transitions.
    for (Transition* t : m_transitions)
    {
        if (!currentIsFinal && !t->hasEvent())
        {
            continue;
        }
        if (currentIsFinal && !t->hasEvent())
        {
            if (t->execute(nullptr, p_data, p_param))
            {
                return true;
            }
        }
        if (t->execute(p_event, p_data, p_param))
        {
            return true;
        }
    }
    return false;
}

} // namespace statechart
