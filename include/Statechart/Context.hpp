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
 * @brief Abstract base class for any state that can hold child states.
 *
 * A @c Context exposes the @c startState pointer used by hierarchical and
 * concurrent states (and by @c Statechart itself). Direct instantiation is
 * forbidden: only @c HierarchicalState, @c ConcurrentState and
 * @c Statechart derive from it.
 */
class Context: public State
{
public:

    Context(std::string p_name,
            Context* p_parent,
            Statechart* p_chart,
            Action p_entryAction = {},
            Action p_doAction = {},
            Action p_exitAction = {});

    ~Context() override = default;

    /** @brief Returns the start pseudo-state of this context, or @c nullptr. */
    PseudoState* startState() const
    {
        return m_startState;
    }

protected:

    PseudoState* m_startState = nullptr;

    friend class PseudoState;
    friend class HierarchicalState;
    friend class ConcurrentState;
    friend class Statechart;
    friend class Transition;
};

} // namespace statechart
