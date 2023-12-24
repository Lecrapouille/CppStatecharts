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

#include "Statechart/Condition.hpp"

#include "Statechart/Statechart.hpp"
#include "Statechart/Transition.hpp"

#include <utility>

namespace statechart {

Condition::Condition(std::string p_name,
                     Context* p_parent,
                     Statechart* p_chart,
                     Guard p_guard,
                     State* p_whenTrue,
                     State* p_otherwise)
    : PseudoState(std::move(p_name),
                  p_parent,
                  p_chart,
                  PseudoStateType::Junction)
{
    if (p_chart != nullptr)
    {
        p_chart->createTransition(this, p_whenTrue, std::move(p_guard));
        p_chart->createTransition(this, p_otherwise);
    }
}

} // namespace statechart
