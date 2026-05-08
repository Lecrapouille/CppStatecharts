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

#include "CppStatecharts/Transition.hpp"
#include "Helpers.hpp"
#include "CppStatecharts/Metadata.hpp"
#include "CppStatecharts/State.hpp"
#include "CppStatecharts/Statechart.hpp"

#include <gtest/gtest.h>

namespace statechart::tests {

// WHEN a Transition is created between two states
// EXPECT it to link the states correctly
TEST(TransitionTest, BasicConstructor)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state1 = chart->create<State>("State1", chart.get());
    auto* state2 = chart->create<State>("State2", chart.get());

    auto* transition = chart->createTransition(state1, state2);

    EXPECT_FALSE(transition->hasEvent());
    EXPECT_FALSE(transition->hasGuard());
    EXPECT_FALSE(transition->hasAction());
}

// WHEN a Transition is created with an event
// EXPECT hasEvent to return true
TEST(TransitionTest, ConstructorWithEvent)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state1 = chart->create<State>("State1", chart.get());
    auto* state2 = chart->create<State>("State2", chart.get());
    auto* evt = chart->createEvent<TestEvent>(1);

    auto* transition = chart->createTransition(state1, state2, evt);

    EXPECT_TRUE(transition->hasEvent());
    EXPECT_EQ(transition->event(), evt);
    EXPECT_FALSE(transition->hasGuard());
    EXPECT_FALSE(transition->hasAction());
}

// WHEN a Transition is created with a guard
// EXPECT hasGuard to return true
TEST(TransitionTest, ConstructorWithGuard)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state1 = chart->create<State>("State1", chart.get());
    auto* state2 = chart->create<State>("State2", chart.get());

    auto* transition =
        chart->createTransition(state1, state2, makeTestGuard(1));

    EXPECT_FALSE(transition->hasEvent());
    EXPECT_TRUE(transition->hasGuard());
    EXPECT_FALSE(transition->hasAction());
}

// WHEN a Transition is created with an action
// EXPECT hasAction to return true
TEST(TransitionTest, ConstructorWithAction)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state1 = chart->create<State>("State1", chart.get());
    auto* state2 = chart->create<State>("State2", chart.get());

    auto* transition =
        chart->createTransition(state1, state2, makeTestAction("trans", "T"));

    EXPECT_FALSE(transition->hasEvent());
    EXPECT_FALSE(transition->hasGuard());
    EXPECT_TRUE(transition->hasAction());
}

// WHEN a Transition is created with event and guard
// EXPECT both hasEvent and hasGuard to return true
TEST(TransitionTest, ConstructorWithEventAndGuard)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state1 = chart->create<State>("State1", chart.get());
    auto* state2 = chart->create<State>("State2", chart.get());
    auto* evt = chart->createEvent<TestEvent>(1);

    auto* transition =
        chart->createTransition(state1, state2, evt, makeTestGuard(1));

    EXPECT_TRUE(transition->hasEvent());
    EXPECT_TRUE(transition->hasGuard());
    EXPECT_FALSE(transition->hasAction());
}

// WHEN a Transition is created with event and action
// EXPECT both hasEvent and hasAction to return true
TEST(TransitionTest, ConstructorWithEventAndAction)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state1 = chart->create<State>("State1", chart.get());
    auto* state2 = chart->create<State>("State2", chart.get());
    auto* evt = chart->createEvent<TestEvent>(1);

    auto* transition = chart->createTransition(
        state1, state2, evt, makeTestAction("trans", "T"));

    EXPECT_TRUE(transition->hasEvent());
    EXPECT_FALSE(transition->hasGuard());
    EXPECT_TRUE(transition->hasAction());
}

// WHEN a Transition is created with guard and action
// EXPECT both hasGuard and hasAction to return true
TEST(TransitionTest, ConstructorWithGuardAndAction)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state1 = chart->create<State>("State1", chart.get());
    auto* state2 = chart->create<State>("State2", chart.get());

    auto* transition = chart->createTransition(
        state1, state2, makeTestGuard(1), makeTestAction("trans", "T"));

    EXPECT_FALSE(transition->hasEvent());
    EXPECT_TRUE(transition->hasGuard());
    EXPECT_TRUE(transition->hasAction());
}

// WHEN a Transition is created with event, guard, and action
// EXPECT all three properties to be present
TEST(TransitionTest, ConstructorWithEventGuardAndAction)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state1 = chart->create<State>("State1", chart.get());
    auto* state2 = chart->create<State>("State2", chart.get());
    auto* evt = chart->createEvent<TestEvent>(1);

    auto* transition = chart->createTransition(
        state1, state2, evt, makeTestGuard(1), makeTestAction("trans", "T"));

    EXPECT_TRUE(transition->hasEvent());
    EXPECT_TRUE(transition->hasGuard());
    EXPECT_TRUE(transition->hasAction());
    EXPECT_EQ(transition->event(), evt);
}

// WHEN a Transition has an action
// AND the transition is executed
// EXPECT the action to be invoked
TEST(TransitionTest, ActionExecution)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state1 = chart->create<State>("State1", chart.get());
    auto* state2 = chart->create<State>("State2", chart.get());

    TestParameter param;
    Metadata data;

    auto* transition =
        chart->createTransition(state1, state2, makeTestAction("action", "A"));

    state1->activate(data, param);

    bool executed = transition->execute(nullptr, data, param);

    EXPECT_TRUE(executed);
    EXPECT_NE(param.path.find("A:action"), std::string::npos);
}

// WHEN a Transition has a failing guard
// EXPECT allowed to return false
TEST(TransitionTest, GuardFailure)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state1 = chart->create<State>("State1", chart.get());
    auto* state2 = chart->create<State>("State2", chart.get());

    TestParameter param;
    param.guardvalue = 0;
    Metadata data;

    auto* transition =
        chart->createTransition(state1, state2, makeTestGuard(1));

    bool canExecute = transition->allowed(data, param);

    EXPECT_FALSE(canExecute);
}

// WHEN a Transition has a passing guard
// EXPECT allowed to return true
TEST(TransitionTest, GuardSuccess)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state1 = chart->create<State>("State1", chart.get());
    auto* state2 = chart->create<State>("State2", chart.get());

    TestParameter param;
    param.guardvalue = 1;
    Metadata data;

    auto* transition =
        chart->createTransition(state1, state2, makeTestGuard(1));

    bool canExecute = transition->allowed(data, param);

    EXPECT_TRUE(canExecute);
}

// WHEN multiple transitions are created from the same state
// EXPECT each to be independent
TEST(TransitionTest, MultipleTransitionsFromSameState)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state1 = chart->create<State>("State1", chart.get());
    auto* state2 = chart->create<State>("State2", chart.get());
    auto* state3 = chart->create<State>("State3", chart.get());

    auto* trans1 = chart->createTransition(state1, state2);
    auto* trans2 = chart->createTransition(state1, state3);

    EXPECT_NE(trans1, trans2);
}

} // namespace statechart::tests
