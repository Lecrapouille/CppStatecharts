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

#include "CppStatecharts/Event.hpp"
#include "Helpers.hpp"
#include "CppStatecharts/Metadata.hpp"

#include <gtest/gtest.h>

namespace statechart::tests {

// WHEN an Event is created with an identifier
// EXPECT the identifier to be retrievable via id() and toString()
TEST(EventTest, ConstructorAndAccessors)
{
    Event evt("TestEvent");

    EXPECT_EQ(evt.id(), "TestEvent");
    EXPECT_EQ(evt.toString(), "TestEvent");
}

// WHEN an Event is default-constructed
// EXPECT it to have an empty identifier
TEST(EventTest, DefaultConstructor)
{
    Event evt;

    EXPECT_TRUE(evt.id().empty());
    EXPECT_TRUE(evt.toString().empty());
}

// WHEN two Events with the same identifier are compared
// EXPECT equals to return true
TEST(EventTest, EqualsSameIdentifier)
{
    Event evt1("SameID");
    Event evt2("SameID");

    Metadata data;
    TestParameter param;

    EXPECT_TRUE(evt1.equals(&evt2, data, param));
    EXPECT_TRUE(evt2.equals(&evt1, data, param));
}

// WHEN two Events with different identifiers are compared
// EXPECT equals to return false
TEST(EventTest, EqualsDifferentIdentifier)
{
    Event evt1("FirstID");
    Event evt2("SecondID");

    Metadata data;
    TestParameter param;

    EXPECT_FALSE(evt1.equals(&evt2, data, param));
    EXPECT_FALSE(evt2.equals(&evt1, data, param));
}

// WHEN an Event is compared to nullptr
// EXPECT equals to return false
TEST(EventTest, EqualsNullptr)
{
    Event evt("TestEvent");

    Metadata data;
    TestParameter param;

    EXPECT_FALSE(evt.equals(nullptr, data, param));
}

// WHEN an Event is copied
// EXPECT the copy to have the same identifier
TEST(EventTest, CopyConstructor)
{
    Event evt1("OriginalEvent");
    Event evt2(evt1);

    EXPECT_EQ(evt2.id(), "OriginalEvent");
    EXPECT_EQ(evt2.toString(), "OriginalEvent");
}

// WHEN an Event is assigned to another Event
// EXPECT the assigned Event to have the same identifier
TEST(EventTest, CopyAssignment)
{
    Event evt1("OriginalEvent");
    Event evt2("DifferentEvent");

    evt2 = evt1;

    EXPECT_EQ(evt2.id(), "OriginalEvent");
}

// WHEN an Event is move-constructed
// EXPECT the new Event to have the identifier
TEST(EventTest, MoveConstructor)
{
    Event evt1("MovableEvent");
    Event evt2(std::move(evt1));

    EXPECT_EQ(evt2.id(), "MovableEvent");
}

// WHEN an Event is move-assigned
// EXPECT the target Event to have the moved identifier
TEST(EventTest, MoveAssignment)
{
    Event evt1("MovableEvent");
    Event evt2("OtherEvent");

    evt2 = std::move(evt1);

    EXPECT_EQ(evt2.id(), "MovableEvent");
}

} // namespace statechart::tests
