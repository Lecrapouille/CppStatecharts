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

#include "Statechart/Action.hpp"
#include "Statechart/Event.hpp"
#include "Statechart/Guard.hpp"
#include "Statechart/Parameter.hpp"

#include <string>

namespace statechart::tests {

/**
 * @brief Test parameter carrying the dispatch path string and a guard value.
 */
class TestParameter: public Parameter
{
public:

    std::string path;
    int guardvalue = 0;
};

/**
 * @brief Returns an action that appends " <p_kind>:<p_name>" to the
 *        @c TestParameter::path field, mirroring the Java @c TestAction.
 */
Action makeTestAction(std::string p_name, std::string p_kind);

/**
 * @brief Returns a guard that succeeds when @c TestParameter::guardvalue
 *        equals @p p_value.
 */
Guard makeTestGuard(int p_value);

/**
 * @brief Test event matching by integer id, mirroring the Java @c TestEvent.
 */
class TestEvent: public Event
{
public:

    explicit TestEvent(int p_value);

    int value() const
    {
        return m_value;
    }

    bool equals(const Event* p_event,
                Metadata& p_data,
                Parameter& p_param) const override;

private:

    int m_value;
};

} // namespace statechart::tests
