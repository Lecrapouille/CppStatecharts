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

#include "CppStatecharts/forward.hpp"

#include <string>
#include <vector>

namespace statechart {

/**
 * @brief Represents a simple state of the statechart.
 *
 * A @c State holds optional entry/do/exit actions and a list of outgoing
 * transitions. Each state belongs to a @c Context (its direct parent) and
 * to a top-level @c Statechart. Both pointers are non-owning: every state
 * is owned by the @c Statechart it belongs to.
 *
 * The header forward-declares @c Context, @c Statechart, @c Transition,
 * @c Metadata, @c Event and @c Parameter to break circular includes; the
 * implementation file pulls the corresponding full definitions.
 */
class State
{
public:

    /**
     * @brief Creates a state with optional entry/do/exit actions.
     *
     * The constructor only stores its arguments. Registration of the state
     * inside its @c Statechart is performed by @c Statechart::create() so
     * that ownership semantics stay obvious to the caller.
     *
     * @param p_name        Unique state name (used for debugging and lookup).
     * @param p_parent      Parent context, or @c nullptr if the state is the
     *                      root @c Statechart itself.
     * @param p_chart       Pointer to the owning @c Statechart, or
     *                      @c nullptr if the state is the root @c Statechart.
     * @param p_entryAction Optional action executed on activation.
     * @param p_doAction    Optional action executed on activation, after the
     *                      entry action.
     * @param p_exitAction  Optional action executed on deactivation.
     */
    State(std::string p_name,
          Context* p_parent,
          Statechart* p_chart,
          Action p_entryAction = {},
          Action p_doAction = {},
          Action p_exitAction = {});

    virtual ~State() = default;

    State(const State&) = delete;
    State& operator=(const State&) = delete;
    State(State&&) = delete;
    State& operator=(State&&) = delete;

    /** @brief Returns the parent context, or @c nullptr for the root
     * statechart. */
    Context* context() const
    {
        return m_context;
    }

    /** @brief Returns the owning statechart, or @c nullptr for the root
     * statechart itself. */
    Statechart* statechart() const
    {
        return m_statechart;
    }

    /** @brief Replaces the entry action. */
    void setEntryAction(Action p_action);

    /** @brief Replaces the do action. */
    void setDoAction(Action p_action);

    /** @brief Replaces the exit action. */
    void setExitAction(Action p_action);

    /** @brief Renames the state (for debugging only; the lookup map is
     * unchanged). */
    void setName(std::string p_name);

    /** @brief Returns the state name. */
    const std::string& name() const
    {
        return m_name;
    }

    /** @brief Returns the state name (Java parity). */
    std::string toString() const
    {
        return m_name;
    }

    /**
     * @brief Activates the state.
     *
     * Triggers timeout events, then runs entry and do actions. Returns
     * @c true on the transition from inactive to active, @c false if the
     * state was already active.
     *
     * @note Framework-internal entry point. End users should invoke
     *       @c Statechart::start instead of calling this method directly.
     *
     * @param p_data  The runtime data of the statechart.
     * @param p_param The parameter forwarded by the caller.
     */
    virtual bool activate(Metadata& p_data, Parameter& p_param);

    /**
     * @brief Deactivates the state and runs the exit action if any.
     *
     * @note Framework-internal entry point.
     */
    virtual void deactivate(Metadata& p_data, Parameter& p_param);

    /**
     * @brief Tries to handle @p p_event by firing one of the outgoing
     *        transitions of this state.
     *
     * @note Framework-internal entry point. End users should invoke
     *       @c Statechart::dispatch instead.
     *
     * @return @c true if a transition fired, @c false otherwise.
     */
    virtual bool dispatch(Metadata& p_data, Event* p_event, Parameter& p_param);

protected:

    /** @brief Adds a transition to this state's outgoing-transition list. */
    void addTransition(Transition* p_transition);

protected:

    //@brief The entry action to be executed when the state is activated.
    Action m_entryAction;
    //@brief The do action to be executed when the state is activated.
    Action m_doAction;
    //@brief The exit action to be executed when the state is deactivated.
    Action m_exitAction;
    //@brief The list of transitions outgoing from this state.
    std::vector<Transition*> m_transitions;
    //@brief The parent context of this state.
    Context* m_context = nullptr;
    //@brief The owning statechart of this state.
    Statechart* m_statechart = nullptr;
    //@brief The name of this state.
    std::string m_name;

    friend class Transition;
    friend class InternalTransition;
    friend class Statechart;
    friend class Metadata;
    friend class Context;
    friend class HierarchicalState;
    friend class ConcurrentState;
    friend class PseudoState;
    friend class EventQueueEntry;
};

} // namespace statechart
