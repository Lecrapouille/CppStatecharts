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

#include "CppStatecharts/Statechart.hpp"
#include "Helpers.hpp"
#include "CppStatecharts/Metadata.hpp"
#include "CppStatecharts/PseudoState.hpp"
#include "CppStatecharts/State.hpp"
#include "CppStatecharts/StatechartException.hpp"
#include "CppStatecharts/Transition.hpp"

#include <gtest/gtest.h>

namespace statechart::tests {

// WHEN a Statechart is created with a name
// EXPECT the name to be stored correctly
TEST(StatechartTest, Constructor)
{
    auto chart = std::make_unique<Statechart>("TestChart", 10, false);

    EXPECT_EQ(chart->name(), "TestChart");
    EXPECT_EQ(chart->toString(), "TestChart");
}

// WHEN a State is created within a Statechart
// EXPECT it to be retrievable by name
TEST(StatechartTest, CreateAndRetrieveState)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);

    auto* state = chart->create<State>("TestState", chart.get());

    EXPECT_NE(state, nullptr);
    EXPECT_EQ(state->name(), "TestState");

    auto* retrieved = chart->getStateByName("TestState");
    EXPECT_EQ(retrieved, state);
}

// WHEN a State with a duplicate name is created
// EXPECT a StatechartException to be thrown
TEST(StatechartTest, DuplicateStateName)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);

    chart->create<State>("DuplicateName", chart.get());

    EXPECT_THROW(chart->create<State>("DuplicateName", chart.get()),
                 StatechartException);
}

// WHEN a State is created with an empty name
// EXPECT a StatechartException to be thrown
TEST(StatechartTest, EmptyStateName)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);

    EXPECT_THROW(chart->create<State>("", chart.get()), StatechartException);
}

// WHEN getStateByName is called with a non-existent name
// EXPECT a StatechartException to be thrown
TEST(StatechartTest, GetNonExistentState)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);

    EXPECT_THROW(chart->getStateByName("NonExistent"), StatechartException);
}

// WHEN a Transition is created between two states
// EXPECT it to be properly configured
TEST(StatechartTest, CreateTransition)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state1 = chart->create<State>("State1", chart.get());
    auto* state2 = chart->create<State>("State2", chart.get());

    auto* transition = chart->createTransition(state1, state2);

    EXPECT_NE(transition, nullptr);
}

// WHEN an Event is created within a Statechart
// EXPECT it to be properly allocated
TEST(StatechartTest, CreateEvent)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);

    auto* evt = chart->createEvent<TestEvent>(42);

    EXPECT_NE(evt, nullptr);
    EXPECT_EQ(evt->value(), 42);
}

// WHEN an InternalTransition is created
// EXPECT it to be properly allocated
TEST(StatechartTest, CreateInternalTransition)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* state = chart->create<State>("State", chart.get());
    auto* evt = chart->createEvent<TestEvent>(1);

    auto* internalTrans = chart->createInternalTransition(
        state, evt, makeTestAction("action", "A"));

    EXPECT_NE(internalTrans, nullptr);
    EXPECT_TRUE(internalTrans->hasEvent());
    EXPECT_TRUE(internalTrans->hasAction());
}

// WHEN a Statechart is started
// EXPECT the initial state to be activated
TEST(StatechartTest, Start)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* start = chart->create<PseudoState>(
        "Start", chart.get(), PseudoStateType::Start);
    auto* state = chart->create<State>("InitialState", chart.get());

    chart->createTransition(start, state);

    Metadata data;
    TestParameter param;

    bool started = chart->start(data, param);

    EXPECT_TRUE(started);
    EXPECT_TRUE(data.isActive(state));
}

// WHEN shutdown is called on a Statechart
// EXPECT isShutdown to return true
TEST(StatechartTest, Shutdown)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);

    EXPECT_FALSE(chart->isShutdown());

    chart->shutdown();

    EXPECT_TRUE(chart->isShutdown());
}

// WHEN an event is dispatched to a Statechart
// EXPECT the appropriate transition to fire
TEST(StatechartTest, DispatchEvent)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);

    auto* start = chart->create<PseudoState>(
        "Start", chart.get(), PseudoStateType::Start);
    auto* state1 = chart->create<State>("State1", chart.get());
    auto* state2 = chart->create<State>("State2", chart.get());

    auto* evt = chart->createEvent<TestEvent>(1);

    chart->createTransition(start, state1);
    chart->createTransition(state1, state2, evt);

    Metadata data;
    TestParameter param;

    chart->start(data, param);
    EXPECT_TRUE(data.isActive(state1));
    EXPECT_FALSE(data.isActive(state2));

    TestEvent dispatchEvt(1);
    bool dispatched = chart->dispatch(data, &dispatchEvt, param);

    EXPECT_TRUE(dispatched);
    EXPECT_FALSE(data.isActive(state1));
    EXPECT_TRUE(data.isActive(state2));
}

// WHEN multiple States are created in a Statechart
// EXPECT each to be independent and retrievable
TEST(StatechartTest, MultipleStates)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);

    auto* state1 = chart->create<State>("State1", chart.get());
    auto* state2 = chart->create<State>("State2", chart.get());
    auto* state3 = chart->create<State>("State3", chart.get());

    EXPECT_EQ(chart->getStateByName("State1"), state1);
    EXPECT_EQ(chart->getStateByName("State2"), state2);
    EXPECT_EQ(chart->getStateByName("State3"), state3);
}

// WHEN a Statechart has a startState
// EXPECT startState() to return it
TEST(StatechartTest, StartStateAccess)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* start = chart->create<PseudoState>(
        "Start", chart.get(), PseudoStateType::Start);

    EXPECT_EQ(chart->startState(), start);
}

// WHEN a Statechart is used as a Context
// EXPECT context() to return nullptr (root context)
TEST(StatechartTest, RootContext)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);

    EXPECT_EQ(chart->context(), nullptr);
}

// WHEN a Statechart is used as a State
// EXPECT statechart() to return itself (it is the root and registers
// itself as the back-pointer so child states can navigate up).
TEST(StatechartTest, RootStatechart)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);

    EXPECT_EQ(chart->statechart(), chart.get());
}

// WHEN VERSION is accessed
// EXPECT it to have a valid version string
TEST(StatechartTest, Version)
{
    EXPECT_FALSE(std::string(Statechart::VERSION).empty());
    EXPECT_EQ(std::string(Statechart::VERSION), "1.2.0");
}

// WHEN shutdown is called multiple times
// EXPECT it to be idempotent
TEST(StatechartTest, MultipleShutdowns)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);

    chart->shutdown();
    EXPECT_TRUE(chart->isShutdown());

    chart->shutdown();
    EXPECT_TRUE(chart->isShutdown());
}

// WHEN three states are connected by trigger-less transitions in a cycle
// AND the chart is started
// EXPECT the dispatch loop to abort with an InfiniteLoopException once the
//        configured threshold is reached.
TEST(StatechartTest, InfiniteLoopDetection)
{
    auto chart = std::make_unique<Statechart>("loopy", 1, false);
    chart->setInfiniteLoopThreshold(5);

    auto* startState = chart->create<PseudoState>(
        "Start", chart.get(), PseudoStateType::Start);
    auto* a = chart->create<State>("A", chart.get());
    auto* b = chart->create<State>("B", chart.get());
    auto* c = chart->create<State>("C", chart.get());

    chart->createTransition(startState, a);
    chart->createTransition(a, b);
    chart->createTransition(b, c);
    chart->createTransition(c, a);

    Metadata data;
    Parameter param;

    EXPECT_THROW(chart->start(data, param), InfiniteLoopException);
}

// WHEN setInfiniteLoopThreshold(0) is called
// EXPECT it to be clamped up to 1 (otherwise the loop would always abort
//        before any trigger-less transition can fire).
TEST(StatechartTest, InfiniteLoopThresholdLowerBound)
{
    auto chart = std::make_unique<Statechart>("loopy", 1, false);

    chart->setInfiniteLoopThreshold(0);
    EXPECT_EQ(chart->infiniteLoopThreshold(), 1u);
}

} // namespace statechart::tests
