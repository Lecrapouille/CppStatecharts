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
 * @file digital_watch.cpp
 * @brief Digital watch example.
 *
 * Equivalent statechart (PlantUML):
 *
 *     [*] -> Idle             : turn on
 *     [*] --> SettingHours    : turn on [min == 0 && hours == 0]
 *     Idle --> SettingHours   : set
 *     SettingHours --> SettingMinutes : set
 *     SettingMinutes --> Idle : set
 *
 *     Idle : do / show current time
 *     SettingHours : entry / beep()
 *     SettingHours : do / show hours()
 *     SettingHours : event incr / hours = (hours + 1) % 24
 *
 *     SettingMinutes : entry / beep()
 *     SettingMinutes : do / show mins()
 *     SettingMinutes : event incr / min = (min + 1) % 60
 */

#include <CppStatecharts/CppStatecharts.hpp>

#include <iostream>
#include <memory>

using namespace statechart;

namespace {

class WatchParameter: public Parameter
{
public:

    int hours = 0;
    int minutes = 0;
};

class SetEvent: public Event
{
public:

    SetEvent() : Event("set") {}
};

class IncrEvent: public Event
{
public:

    IncrEvent() : Event("incr") {}
};

Action beep()
{
    return [](Metadata&, Parameter&) { std::cout << "  *beep*\n"; };
}

Action incrementHours()
{
    return [](Metadata&, Parameter& p_param) {
        auto& wp = static_cast<WatchParameter&>(p_param);
        wp.hours = (wp.hours + 1) % 24;
        std::cout << "  Hours -> " << wp.hours << '\n';
    };
}

Action incrementMinutes()
{
    return [](Metadata&, Parameter& p_param) {
        auto& wp = static_cast<WatchParameter&>(p_param);
        wp.minutes = (wp.minutes + 1) % 60;
        std::cout << "  Minutes -> " << wp.minutes << '\n';
    };
}

Guard timeIsZero()
{
    return [](Metadata const&, Parameter const& p_param) {
        const auto& wp = static_cast<const WatchParameter&>(p_param);
        return wp.hours == 0 && wp.minutes == 0;
    };
}

} // namespace

int main()
{
    auto chart = std::make_unique<Statechart>("DigitalWatch", 2, false);

    auto* start = chart->create<PseudoState>(
        "start", chart.get(), PseudoStateType::Start);

    auto* idle = chart->create<State>(
        "Idle",
        chart.get(),
        Action{},
        [](Metadata&, Parameter& p_param) {
            const auto& wp = static_cast<const WatchParameter&>(p_param);
            std::cout << "  Display: " << wp.hours << "h" << wp.minutes
                      << "min\n";
        });

    auto* settingHours = chart->create<State>(
        "SettingHours",
        chart.get(),
        beep(),
        [](Metadata&, Parameter& p_param) {
            std::cout << "  Setting hours = "
                      << static_cast<const WatchParameter&>(p_param).hours
                      << '\n';
        });

    auto* settingMinutes = chart->create<State>(
        "SettingMinutes",
        chart.get(),
        beep(),
        [](Metadata&, Parameter& p_param) {
            std::cout << "  Setting minutes = "
                      << static_cast<const WatchParameter&>(p_param).minutes
                      << '\n';
        });

    auto* initJunction = chart->create<Condition>(
        "initJunction", chart.get(),
        timeIsZero(), settingHours, idle);

    chart->createTransition(start, initJunction);

    chart->createTransition(idle, settingHours, chart->createEvent<SetEvent>());
    chart->createTransition(
        settingHours, settingMinutes, chart->createEvent<SetEvent>());
    chart->createTransition(
        settingMinutes, idle, chart->createEvent<SetEvent>());

    chart->createInternalTransition(
        settingHours, chart->createEvent<IncrEvent>(), incrementHours());
    chart->createInternalTransition(
        settingMinutes, chart->createEvent<IncrEvent>(), incrementMinutes());

    WatchParameter parameter;
    Metadata data;

    std::cout << "=== Digital watch (initial: 0h0min => SettingHours) ===\n";
    chart->start(data, parameter);

    SetEvent set;
    IncrEvent incr;

    std::cout << "\n>> Increment hours 3 times\n";
    chart->dispatch(data, &incr, parameter);
    chart->dispatch(data, &incr, parameter);
    chart->dispatch(data, &incr, parameter);

    std::cout << "\n>> set : SettingHours -> SettingMinutes\n";
    chart->dispatch(data, &set, parameter);

    std::cout << "\n>> Increment minutes 5 times\n";
    for (int i = 0; i < 5; ++i)
    {
        chart->dispatch(data, &incr, parameter);
    }

    std::cout << "\n>> set : SettingMinutes -> Idle\n";
    chart->dispatch(data, &set, parameter);

    chart->shutdown();
    return 0;
}
