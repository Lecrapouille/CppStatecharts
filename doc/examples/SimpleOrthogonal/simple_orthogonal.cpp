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
 * @file simple_orthogonal.cpp
 * @brief Test simple orthogonal (concurrent) state.
 *
 * Equivalent statechart (PlantUML):
 *
 *     [*] -> EnableSystem
 *     state EnableSystem {
 *       [*] -> State1
 *       State1 -> [*]
 *     --
 *       [*] -> State2
 *       State2 -> [*]
 *     }
 *     EnableSystem -> DisableSystem
 */

#include "Statechart/ConcurrentState.hpp"
#include "Statechart/FinalState.hpp"
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

int main()
{
    auto chart = std::make_unique<Statechart>("SimpleOrthogonal", 2, false);

    auto* start = chart->create<PseudoState>(
        "start", chart.get(), PseudoStateType::Start);

    auto* enableSystem = chart->create<ConcurrentState>(
        "EnableSystem",
        chart.get(),
        [](Metadata&, Parameter&) { std::cout << "[EnableSystem] entered\n"; },
        Action{},
        [](Metadata&, Parameter&) { std::cout << "[EnableSystem] exited\n"; });

    auto* region1 = chart->create<HierarchicalState>("Region1", enableSystem);
    auto* r1Start = chart->create<PseudoState>(
        "r1Start", region1, PseudoStateType::Start);
    auto* state1 = chart->create<State>(
        "State1",
        region1,
        [](Metadata&, Parameter&) { std::cout << "  [Region1] State1\n"; });
    auto* r1Final = chart->create<FinalState>("r1Final", region1);

    auto* region2 = chart->create<HierarchicalState>("Region2", enableSystem);
    auto* r2Start = chart->create<PseudoState>(
        "r2Start", region2, PseudoStateType::Start);
    auto* state2 = chart->create<State>(
        "State2",
        region2,
        [](Metadata&, Parameter&) { std::cout << "  [Region2] State2\n"; });
    auto* r2Final = chart->create<FinalState>("r2Final", region2);

    auto* disableSystem = chart->create<State>(
        "DisableSystem",
        chart.get(),
        [](Metadata&, Parameter&) { std::cout << "[DisableSystem] entered\n"; });

    chart->createTransition(start, enableSystem);

    chart->createTransition(r1Start, state1);
    chart->createTransition(state1, r1Final);

    chart->createTransition(r2Start, state2);
    chart->createTransition(state2, r2Final);

    chart->createTransition(enableSystem, disableSystem);

    Parameter parameter;
    Metadata data;

    std::cout << "=== Simple orthogonal (concurrent) state ===\n";
    chart->start(data, parameter);

    chart->shutdown();
    return 0;
}
