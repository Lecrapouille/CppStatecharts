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
 * @file lane_keeping.cpp
 * @brief Vehicle Lane Keeping System (LKS) example.
 *
 * The LKS controls four boolean flags: LED_LKS, LED_lane, LED_steering and
 * servoing. The driver presses btn_LKS to enable/disable the system, and the
 * vehicle reports lane detection events.
 *
 * Equivalent statechart (PlantUML):
 *
 * [*] --> LKSModeOff
 * LKSModeOff : comment / Line Keep System OFF
 * LKSModeOn : comment / Line Keep System ON
 * WaitDetect : comment / LKS is not detecting the lane
 * DetectLane : comment / LKS is detecting crossing the lane
 * FollowLane : comment / LKS is following the lane
 *
 * LKSModeOff --> LKSModeOn : btn_LKS \n--\n LED_LKS = Enable
 * LKSModeOn --> LKSModeOff : btn_LKS \n--\n LED_LKS = Disable
 * LKSModeOn -> DetectLane : detect \n--\n LED_lane = Enable
 *
 * DetectLane --> LKSModeOff : btn_LKS \n--\n LED_LKS = Disable; LED_lane =
 * Disable DetectLane -> WaitDetect : not detect \n--\n LED_lane = Disable
 * DetectLane --> FollowLane : set \n--\n LED_steering = Enable; servoing =
 * Enable
 *
 * FollowLane --> WaitDetect : not detect \n--\n LED_lane = Disable;
 * LED_steering = Disable; servoing = Disable FollowLane -> LKSModeOff : btn_LKS
 * \n--\n LED_LKS = Disable; LED_lane = Disable; LED_steering = Disable;
 * servoing = Disable FollowLane -> DetectLane : cancel \n--\n LED_steering =
 * Disable; servoing = Disable
 *
 * WaitDetect -> DetectLane : detect \n--\n LED_lane = Enable
 * WaitDetect -> LKSModeOff : btn_LKS \n--\n LED_LKS = Disable
 */

#include <CppStatecharts/CppStatecharts.hpp>

#include <iostream>
#include <memory>

using namespace statechart;

namespace {

class LksParameter: public Parameter
{
public:

    bool LED_LKS = false;
    bool LED_lane = false;
    bool LED_steering = false;
    bool servoing = false;

    void dump() const
    {
        std::cout << "  LKS=" << LED_LKS << " lane=" << LED_lane
                  << " steering=" << LED_steering << " servoing=" << servoing
                  << '\n';
    }
};

#define DECLARE_EVENT(name, label)      \
    class name##Event: public Event     \
    {                                   \
    public:                             \
                                        \
        name##Event() : Event(label) {} \
    }

DECLARE_EVENT(BtnLks, "btn_LKS");
DECLARE_EVENT(Detect, "detect");
DECLARE_EVENT(NotDetect, "not_detect");
DECLARE_EVENT(Set, "set");
DECLARE_EVENT(Cancel, "cancel");

#undef DECLARE_EVENT

template <bool LKS, bool LANE, bool STEERING, bool SERVOING>
Action setFlags()
{
    return [](Metadata&, Parameter& p_param) {
        auto& lp = static_cast<LksParameter&>(p_param);
        lp.LED_LKS = LKS;
        lp.LED_lane = LANE;
        lp.LED_steering = STEERING;
        lp.servoing = SERVOING;
        lp.dump();
    };
}

} // namespace

int main()
{
    auto chart = std::make_unique<Statechart>("LaneKeeping", 2, false);

    auto* start = chart->create<PseudoState>(
        "start", chart.get(), PseudoStateType::Start);

    auto* lksOff = chart->create<State>("LKSModeOff", chart.get());
    auto* lksOn = chart->create<State>("LKSModeOn", chart.get());
    auto* detectLane = chart->create<State>("DetectLane", chart.get());
    auto* followLane = chart->create<State>("FollowLane", chart.get());
    auto* waitDetect = chart->create<State>("WaitDetect", chart.get());

    chart->createTransition(start, lksOff);

    chart->createTransition(lksOff,
                            lksOn,
                            chart->createEvent<BtnLksEvent>(),
                            setFlags<true, false, false, false>());

    chart->createTransition(lksOn,
                            lksOff,
                            chart->createEvent<BtnLksEvent>(),
                            setFlags<false, false, false, false>());

    chart->createTransition(lksOn,
                            detectLane,
                            chart->createEvent<DetectEvent>(),
                            setFlags<true, true, false, false>());

    chart->createTransition(detectLane,
                            lksOff,
                            chart->createEvent<BtnLksEvent>(),
                            setFlags<false, false, false, false>());

    chart->createTransition(detectLane,
                            waitDetect,
                            chart->createEvent<NotDetectEvent>(),
                            setFlags<true, false, false, false>());

    chart->createTransition(detectLane,
                            followLane,
                            chart->createEvent<SetEvent>(),
                            setFlags<true, true, true, true>());

    chart->createTransition(followLane,
                            waitDetect,
                            chart->createEvent<NotDetectEvent>(),
                            setFlags<true, false, false, false>());

    chart->createTransition(followLane,
                            lksOff,
                            chart->createEvent<BtnLksEvent>(),
                            setFlags<false, false, false, false>());

    chart->createTransition(followLane,
                            detectLane,
                            chart->createEvent<CancelEvent>(),
                            setFlags<true, true, false, false>());

    chart->createTransition(waitDetect,
                            detectLane,
                            chart->createEvent<DetectEvent>(),
                            setFlags<true, true, false, false>());

    chart->createTransition(waitDetect,
                            lksOff,
                            chart->createEvent<BtnLksEvent>(),
                            setFlags<false, false, false, false>());

    LksParameter parameter;
    Metadata data;

    std::cout << "=== Lane Keeping System ===\n";
    chart->start(data, parameter);

    BtnLksEvent btn;
    DetectEvent detect;
    NotDetectEvent notDetect;
    SetEvent set;
    CancelEvent cancel;

    std::cout << "\n>> Driver presses btn_LKS\n";
    chart->dispatch(data, &btn, parameter);

    std::cout << "\n>> Lane detected\n";
    chart->dispatch(data, &detect, parameter);

    std::cout << "\n>> Driver activates set (follow lane)\n";
    chart->dispatch(data, &set, parameter);

    std::cout << "\n>> Lane lost\n";
    chart->dispatch(data, &notDetect, parameter);

    std::cout << "\n>> Lane re-detected\n";
    chart->dispatch(data, &detect, parameter);

    std::cout << "\n>> Driver disables LKS\n";
    chart->dispatch(data, &btn, parameter);

    chart->shutdown();
    return 0;
}
