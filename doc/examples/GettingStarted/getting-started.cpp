/*
 * UML statechart framework
 * (http://github.com/klangfarbe/UML-Statechart-Framework-for-Java)
 *
 * Copyright (C) 2006-2013 Christian Mocek (christian.mocek@gmail.com)
 * Copyright (c) 2026 Quentin Quadrat (lecrapouille@gmail.com)
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
#include "Statechart/Condition.hpp"
#include "Statechart/FinalState.hpp"
#include "Statechart/HierarchicalState.hpp"
#include "Statechart/Metadata.hpp"
#include "Statechart/Parameter.hpp"
#include "Statechart/PseudoState.hpp"
#include "Statechart/State.hpp"
#include "Statechart/Statechart.hpp"
#include "Statechart/TimeoutEvent.hpp"
#include "Statechart/Transition.hpp"

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

using namespace statechart;

namespace {

/**
 * @brief User-defined parameter holding the integer value the statechart
 *        manipulates. Equivalent to the Java `MyMetadata.value` field.
 */
class MyParameter: public Parameter
{
public:

    int value = 0;
};

/** @brief Domain event corresponding to the README's `anEvent`. */
class AnEvent: public Event
{
public:

    AnEvent() : Event("AnEvent") {}
};

/** @brief Domain event corresponding to the README's `anotherEvent`. */
class AnotherEvent: public Event
{
public:

    AnotherEvent() : Event("AnotherEvent") {}
};

/** @brief Action that sets the user value. */
Action setValue(int p_value)
{
    return [p_value](Metadata& /*p_data*/, Parameter& p_param) noexcept {
        static_cast<MyParameter&>(p_param).value = p_value;
    };
}

/** @brief Action that decrements the user value. */
Action decrementValue()
{
    return [](Metadata& /*p_data*/, Parameter& p_param) noexcept {
        --static_cast<MyParameter&>(p_param).value;
    };
}

/** @brief Action that prints a message to stdout. */
Action print(std::string p_message)
{
    return [msg = std::move(p_message)](Metadata& /*p_data*/,
                                        Parameter& /*p_param*/) noexcept {
        std::cout << msg << '\n';
    };
}

/** @brief Guard that succeeds when the user value equals @p p_value. */
Guard valueEquals(int p_value)
{
    return [p_value](Metadata const& /*p_data*/,
                     Parameter const& p_param) noexcept {
        return static_cast<const MyParameter&>(p_param).value == p_value;
    };
}

} // namespace

int main()
{
    // Top-level statechart with two worker threads (the minimum for async +
    // timeout dispatching).
    auto chart = std::make_unique<Statechart>("example", 2, false);

    // Top-level pseudo-states.
    auto* state_start = chart->create<PseudoState>(
        "start", chart.get(), PseudoStateType::Start);
    auto* state_final = chart->create<FinalState>("final", chart.get());

    // Hierarchical OR-state initialising the user value to 10.
    auto* state_a = chart->create<HierarchicalState>(
        "a", chart.get(), setValue(10), Action{}, Action{});

    // Substates of state_a.
    auto* state_b = chart->create<State>("b", state_a);
    auto* state_j =
        chart->create<PseudoState>("j", state_a, PseudoStateType::Junction);
    auto* state_a_final = chart->create<FinalState>("a_final", state_a);
    auto* state_c = chart->create<ConcurrentState>("c", state_a);

    // Region 1 of state_c.
    auto* state_c_r1 = chart->create<HierarchicalState>("c_r1", state_c);
    auto* state_c_r1_start = chart->create<PseudoState>(
        "c_r1_start", state_c_r1, PseudoStateType::Start);
    auto* state_d = chart->create<State>("d",
                                         state_c_r1,
                                         print("c_r1 active"),
                                         Action{},
                                         print("c_r1 inactive"));

    // Region 2 of state_c.
    auto* state_c_r2 = chart->create<HierarchicalState>("c_r2", state_c);
    auto* state_c_r2_start = chart->create<PseudoState>(
        "c_r2_start", state_c_r2, PseudoStateType::Start);
    auto* state_e =
        chart->create<State>("e", state_c_r2, print("start timeout"));
    auto* state_f = chart->create<State>("f", state_c_r2, decrementValue());

    // Transitions.
    chart->createTransition(state_start, state_b);
    chart->createTransition(state_b, state_c);
    chart->createTransition(state_c_r1_start, state_d);
    chart->createTransition(state_c_r2_start, state_e);
    chart->createTransition(state_e,
                            state_f,
                            chart->createEvent<TimeoutEvent>(1000),
                            print("timeout"));
    chart->createTransition(
        state_f, state_e, chart->createEvent<AnotherEvent>());
    chart->createTransition(state_c, state_j, chart->createEvent<AnEvent>());
    chart->createTransition(state_j, state_b, valueEquals(0));
    chart->createTransition(state_j, state_a_final);
    chart->createTransition(state_a, state_final);

    (void)state_d;

    // Drive the statechart.
    MyParameter parameter;
    Metadata data;

    std::cout << "Starting the statechart...\n";
    chart->start(data, parameter);

    std::cout << "Sleeping 1.5 s so the timeout transition (e -> f) fires.\n";
    std::this_thread::sleep_for(std::chrono::milliseconds{1500});

    std::cout << "Dispatching AnEvent (drives c -> j; value = "
              << parameter.value << ").\n";
    AnEvent ev;
    chart->dispatch(data, &ev, parameter);

    std::cout << "Final value = " << parameter.value << '\n';

    chart->shutdown();
    return 0;
}
