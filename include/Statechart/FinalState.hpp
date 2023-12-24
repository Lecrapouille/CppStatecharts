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
 * @brief Marker state representing the end of a statechart or sub-region.
 *
 * @c FinalState reuses the regular @c State semantics with no additional
 * behaviour: detection is performed via @c dynamic_cast at call sites.
 */
class FinalState: public State
{
public:

    /**
     * @brief Creates a final state attached to @p p_parent.
     *
     * @param p_name   Unique state name.
     * @param p_parent Parent context (must not be @c nullptr).
     * @param p_chart  Owning statechart (must not be @c nullptr).
     */
    FinalState(std::string p_name, Context* p_parent, Statechart* p_chart)
        : State(std::move(p_name), p_parent, p_chart)
    {
    }

    ~FinalState() override = default;
};

} // namespace statechart
