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

#include "Statechart/forward.hpp"

#include <vector>

namespace statechart {

/**
 * @brief Describes a transition between two @c State objects.
 *
 * A transition optionally carries a triggering event, a guard and an action.
 * On firing, it deactivates a chain of states and activates another, walking
 * up to the least common ancestor of source and destination.
 *
 * The @c Statechart owns the @c Transition; the source state owns a
 * non-owning pointer back to it. The @c m_event pointer is also non-owning,
 * mirroring the Java semantics where events live independently from
 * transitions.
 */
class Transition
{
public:

    /** @brief Creates a trigger-less, guard-less, action-less transition. */
    Transition(State* p_start, State* p_end);

    /** @brief Creates a transition triggered by @p p_event. */
    Transition(State* p_start, State* p_end, Event* p_event);

    /** @brief Creates a transition guarded by @p p_guard. */
    Transition(State* p_start, State* p_end, Guard p_guard);

    /** @brief Creates a transition with an action. */
    Transition(State* p_start, State* p_end, Action p_action);

    /** @brief Creates a transition with an event and a guard. */
    Transition(State* p_start, State* p_end, Event* p_event, Guard p_guard);

    /** @brief Creates a transition with an event and an action. */
    Transition(State* p_start, State* p_end, Event* p_event, Action p_action);

    /** @brief Creates a transition with a guard and an action. */
    Transition(State* p_start, State* p_end, Guard p_guard, Action p_action);

    /** @brief Creates a transition with event, guard and action. */
    Transition(State* p_start,
               State* p_end,
               Event* p_event,
               Guard p_guard,
               Action p_action);

    virtual ~Transition() = default;

    Transition(const Transition&) = delete;
    Transition& operator=(const Transition&) = delete;
    Transition(Transition&&) = delete;
    Transition& operator=(Transition&&) = delete;

    /** @brief Returns @c true if this transition is triggered by an event. */
    bool hasEvent() const
    {
        return m_event != nullptr;
    }

    /** @brief Returns @c true if this transition has a guard. */
    bool hasGuard() const
    {
        return static_cast<bool>(m_guard);
    }

    /** @brief Returns @c true if this transition has an action. */
    bool hasAction() const
    {
        return static_cast<bool>(m_action);
    }

    /** @brief Returns the triggering event (non-owning, may be @c nullptr). */
    Event* event() const
    {
        return m_event;
    }

    /**
     * @brief Tries to fire the transition for the given event.
     *
     * @note Framework-internal.
     * @return @c true if the transition fired (i.e. matched event, guard,
     *         and reachable target).
     */
    virtual bool execute(Event* p_event, Metadata& p_data, Parameter& p_param);

    /**
     * @brief Returns @c true if guard and target reachability allow this
     *        transition to fire right now.
     */
    virtual bool allowed(Metadata& p_data, Parameter& p_param);

protected:

    //@brief The event which triggers the transition.
    Event* m_event = nullptr;
    //@brief The guard watching if the transition can trigger or not.
    Guard m_guard;
    //@brief The action to be executed when the transition triggers.
    Action m_action;
    //@brief List of all states which must be deactivated when triggering.
    std::vector<State*> m_deactivate;
    //@brief List of all states which must be activated when triggering.
    std::vector<State*> m_activate;

    friend class State;
    friend class PseudoState;
    friend class HierarchicalState;
    friend class ConcurrentState;
    friend class Statechart;

private:

    /** @brief Common initialization shared by all constructors. */
    void init(State* p_start, State* p_end);

    /**
     * @brief Computes deactivate/activate paths up to the least common
     *        ancestor of @p p_start and @p p_end.
     */
    static void calculateStateSet(State* p_start,
                                  State* p_end,
                                  std::vector<State*>& p_deactivate,
                                  std::vector<State*>& p_activate);
};

} // namespace statechart
