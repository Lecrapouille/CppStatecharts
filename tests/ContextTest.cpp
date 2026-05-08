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

#include "Statechart/Context.hpp"
#include "Helpers.hpp"
#include "Statechart/HierarchicalState.hpp"
#include "Statechart/Metadata.hpp"
#include "Statechart/PseudoState.hpp"
#include "Statechart/Statechart.hpp"

#include <gtest/gtest.h>

namespace statechart::tests {

// WHEN a Context (HierarchicalState) is created
// EXPECT it to have the provided name
TEST(ContextTest, Constructor)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* context =
        chart->create<HierarchicalState>("ContextState", chart.get());

    EXPECT_EQ(context->name(), "ContextState");
    EXPECT_EQ(context->toString(), "ContextState");
}

// WHEN a Context has a start state assigned
// EXPECT startState() to return it
TEST(ContextTest, StartState)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* context =
        chart->create<HierarchicalState>("ContextState", chart.get());
    auto* start =
        chart->create<PseudoState>("Start", context, PseudoStateType::Start);

    EXPECT_EQ(context->startState(), start);
}

// WHEN a Context has no start state
// EXPECT startState() to return nullptr
TEST(ContextTest, NoStartState)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* context =
        chart->create<HierarchicalState>("ContextState", chart.get());

    EXPECT_EQ(context->startState(), nullptr);
}

// WHEN a Context has child states
// EXPECT the children to reference the context as parent
TEST(ContextTest, ChildStates)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* context = chart->create<HierarchicalState>("Parent", chart.get());
    auto* child1 = chart->create<State>("Child1", context);
    auto* child2 = chart->create<State>("Child2", context);

    EXPECT_EQ(child1->context(), context);
    EXPECT_EQ(child2->context(), context);
}

// WHEN a Context has entry and exit actions
// EXPECT the actions to be executed on activation/deactivation
TEST(ContextTest, EntryAndExitActions)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    TestParameter param;
    Metadata data;

    auto* context =
        chart->create<HierarchicalState>("ContextState",
                                         chart.get(),
                                         makeTestAction("entry", "E"),
                                         Action{},
                                         makeTestAction("exit", "X"));

    context->activate(data, param);
    EXPECT_NE(param.path.find("E:entry"), std::string::npos);

    context->deactivate(data, param);
    EXPECT_NE(param.path.find("X:exit"), std::string::npos);
}

// WHEN a Context name is changed
// EXPECT the new name to be reflected
TEST(ContextTest, SetName)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* context = chart->create<HierarchicalState>("OldName", chart.get());

    EXPECT_EQ(context->name(), "OldName");

    context->setName("NewName");

    EXPECT_EQ(context->name(), "NewName");
}

// WHEN a Context is polymorphically used as a State
// EXPECT dynamic_cast to work correctly
TEST(ContextTest, PolymorphicBehavior)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* context =
        chart->create<HierarchicalState>("ContextState", chart.get());

    State* statePtr = context;

    EXPECT_EQ(statePtr->name(), "ContextState");

    Context* contextPtr = dynamic_cast<Context*>(statePtr);
    EXPECT_NE(contextPtr, nullptr);
}

// WHEN a Context belongs to a Statechart
// EXPECT statechart() to return the owning chart
TEST(ContextTest, OwningStatechart)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* context =
        chart->create<HierarchicalState>("ContextState", chart.get());

    EXPECT_EQ(context->statechart(), chart.get());
}

// WHEN a Context has a parent context
// EXPECT context() to return the parent
TEST(ContextTest, ParentContext)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* context =
        chart->create<HierarchicalState>("ContextState", chart.get());

    EXPECT_EQ(context->context(), chart.get());
}

// WHEN nested Contexts are created
// EXPECT the hierarchy to be maintained
TEST(ContextTest, NestedContexts)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* level1 = chart->create<HierarchicalState>("Level1", chart.get());
    auto* level2 = chart->create<HierarchicalState>("Level2", level1);
    auto* level3 = chart->create<HierarchicalState>("Level3", level2);

    EXPECT_EQ(level1->context(), chart.get());
    EXPECT_EQ(level2->context(), level1);
    EXPECT_EQ(level3->context(), level2);
}

// WHEN a Context is activated
// EXPECT activation to succeed without errors
TEST(ContextTest, Activation)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* context =
        chart->create<HierarchicalState>("ContextState", chart.get());

    TestParameter param;
    Metadata data;

    EXPECT_NO_THROW(context->activate(data, param));
}

// WHEN a Context is deactivated
// EXPECT deactivation to succeed without errors
TEST(ContextTest, Deactivation)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* context =
        chart->create<HierarchicalState>("ContextState", chart.get());

    TestParameter param;
    Metadata data;

    context->activate(data, param);
    EXPECT_NO_THROW(context->deactivate(data, param));
}

// WHEN a Context with multiple children is created
// EXPECT each child to be independent
TEST(ContextTest, MultipleChildren)
{
    auto chart = std::make_unique<Statechart>("test", 10, false);
    auto* context = chart->create<HierarchicalState>("Parent", chart.get());

    auto* child1 = chart->create<State>("Child1", context);
    auto* child2 = chart->create<State>("Child2", context);
    auto* child3 = chart->create<State>("Child3", context);

    EXPECT_NE(child1, child2);
    EXPECT_NE(child2, child3);
    EXPECT_NE(child1, child3);

    EXPECT_EQ(child1->context(), context);
    EXPECT_EQ(child2->context(), context);
    EXPECT_EQ(child3->context(), context);
}

} // namespace statechart::tests
