/*
 * UML statechart framework (https://github.com/Lecrapouille/CppStatecharts)
 * Copyright (c) 2026 Quentin Quadrat (lecrapouille@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

/**
 * @file triggers.cpp
 * @brief Two outgoing transitions sharing the same event but with different
 *        guards routing to different states.
 *
 * Equivalent statechart (PlantUML):
 *
 *     [*] -> A
 *     A -> B  : e [x == 10]
 *     A --> C : e
 *     A --> D : [x > 10]
 */

#include "Statechart/Event.hpp"
#include "Statechart/Metadata.hpp"
#include "Statechart/Parameter.hpp"
#include "Statechart/PseudoState.hpp"
#include "Statechart/State.hpp"
#include "Statechart/Statechart.hpp"
#include "Statechart/Transition.hpp"

#include <iostream>
#include <memory>

using namespace statechart;

namespace {

class TriggerParameter: public Parameter
{
public:

    int x = 0;
};

/// @brief Domain event "e".
class EEvent: public Event
{
public:

    EEvent() : Event("e") {}
};

Guard xEqualsTo(int p_value)
{
    return [p_value](Metadata const&, Parameter const& p_param) {
        return static_cast<const TriggerParameter&>(p_param).x == p_value;
    };
}

Guard xGreaterThan(int p_value)
{
    return [p_value](Metadata const&, Parameter const& p_param) {
        return static_cast<const TriggerParameter&>(p_param).x > p_value;
    };
}

} // namespace

int main()
{
    auto chart = std::make_unique<Statechart>("Triggers", 2, false);

    auto* start = chart->create<PseudoState>(
        "start", chart.get(), PseudoStateType::Start);

    auto* a = chart->create<State>(
        "A", chart.get(), [](Metadata&, Parameter& p_param) {
            std::cout << "[A] x = "
                      << static_cast<const TriggerParameter&>(p_param).x
                      << '\n';
        });
    auto* b = chart->create<State>(
        "B", chart.get(), [](Metadata&, Parameter&) { std::cout << "[B]\n"; });
    auto* c = chart->create<State>(
        "C", chart.get(), [](Metadata&, Parameter&) { std::cout << "[C]\n"; });
    auto* d = chart->create<State>(
        "D", chart.get(), [](Metadata&, Parameter&) { std::cout << "[D]\n"; });

    chart->createTransition(start, a);

    // Two `e`-triggered transitions with different guards: the order in
    // which they are registered matters when both guards evaluate true.
    // The first registered transition is checked first.
    chart->createTransition(a, b, chart->createEvent<EEvent>(), xEqualsTo(10));
    chart->createTransition(a, c, chart->createEvent<EEvent>());

    // Trigger-less transition with a guard. Will fire on entering `A` if the
    // guard is true, regardless of any incoming event.
    chart->createTransition(a, d, xGreaterThan(10));

    TriggerParameter parameter;
    Metadata data;

    EEvent e;

    // Scenario 1: x = 0 -> 'e' should pick the C path.
    std::cout << "=== Scenario 1: x=0, dispatch e -> C ===\n";
    parameter.x = 0;
    chart->start(data, parameter);
    chart->dispatch(data, &e, parameter);

    // Scenario 2: rebuild a fresh metadata. x = 10 -> 'e' should pick the
    // first registered transition (B).
    std::cout << "\n=== Scenario 2: x=10, dispatch e -> B ===\n";
    Metadata data2;
    parameter.x = 10;
    chart->start(data2, parameter);
    chart->dispatch(data2, &e, parameter);

    // Scenario 3: x = 20 -> trigger-less transition fires immediately on
    // entering A, we never see C nor B.
    std::cout
        << "\n=== Scenario 3: x=20, trigger-less transition fires -> D ===\n";
    Metadata data3;
    parameter.x = 20;
    chart->start(data3, parameter);

    chart->shutdown();
    return 0;
}
