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
#include "Helpers.hpp"
#include "Statechart/Metadata.hpp"
#include "Statechart/State.hpp"
#include "Statechart/Statechart.hpp"

#include <gtest/gtest.h>

namespace statechart::tests {

// WHEN an InternalTransition is created with event and action
// EXPECT it to be properly configured
TEST(InternalTransitionTest, ConstructorWithEventAndAction)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("State", chart.get());
    auto* evt = chart->createEvent<TestEvent>(1);

    auto* transition = chart->createInternalTransition(
        state, evt, makeTestAction("internal", "I"));

    EXPECT_TRUE(transition->hasEvent());
    EXPECT_TRUE(transition->hasAction());
    EXPECT_FALSE(transition->hasGuard());
}

// WHEN an InternalTransition is created with event, guard, and action
// EXPECT all properties to be present
TEST(InternalTransitionTest, ConstructorWithEventGuardAndAction)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("State", chart.get());
    auto* evt = chart->createEvent<TestEvent>(1);

    auto* transition = chart->createInternalTransition(
        state, evt, makeTestGuard(1), makeTestAction("internal", "I"));

    EXPECT_TRUE(transition->hasEvent());
    EXPECT_TRUE(transition->hasGuard());
    EXPECT_TRUE(transition->hasAction());
}

// WHEN an InternalTransition is executed
// EXPECT the action to be invoked without state exit/entry
TEST(InternalTransitionTest, ActionExecutionWithoutStateChange)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    TestParameter param;
    Metadata data;

    auto* state = chart->create<State>("State",
                                       chart.get(),
                                       makeTestAction("entry", "E"),
                                       Action{},
                                       makeTestAction("exit", "X"));

    auto* evt = chart->createEvent<TestEvent>(1);
    auto* transition = chart->createInternalTransition(
        state, evt, makeTestAction("internal", "I"));

    state->activate(data, param);

    std::string pathBeforeTransition = param.path;

    TestEvent triggerEvt(1);
    transition->execute(&triggerEvt, data, param);

    EXPECT_NE(param.path.find("E:entry"), std::string::npos);
    EXPECT_NE(param.path.find("I:internal"), std::string::npos);
    EXPECT_EQ(param.path.find("X:exit"), std::string::npos);
}

// WHEN an InternalTransition with a guard is checked for allowed
// AND the guard evaluates to true
// EXPECT allowed to return true
TEST(InternalTransitionTest, GuardAllowsTransition)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("State", chart.get());
    auto* evt = chart->createEvent<TestEvent>(1);

    TestParameter param;
    param.guardvalue = 1;
    Metadata data;

    auto* transition = chart->createInternalTransition(
        state, evt, makeTestGuard(1), makeTestAction("internal", "I"));

    EXPECT_TRUE(transition->allowed(data, param));
}

// WHEN an InternalTransition with a guard is checked for allowed
// AND the guard evaluates to false
// EXPECT allowed to return false
TEST(InternalTransitionTest, GuardBlocksTransition)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("State", chart.get());
    auto* evt = chart->createEvent<TestEvent>(1);

    TestParameter param;
    param.guardvalue = 0;
    Metadata data;

    auto* transition = chart->createInternalTransition(
        state, evt, makeTestGuard(1), makeTestAction("internal", "I"));

    EXPECT_FALSE(transition->allowed(data, param));
}

// WHEN an InternalTransition is executed with matching event
// EXPECT the transition to fire and return true
TEST(InternalTransitionTest, ExecuteWithMatchingEvent)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("State", chart.get());
    auto* evt = chart->createEvent<TestEvent>(1);

    TestParameter param;
    Metadata data;

    auto* transition = chart->createInternalTransition(
        state, evt, makeTestAction("internal", "I"));

    state->activate(data, param);

    TestEvent triggerEvt(1);
    bool executed = transition->execute(&triggerEvt, data, param);

    EXPECT_TRUE(executed);
    EXPECT_NE(param.path.find("I:internal"), std::string::npos);
}

// WHEN an InternalTransition is executed with non-matching event
// EXPECT the transition not to fire
TEST(InternalTransitionTest, ExecuteWithNonMatchingEvent)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("State", chart.get());
    auto* evt = chart->createEvent<TestEvent>(1);

    TestParameter param;
    Metadata data;

    auto* transition = chart->createInternalTransition(
        state, evt, makeTestAction("internal", "I"));

    state->activate(data, param);

    std::string pathBeforeTransition = param.path;

    TestEvent differentEvt(2);
    bool executed = transition->execute(&differentEvt, data, param);

    EXPECT_FALSE(executed);
    EXPECT_EQ(param.path, pathBeforeTransition);
}

// WHEN multiple InternalTransitions are created for the same state
// EXPECT each to be independent
TEST(InternalTransitionTest, MultipleInternalTransitions)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("State", chart.get());
    auto* evt1 = chart->createEvent<TestEvent>(1);
    auto* evt2 = chart->createEvent<TestEvent>(2);

    auto* trans1 = chart->createInternalTransition(
        state, evt1, makeTestAction("action1", "A"));
    auto* trans2 = chart->createInternalTransition(
        state, evt2, makeTestAction("action2", "B"));

    EXPECT_NE(trans1, trans2);
    EXPECT_TRUE(trans1->hasEvent());
    EXPECT_TRUE(trans2->hasEvent());
}

// WHEN an InternalTransition is polymorphically used as a Transition
// EXPECT dynamic_cast to work correctly
TEST(InternalTransitionTest, PolymorphicBehavior)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("State", chart.get());
    auto* evt = chart->createEvent<TestEvent>(1);

    auto* internalTrans = chart->createInternalTransition(
        state, evt, makeTestAction("internal", "I"));

    Transition* transPtr = internalTrans;

    EXPECT_TRUE(transPtr->hasEvent());

    InternalTransition* internalPtr =
        dynamic_cast<InternalTransition*>(transPtr);
    EXPECT_NE(internalPtr, nullptr);
}

// WHEN an InternalTransition action is invoked
// EXPECT it to execute in the context of the same state
TEST(InternalTransitionTest, ActionExecutesInSameState)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    TestParameter param;
    Metadata data;

    int entryCount = 0;
    int exitCount = 0;
    int actionCount = 0;

    auto* state = chart->create<State>(
        "State",
        chart.get(),
        [&entryCount](Metadata&, Parameter&) { entryCount++; },
        Action{},
        [&exitCount](Metadata&, Parameter&) { exitCount++; });

    auto* evt = chart->createEvent<TestEvent>(1);
    auto* transition = chart->createInternalTransition(
        state, evt, [&actionCount](Metadata&, Parameter&) { actionCount++; });

    state->activate(data, param);
    EXPECT_EQ(entryCount, 1);
    EXPECT_EQ(exitCount, 0);
    EXPECT_EQ(actionCount, 0);

    TestEvent triggerEvt(1);
    transition->execute(&triggerEvt, data, param);

    EXPECT_EQ(entryCount, 1);
    EXPECT_EQ(exitCount, 0);
    EXPECT_EQ(actionCount, 1);
}

} // namespace statechart::tests
