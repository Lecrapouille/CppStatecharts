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
 * @file pompe.cpp
 * @brief Dead-Man (Homme Mort) detector for a pump.
 *
 *  Equivalent statechart (PlantUML):
 *
 *      [*] --> PumpStopped
 *      Default : entry / stopPump()
 *      Default : do    / alarmType3()
 *
 *      state PumpRunning {
 *        state WaitForHMRelease {
 *          StartPump --> Wait40Seconds
 *          StartPump : entry / startPump()
 *          Wait40Seconds --> Wait20Seconds : after40s
 *          Wait20Seconds --> Default : after20s
 *          Wait20Seconds : do / alarmType1()
 *        }
 *
 *        WaitForHMRelease -> WaitForHM : falling HM
 *        WaitForHM : do / alarmType2()
 *        Wait40Seconds <- WaitForHM : rising HM
 *        WaitForHM --> Default : after5s
 *      }
 *
 *      PumpStopped --> StartPump : BPM [HM && !AU && !DHM && !DPP]
 *      ReturnToService -> PumpStopped
 *      ReturnToService <- Default : Acq
 *      PumpStopped <-- PumpRunning : AU
 *      PumpStopped <-- PumpRunning : BPA
 *      PumpStopped <-- PumpRunning : DPP
 *
 *  Real timeouts are 40s / 20s / 5s. To keep the demo short, every delay is
 *  scaled down by a factor of 10 (`SIM_DIV`) so the run takes a few seconds.
 *
 *  Scenario:
 *    1. Press BPM with HM held       -> pump starts.
 *    2. Wait ~2 s (= 20 s real time) -> pump still running.
 *    3. Press BPA                    -> pump stops.
 *    4. Press BPM again              -> pump restarts.
 *    5. Release HM (falling HM)      -> WaitForHM, alarm type 2.
 *    6. Press BPM (no effect, dead)  -> stays in WaitForHM.
 *    7. Press HM (rising HM)         -> back to Wait40Seconds.
 */

#include <CppStatecharts/CppStatecharts.hpp>

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

using namespace statechart;

namespace {

/// @brief Scale-down factor applied to every timeout to keep the demo short.
constexpr std::int64_t SIM_DIV = 10;

/// Real 40 s / 10 = 4 s in simulation.
constexpr std::int64_t TIMEOUT_40S_MS = 40000 / SIM_DIV;
constexpr std::int64_t TIMEOUT_20S_MS = 20000 / SIM_DIV;
constexpr std::int64_t TIMEOUT_5S_MS = 5000 / SIM_DIV;

class PompeParameter: public Parameter
{
public:

    bool HM = true;   ///< Dead-man lever (held by operator).
    bool AU = false;  ///< Emergency stop button.
    bool DHM = false; ///< HM fault.
    bool DPP = false; ///< Water fault.
};

#define DECLARE_EVENT(name, label)      \
    class name##Event: public Event     \
    {                                   \
    public:                             \
                                        \
        name##Event() : Event(label) {} \
    }

DECLARE_EVENT(BPM, "BPM");
DECLARE_EVENT(BPA, "BPA");
DECLARE_EVENT(AU, "AU");
DECLARE_EVENT(DPP, "DPP");
DECLARE_EVENT(FallingHM, "falling_HM");
DECLARE_EVENT(RisingHM, "rising_HM");
DECLARE_EVENT(Acq, "Acq");

#undef DECLARE_EVENT

Action startPump()
{
    return [](Metadata&, Parameter&) { std::cout << "  >> startPump()\n"; };
}

Action stopPump()
{
    return [](Metadata&, Parameter&) { std::cout << "  >> stopPump()\n"; };
}

Action alarm(int type)
{
    return [type](Metadata&, Parameter&) {
        std::cout << "  >> alarmType" << type << "()\n";
    };
}

Guard canStart()
{
    return [](Metadata const&, Parameter const& param) {
        const auto& pp = static_cast<const PompeParameter&>(param);
        return pp.HM && !pp.AU && !pp.DHM && !pp.DPP;
    };
}

void waitMs(std::int64_t ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

} // namespace

int main()
{
    auto chart = std::make_unique<Statechart>("Pump", 2, false);

    auto* start = chart->create<PseudoState>(
        "start", chart.get(), PseudoStateType::Start);

    auto* pumpStopped = chart->create<State>(
        "PumpStopped", chart.get(), [](Metadata&, Parameter&) {
            std::cout << "[PumpStopped] pump idle\n";
        });

    auto* defaultState =
        chart->create<State>("Default", chart.get(), stopPump(), alarm(3));

    auto* returnToService = chart->create<State>(
        "ReturnToService", chart.get(), [](Metadata&, Parameter&) {
            std::cout << "[ReturnToService] returning to service\n";
        });

    auto* pumpRunning = chart->create<HierarchicalState>(
        "PumpRunning", chart.get(), [](Metadata&, Parameter&) {
            std::cout << "[PumpRunning] pump running\n";
        });

    auto* waitForHMRelease =
        chart->create<HierarchicalState>("WaitForHMRelease", pumpRunning);

    auto* whmrStart = chart->create<PseudoState>(
        "whmrStart", waitForHMRelease, PseudoStateType::Start);

    auto* startPumpState =
        chart->create<State>("StartPump", waitForHMRelease, startPump());

    auto* wait40 = chart->create<State>(
        "Wait40Seconds", waitForHMRelease, [](Metadata&, Parameter&) {
            std::cout << "  [Wait40Seconds]\n";
        });

    auto* wait20 = chart->create<State>(
        "Wait20Seconds",
        waitForHMRelease,
        [](Metadata&, Parameter&) {
            std::cout << "  [Wait20Seconds] entering alarm window\n";
        },
        alarm(1));

    auto* waitForHM = chart->create<State>(
        "WaitForHM",
        pumpRunning,
        [](Metadata&, Parameter&) {
            std::cout << "  [WaitForHM] HM released, type-2 alarm\n";
        },
        alarm(2));

    chart->createTransition(start, pumpStopped);

    chart->createTransition(pumpStopped,
                            startPumpState,
                            chart->createEvent<BPMEvent>(),
                            canStart());

    chart->createTransition(whmrStart, startPumpState);
    chart->createTransition(startPumpState, wait40);

    // The original 40 s / 20 s / 5 s timeouts are scaled down by SIM_DIV so
    // the demo run completes in a few seconds.
    chart->createTransition(
        wait40, wait20, chart->createEvent<TimeoutEvent>(TIMEOUT_40S_MS));
    chart->createTransition(
        wait20, defaultState, chart->createEvent<TimeoutEvent>(TIMEOUT_20S_MS));

    chart->createTransition(
        waitForHMRelease, waitForHM, chart->createEvent<FallingHMEvent>());
    chart->createTransition(
        waitForHM, wait40, chart->createEvent<RisingHMEvent>());
    chart->createTransition(waitForHM,
                            defaultState,
                            chart->createEvent<TimeoutEvent>(TIMEOUT_5S_MS));

    chart->createTransition(
        pumpRunning, pumpStopped, chart->createEvent<AUEvent>());
    chart->createTransition(
        pumpRunning, pumpStopped, chart->createEvent<BPAEvent>());
    chart->createTransition(
        pumpRunning, pumpStopped, chart->createEvent<DPPEvent>());

    chart->createTransition(
        defaultState, returnToService, chart->createEvent<AcqEvent>());
    chart->createTransition(returnToService, pumpStopped);

    PompeParameter parameter;
    Metadata data;

    std::cout << "=== Dead-Man pump controller (timeouts /" << SIM_DIV
              << ") ===\n";
    chart->start(data, parameter);

    BPMEvent bpm;
    BPAEvent bpa;
    FallingHMEvent fallingHM;
    RisingHMEvent risingHM;

    // ------------------------------------------------------------------
    // Cycle 1: pump runs ~20 s of simulated time then operator stops it.
    // ------------------------------------------------------------------
    std::cout << "\n>> [1] Press BPM (HM held): start the pump\n";
    chart->dispatch(data, &bpm, parameter);

    std::cout << "\n>> [1] Wait ~20 s sim-time (= " << TIMEOUT_20S_MS
              << " ms)\n";
    waitMs(TIMEOUT_20S_MS);

    std::cout << "\n>> [1] Press BPA: stop the pump\n";
    chart->dispatch(data, &bpa, parameter);

    // ------------------------------------------------------------------
    // Cycle 2: restart, release HM, try to restart, press HM back.
    // ------------------------------------------------------------------
    std::cout << "\n>> [2] Press BPM again: restart the pump\n";
    chart->dispatch(data, &bpm, parameter);

    std::cout << "\n>> [2] Wait a bit so the pump is well in Wait40Seconds\n";
    waitMs(200);

    std::cout << "\n>> [2] Operator releases HM (falling HM): alarm type 2\n";
    parameter.HM = false;
    chart->dispatch(data, &fallingHM, parameter);

    std::cout
        << "\n>> [2] Press BPM (does nothing: we are not in PumpStopped)\n";
    chart->dispatch(data, &bpm, parameter);

    std::cout << "\n>> [2] Operator presses HM back (rising HM): resume\n";
    parameter.HM = true;
    chart->dispatch(data, &risingHM, parameter);

    waitMs(200);

    std::cout << "\n>> [2] Final BPA to terminate cleanly\n";
    chart->dispatch(data, &bpa, parameter);

    chart->shutdown();
    return 0;
}
