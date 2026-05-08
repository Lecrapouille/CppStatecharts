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
 * @file complex_composite.cpp
 * @brief Deeply-nested composite (hierarchical) state example.
 *
 * Equivalent statechart (PlantUML):
 *
 *     state State0 {
 *       state State1 {
 *         state State11 {}
 *         [*] -> State11
 *       }
 *       state State2 {
 *         state State21 {
 *           state State211 {}
 *           [*] -> State211
 *         }
 *         [*] -> State21
 *       }
 *       [*] -> State1
 *     }
 *     [*] -> State0
 *     State1 -> State211 : f
 */

#include "Statechart/Event.hpp"
#include "Statechart/HierarchicalState.hpp"
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

class FEvent: public Event
{
public:

    FEvent() : Event("f") {}
};

#define ENTRY(label)                                                           \
    [](Metadata&, Parameter&) { std::cout << "  Entering " #label "\n"; }
#define EXIT(label)                                                            \
    [](Metadata&, Parameter&) { std::cout << "  Leaving " #label "\n"; }

} // namespace

int main()
{
    auto chart = std::make_unique<Statechart>("ComplexComposite", 2, false);

    auto* start = chart->create<PseudoState>(
        "start", chart.get(), PseudoStateType::Start);

    auto* state0 = chart->create<HierarchicalState>(
        "State0", chart.get(), ENTRY(State0), Action{}, EXIT(State0));
    auto* s0Start =
        chart->create<PseudoState>("s0Start", state0, PseudoStateType::Start);

    auto* state1 = chart->create<HierarchicalState>(
        "State1", state0, ENTRY(State1), Action{}, EXIT(State1));
    auto* s1Start =
        chart->create<PseudoState>("s1Start", state1, PseudoStateType::Start);

    auto* state11 = chart->create<State>(
        "State11", state1, ENTRY(State11), Action{}, EXIT(State11));

    auto* state2 = chart->create<HierarchicalState>(
        "State2", state0, ENTRY(State2), Action{}, EXIT(State2));
    auto* s2Start =
        chart->create<PseudoState>("s2Start", state2, PseudoStateType::Start);

    auto* state21 = chart->create<HierarchicalState>(
        "State21", state2, ENTRY(State21), Action{}, EXIT(State21));
    auto* s21Start = chart->create<PseudoState>(
        "s21Start", state21, PseudoStateType::Start);

    auto* state211 = chart->create<State>(
        "State211", state21, ENTRY(State211), Action{}, EXIT(State211));

    chart->createTransition(start, state0);
    chart->createTransition(s0Start, state1);
    chart->createTransition(s1Start, state11);
    chart->createTransition(s2Start, state21);
    chart->createTransition(s21Start, state211);

    chart->createTransition(state1, state211, chart->createEvent<FEvent>());

    Parameter parameter;
    Metadata data;

    std::cout << "=== Complex composite (nested 3-level) ===\n";
    chart->start(data, parameter);

    std::cout << "\n>> Dispatch event 'f' : State1 -> State211\n";
    FEvent f;
    chart->dispatch(data, &f, parameter);

    chart->shutdown();
    return 0;
}
