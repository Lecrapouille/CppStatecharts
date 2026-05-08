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
 * @file infinite_loop.cpp
 * @brief Demonstrates the framework's protection against infinite loops of
 *        trigger-less transitions.
 *
 * Equivalent statechart (PlantUML):
 *
 *     [*] -> State1
 *     State1 -> State2
 *     State2 -> State3
 *     State3 -> State1
 *
 * Since none of the transitions has an event nor a guard, dispatch() would
 * loop indefinitely. The library detects the situation and throws an
 * @c InfiniteLoopException after the configured threshold is reached. The
 * threshold is lowered here to keep the demo output short.
 */

#include <CppStatecharts/CppStatecharts.hpp>

#include <iostream>
#include <memory>

using namespace statechart;

int main()
{
    auto chart = std::make_unique<Statechart>("InfiniteLoop", 2, false);

    // Lower the threshold so the demo run terminates quickly.
    chart->setInfiniteLoopThreshold(10);

    auto* start = chart->create<PseudoState>(
        "start", chart.get(), PseudoStateType::Start);

    auto* state1 = chart->create<State>(
        "State1", chart.get(), [](Metadata&, Parameter&) {
            std::cout << "[State1]\n";
        });
    auto* state2 = chart->create<State>(
        "State2", chart.get(), [](Metadata&, Parameter&) {
            std::cout << "[State2]\n";
        });
    auto* state3 = chart->create<State>(
        "State3", chart.get(), [](Metadata&, Parameter&) {
            std::cout << "[State3]\n";
        });

    chart->createTransition(start, state1);

    // Trigger-less cycle. None of the transitions has a guard or an event,
    // so dispatch() will fire them in an infinite loop until the framework
    // breaks it with an InfiniteLoopException.
    chart->createTransition(state1, state2);
    chart->createTransition(state2, state3);
    chart->createTransition(state3, state1);

    Parameter parameter;
    Metadata data;

    std::cout << "=== Infinite-loop detection demo ===\n";
    std::cout << "Threshold = " << chart->infiniteLoopThreshold()
              << " trigger-less transitions per dispatch.\n\n";

    try
    {
        chart->start(data, parameter);
        std::cout << "ERROR: dispatch should have thrown!\n";
    }
    catch (const InfiniteLoopException& e)
    {
        std::cout << "\n[OK] InfiniteLoopException caught:\n  "
                  << e.what() << '\n';
    }
    catch (const StatechartException& e)
    {
        std::cout << "\n[KO] Wrong exception caught:\n  " << e.what() << '\n';
    }

    chart->shutdown();
    return 0;
}
