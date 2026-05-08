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

#include "Statechart/State.hpp"
#include "Helpers.hpp"
#include "Statechart/Metadata.hpp"
#include "Statechart/Statechart.hpp"
#include "TestCharts.hpp"

#include <gtest/gtest.h>

namespace statechart::tests {

// WHEN a State is created with a name
// EXPECT the name to be retrievable via name() and toString()
TEST(StateTest, ConstructorAndName)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("TestState", chart.get());

    EXPECT_EQ(state->name(), "TestState");
    EXPECT_EQ(state->toString(), "TestState");
}

// WHEN a State's name is changed with setName
// EXPECT the new name to be reflected
TEST(StateTest, SetName)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("OldName", chart.get());

    EXPECT_EQ(state->name(), "OldName");

    state->setName("NewName");

    EXPECT_EQ(state->name(), "NewName");
    EXPECT_EQ(state->toString(), "NewName");
}

// WHEN a State is created with entry, do, and exit actions
// EXPECT the actions to be executed in the correct order during activation
TEST(StateTest, ActionsExecutionOrder)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    TestParameter param;

    auto* state = chart->create<State>("ActionState",
                                       chart.get(),
                                       makeTestAction("entry", "E"),
                                       makeTestAction("do", "D"),
                                       makeTestAction("exit", "X"));

    Metadata data;
    state->activate(data, param);

    EXPECT_EQ(param.path, "E:entry D:do");

    state->deactivate(data, param);

    EXPECT_EQ(param.path, "E:entry D:do X:exit");
}

// WHEN entry action is set after State creation
// EXPECT the new entry action to be used
TEST(StateTest, SetEntryAction)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("TestState", chart.get());
    TestParameter param;
    Metadata data;

    state->setEntryAction(makeTestAction("newEntry", "E"));
    state->activate(data, param);

    EXPECT_EQ(param.path, "E:newEntry");
}

// WHEN do action is set after State creation
// EXPECT the new do action to be used
TEST(StateTest, SetDoAction)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("TestState", chart.get());
    TestParameter param;
    Metadata data;

    state->setDoAction(makeTestAction("newDo", "D"));
    state->activate(data, param);

    EXPECT_EQ(param.path, "D:newDo");
}

// WHEN exit action is set after State creation
// EXPECT the new exit action to be used
TEST(StateTest, SetExitAction)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("TestState", chart.get());
    TestParameter param;
    Metadata data;

    state->setExitAction(makeTestAction("newExit", "X"));
    state->activate(data, param);
    state->deactivate(data, param);

    EXPECT_EQ(param.path, "X:newExit");
}

// WHEN a State is activated for the first time
// EXPECT activate to return true
TEST(StateTest, FirstActivationReturnsTrue)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("TestState", chart.get());
    TestParameter param;
    Metadata data;

    bool result = state->activate(data, param);

    EXPECT_TRUE(result);
}

// WHEN a State has a parent context
// EXPECT context() to return the parent
TEST(StateTest, ParentContext)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("TestState", chart.get());

    EXPECT_EQ(state->context(), chart.get());
}

// WHEN a State belongs to a Statechart
// EXPECT statechart() to return the owning chart
TEST(StateTest, OwningStatechart)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("TestState", chart.get());

    EXPECT_EQ(state->statechart(), chart.get());
}

// WHEN a State is created without actions
// EXPECT activation and deactivation to work without errors
TEST(StateTest, NoActionsState)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("EmptyState", chart.get());
    TestParameter param;
    Metadata data;

    EXPECT_NO_THROW({
        state->activate(data, param);
        state->deactivate(data, param);
    });
}

// WHEN a State is deactivated without being activated
// EXPECT no error to occur
TEST(StateTest, DeactivateWithoutActivate)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("TestState", chart.get());
    TestParameter param;
    Metadata data;

    EXPECT_NO_THROW(state->deactivate(data, param));
}

// WHEN multiple States are created in the same Statechart
// EXPECT each to maintain its own identity
TEST(StateTest, MultipleStates)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);

    auto* state1 = chart->create<State>("State1", chart.get());
    auto* state2 = chart->create<State>("State2", chart.get());
    auto* state3 = chart->create<State>("State3", chart.get());

    EXPECT_EQ(state1->name(), "State1");
    EXPECT_EQ(state2->name(), "State2");
    EXPECT_EQ(state3->name(), "State3");

    EXPECT_NE(state1, state2);
    EXPECT_NE(state2, state3);
    EXPECT_NE(state1, state3);
}

// WHEN a State with an entry action is activated multiple times
// EXPECT the entry action to be called each time
TEST(StateTest, MultipleActivations)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    TestParameter param;
    Metadata data;

    int callCount = 0;
    auto* state = chart->create<State>(
        "TestState", chart.get(), [&callCount](Metadata&, Parameter&) {
            callCount++;
        });

    state->activate(data, param);
    EXPECT_EQ(callCount, 1);

    state->deactivate(data, param);
    state->activate(data, param);
    EXPECT_EQ(callCount, 2);

    state->deactivate(data, param);
    state->activate(data, param);
    EXPECT_EQ(callCount, 3);
}

} // namespace statechart::tests
