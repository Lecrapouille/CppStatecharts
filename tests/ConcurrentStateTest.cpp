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

#include "Statechart/ConcurrentState.hpp"
#include "Helpers.hpp"
#include "Statechart/HierarchicalState.hpp"
#include "Statechart/Metadata.hpp"
#include "Statechart/Statechart.hpp"

#include <gtest/gtest.h>

namespace statechart::tests {

// WHEN a ConcurrentState is created
// EXPECT it to have the provided name
TEST(ConcurrentStateTest, Constructor)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* concurrent = chart->create<ConcurrentState>("ConcState", chart.get());

    EXPECT_EQ(concurrent->name(), "ConcState");
    EXPECT_EQ(concurrent->toString(), "ConcState");
}

// WHEN a ConcurrentState has regions added
// EXPECT regions() to return them
TEST(ConcurrentStateTest, AddRegions)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* concurrent = chart->create<ConcurrentState>("ConcState", chart.get());
    auto* region1 = chart->create<HierarchicalState>("Region1", concurrent);
    auto* region2 = chart->create<HierarchicalState>("Region2", concurrent);

    const auto& regions = concurrent->regions();

    EXPECT_EQ(regions.size(), 2u);
    EXPECT_NE(std::find(regions.begin(), regions.end(), region1),
              regions.end());
    EXPECT_NE(std::find(regions.begin(), regions.end(), region2),
              regions.end());
}

// WHEN a ConcurrentState has no regions
// EXPECT regions() to return an empty vector
TEST(ConcurrentStateTest, NoRegions)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* concurrent = chart->create<ConcurrentState>("ConcState", chart.get());

    const auto& regions = concurrent->regions();

    EXPECT_TRUE(regions.empty());
}

// WHEN a ConcurrentState has entry and exit actions
// EXPECT the actions to be executed on activation/deactivation
TEST(ConcurrentStateTest, EntryAndExitActions)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    TestParameter param;
    Metadata data;

    auto* concurrent =
        chart->create<ConcurrentState>("ConcState",
                                       chart.get(),
                                       makeTestAction("entry", "E"),
                                       Action{},
                                       makeTestAction("exit", "X"));

    concurrent->activate(data, param);
    EXPECT_NE(param.path.find("E:entry"), std::string::npos);

    concurrent->deactivate(data, param);
    EXPECT_NE(param.path.find("X:exit"), std::string::npos);
}

// WHEN a ConcurrentState name is changed
// EXPECT the new name to be reflected
TEST(ConcurrentStateTest, SetName)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* concurrent = chart->create<ConcurrentState>("OldName", chart.get());

    EXPECT_EQ(concurrent->name(), "OldName");

    concurrent->setName("NewName");

    EXPECT_EQ(concurrent->name(), "NewName");
}

// WHEN a ConcurrentState is polymorphically used as a State
// EXPECT dynamic_cast to work correctly
TEST(ConcurrentStateTest, PolymorphicBehavior)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* concurrent = chart->create<ConcurrentState>("ConcState", chart.get());

    State* statePtr = concurrent;

    EXPECT_EQ(statePtr->name(), "ConcState");

    ConcurrentState* concPtr = dynamic_cast<ConcurrentState*>(statePtr);
    EXPECT_NE(concPtr, nullptr);
}

// WHEN a ConcurrentState belongs to a Statechart
// EXPECT statechart() to return the owning chart
TEST(ConcurrentStateTest, OwningStatechart)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* concurrent = chart->create<ConcurrentState>("ConcState", chart.get());

    EXPECT_EQ(concurrent->statechart(), chart.get());
}

// WHEN a ConcurrentState has a parent context
// EXPECT context() to return the parent
TEST(ConcurrentStateTest, ParentContext)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* concurrent = chart->create<ConcurrentState>("ConcState", chart.get());

    EXPECT_EQ(concurrent->context(), chart.get());
}

// WHEN multiple regions are added to a ConcurrentState
// EXPECT each region to have the ConcurrentState as parent
TEST(ConcurrentStateTest, RegionParenthood)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* concurrent = chart->create<ConcurrentState>("ConcState", chart.get());
    auto* region1 = chart->create<HierarchicalState>("Region1", concurrent);
    auto* region2 = chart->create<HierarchicalState>("Region2", concurrent);
    auto* region3 = chart->create<HierarchicalState>("Region3", concurrent);

    EXPECT_EQ(region1->context(), concurrent);
    EXPECT_EQ(region2->context(), concurrent);
    EXPECT_EQ(region3->context(), concurrent);

    const auto& regions = concurrent->regions();
    EXPECT_EQ(regions.size(), 3u);
}

// WHEN a ConcurrentState has a start state
// EXPECT startState() to return it
TEST(ConcurrentStateTest, StartState)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* concurrent = chart->create<ConcurrentState>("ConcState", chart.get());
    auto* start =
        chart->create<PseudoState>("Start", concurrent, PseudoStateType::Start);

    EXPECT_EQ(concurrent->startState(), start);
}

// WHEN a ConcurrentState is activated
// EXPECT activation to succeed without errors
TEST(ConcurrentStateTest, Activation)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* concurrent = chart->create<ConcurrentState>("ConcState", chart.get());

    TestParameter param;
    Metadata data;

    EXPECT_NO_THROW(concurrent->activate(data, param));
}

// WHEN a ConcurrentState is deactivated
// EXPECT deactivation to succeed without errors
TEST(ConcurrentStateTest, Deactivation)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* concurrent = chart->create<ConcurrentState>("ConcState", chart.get());

    TestParameter param;
    Metadata data;

    concurrent->activate(data, param);
    EXPECT_NO_THROW(concurrent->deactivate(data, param));
}

} // namespace statechart::tests
