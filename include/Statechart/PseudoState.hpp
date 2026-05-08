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

#include "Statechart/State.hpp"

namespace statechart {

/**
 * @brief Categorizes the kind of a @c PseudoState.
 *
 * Values mirror the integer constants exposed by the Java implementation
 * but are now type-safe.
 */
enum class PseudoStateType
{
    Start = 1,
    Junction = 2,
    Fork = 3,
    Join = 4,
    History = 5,
    DeepHistory = 6,
};

/**
 * @brief Implements UML pseudo-states (start, junction, fork, join, history,
 *        deep-history).
 *
 * Pseudo-states cannot be the resting place of a statechart: every visit
 * is transient and triggers either history restoration or transition
 * cascades depending on @c type().
 */
class PseudoState: public State
{
public:

    /**
     * @brief Creates a pseudo-state of the given type.
     *
     * Registers itself with the parent for @c Start (sets the parent's
     * start state), @c History and @c DeepHistory (sets the parent's
     * history slot, requiring a @c HierarchicalState parent).
     *
     * @throws StatechartException if the parent already has a start or
     *         history pseudo-state of the corresponding kind, or if a
     *         history pseudo-state is attached to a non-hierarchical parent.
     */
    PseudoState(std::string p_name,
                Context* p_parent,
                Statechart* p_chart,
                PseudoStateType p_type);

    ~PseudoState() override = default;

    /** @brief Returns the kind of this pseudo-state. */
    PseudoStateType type() const
    {
        return m_type;
    }

    /**
     * @brief Returns whether this pseudo-state can fire at least one of its
     *        outgoing transitions, i.e. whether activation does not lead to
     *        a dead-end.
     *
     * @note Framework-internal.
     */
    bool lookup(Metadata& p_data, Parameter& p_param) const;

    bool activate(Metadata& p_data, Parameter& p_param) override;
    bool
    dispatch(Metadata& p_data, Event* p_event, Parameter& p_param) override;

    /** @brief Records the active substate path for history restoration. */
    void storeHistory(Metadata& p_data);

    /** @brief Registers @p p_transition as an incoming join transition. */
    void addIncomingTransition(Transition* p_transition);

protected:

    PseudoStateType m_type = PseudoStateType::Start;
    std::vector<Transition*> m_incoming;

    friend class Transition;
    friend class HierarchicalState;
    friend class ConcurrentState;
    friend class Statechart;
};

} // namespace statechart
