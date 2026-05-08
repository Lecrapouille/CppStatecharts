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
 * @file richman.cpp
 * @brief A punk guy with no money in pockets, finds coins in the street but
 *        his pockets are limited to 10 quarters.
 *
 * Equivalent statechart (PlantUML):
 *
 *     [*] -> FindQuarter : / quarters = 0
 *     FindQuarter --> CountQuarter : Get quarter [quarters < 10] / incr(quarters)
 *     CountQuarter -> Yeah : [quarters == 1]
 *     CountQuarter --> FindQuarter : [quarters > 1]
 *     Yeah --> FindQuarter : / yes
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

class RichManParameter: public Parameter
{
public:

    int quarters = 0;
};

class GetQuarterEvent: public Event
{
public:

    GetQuarterEvent() : Event("getQuarter") {}
};

Action incrementQuarters()
{
    return [](Metadata&, Parameter& p_param) {
        auto& rmp = static_cast<RichManParameter&>(p_param);
        rmp.quarters += 1;
        std::cout << "  Quarters: " << rmp.quarters << '\n';
    };
}

Action resetQuarters()
{
    return [](Metadata&, Parameter& p_param) {
        static_cast<RichManParameter&>(p_param).quarters = 0;
    };
}

Guard quartersLessThan(int p_value)
{
    return [p_value](Metadata const&, Parameter const& p_param) {
        return static_cast<const RichManParameter&>(p_param).quarters
               < p_value;
    };
}

Guard quartersEqualsTo(int p_value)
{
    return [p_value](Metadata const&, Parameter const& p_param) {
        return static_cast<const RichManParameter&>(p_param).quarters
               == p_value;
    };
}

Guard quartersGreaterThan(int p_value)
{
    return [p_value](Metadata const&, Parameter const& p_param) {
        return static_cast<const RichManParameter&>(p_param).quarters
               > p_value;
    };
}

} // namespace

int main()
{
    auto chart = std::make_unique<Statechart>("RichMan", 2, false);

    auto* start = chart->create<PseudoState>(
        "start", chart.get(), PseudoStateType::Start);

    auto* findQuarter = chart->create<State>(
        "FindQuarter",
        chart.get(),
        [](Metadata&, Parameter&) {
            std::cout << "[FindQuarter] Looking for coins on the ground...\n";
        });

    auto* countQuarter = chart->create<State>(
        "CountQuarter",
        chart.get(),
        [](Metadata&, Parameter& p_param) {
            std::cout << "[CountQuarter] Counting (current: "
                      << static_cast<RichManParameter&>(p_param).quarters
                      << ")\n";
        });

    auto* yeah = chart->create<State>(
        "Yeah",
        chart.get(),
        [](Metadata&, Parameter&) {
            std::cout << "[Yeah] Yes! My first coin found!\n";
        });

    chart->createTransition(start, findQuarter, resetQuarters());

    chart->createTransition(findQuarter,
                            countQuarter,
                            chart->createEvent<GetQuarterEvent>(),
                            quartersLessThan(10),
                            incrementQuarters());

    chart->createTransition(countQuarter, yeah, quartersEqualsTo(1));

    // Disjoint guard with [quarters == 1]: ensures Yeah is reached exactly on
    // the first coin. Using `quarters < 11` would also be true when
    // `quarters == 1`, leading to an order-dependent transition pick.
    chart->createTransition(countQuarter, findQuarter, quartersGreaterThan(1));

    chart->createTransition(yeah, findQuarter);

    RichManParameter parameter;
    Metadata data;

    std::cout << "=== A punk gets rich, one quarter at a time ===\n";
    chart->start(data, parameter);

    GetQuarterEvent gq;

    for (int i = 0; i < 12; ++i)
    {
        std::cout << "\n>> Get quarter (attempt " << (i + 1) << ")\n";
        chart->dispatch(data, &gq, parameter);
    }

    chart->shutdown();
    return 0;
}
