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

#include "CppStatecharts/TimeoutEvent.hpp"
#include "Helpers.hpp"
#include "CppStatecharts/Metadata.hpp"

#include <gtest/gtest.h>

namespace statechart::tests {

// WHEN a TimeoutEvent is created with a timeout value
// EXPECT the timeout to be retrievable via timeout()
TEST(TimeoutEventTest, ConstructorAndTimeout)
{
    TimeoutEvent evt(1000);

    EXPECT_EQ(evt.timeout(), 1000);
}

// WHEN a TimeoutEvent timeout is modified with setTimeout
// EXPECT the new timeout value to be stored
TEST(TimeoutEventTest, SetTimeout)
{
    TimeoutEvent evt(500);

    EXPECT_EQ(evt.timeout(), 500);

    evt.setTimeout(1500);

    EXPECT_EQ(evt.timeout(), 1500);
}

// WHEN two different TimeoutEvent instances are compared
// EXPECT equals to return false (different UUIDs)
TEST(TimeoutEventTest, EqualsDifferentInstances)
{
    TimeoutEvent evt1(1000);
    TimeoutEvent evt2(1000);

    Metadata data;
    TestParameter param;

    EXPECT_FALSE(evt1.equals(&evt2, data, param));
    EXPECT_FALSE(evt2.equals(&evt1, data, param));
}

// WHEN a TimeoutEvent is compared to itself
// EXPECT equals to return true (same UUID)
TEST(TimeoutEventTest, EqualsSameInstance)
{
    TimeoutEvent evt(1000);

    Metadata data;
    TestParameter param;

    EXPECT_TRUE(evt.equals(&evt, data, param));
}

// WHEN a TimeoutEvent is compared to nullptr
// EXPECT equals to return false
TEST(TimeoutEventTest, EqualsNullptr)
{
    TimeoutEvent evt(1000);

    Metadata data;
    TestParameter param;

    EXPECT_FALSE(evt.equals(nullptr, data, param));
}

// WHEN a TimeoutEvent is compared to a different Event type
// EXPECT equals to return false
TEST(TimeoutEventTest, EqualsDifferentEventType)
{
    TimeoutEvent timeoutEvt(1000);
    Event regularEvt("RegularEvent");

    Metadata data;
    TestParameter param;

    EXPECT_FALSE(timeoutEvt.equals(&regularEvt, data, param));
}

// WHEN multiple TimeoutEvents are created with different timeouts
// EXPECT each to have its own unique UUID and timeout value
TEST(TimeoutEventTest, MultipleUniqueInstances)
{
    TimeoutEvent evt1(100);
    TimeoutEvent evt2(200);
    TimeoutEvent evt3(300);

    Metadata data;
    TestParameter param;

    EXPECT_EQ(evt1.timeout(), 100);
    EXPECT_EQ(evt2.timeout(), 200);
    EXPECT_EQ(evt3.timeout(), 300);

    EXPECT_FALSE(evt1.equals(&evt2, data, param));
    EXPECT_FALSE(evt1.equals(&evt3, data, param));
    EXPECT_FALSE(evt2.equals(&evt3, data, param));
}

// WHEN a TimeoutEvent is created with zero timeout
// EXPECT it to be valid with timeout of zero
TEST(TimeoutEventTest, ZeroTimeout)
{
    TimeoutEvent evt(0);

    EXPECT_EQ(evt.timeout(), 0);
}

// WHEN a TimeoutEvent is created with negative timeout
// EXPECT it to store the negative value
TEST(TimeoutEventTest, NegativeTimeout)
{
    TimeoutEvent evt(-100);

    EXPECT_EQ(evt.timeout(), -100);
}

} // namespace statechart::tests
