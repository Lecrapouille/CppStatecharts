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

#include "Statechart/ConcurrentState.hpp"

#include "Statechart/FinalState.hpp"
#include "Statechart/HierarchicalState.hpp"
#include "Statechart/Metadata.hpp"
#include "Statechart/Parameter.hpp"
#include "Statechart/StateRuntimedata.hpp"
#include "Statechart/Transition.hpp"

namespace statechart {

ConcurrentState::ConcurrentState(std::string p_name,
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
}

void ConcurrentState::addRegion(HierarchicalState* p_region)
{
    if (p_region != nullptr)
    {
        m_regions.push_back(p_region);
    }
}

bool ConcurrentState::activate(Metadata& p_data, Parameter& p_param)
{
    if (!State::activate(p_data, p_param))
    {
        return false;
    }
    StateRuntimedata* statedata = p_data.getData(this);
    if (statedata == nullptr)
    {
        return true;
    }
    for (HierarchicalState* h : m_regions)
    {
        // Skip regions that were activated implicitly through an incoming
        // fork/transition (registered in stateset).
        bool alreadyTargeted = false;
        for (State* s : statedata->stateset)
        {
            if (s == h)
            {
                alreadyTargeted = true;
                break;
            }
        }
        if (!alreadyTargeted)
        {
            if (h->activate(p_data, p_param))
            {
                h->dispatch(p_data, nullptr, p_param);
            }
        }
    }
    return true;
}

void ConcurrentState::deactivate(Metadata& p_data, Parameter& p_param)
{
    StateRuntimedata* statedata = p_data.getData(this);
    if (statedata != nullptr)
    {
        statedata->stateset.clear();
    }
    for (HierarchicalState* h : m_regions)
    {
        h->deactivate(p_data, p_param);
    }
    State::deactivate(p_data, p_param);
}

bool ConcurrentState::dispatch(Metadata& p_data,
                               Event* p_event,
                               Parameter& p_param)
{
    bool dispatched = false;

    // Dispatch to all regions while we remain active. Without that guard, an
    // implicit exit caused by one region would be ignored by the next.
    // Re-check isActive() each iteration since a region's transition may
    // have deactivated `this` and freed its runtime data.
    for (HierarchicalState* h : m_regions)
    {
        if (!p_data.isActive(this))
        {
            break;
        }
        if (h->dispatch(p_data, p_event, p_param))
        {
            dispatched = true;
        }
    }

    if (dispatched)
    {
        return true;
    }

    // Nobody consumed the event: try transitions on this state. Completion
    // transitions only fire once every region has reached its FinalState.
    for (Transition* t : m_transitions)
    {
        if (!p_data.isActive(this))
        {
            break;
        }
        if (t->event() == nullptr && !finished(p_data))
        {
            continue;
        }
        if (t->execute(p_event, p_data, p_param))
        {
            return true;
        }
    }
    return false;
}

bool ConcurrentState::finished(Metadata& p_data) const
{
    for (HierarchicalState* h : m_regions)
    {
        StateRuntimedata* d = p_data.getData(h);
        if (d == nullptr)
        {
            return false;
        }
        if (dynamic_cast<FinalState*>(d->currentState) == nullptr)
        {
            return false;
        }
    }
    return true;
}

} // namespace statechart
