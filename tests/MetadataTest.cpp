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

#include "Statechart/Metadata.hpp"
#include "Helpers.hpp"
#include "Statechart/State.hpp"
#include "Statechart/Statechart.hpp"

#include <gtest/gtest.h>

namespace statechart::tests {

// WHEN a Metadata object is created
// EXPECT it to be in a clean state
TEST(MetadataTest, DefaultConstructor)
{
    Metadata data;

    EXPECT_FALSE(data.isRunning());
    EXPECT_TRUE(data.getActiveStates().empty());
}

// WHEN a State is activated in the Metadata
// EXPECT isActive to return true for that state
TEST(MetadataTest, ActivateState)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("TestState", chart.get());

    Metadata data;
    TestParameter param;

    state->activate(data, param);

    EXPECT_TRUE(data.isActive(state));
    EXPECT_TRUE(data.isActive("TestState"));
}

// WHEN a State is deactivated in the Metadata
// EXPECT isActive to return false for that state
TEST(MetadataTest, DeactivateState)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("TestState", chart.get());

    Metadata data;
    TestParameter param;

    state->activate(data, param);
    EXPECT_TRUE(data.isActive(state));

    state->deactivate(data, param);
    EXPECT_FALSE(data.isActive(state));
}

// WHEN multiple States are activated
// EXPECT getActiveStates to return all of them
TEST(MetadataTest, MultipleActiveStates)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state1 = chart->create<State>("State1", chart.get());
    auto* state2 = chart->create<State>("State2", chart.get());
    auto* state3 = chart->create<State>("State3", chart.get());

    Metadata data;
    TestParameter param;

    state1->activate(data, param);
    state2->activate(data, param);
    state3->activate(data, param);

    auto activeStates = data.getActiveStates();

    EXPECT_EQ(activeStates.size(), 3u);
    EXPECT_NE(activeStates.find(state1), activeStates.end());
    EXPECT_NE(activeStates.find(state2), activeStates.end());
    EXPECT_NE(activeStates.find(state3), activeStates.end());
}

// WHEN Metadata is reset
// EXPECT all active states to be cleared
TEST(MetadataTest, Reset)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state1 = chart->create<State>("State1", chart.get());
    auto* state2 = chart->create<State>("State2", chart.get());

    Metadata data;
    TestParameter param;

    state1->activate(data, param);
    state2->activate(data, param);

    EXPECT_EQ(data.getActiveStates().size(), 2u);

    data.reset();

    EXPECT_TRUE(data.getActiveStates().empty());
    EXPECT_FALSE(data.isActive(state1));
    EXPECT_FALSE(data.isActive(state2));
}

// WHEN an activate observer is registered
// EXPECT it to be called on state activation
TEST(MetadataTest, ActivateObserver)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("TestState", chart.get());

    Metadata data;
    TestParameter param;

    int observerCallCount = 0;
    State* observedState = nullptr;

    data.addActivateObserver([&](State* s) {
        observerCallCount++;
        observedState = s;
    });

    state->activate(data, param);

    EXPECT_EQ(observerCallCount, 1);
    EXPECT_EQ(observedState, state);
}

// WHEN a deactivate observer is registered
// EXPECT it to be called on state deactivation
TEST(MetadataTest, DeactivateObserver)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("TestState", chart.get());

    Metadata data;
    TestParameter param;

    int observerCallCount = 0;
    State* observedState = nullptr;

    data.addDeactivateObserver([&](State* s) {
        observerCallCount++;
        observedState = s;
    });

    state->activate(data, param);
    state->deactivate(data, param);

    EXPECT_EQ(observerCallCount, 1);
    EXPECT_EQ(observedState, state);
}

// WHEN runtime data is requested for an active state
// EXPECT getData to return non-null
TEST(MetadataTest, GetDataForActiveState)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("TestState", chart.get());

    Metadata data;
    TestParameter param;

    state->activate(data, param);

    auto* runtimeData = data.getData(state);

    EXPECT_NE(runtimeData, nullptr);
}

// WHEN runtime data is requested for an inactive state
// EXPECT getData to return nullptr
TEST(MetadataTest, GetDataForInactiveState)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("TestState", chart.get());

    Metadata data;

    auto* runtimeData = data.getData(state);

    EXPECT_EQ(runtimeData, nullptr);
}

// WHEN isRunning is queried with active states
// EXPECT it to return true
TEST(MetadataTest, IsRunningWithActiveStates)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("TestState", chart.get());

    Metadata data;
    TestParameter param;

    EXPECT_FALSE(data.isRunning());

    state->activate(data, param);

    EXPECT_TRUE(data.isRunning());
}

// WHEN multiple observers are registered
// EXPECT all to be called
TEST(MetadataTest, MultipleObservers)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("TestState", chart.get());

    Metadata data;
    TestParameter param;

    int observer1CallCount = 0;
    int observer2CallCount = 0;
    int observer3CallCount = 0;

    data.addActivateObserver([&](State*) { observer1CallCount++; });
    data.addActivateObserver([&](State*) { observer2CallCount++; });
    data.addActivateObserver([&](State*) { observer3CallCount++; });

    state->activate(data, param);

    EXPECT_EQ(observer1CallCount, 1);
    EXPECT_EQ(observer2CallCount, 1);
    EXPECT_EQ(observer3CallCount, 1);
}

// WHEN isActive is queried for a non-existent state name
// EXPECT an exception to be thrown
TEST(MetadataTest, IsActiveWithInvalidName)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);

    Metadata data;

    EXPECT_THROW(data.isActive("NonExistentState"), StatechartException);
}

} // namespace statechart::tests
