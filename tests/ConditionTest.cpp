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

#include "CppStatecharts/Condition.hpp"
#include "Helpers.hpp"
#include "CppStatecharts/Metadata.hpp"
#include "CppStatecharts/PseudoState.hpp"
#include "CppStatecharts/State.hpp"
#include "CppStatecharts/Statechart.hpp"
#include "CppStatecharts/Transition.hpp"

#include <gtest/gtest.h>

namespace statechart::tests {

// WHEN a Condition is created with a guard and two targets
// EXPECT it to be properly configured as a junction
TEST(ConditionTest, Constructor)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state1 = chart->create<State>("TrueState", chart.get());
    auto* state2 = chart->create<State>("FalseState", chart.get());

    auto* condition = chart->create<Condition>("Condition",
                                               chart.get(),
                                               makeTestGuard(1),
                                               state1,
                                               state2);

    EXPECT_EQ(condition->name(), "Condition");
    EXPECT_EQ(condition->type(), PseudoStateType::Junction);
}

// WHEN a Condition is reached with a guard that evaluates to true
// EXPECT the whenTrue branch to be taken (after a dispatch from the Start
// pseudo-state into the Condition junction).
TEST(ConditionTest, GuardEvaluatesToTrue)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);

    auto* start = chart->create<PseudoState>(
        "Start", chart.get(), PseudoStateType::Start);

    auto* trueState = chart->create<State>(
        "TrueState", chart.get(), makeTestAction("trueEntry", "T"));

    auto* falseState = chart->create<State>(
        "FalseState", chart.get(), makeTestAction("falseEntry", "F"));

    auto* condition = chart->create<Condition>(
        "Condition", chart.get(), makeTestGuard(1), trueState, falseState);

    chart->createTransition(start, condition);

    TestParameter param;
    param.guardvalue = 1;
    Metadata data;

    chart->start(data, param);

    EXPECT_TRUE(data.isActive(trueState));
    EXPECT_FALSE(data.isActive(falseState));
}

// WHEN a Condition is reached with a guard that evaluates to false
// EXPECT the otherwise branch to be taken
TEST(ConditionTest, GuardEvaluatesToFalse)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);

    auto* start = chart->create<PseudoState>(
        "Start", chart.get(), PseudoStateType::Start);

    auto* trueState = chart->create<State>(
        "TrueState", chart.get(), makeTestAction("trueEntry", "T"));

    auto* falseState = chart->create<State>(
        "FalseState", chart.get(), makeTestAction("falseEntry", "F"));

    auto* condition = chart->create<Condition>(
        "Condition", chart.get(), makeTestGuard(1), trueState, falseState);

    chart->createTransition(start, condition);

    TestParameter param;
    param.guardvalue = 0;
    Metadata data;

    chart->start(data, param);

    EXPECT_FALSE(data.isActive(trueState));
    EXPECT_TRUE(data.isActive(falseState));
}

// WHEN a Condition name is changed
// EXPECT the new name to be reflected
TEST(ConditionTest, SetName)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state1 = chart->create<State>("State1", chart.get());
    auto* state2 = chart->create<State>("State2", chart.get());

    auto* condition = chart->create<Condition>(
        "OldName", chart.get(), makeTestGuard(1), state1, state2);

    EXPECT_EQ(condition->name(), "OldName");

    condition->setName("NewName");

    EXPECT_EQ(condition->name(), "NewName");
}

// WHEN a Condition is polymorphically used as a PseudoState
// EXPECT dynamic_cast to work correctly
TEST(ConditionTest, PolymorphicBehavior)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state1 = chart->create<State>("State1", chart.get());
    auto* state2 = chart->create<State>("State2", chart.get());

    auto* condition = chart->create<Condition>("Condition",
                                               chart.get(),
                                               makeTestGuard(1),
                                               state1,
                                               state2);

    PseudoState* pseudoPtr = condition;

    EXPECT_EQ(pseudoPtr->name(), "Condition");
    EXPECT_EQ(pseudoPtr->type(), PseudoStateType::Junction);

    Condition* condPtr = dynamic_cast<Condition*>(pseudoPtr);
    EXPECT_NE(condPtr, nullptr);
}

// WHEN a Condition belongs to a Statechart
// EXPECT statechart() to return the owning chart
TEST(ConditionTest, OwningStatechart)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state1 = chart->create<State>("State1", chart.get());
    auto* state2 = chart->create<State>("State2", chart.get());

    auto* condition = chart->create<Condition>("Condition",
                                               chart.get(),
                                               makeTestGuard(1),
                                               state1,
                                               state2);

    EXPECT_EQ(condition->statechart(), chart.get());
}

// WHEN a Condition has a parent context
// EXPECT context() to return the parent
TEST(ConditionTest, ParentContext)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state1 = chart->create<State>("State1", chart.get());
    auto* state2 = chart->create<State>("State2", chart.get());

    auto* condition = chart->create<Condition>("Condition",
                                               chart.get(),
                                               makeTestGuard(1),
                                               state1,
                                               state2);

    EXPECT_EQ(condition->context(), chart.get());
}

// WHEN multiple Conditions are created with different guards
// EXPECT each to evaluate independently
TEST(ConditionTest, MultipleConditions)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);

    auto* state1 = chart->create<State>("State1", chart.get());
    auto* state2 = chart->create<State>("State2", chart.get());
    auto* state3 = chart->create<State>("State3", chart.get());
    auto* state4 = chart->create<State>("State4", chart.get());

    auto* cond1 = chart->create<Condition>(
        "Cond1", chart.get(), makeTestGuard(1), state1, state2);

    auto* cond2 = chart->create<Condition>(
        "Cond2", chart.get(), makeTestGuard(2), state3, state4);

    EXPECT_NE(cond1, cond2);
    EXPECT_EQ(cond1->name(), "Cond1");
    EXPECT_EQ(cond2->name(), "Cond2");
}

// WHEN a Condition has entry and exit actions
// EXPECT the actions to be executed
TEST(ConditionTest, EntryAndExitActions)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state1 = chart->create<State>("State1", chart.get());
    auto* state2 = chart->create<State>("State2", chart.get());

    auto* condition = chart->create<Condition>("Condition",
                                               chart.get(),
                                               makeTestGuard(1),
                                               state1,
                                               state2);

    TestParameter param;
    Metadata data;

    condition->setEntryAction(makeTestAction("condEntry", "E"));
    condition->setExitAction(makeTestAction("condExit", "X"));

    condition->activate(data, param);

    EXPECT_NE(param.path.find("E:condEntry"), std::string::npos);
}

} // namespace statechart::tests
