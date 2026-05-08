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
 * @file self_parking.cpp
 * @brief Park-Assistant FSM looking for a parking slot and driving along
 *        the trajectory to reach it.
 *
 * Equivalent statechart (PlantUML, simplified):
 *
 *     [*] --> IDLE
 *     IDLE --> SCAN_PARKING_SPOTS : turning indicator [!isParked()]
 *     IDLE --> COMPUTE_LEAVING_TRAJECTORY : turning indicator [isParked()]
 *
 *     SCAN_PARKING_SPOTS --> COMPUTE_ENTERING_TRAJECTORY :
 *         [m_fsm_scan.status() == PARKING_SLOT_FOUND]
 *     SCAN_PARKING_SPOTS --> TRAJECTORY_DONE :
 *         [m_fsm_scan.status() == PARKING_SLOT_NOT_FOUND]
 *     SCAN_PARKING_SPOTS --> SCAN_PARKING_SPOTS [SEARCHING]
 *
 *     COMPUTE_ENTERING_TRAJECTORY --> DRIVE_ALONG_TRAJECTORY [hasTrajectory()]
 *     COMPUTE_ENTERING_TRAJECTORY --> IDLE [!canPark()]
 *     COMPUTE_ENTERING_TRAJECTORY --> TRAJECTORY_DONE [!hasTrajectory()]
 *
 *     COMPUTE_LEAVING_TRAJECTORY --> DRIVE_ALONG_TRAJECTORY [hasTrajectory()]
 *     COMPUTE_LEAVING_TRAJECTORY --> TRAJECTORY_DONE [!hasTrajectory()]
 *
 *     DRIVE_ALONG_TRAJECTORY --> TRAJECTORY_DONE [!canDrive(dt)]
 *     TRAJECTORY_DONE --> IDLE
 */

#include <CppStatecharts/CppStatecharts.hpp>

#include <iostream>
#include <memory>
#include <string>

using namespace statechart;

namespace {

/// @brief Outcome of the slot scanner FSM.
enum class ScanStatus
{
    SEARCHING,
    SLOT_FOUND,
    SLOT_NOT_FOUND,
};

/**
 * @brief Park-Assistant world model held in the @c Parameter.
 *
 * The original PlantUML model uses C++ method calls inside guards/actions:
 *  isParked(), hasTrajectory(), canPark(), canDrive(dt), and a sub-FSM
 *  m_fsm_scan. We expose the same observables as plain fields so guards
 *  can stay as small lambdas.
 */
class ParkParameter: public Parameter
{
public:

    bool isParked = false;
    bool hasTrajectory = true;
    bool canPark = true;
    bool canDrive = true;
    ScanStatus scanStatus = ScanStatus::SEARCHING;
};

/// @brief External event "turning indicator on/off".
class TurningIndicatorEvent: public Event
{
public:

    TurningIndicatorEvent() : Event("turning_indicator") {}
};

/// @brief Periodic update tick (dt elapsed).
class UpdateEvent: public Event
{
public:

    UpdateEvent() : Event("update") {}
};

template <class Pred>
Guard guardOn(Pred p_pred)
{
    return [p_pred = std::move(p_pred)](Metadata const&,
                                        Parameter const& p_param) {
        return p_pred(static_cast<const ParkParameter&>(p_param));
    };
}

Action log(std::string p_msg)
{
    return [m = std::move(p_msg)](Metadata&, Parameter&) {
        std::cout << "  " << m << '\n';
    };
}

const char* statusToString(ScanStatus p_status)
{
    switch (p_status)
    {
        case ScanStatus::SEARCHING: return "SEARCHING";
        case ScanStatus::SLOT_FOUND: return "SLOT_FOUND";
        case ScanStatus::SLOT_NOT_FOUND: return "SLOT_NOT_FOUND";
    }
    return "?";
}

void dump(ParkParameter const& p_param)
{
    std::cout << "    -> isParked=" << std::boolalpha << p_param.isParked
              << " scan=" << statusToString(p_param.scanStatus)
              << " hasTrajectory=" << p_param.hasTrajectory
              << " canPark=" << p_param.canPark
              << " canDrive=" << p_param.canDrive << '\n';
}

} // namespace

int main()
{
    auto chart = std::make_unique<Statechart>("SelfParking", 2, false);

    auto* start = chart->create<PseudoState>(
        "start", chart.get(), PseudoStateType::Start);

    auto* idle = chart->create<State>(
        "IDLE",
        chart.get(),
        [](Metadata&, Parameter&) { std::cout << "[IDLE]\n"; });

    auto* scan = chart->create<State>(
        "SCAN_PARKING_SPOTS",
        chart.get(),
        log("[SCAN_PARKING_SPOTS] entering"),
        log("(scanning slots ...)"));

    auto* computeEntering = chart->create<State>(
        "COMPUTE_ENTERING_TRAJECTORY",
        chart.get(),
        log("[COMPUTE_ENTERING_TRAJECTORY] computeTrajectoryEnteringSlot()"));

    auto* computeLeaving = chart->create<State>(
        "COMPUTE_LEAVING_TRAJECTORY",
        chart.get(),
        log("[COMPUTE_LEAVING_TRAJECTORY] computeTrajectoryLeavingSlot()"));

    auto* drive = chart->create<State>(
        "DRIVE_ALONG_TRAJECTORY",
        chart.get(),
        log("[DRIVE_ALONG_TRAJECTORY] entering"),
        log("(driving along trajectory)"));

    auto* done = chart->create<State>(
        "TRAJECTORY_DONE",
        chart.get(),
        [](Metadata&, Parameter& p_param) {
            std::cout << "[TRAJECTORY_DONE] setRefSpeed(0) "
                         "turning_indicator(false,false)\n";
            // Mirrors the original 'entering' actions of the state.
            (void)p_param;
        });

    chart->createTransition(start, idle);

    chart->createTransition(
        idle,
        scan,
        chart->createEvent<TurningIndicatorEvent>(),
        guardOn([](ParkParameter const& p) { return !p.isParked; }));
    chart->createTransition(
        idle,
        computeLeaving,
        chart->createEvent<TurningIndicatorEvent>(),
        guardOn([](ParkParameter const& p) { return p.isParked; }));

    chart->createTransition(
        scan,
        computeEntering,
        guardOn([](ParkParameter const& p) {
            return p.scanStatus == ScanStatus::SLOT_FOUND;
        }));
    chart->createTransition(
        scan,
        done,
        guardOn([](ParkParameter const& p) {
            return p.scanStatus == ScanStatus::SLOT_NOT_FOUND;
        }));

    chart->createTransition(
        computeEntering,
        drive,
        guardOn([](ParkParameter const& p) { return p.hasTrajectory; }));
    chart->createTransition(
        computeEntering,
        idle,
        guardOn([](ParkParameter const& p) { return !p.canPark; }));
    chart->createTransition(
        computeEntering,
        done,
        guardOn([](ParkParameter const& p) { return !p.hasTrajectory; }));

    chart->createTransition(
        computeLeaving,
        drive,
        guardOn([](ParkParameter const& p) { return p.hasTrajectory; }));
    chart->createTransition(
        computeLeaving,
        done,
        guardOn([](ParkParameter const& p) { return !p.hasTrajectory; }));

    chart->createTransition(
        drive,
        done,
        chart->createEvent<UpdateEvent>(),
        guardOn([](ParkParameter const& p) { return !p.canDrive; }));

    chart->createTransition(done, idle);

    ParkParameter parameter;
    Metadata data;
    TurningIndicatorEvent turning;
    UpdateEvent update;

    // ------------------------------------------------------------------
    // Scenario 1: not parked, scanner finds a slot, drive then done.
    // ------------------------------------------------------------------
    std::cout << "=== Scenario 1: park into the next free slot ===\n";
    parameter.isParked = false;
    parameter.scanStatus = ScanStatus::SEARCHING;
    parameter.hasTrajectory = true;
    parameter.canPark = true;
    parameter.canDrive = true;
    chart->start(data, parameter);
    dump(parameter);

    std::cout << ">> turning indicator on\n";
    chart->dispatch(data, &turning, parameter);
    dump(parameter);

    std::cout << ">> scanner reports SLOT_FOUND\n";
    parameter.scanStatus = ScanStatus::SLOT_FOUND;
    chart->dispatch(data, nullptr, parameter);

    std::cout << ">> tick (dt) - keep driving\n";
    chart->dispatch(data, &update, parameter);

    std::cout << ">> tick (dt) - canDrive becomes false\n";
    parameter.canDrive = false;
    chart->dispatch(data, &update, parameter);

    chart->shutdown();
    return 0;
}
