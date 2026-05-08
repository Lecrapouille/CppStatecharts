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

#include "CppStatecharts/StatechartException.hpp"

#include <gtest/gtest.h>

namespace statechart::tests {

// WHEN a StatechartException is thrown with a message
// EXPECT the message to be retrievable via what()
TEST(StatechartExceptionTest, ConstructorWithMessage)
{
    try
    {
        throw StatechartException("Test error message");
    }
    catch (const StatechartException& e)
    {
        EXPECT_STREQ(e.what(), "Test error message");
    }
}

// WHEN a StatechartException is thrown with a C-string
// EXPECT the message to be retrievable via what()
TEST(StatechartExceptionTest, ConstructorWithCString)
{
    try
    {
        throw StatechartException("C-string error");
    }
    catch (const StatechartException& e)
    {
        EXPECT_STREQ(e.what(), "C-string error");
    }
}

// WHEN a StatechartException is thrown with default constructor
// EXPECT a default error message
TEST(StatechartExceptionTest, DefaultConstructor)
{
    try
    {
        throw StatechartException();
    }
    catch (const StatechartException& e)
    {
        EXPECT_STREQ(e.what(), "statechart error");
    }
}

// WHEN a StatechartException is caught as std::runtime_error
// EXPECT polymorphism to work correctly
TEST(StatechartExceptionTest, PolymorphicBehavior)
{
    try
    {
        throw StatechartException("Polymorphic error");
    }
    catch (const std::runtime_error& e)
    {
        EXPECT_STREQ(e.what(), "Polymorphic error");
    }
}

// WHEN a StatechartException is caught as std::exception
// EXPECT base class catching to work
TEST(StatechartExceptionTest, BaseExceptionCatch)
{
    try
    {
        throw StatechartException("Base exception test");
    }
    catch (const std::exception& e)
    {
        EXPECT_STREQ(e.what(), "Base exception test");
    }
}

// WHEN multiple StatechartExceptions are thrown
// EXPECT each to have independent messages
TEST(StatechartExceptionTest, MultipleExceptions)
{
    try
    {
        throw StatechartException("First error");
    }
    catch (const StatechartException& e1)
    {
        EXPECT_STREQ(e1.what(), "First error");

        try
        {
            throw StatechartException("Second error");
        }
        catch (const StatechartException& e2)
        {
            EXPECT_STREQ(e2.what(), "Second error");
            EXPECT_STRNE(e2.what(), e1.what());
        }
    }
}

// WHEN a StatechartException is copied
// EXPECT the copy to have the same message
TEST(StatechartExceptionTest, CopyException)
{
    StatechartException original("Original message");
    StatechartException copy = original;

    EXPECT_STREQ(copy.what(), original.what());
}

// WHEN a StatechartException message contains special characters
// EXPECT the message to be preserved correctly
TEST(StatechartExceptionTest, SpecialCharactersInMessage)
{
    std::string message = "Error: <State> already exists! Use unique name.";

    try
    {
        throw StatechartException(message);
    }
    catch (const StatechartException& e)
    {
        EXPECT_EQ(std::string(e.what()), message);
    }
}

// WHEN an InfiniteLoopException is thrown
// EXPECT it to be catchable as StatechartException via polymorphism
TEST(StatechartExceptionTest, InfiniteLoopExceptionIsStatechartException)
{
    try
    {
        throw InfiniteLoopException("loop detected");
    }
    catch (const StatechartException& e)
    {
        EXPECT_STREQ(e.what(), "loop detected");
    }
}

} // namespace statechart::tests
