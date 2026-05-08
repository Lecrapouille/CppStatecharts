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

#include "Statechart/FinalState.hpp"
#include "Helpers.hpp"
#include "Statechart/Metadata.hpp"
#include "Statechart/Statechart.hpp"

#include <gtest/gtest.h>

namespace statechart::tests {

// WHEN a FinalState is created
// EXPECT it to have the provided name
TEST(FinalStateTest, Constructor)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* finalState = chart->create<FinalState>("EndState", chart.get());

    EXPECT_EQ(finalState->name(), "EndState");
    EXPECT_EQ(finalState->toString(), "EndState");
}

// WHEN a FinalState is activated
// EXPECT activation to succeed
TEST(FinalStateTest, Activation)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* finalState = chart->create<FinalState>("EndState", chart.get());

    TestParameter param;
    Metadata data;

    EXPECT_NO_THROW(finalState->activate(data, param));
}

// WHEN a FinalState is deactivated
// EXPECT deactivation to succeed
TEST(FinalStateTest, Deactivation)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* finalState = chart->create<FinalState>("EndState", chart.get());

    TestParameter param;
    Metadata data;

    finalState->activate(data, param);
    EXPECT_NO_THROW(finalState->deactivate(data, param));
}

// WHEN a FinalState's name is changed
// EXPECT the new name to be reflected
TEST(FinalStateTest, SetName)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* finalState = chart->create<FinalState>("OldName", chart.get());

    EXPECT_EQ(finalState->name(), "OldName");

    finalState->setName("NewName");

    EXPECT_EQ(finalState->name(), "NewName");
}

// WHEN a FinalState has entry and exit actions
// EXPECT the actions to be executed
TEST(FinalStateTest, EntryAndExitActions)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* finalState = chart->create<FinalState>("EndState", chart.get());

    TestParameter param;
    Metadata data;

    finalState->setEntryAction(makeTestAction("finalEntry", "E"));
    finalState->setExitAction(makeTestAction("finalExit", "X"));

    finalState->activate(data, param);
    EXPECT_EQ(param.path, "E:finalEntry");

    finalState->deactivate(data, param);
    EXPECT_EQ(param.path, "E:finalEntry X:finalExit");
}

// WHEN a FinalState is compared to its parent State class
// EXPECT polymorphism to work correctly
TEST(FinalStateTest, PolymorphicBehavior)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* finalState = chart->create<FinalState>("EndState", chart.get());

    State* statePtr = finalState;

    EXPECT_EQ(statePtr->name(), "EndState");

    FinalState* finalPtr = dynamic_cast<FinalState*>(statePtr);
    EXPECT_NE(finalPtr, nullptr);
}

// WHEN a FinalState has a parent context
// EXPECT context() to return the parent
TEST(FinalStateTest, ParentContext)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* finalState = chart->create<FinalState>("EndState", chart.get());

    EXPECT_EQ(finalState->context(), chart.get());
}

// WHEN a FinalState belongs to a Statechart
// EXPECT statechart() to return the owning chart
TEST(FinalStateTest, OwningStatechart)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* finalState = chart->create<FinalState>("EndState", chart.get());

    EXPECT_EQ(finalState->statechart(), chart.get());
}

// WHEN multiple FinalStates are created
// EXPECT each to be independent
TEST(FinalStateTest, MultipleFinalStates)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);

    auto* final1 = chart->create<FinalState>("End1", chart.get());
    auto* final2 = chart->create<FinalState>("End2", chart.get());

    EXPECT_EQ(final1->name(), "End1");
    EXPECT_EQ(final2->name(), "End2");
    EXPECT_NE(final1, final2);
}

// WHEN a FinalState is activated and deactivated multiple times
// EXPECT no errors to occur
TEST(FinalStateTest, MultipleActivationsDeactivations)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* finalState = chart->create<FinalState>("EndState", chart.get());

    TestParameter param;
    Metadata data;

    for (int i = 0; i < 5; ++i)
    {
        EXPECT_NO_THROW(finalState->activate(data, param));
        EXPECT_NO_THROW(finalState->deactivate(data, param));
    }
}

} // namespace statechart::tests
