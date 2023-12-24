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

#include "Helpers.hpp"

#include <utility>

namespace statechart::tests {

Action makeTestAction(std::string p_name, std::string p_kind)
{
    return [name = std::move(p_name), kind = std::move(p_kind)](
               Metadata& /*p_data*/, Parameter& p_param) {
        auto& tp = static_cast<TestParameter&>(p_param);
        if (!tp.path.empty())
        {
            tp.path.push_back(' ');
        }
        tp.path.append(kind);
        tp.path.push_back(':');
        tp.path.append(name);
    };
}

Guard makeTestGuard(int p_value)
{
    return [p_value](Metadata const& /*p_data*/, Parameter const& p_param) {
        auto& tp = static_cast<const TestParameter&>(p_param);
        return tp.guardvalue == p_value;
    };
}

TestEvent::TestEvent(int p_value) : Event("TestEvent"), m_value(p_value) {}

bool TestEvent::equals(const Event* p_event,
                       Metadata& /*p_data*/,
                       Parameter& /*p_param*/) const
{
    const auto* other = dynamic_cast<const TestEvent*>(p_event);
    if (other == nullptr)
    {
        return false;
    }
    return other->m_value == m_value;
}

} // namespace statechart::tests
