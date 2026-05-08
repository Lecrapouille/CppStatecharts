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

#include "Statechart/PseudoState.hpp"

#include "Statechart/ConcurrentState.hpp"
#include "Statechart/HierarchicalState.hpp"
#include "Statechart/Metadata.hpp"
#include "Statechart/Parameter.hpp"
#include "Statechart/StateRuntimedata.hpp"
#include "Statechart/Statechart.hpp"
#include "Statechart/StatechartException.hpp"
#include "Statechart/Transition.hpp"

namespace statechart {

namespace {

/**
 * @brief Depth-first traversal recording the active descendant chain for
 *        history bookkeeping.
 */
void calculateHistoryChain(std::vector<State*>& p_history,
                           State* p_state,
                           Metadata& p_data,
                           PseudoStateType p_type)
{
    if (p_state == nullptr)
    {
        return;
    }
    p_history.push_back(p_state);

    if (p_type == PseudoStateType::History)
    {
        return;
    }

    if (auto* hier = dynamic_cast<HierarchicalState*>(p_state))
    {
        StateRuntimedata* runtimedata = p_data.getData(hier);
        State* substate =
            (runtimedata != nullptr) ? runtimedata->currentState : nullptr;
        if (substate != nullptr)
        {
            calculateHistoryChain(p_history, substate, p_data, p_type);
        }
    }
}

} // namespace

PseudoState::PseudoState(std::string p_name,
                         Context* p_parent,
                         Statechart* p_chart,
                         PseudoStateType p_type)
    : State(std::move(p_name), p_parent, p_chart), m_type(p_type)
{
    if (p_parent == nullptr)
    {
        throw StatechartException(
            "PseudoState requires a non-null parent context");
    }

    if (p_type == PseudoStateType::Start)
    {
        if (p_parent->m_startState == nullptr)
        {
            p_parent->m_startState = this;
        }
        else
        {
            throw StatechartException("Parent has already a start state!");
        }
    }
    else if (p_type == PseudoStateType::History ||
             p_type == PseudoStateType::DeepHistory)
    {
        auto* hier = dynamic_cast<HierarchicalState*>(p_parent);
        if (hier != nullptr)
        {
            if (hier->m_history == nullptr)
            {
                hier->m_history = this;
            }
            else
            {
                throw StatechartException(
                    "Parent has already a history state!");
            }
        }
        else
        {
            throw StatechartException("Parent is not hierarchical state!");
        }
    }
}

bool PseudoState::lookup(Metadata& p_data, Parameter& p_param) const
{
    // For join states: every incoming transition must have its source state
    // active and any guard must accept.
    if (m_type == PseudoStateType::Join)
    {
        for (Transition* t : m_incoming)
        {
            if (t == nullptr || t->m_deactivate.empty())
            {
                return false;
            }
            const StateRuntimedata* d = p_data.getData(t->m_deactivate.front());
            if (d == nullptr || !d->active)
            {
                return false;
            }
            if (t->hasGuard() && !t->m_guard(p_data, p_param))
            {
                return false;
            }
        }
    }

    // The pseudo-state is reachable only if at least one outgoing transition
    // can immediately fire.
    for (Transition* t : m_transitions)
    {
        if (t->allowed(p_data, p_param))
        {
            return true;
        }
    }
    return false;
}

bool PseudoState::activate(Metadata& p_data, Parameter& p_param)
{
    p_data.activate(this);
    const StateRuntimedata* d = p_data.getData(this);

    if (m_entryAction)
    {
        m_entryAction(p_data, p_param);
    }

    if (m_type == PseudoStateType::History ||
        m_type == PseudoStateType::DeepHistory)
    {
        if (d != nullptr)
        {
            for (State* s : d->stateset)
            {
                if (s != nullptr)
                {
                    s->activate(p_data, p_param);
                }
            }
        }
    }
    else if (m_type == PseudoStateType::Fork)
    {
        // Mirror the Java code: when entering a region implicitly through a
        // concurrent state, register the region in the parent's stateset to
        // skip its automatic activation.
        for (Transition* t : m_transitions)
        {
            if (t->m_guard && !t->m_guard(p_data, p_param))
            {
                continue;
            }
            for (std::size_t i = 0; i < t->m_activate.size(); ++i)
            {
                if (i + 1 < t->m_activate.size())
                {
                    auto* concurrent =
                        dynamic_cast<ConcurrentState*>(t->m_activate[i]);
                    if (concurrent != nullptr)
                    {
                        StateRuntimedata* cd =
                            p_data.createRuntimedata(concurrent);
                        State* region = t->m_activate[i + 1];
                        bool already = false;
                        for (const State* s : cd->stateset)
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
            }
        }
    }
    // Join states do not run additional activation logic here; transitions
    // execute the actions of all incoming transitions in sequence.

    return true;
}

bool PseudoState::dispatch(Metadata& p_data, Event* p_event, Parameter& p_param)
{
    if (m_type == PseudoStateType::History ||
        m_type == PseudoStateType::DeepHistory)
    {
        StateRuntimedata* d =
            (m_context != nullptr) ? p_data.getData(m_context) : nullptr;
        if (d != nullptr && d->currentState != nullptr &&
            d->currentState != this)
        {
            return d->currentState->dispatch(p_data, p_event, p_param);
        }
    }
    else if (m_type == PseudoStateType::Fork)
    {
        for (Transition* t : m_transitions)
        {
            t->execute(p_event, p_data, p_param);
        }
        return true;
    }
    return State::dispatch(p_data, p_event, p_param);
}

void PseudoState::storeHistory(Metadata& p_data)
{
    StateRuntimedata* d = p_data.getData(this);
    if (d == nullptr || m_context == nullptr)
    {
        return;
    }
    d->stateset.clear();

    StateRuntimedata* parent = p_data.getData(m_context);
    State* current = (parent != nullptr) ? parent->currentState : nullptr;
    calculateHistoryChain(d->stateset, current, p_data, m_type);
}

void PseudoState::addIncomingTransition(Transition* p_transition)
{
    if (p_transition != nullptr)
    {
        m_incoming.push_back(p_transition);
    }
}

} // namespace statechart
