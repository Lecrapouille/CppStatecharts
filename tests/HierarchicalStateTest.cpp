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

#include "CppStatecharts/HierarchicalState.hpp"
#include "Helpers.hpp"
#include "CppStatecharts/Metadata.hpp"
#include "CppStatecharts/PseudoState.hpp"
#include "CppStatecharts/Statechart.hpp"

#include <gtest/gtest.h>

namespace statechart::tests {

// WHEN a HierarchicalState is created
// EXPECT it to have the provided name
TEST(HierarchicalStateTest, Constructor)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* hier = chart->create<HierarchicalState>("HierState", chart.get());

    EXPECT_EQ(hier->name(), "HierState");
    EXPECT_EQ(hier->toString(), "HierState");
}

// WHEN a HierarchicalState has a start state
// EXPECT startState() to return it
TEST(HierarchicalStateTest, StartState)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* hier = chart->create<HierarchicalState>("HierState", chart.get());
    auto* start =
        chart->create<PseudoState>("Start", hier, PseudoStateType::Start);

    EXPECT_EQ(hier->startState(), start);
}

// WHEN a HierarchicalState has child states
// EXPECT the hierarchy to be established
TEST(HierarchicalStateTest, ChildStates)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* hier = chart->create<HierarchicalState>("Parent", chart.get());
    auto* child1 = chart->create<State>("Child1", hier);
    auto* child2 = chart->create<State>("Child2", hier);

    EXPECT_EQ(child1->context(), hier);
    EXPECT_EQ(child2->context(), hier);
}

// WHEN a HierarchicalState has entry and exit actions
// EXPECT the actions to be executed on activation/deactivation
TEST(HierarchicalStateTest, EntryAndExitActions)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    TestParameter param;
    Metadata data;

    auto* hier = chart->create<HierarchicalState>("HierState",
                                                  chart.get(),
                                                  makeTestAction("entry", "E"),
                                                  Action{},
                                                  makeTestAction("exit", "X"));

    hier->activate(data, param);
    EXPECT_NE(param.path.find("E:entry"), std::string::npos);

    hier->deactivate(data, param);
    EXPECT_NE(param.path.find("X:exit"), std::string::npos);
}

// WHEN a HierarchicalState has a history pseudo-state
// EXPECT history() to return it
TEST(HierarchicalStateTest, HistoryPseudoState)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* hier = chart->create<HierarchicalState>("HierState", chart.get());
    auto* history =
        chart->create<PseudoState>("History", hier, PseudoStateType::History);

    EXPECT_EQ(hier->history(), history);
}

// WHEN a HierarchicalState has a deep history pseudo-state
// EXPECT history() to return it
TEST(HierarchicalStateTest, DeepHistoryPseudoState)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* hier = chart->create<HierarchicalState>("HierState", chart.get());
    auto* deepHistory = chart->create<PseudoState>(
        "DeepHistory", hier, PseudoStateType::DeepHistory);

    EXPECT_EQ(hier->history(), deepHistory);
}

// WHEN a HierarchicalState name is changed
// EXPECT the new name to be reflected
TEST(HierarchicalStateTest, SetName)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* hier = chart->create<HierarchicalState>("OldName", chart.get());

    EXPECT_EQ(hier->name(), "OldName");

    hier->setName("NewName");

    EXPECT_EQ(hier->name(), "NewName");
}

// WHEN a HierarchicalState is polymorphically used as a State
// EXPECT dynamic_cast to work correctly
TEST(HierarchicalStateTest, PolymorphicBehavior)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* hier = chart->create<HierarchicalState>("HierState", chart.get());

    State* statePtr = hier;

    EXPECT_EQ(statePtr->name(), "HierState");

    HierarchicalState* hierPtr = dynamic_cast<HierarchicalState*>(statePtr);
    EXPECT_NE(hierPtr, nullptr);
}

// WHEN a HierarchicalState belongs to a Statechart
// EXPECT statechart() to return the owning chart
TEST(HierarchicalStateTest, OwningStatechart)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* hier = chart->create<HierarchicalState>("HierState", chart.get());

    EXPECT_EQ(hier->statechart(), chart.get());
}

// WHEN a HierarchicalState has a parent context
// EXPECT context() to return the parent
TEST(HierarchicalStateTest, ParentContext)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* hier = chart->create<HierarchicalState>("HierState", chart.get());

    EXPECT_EQ(hier->context(), chart.get());
}

// WHEN nested HierarchicalStates are created
// EXPECT the hierarchy to be maintained
TEST(HierarchicalStateTest, NestedHierarchy)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* level1 = chart->create<HierarchicalState>("Level1", chart.get());
    auto* level2 = chart->create<HierarchicalState>("Level2", level1);
    auto* level3 = chart->create<HierarchicalState>("Level3", level2);

    EXPECT_EQ(level1->context(), chart.get());
    EXPECT_EQ(level2->context(), level1);
    EXPECT_EQ(level3->context(), level2);
}

// WHEN a HierarchicalState without a history state is queried
// EXPECT history() to return nullptr
TEST(HierarchicalStateTest, NoHistoryState)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* hier = chart->create<HierarchicalState>("HierState", chart.get());

    EXPECT_EQ(hier->history(), nullptr);
}

} // namespace statechart::tests
