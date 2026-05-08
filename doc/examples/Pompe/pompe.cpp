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
 *      [*] --> PompeEnArret
 *      Default : entry / arretPompe()
 *      Default : do    / alarmeType3()
 *
 *      state PompeEnMarche {
 *        state AttenteLiberationHM {
 *          MiseEnMarchePompe --> Attente40Secondes
 *          MiseEnMarchePompe : entry / marchePompe()
 *          Attente40Secondes --> Attente20Secondes : after40s
 *          Attente20Secondes --> Default : after20s
 *          Attente20Secondes : do / alarmeType1()
 *        }
 *
 *        AttenteLiberationHM -> AttenteHM : falling HM
 *        AttenteHM : do / alarmeType2()
 *        Attente40Secondes <- AttenteHM : rising HM
 *        AttenteHM --> Default : after5s
 *      }
 *
 *      PompeEnArret --> MiseEnMarchePompe : BPM [HM && !AU && !DHM && !DPP]
 *      RemiseEnService -> PompeEnArret
 *      RemiseEnService <- Default : Acq
 *      PompeEnArret <-- PompeEnMarche : AU
 *      PompeEnArret <-- PompeEnMarche : BPA
 *      PompeEnArret <-- PompeEnMarche : DPP
 *
 *  Real timeouts are 40s / 20s / 5s. To keep the demo short, every delay is
 *  scaled down by a factor of 10 (`SIM_DIV`) so the run takes a few seconds.
 *
 *  Scenario:
 *    1. Press BPM with HM held       -> pump starts.
 *    2. Wait ~2 s (= 20 s real time) -> pump still running.
 *    3. Press BPA                    -> pump stops.
 *    4. Press BPM again              -> pump restarts.
 *    5. Release HM (falling HM)      -> AttenteHM, alarm type 2.
 *    6. Press BPM (no effect, dead)  -> stays in AttenteHM.
 *    7. Press HM (rising HM)         -> back to Attente40Secondes.
 */

#include "Statechart/Event.hpp"
#include "Statechart/HierarchicalState.hpp"
#include "Statechart/Metadata.hpp"
#include "Statechart/Parameter.hpp"
#include "Statechart/PseudoState.hpp"
#include "Statechart/State.hpp"
#include "Statechart/Statechart.hpp"
#include "Statechart/TimeoutEvent.hpp"
#include "Statechart/Transition.hpp"

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
    bool DHM = false; ///< HM defect.
    bool DPP = false; ///< Water defect.
};

#define DECLARE_EVENT(name, label)                                             \
    class name##Event: public Event                                            \
    {                                                                          \
    public:                                                                    \
        name##Event() : Event(label) {}                                        \
    }

DECLARE_EVENT(BPM, "BPM");
DECLARE_EVENT(BPA, "BPA");
DECLARE_EVENT(AU, "AU");
DECLARE_EVENT(DPP, "DPP");
DECLARE_EVENT(FallingHM, "falling_HM");
DECLARE_EVENT(RisingHM, "rising_HM");
DECLARE_EVENT(Acq, "Acq");

#undef DECLARE_EVENT

Action marchePompe()
{
    return [](Metadata&, Parameter&) {
        std::cout << "  >> marchePompe()\n";
    };
}

Action arretPompe()
{
    return
        [](Metadata&, Parameter&) { std::cout << "  >> arretPompe()\n"; };
}

Action alarme(int p_type)
{
    return [p_type](Metadata&, Parameter&) {
        std::cout << "  >> alarmeType" << p_type << "()\n";
    };
}

Guard canStart()
{
    return [](Metadata const&, Parameter const& p_param) {
        const auto& pp = static_cast<const PompeParameter&>(p_param);
        return pp.HM && !pp.AU && !pp.DHM && !pp.DPP;
    };
}

void waitMs(std::int64_t p_ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(p_ms));
}

} // namespace

int main()
{
    auto chart = std::make_unique<Statechart>("Pompe", 2, false);

    auto* start = chart->create<PseudoState>(
        "start", chart.get(), PseudoStateType::Start);

    auto* pompeArret = chart->create<State>(
        "PompeEnArret",
        chart.get(),
        [](Metadata&, Parameter&) {
            std::cout << "[PompeEnArret] pump idle\n";
        });

    auto* defaultState = chart->create<State>(
        "Default",
        chart.get(),
        arretPompe(),
        alarme(3));

    auto* remiseService = chart->create<State>(
        "RemiseEnService",
        chart.get(),
        [](Metadata&, Parameter&) {
            std::cout << "[RemiseEnService] returning to service\n";
        });

    auto* pompeMarche = chart->create<HierarchicalState>(
        "PompeEnMarche",
        chart.get(),
        [](Metadata&, Parameter&) {
            std::cout << "[PompeEnMarche] pump running\n";
        });

    auto* attenteHmHier = chart->create<HierarchicalState>(
        "AttenteLiberationHM", pompeMarche);

    auto* mehStart = chart->create<PseudoState>(
        "mehStart", attenteHmHier, PseudoStateType::Start);

    auto* miseEnMarche = chart->create<State>(
        "MiseEnMarchePompe",
        attenteHmHier,
        marchePompe());

    auto* attente40 = chart->create<State>(
        "Attente40Secondes",
        attenteHmHier,
        [](Metadata&, Parameter&) {
            std::cout << "  [Attente40Secondes]\n";
        });

    auto* attente20 = chart->create<State>(
        "Attente20Secondes",
        attenteHmHier,
        [](Metadata&, Parameter&) {
            std::cout << "  [Attente20Secondes] entering alarm window\n";
        },
        alarme(1));

    auto* attenteHM = chart->create<State>(
        "AttenteHM",
        pompeMarche,
        [](Metadata&, Parameter&) {
            std::cout << "  [AttenteHM] HM released, type-2 alarm\n";
        },
        alarme(2));

    chart->createTransition(start, pompeArret);

    chart->createTransition(pompeArret,
                            miseEnMarche,
                            chart->createEvent<BPMEvent>(),
                            canStart());

    chart->createTransition(mehStart, miseEnMarche);
    chart->createTransition(miseEnMarche, attente40);

    // The original 40 s / 20 s / 5 s timeouts are scaled down by SIM_DIV so
    // the demo run completes in a few seconds.
    chart->createTransition(attente40,
                            attente20,
                            chart->createEvent<TimeoutEvent>(TIMEOUT_40S_MS));
    chart->createTransition(attente20,
                            defaultState,
                            chart->createEvent<TimeoutEvent>(TIMEOUT_20S_MS));

    chart->createTransition(
        attenteHmHier, attenteHM, chart->createEvent<FallingHMEvent>());
    chart->createTransition(
        attenteHM, attente40, chart->createEvent<RisingHMEvent>());
    chart->createTransition(attenteHM,
                            defaultState,
                            chart->createEvent<TimeoutEvent>(TIMEOUT_5S_MS));

    chart->createTransition(
        pompeMarche, pompeArret, chart->createEvent<AUEvent>());
    chart->createTransition(
        pompeMarche, pompeArret, chart->createEvent<BPAEvent>());
    chart->createTransition(
        pompeMarche, pompeArret, chart->createEvent<DPPEvent>());

    chart->createTransition(
        defaultState, remiseService, chart->createEvent<AcqEvent>());
    chart->createTransition(remiseService, pompeArret);

    PompeParameter parameter;
    Metadata data;

    std::cout << "=== Dead-Man pump controller (timeouts /"
              << SIM_DIV << ") ===\n";
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

    std::cout << "\n>> [2] Wait a bit so the pump is well in Attente40s\n";
    waitMs(200);

    std::cout << "\n>> [2] Operator releases HM (falling HM): alarm type 2\n";
    parameter.HM = false;
    chart->dispatch(data, &fallingHM, parameter);

    std::cout << "\n>> [2] Press BPM (does nothing: we are not in PompeEnArret)\n";
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
