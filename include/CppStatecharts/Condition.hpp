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

#include "CppStatecharts/PseudoState.hpp"

namespace statechart {

/**
 * @brief Convenience pseudo-state that branches between two targets based
 *        on a guard.
 *
 * It is implemented as a junction with two outgoing transitions: one
 * gated by @p p_guard leading to @p p_whenTrue, and an unconditional
 * fallback to @p p_otherwise.
 */
class Condition: public PseudoState
{
public:

    /**
     * @brief Creates a junction-based condition node.
     *
     * @param p_name      Unique state name.
     * @param p_parent    Parent context.
     * @param p_chart     Owning statechart.
     * @param p_guard     Guard evaluated to choose @p p_whenTrue.
     * @param p_whenTrue  Target reached when @p p_guard returns @c true.
     * @param p_otherwise Target reached otherwise.
     */
    Condition(std::string p_name,
              Context* p_parent,
              Statechart* p_chart,
              Guard p_guard,
              State* p_whenTrue,
              State* p_otherwise);

    ~Condition() override = default;
};

} // namespace statechart
