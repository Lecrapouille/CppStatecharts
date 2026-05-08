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
 * @file ethernet_box.cpp
 * @brief Ethernet box manager example for pairing a phone via Wi-Fi.
 *
 * Equivalent statechart (PlantUML):
 * [*] --> EthernetBoxOff
 * EthernetBoxOff --> WifiDiscoverable : wifi powered on
 * EthernetBoxOff <-- WifiDiscoverable : wifi powered off
 * EthernetBoxOff <-- WifiDisconnected : wifi powered off

 * WifiDiscoverable --> WifiConnected : pairing successful
 * WifiDiscoverable <-- WifiConnected : long button press /  pairing_phone()
 * WifiDiscoverable --> WifiDisconnected : Wifi failure
 * WifiDiscoverable <-- WifiDisconnected : long button press / pairing_phone()
 *
 * WifiConnected --> WifiDisconnected : phone Wifi disconnected
 *
 * EthernetBoxOff : comment / Not available for pairing Wifi
 * EthernetBoxOff : entry / led_off()
 * WifiDiscoverable : comment / Avaialble for pairing or automatic connection or
 * manual connection
 * WifiDiscoverable : entry / led_blinking()
 * WifiConnected : comment / Connected to phone via
 * Wifi WifiConnected : entry / led_constant_glow()
 * WifiConnected : on short button press / launch_home_screen()
 * WifiDisconnected : comment / No Wifi connected
 * WifiDisconnected : entry / led_glow()
 */

#include <CppStatecharts/CppStatecharts.hpp>

#include <iostream>
#include <memory>

using namespace statechart;

namespace {

#define DECLARE_EVENT(name, label)      \
    class name##Event: public Event     \
    {                                   \
    public:                             \
                                        \
        name##Event() : Event(label) {} \
    }

DECLARE_EVENT(WifiOn, "wifi_powered_on");
DECLARE_EVENT(WifiOff, "wifi_powered_off");
DECLARE_EVENT(PairingOk, "pairing_successful");
DECLARE_EVENT(LongBtn, "long_button_press");
DECLARE_EVENT(ShortBtn, "short_button_press");
DECLARE_EVENT(WifiFail, "wifi_failure");
DECLARE_EVENT(PhoneDisc, "phone_disconnected");

#undef DECLARE_EVENT

Action ledOff()
{
    return [](Metadata&, Parameter&) { std::cout << "  LED: off\n"; };
}
Action ledBlinking()
{
    return [](Metadata&, Parameter&) { std::cout << "  LED: blinking\n"; };
}
Action ledGlow()
{
    return [](Metadata&, Parameter&) { std::cout << "  LED: glow\n"; };
}
Action ledConstantGlow()
{
    return [](Metadata&, Parameter&) { std::cout << "  LED: constant glow\n"; };
}
Action pairingPhone()
{
    return
        [](Metadata&, Parameter&) { std::cout << "  ACTION: pairing phone\n"; };
}
Action launchHome()
{
    return [](Metadata&, Parameter&) {
        std::cout << "  ACTION: launch home screen\n";
    };
}

} // namespace

int main()
{
    auto chart = std::make_unique<Statechart>("EthernetBox", 2, false);

    auto* start = chart->create<PseudoState>(
        "start", chart.get(), PseudoStateType::Start);

    auto* boxOff =
        chart->create<State>("EthernetBoxOff", chart.get(), ledOff());

    auto* discoverable =
        chart->create<State>("WifiDiscoverable", chart.get(), ledBlinking());

    auto* connected =
        chart->create<State>("WifiConnected", chart.get(), ledConstantGlow());

    auto* disconnected =
        chart->create<State>("WifiDisconnected", chart.get(), ledGlow());

    chart->createTransition(start, boxOff);

    chart->createTransition(
        boxOff, discoverable, chart->createEvent<WifiOnEvent>());
    chart->createTransition(
        discoverable, boxOff, chart->createEvent<WifiOffEvent>());
    chart->createTransition(
        disconnected, boxOff, chart->createEvent<WifiOffEvent>());

    chart->createTransition(
        discoverable, connected, chart->createEvent<PairingOkEvent>());

    chart->createTransition(connected,
                            discoverable,
                            chart->createEvent<LongBtnEvent>(),
                            pairingPhone());

    chart->createTransition(
        discoverable, disconnected, chart->createEvent<WifiFailEvent>());

    chart->createTransition(disconnected,
                            discoverable,
                            chart->createEvent<LongBtnEvent>(),
                            pairingPhone());

    chart->createTransition(
        connected, disconnected, chart->createEvent<PhoneDiscEvent>());

    chart->createInternalTransition(
        connected, chart->createEvent<ShortBtnEvent>(), launchHome());

    Parameter parameter;
    Metadata data;

    std::cout << "=== Ethernet box manager ===\n";
    chart->start(data, parameter);

    WifiOnEvent on;
    PairingOkEvent ok;
    ShortBtnEvent shortBtn;
    PhoneDiscEvent phoneDisc;
    LongBtnEvent longBtn;
    WifiOffEvent off;

    std::cout << "\n>> Wi-Fi powered on\n";
    chart->dispatch(data, &on, parameter);

    std::cout << "\n>> Pairing successful\n";
    chart->dispatch(data, &ok, parameter);

    std::cout << "\n>> Short press => launch home\n";
    chart->dispatch(data, &shortBtn, parameter);

    std::cout << "\n>> Phone disconnects\n";
    chart->dispatch(data, &phoneDisc, parameter);

    std::cout << "\n>> Long press => re-pair\n";
    chart->dispatch(data, &longBtn, parameter);

    std::cout << "\n>> Wi-Fi powered off\n";
    chart->dispatch(data, &off, parameter);

    chart->shutdown();
    return 0;
}
