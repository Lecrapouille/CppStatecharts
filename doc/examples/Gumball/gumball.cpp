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
 * @file gumball.cpp
 * @brief Gumball distributor example.
 *
 * Equivalent statechart (PlantUML):
 *
 *     [*] --> NoQuarter      : [gumballs > 0]
 *     [*] --> OutOfGumballs  : [gumballs == 0]
 *     NoQuarter -> HasQuarter : insert quarter
 *     HasQuarter -> NoQuarter : eject quarter
 *     HasQuarter --> GumballSold : turn crank / --gumballs
 *     GumballSold -> NoQuarter      : [gumballs > 0]
 *     GumballSold -> OutOfGumballs  : [gumballs == 0]
 */

#include "Statechart/Condition.hpp"
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

/** @brief Custom parameter holding the number of gumballs left. */
class GumballParameter: public Parameter
{
public:

    int gumballs = 0;
};

class InsertQuarterEvent: public Event
{
public:

    InsertQuarterEvent() : Event("insertQuarter") {}
};

class EjectQuarterEvent: public Event
{
public:

    EjectQuarterEvent() : Event("ejectQuarter") {}
};

class TurnCrankEvent: public Event
{
public:

    TurnCrankEvent() : Event("turnCrank") {}
};

/** @brief Action: decrement the gumballs counter. */
Action decrementGumballs()
{
    return [](Metadata&, Parameter& p_param) {
        auto& gp = static_cast<GumballParameter&>(p_param);
        --gp.gumballs;
        std::cout << "  -> Remaining gumballs: " << gp.gumballs << '\n';
    };
}

/** @brief Guard: gumballs > 0. */
Guard hasGumballs()
{
    return [](Metadata const&, Parameter const& p_param) {
        return static_cast<const GumballParameter&>(p_param).gumballs > 0;
    };
}

} // namespace

int main()
{
    auto chart = std::make_unique<Statechart>("Gumball", 2, false);

    auto* start = chart->create<PseudoState>(
        "start", chart.get(), PseudoStateType::Start);

    auto* noQuarter = chart->create<State>(
        "NoQuarter",
        chart.get(),
        [](Metadata&, Parameter&) {
            std::cout << "[NoQuarter] Waiting for a coin...\n";
        });

    auto* hasQuarter = chart->create<State>(
        "HasQuarter",
        chart.get(),
        [](Metadata&, Parameter&) {
            std::cout << "[HasQuarter] Waiting for the crank...\n";
        });

    auto* gumballSold = chart->create<State>(
        "GumballSold",
        chart.get(),
        [](Metadata&, Parameter&) {
            std::cout << "[GumballSold] A gumball is rolling...\n";
        });

    auto* outOfGumballs = chart->create<State>(
        "OutOfGumballs",
        chart.get(),
        [](Metadata&, Parameter&) {
            std::cout << "[OutOfGumballs] Sorry, no more gumballs.\n";
        });

    auto* initJunction = chart->create<Condition>(
        "initJunction", chart.get(),
        hasGumballs(), noQuarter, outOfGumballs);

    auto* soldJunction = chart->create<Condition>(
        "soldJunction", chart.get(),
        hasGumballs(), noQuarter, outOfGumballs);

    chart->createTransition(start, initJunction);

    chart->createTransition(
        noQuarter, hasQuarter, chart->createEvent<InsertQuarterEvent>());

    chart->createTransition(
        hasQuarter, noQuarter, chart->createEvent<EjectQuarterEvent>());

    chart->createTransition(hasQuarter,
                            gumballSold,
                            chart->createEvent<TurnCrankEvent>(),
                            decrementGumballs());

    chart->createTransition(gumballSold, soldJunction);

    GumballParameter parameter;
    parameter.gumballs = 3;
    Metadata data;

    std::cout << "=== Gumball distributor (initial: " << parameter.gumballs
              << " gumballs) ===\n";
    chart->start(data, parameter);

    InsertQuarterEvent insert;
    EjectQuarterEvent eject;
    TurnCrankEvent turn;

    std::cout << "\n>> Buy gumball #1\n";
    chart->dispatch(data, &insert, parameter);
    chart->dispatch(data, &turn, parameter);

    std::cout << "\n>> Buy gumball #2\n";
    chart->dispatch(data, &insert, parameter);
    chart->dispatch(data, &turn, parameter);

    std::cout << "\n>> Eject quarter test\n";
    chart->dispatch(data, &insert, parameter);
    chart->dispatch(data, &eject, parameter);

    std::cout << "\n>> Buy gumball #3 (last one)\n";
    chart->dispatch(data, &insert, parameter);
    chart->dispatch(data, &turn, parameter);

    chart->shutdown();
    return 0;
}
