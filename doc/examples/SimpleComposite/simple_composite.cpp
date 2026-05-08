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
 * @file simple_composite.cpp
 * @brief Test simple composite (hierarchical) state.
 *
 * Equivalent statechart (PlantUML):
 *
 *     [*] -> EnableSystem
 *     state EnableSystem {
 *       [*] -> ON
 *       ON -> OFF : off
 *       OFF -> ON : on
 *     }
 *     EnableSystem -> DisableSystem : disable
 *     EnableSystem <- DisableSystem : enable
 */

#include <CppStatecharts/CppStatecharts.hpp>

#include <iostream>
#include <memory>

using namespace statechart;

namespace {

class OnEvent: public Event
{
public:

    OnEvent() : Event("on") {}
};

class OffEvent: public Event
{
public:

    OffEvent() : Event("off") {}
};

class EnableEvent: public Event
{
public:

    EnableEvent() : Event("enable") {}
};

class DisableEvent: public Event
{
public:

    DisableEvent() : Event("disable") {}
};

} // namespace

int main()
{
    auto chart = std::make_unique<Statechart>("SimpleComposite", 2, false);

    auto* start = chart->create<PseudoState>(
        "start", chart.get(), PseudoStateType::Start);

    auto* enableSystem = chart->create<HierarchicalState>(
        "EnableSystem",
        chart.get(),
        [](Metadata&, Parameter&) { std::cout << "[EnableSystem] entered\n"; },
        Action{},
        [](Metadata&, Parameter&) { std::cout << "[EnableSystem] exited\n"; });

    auto* enableStart = chart->create<PseudoState>(
        "enableStart", enableSystem, PseudoStateType::Start);

    auto* on = chart->create<State>(
        "ON",
        enableSystem,
        [](Metadata&, Parameter&) { std::cout << "  [ON] light is on\n"; });

    auto* off = chart->create<State>(
        "OFF",
        enableSystem,
        [](Metadata&, Parameter&) { std::cout << "  [OFF] light is off\n"; });

    auto* disableSystem = chart->create<State>(
        "DisableSystem",
        chart.get(),
        [](Metadata&, Parameter&) { std::cout << "[DisableSystem] entered\n"; });

    chart->createTransition(start, enableSystem);
    chart->createTransition(enableStart, on);
    chart->createTransition(on, off, chart->createEvent<OffEvent>());
    chart->createTransition(off, on, chart->createEvent<OnEvent>());

    chart->createTransition(
        enableSystem, disableSystem, chart->createEvent<DisableEvent>());
    chart->createTransition(
        disableSystem, enableSystem, chart->createEvent<EnableEvent>());

    Parameter parameter;
    Metadata data;

    std::cout << "=== Simple composite state ===\n";
    chart->start(data, parameter);

    OnEvent on_e;
    OffEvent off_e;
    EnableEvent enable_e;
    DisableEvent disable_e;

    std::cout << "\n>> off: ON -> OFF\n";
    chart->dispatch(data, &off_e, parameter);

    std::cout << "\n>> on : OFF -> ON\n";
    chart->dispatch(data, &on_e, parameter);

    std::cout << "\n>> disable: EnableSystem -> DisableSystem\n";
    chart->dispatch(data, &disable_e, parameter);

    std::cout << "\n>> enable : DisableSystem -> EnableSystem\n";
    chart->dispatch(data, &enable_e, parameter);

    chart->shutdown();
    return 0;
}
