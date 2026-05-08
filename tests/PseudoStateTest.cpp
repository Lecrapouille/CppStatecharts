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

#include "CppStatecharts/PseudoState.hpp"
#include "Helpers.hpp"
#include "CppStatecharts/HierarchicalState.hpp"
#include "CppStatecharts/Metadata.hpp"
#include "CppStatecharts/Statechart.hpp"
#include "CppStatecharts/Transition.hpp"

#include <gtest/gtest.h>

namespace statechart::tests {

// WHEN a Start PseudoState is created
// EXPECT its type to be Start
TEST(PseudoStateTest, StartType)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* start = chart->create<PseudoState>(
        "Start", chart.get(), PseudoStateType::Start);

    EXPECT_EQ(start->type(), PseudoStateType::Start);
    EXPECT_EQ(start->name(), "Start");
}

// WHEN a Junction PseudoState is created
// EXPECT its type to be Junction
TEST(PseudoStateTest, JunctionType)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* junction = chart->create<PseudoState>(
        "Junction", chart.get(), PseudoStateType::Junction);

    EXPECT_EQ(junction->type(), PseudoStateType::Junction);
    EXPECT_EQ(junction->name(), "Junction");
}

// WHEN a Fork PseudoState is created
// EXPECT its type to be Fork
TEST(PseudoStateTest, ForkType)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* fork =
        chart->create<PseudoState>("Fork", chart.get(), PseudoStateType::Fork);

    EXPECT_EQ(fork->type(), PseudoStateType::Fork);
    EXPECT_EQ(fork->name(), "Fork");
}

// WHEN a Join PseudoState is created
// EXPECT its type to be Join
TEST(PseudoStateTest, JoinType)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* join =
        chart->create<PseudoState>("Join", chart.get(), PseudoStateType::Join);

    EXPECT_EQ(join->type(), PseudoStateType::Join);
    EXPECT_EQ(join->name(), "Join");
}

// WHEN a History PseudoState is created
// EXPECT its type to be History
TEST(PseudoStateTest, HistoryType)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* hier = chart->create<HierarchicalState>("Hier", chart.get());
    auto* history =
        chart->create<PseudoState>("History", hier, PseudoStateType::History);

    EXPECT_EQ(history->type(), PseudoStateType::History);
    EXPECT_EQ(history->name(), "History");
}

// WHEN a DeepHistory PseudoState is created
// EXPECT its type to be DeepHistory
TEST(PseudoStateTest, DeepHistoryType)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* hier = chart->create<HierarchicalState>("Hier", chart.get());
    auto* deepHistory = chart->create<PseudoState>(
        "DeepHistory", hier, PseudoStateType::DeepHistory);

    EXPECT_EQ(deepHistory->type(), PseudoStateType::DeepHistory);
    EXPECT_EQ(deepHistory->name(), "DeepHistory");
}

// WHEN a Start PseudoState is set as the context's start state
// EXPECT the context to reference it
TEST(PseudoStateTest, StartStateRegistration)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* start = chart->create<PseudoState>(
        "Start", chart.get(), PseudoStateType::Start);

    EXPECT_EQ(chart->startState(), start);
}

// WHEN a PseudoState has entry and exit actions
// EXPECT the actions to be executable
TEST(PseudoStateTest, EntryAndExitActions)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* pseudo = chart->create<PseudoState>(
        "Pseudo", chart.get(), PseudoStateType::Junction);

    TestParameter param;
    Metadata data;

    pseudo->setEntryAction(makeTestAction("entry", "E"));
    pseudo->setExitAction(makeTestAction("exit", "X"));

    pseudo->activate(data, param);

    EXPECT_NE(param.path.find("E:entry"), std::string::npos);
}

// WHEN a PseudoState name is changed
// EXPECT the new name to be reflected
TEST(PseudoStateTest, SetName)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* pseudo = chart->create<PseudoState>(
        "OldName", chart.get(), PseudoStateType::Junction);

    EXPECT_EQ(pseudo->name(), "OldName");

    pseudo->setName("NewName");

    EXPECT_EQ(pseudo->name(), "NewName");
}

// WHEN a PseudoState belongs to a context
// EXPECT context() to return the parent
TEST(PseudoStateTest, ParentContext)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* pseudo = chart->create<PseudoState>(
        "Pseudo", chart.get(), PseudoStateType::Start);

    EXPECT_EQ(pseudo->context(), chart.get());
}

// WHEN a PseudoState belongs to a Statechart
// EXPECT statechart() to return the owning chart
TEST(PseudoStateTest, OwningStatechart)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* pseudo = chart->create<PseudoState>(
        "Pseudo", chart.get(), PseudoStateType::Start);

    EXPECT_EQ(pseudo->statechart(), chart.get());
}

// WHEN multiple PseudoStates of different types are created
// EXPECT each to maintain its type
TEST(PseudoStateTest, MultiplePseudoStates)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);

    auto* start = chart->create<PseudoState>(
        "Start", chart.get(), PseudoStateType::Start);
    auto* junction = chart->create<PseudoState>(
        "Junction", chart.get(), PseudoStateType::Junction);
    auto* fork =
        chart->create<PseudoState>("Fork", chart.get(), PseudoStateType::Fork);

    EXPECT_EQ(start->type(), PseudoStateType::Start);
    EXPECT_EQ(junction->type(), PseudoStateType::Junction);
    EXPECT_EQ(fork->type(), PseudoStateType::Fork);
}

// WHEN a PseudoState is polymorphically used as a State
// EXPECT dynamic_cast to work correctly
TEST(PseudoStateTest, PolymorphicBehavior)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* pseudo = chart->create<PseudoState>(
        "Pseudo", chart.get(), PseudoStateType::Junction);

    State* statePtr = pseudo;

    EXPECT_EQ(statePtr->name(), "Pseudo");

    PseudoState* pseudoPtr = dynamic_cast<PseudoState*>(statePtr);
    EXPECT_NE(pseudoPtr, nullptr);
    EXPECT_EQ(pseudoPtr->type(), PseudoStateType::Junction);
}

} // namespace statechart::tests
