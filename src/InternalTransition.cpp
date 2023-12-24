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

#include "Statechart/InternalTransition.hpp"

#include "Statechart/Event.hpp"
#include "Statechart/Metadata.hpp"
#include "Statechart/Parameter.hpp"

#include <utility>

namespace statechart {

InternalTransition::InternalTransition(State* p_state,
                                       Event* p_event,
                                       Action p_action)
    : Transition(p_state, p_state, p_event, std::move(p_action))
{
    m_deactivate.clear();
    m_activate.clear();
}

InternalTransition::InternalTransition(State* p_state,
                                       Event* p_event,
                                       Guard p_guard,
                                       Action p_action)
    : Transition(p_state,
                 p_state,
                 p_event,
                 std::move(p_guard),
                 std::move(p_action))
{
    m_deactivate.clear();
    m_activate.clear();
}

bool InternalTransition::execute(Event* p_event,
                                 Metadata& p_data,
                                 Parameter& p_param)
{
    if (m_event != nullptr && !m_event->equals(p_event, p_data, p_param))
    {
        return false;
    }
    if (m_event != nullptr && p_event == nullptr)
    {
        return false;
    }
    if (!allowed(p_data, p_param))
    {
        return false;
    }
    if (m_action)
    {
        m_action(p_data, p_param);
    }
    return true;
}

bool InternalTransition::allowed(Metadata& p_data, Parameter& p_param)
{
    if (m_guard && !m_guard(p_data, p_param))
    {
        return false;
    }
    return true;
}

} // namespace statechart
