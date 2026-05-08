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
 * @file motor.cpp
 * @brief Motor controller example.
 *
 * Equivalent statechart (PlantUML):
 *
 *     [*] --> Idle
 *     Idle : entry / printf("Motor is halted")
 *     Idle : exit  / printf("Motor is started")
 *     Idle --> Start    : setSpeed(refSpeed) [refSpeed > 0] / m_reference_speed = refSpeed
 *     Start --> Stop    : halt
 *     Start -> Spinning : setSpeed(refSpeed) [refSpeed > 0] / m_reference_speed = refSpeed
 *     Spinning --> Stop : halt
 *     Stop --> Idle
 *     Spinning : on setSpeed(refSpeed) [refSpeed >= 0] / m_reference_speed = refSpeed
 */

#include <CppStatecharts/CppStatecharts.hpp>

#include <iostream>
#include <memory>

using namespace statechart;

namespace {

/**
 * @brief Custom parameter holding the requested motor reference speed.
 *
 * @note Why not pass the speed through @c SetSpeedEvent itself?
 *
 *   In this framework events are passive identifiers: they are only used to
 *   match transitions (their @c equals() method compares string IDs). Domain
 *   data lives in the @c Parameter subclass because:
 *
 *   - the parameter is the only object shared with guards, actions and the
 *     caller; an event's lifetime ends as soon as @c dispatch() returns;
 *   - parameters survive across dispatches, so @c m_reference_speed remains
 *     readable after the transition fired (here: by the @c Spinning state);
 *   - putting data inside the event would force every guard/action to
 *     @c dynamic_cast the @c Event* and break that information sharing.
 *
 *   It is technically possible to subclass @c Event with extra fields (this
 *   is what @c TimeoutEvent does with its timeout value) but for ordinary
 *   business data the @c Parameter idiom is preferred.
 */
class MotorParameter: public Parameter
{
public:

    /// @brief Speed value carried by the @c SetSpeedEvent (input).
    int reference_speed = 0;

    /// @brief Latched motor speed updated by the action (state).
    int m_reference_speed = 0;
};

/**
 * @brief Event corresponding to PlantUML `setSpeed(refSpeed)`.
 *
 * Carries the requested reference speed in @c MotorParameter.
 */
class SetSpeedEvent: public Event
{
public:

    SetSpeedEvent() : Event("setSpeed") {}
};

/**
 * @brief Event corresponding to PlantUML `halt`.
 */
class HaltEvent: public Event
{
public:

    HaltEvent() : Event("halt") {}
};

/** @brief Action: store the requested speed into @c m_reference_speed. */
Action saveReferenceSpeed()
{
    return [](Metadata&, Parameter& p_param) {
        auto& mp = static_cast<MotorParameter&>(p_param);
        mp.m_reference_speed = mp.reference_speed;
        std::cout << "  -> m_reference_speed = " << mp.m_reference_speed
                  << '\n';
    };
}

/** @brief Guard: refSpeed > 0 (motor must be requested to spin). */
Guard refSpeedPositive()
{
    return [](Metadata const&, Parameter const& p_param) {
        return static_cast<const MotorParameter&>(p_param).reference_speed > 0;
    };
}

} // namespace

int main()
{
    auto chart = std::make_unique<Statechart>("Motor", 2, false);

    auto* start = chart->create<PseudoState>(
        "start", chart.get(), PseudoStateType::Start);

    auto* idle = chart->create<State>(
        "Idle",
        chart.get(),
        [](Metadata&, Parameter&) { std::cout << "Motor is halted\n"; },
        Action{},
        [](Metadata&, Parameter&) { std::cout << "Motor is started\n"; });

    auto* startState = chart->create<State>("Start", chart.get());
    auto* spinning = chart->create<State>("Spinning", chart.get());
    auto* stop = chart->create<State>("Stop", chart.get());

    chart->createTransition(start, idle);

    chart->createTransition(idle,
                            startState,
                            chart->createEvent<SetSpeedEvent>(),
                            refSpeedPositive(),
                            saveReferenceSpeed());

    chart->createTransition(
        startState, stop, chart->createEvent<HaltEvent>());

    chart->createTransition(startState,
                            spinning,
                            chart->createEvent<SetSpeedEvent>(),
                            refSpeedPositive(),
                            saveReferenceSpeed());

    chart->createInternalTransition(spinning,
                                    chart->createEvent<SetSpeedEvent>(),
                                    saveReferenceSpeed());

    chart->createTransition(spinning, stop, chart->createEvent<HaltEvent>());

    chart->createTransition(stop, idle);

    MotorParameter parameter;
    Metadata data;

    std::cout << "=== Motor controller example ===\n";
    chart->start(data, parameter);

    std::cout << "\n>> setSpeed(100) : start motor\n";
    parameter.reference_speed = 100;
    SetSpeedEvent setSpeed;
    chart->dispatch(data, &setSpeed, parameter);

    std::cout << "\n>> setSpeed(200) : update reference speed (Start -> "
                 "Spinning)\n";
    parameter.reference_speed = 200;
    chart->dispatch(data, &setSpeed, parameter);

    std::cout << "\n>> setSpeed(150) : internal transition (Spinning stays "
                 "Spinning)\n";
    parameter.reference_speed = 150;
    chart->dispatch(data, &setSpeed, parameter);

    std::cout << "\n>> halt : stop motor\n";
    HaltEvent halt;
    chart->dispatch(data, &halt, parameter);

    chart->shutdown();
    return 0;
}
