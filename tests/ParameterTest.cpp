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

#include "CppStatecharts/Parameter.hpp"
#include "Helpers.hpp"

#include <gtest/gtest.h>

namespace statechart::tests {

// WHEN a Parameter is default-constructed
// EXPECT it to be instantiable as a base class
TEST(ParameterTest, DefaultConstructor)
{
    Parameter param;

    SUCCEED();
}

// WHEN a custom Parameter subclass is created
// EXPECT it to be usable polymorphically
TEST(ParameterTest, PolymorphicUsage)
{
    TestParameter testParam;
    testParam.path = "TestPath";
    testParam.guardvalue = 42;

    Parameter* basePtr = &testParam;
    auto* derivedPtr = dynamic_cast<TestParameter*>(basePtr);

    ASSERT_NE(derivedPtr, nullptr);
    EXPECT_EQ(derivedPtr->path, "TestPath");
    EXPECT_EQ(derivedPtr->guardvalue, 42);
}

// WHEN a Parameter is copied
// EXPECT the copy to be independent
TEST(ParameterTest, CopyConstructor)
{
    TestParameter param1;
    param1.path = "OriginalPath";
    param1.guardvalue = 10;

    TestParameter param2(param1);

    EXPECT_EQ(param2.path, "OriginalPath");
    EXPECT_EQ(param2.guardvalue, 10);

    param2.path = "ModifiedPath";
    EXPECT_EQ(param1.path, "OriginalPath");
}

// WHEN a Parameter is assigned
// EXPECT the assignment to copy the data
TEST(ParameterTest, CopyAssignment)
{
    TestParameter param1;
    param1.path = "FirstPath";
    param1.guardvalue = 5;

    TestParameter param2;
    param2.path = "SecondPath";
    param2.guardvalue = 15;

    param2 = param1;

    EXPECT_EQ(param2.path, "FirstPath");
    EXPECT_EQ(param2.guardvalue, 5);
}

// WHEN a Parameter is move-constructed
// EXPECT the moved-from object to be in valid state
TEST(ParameterTest, MoveConstructor)
{
    TestParameter param1;
    param1.path = "MovablePath";
    param1.guardvalue = 20;

    TestParameter param2(std::move(param1));

    EXPECT_EQ(param2.path, "MovablePath");
    EXPECT_EQ(param2.guardvalue, 20);
}

// WHEN a Parameter is move-assigned
// EXPECT the target to receive the moved data
TEST(ParameterTest, MoveAssignment)
{
    TestParameter param1;
    param1.path = "SourcePath";
    param1.guardvalue = 30;

    TestParameter param2;
    param2 = std::move(param1);

    EXPECT_EQ(param2.path, "SourcePath");
    EXPECT_EQ(param2.guardvalue, 30);
}

// WHEN TestParameter fields are modified
// EXPECT the changes to be reflected correctly
TEST(ParameterTest, FieldModification)
{
    TestParameter param;

    EXPECT_TRUE(param.path.empty());
    EXPECT_EQ(param.guardvalue, 0);

    param.path = "NewPath";
    param.guardvalue = 100;

    EXPECT_EQ(param.path, "NewPath");
    EXPECT_EQ(param.guardvalue, 100);
}

} // namespace statechart::tests
