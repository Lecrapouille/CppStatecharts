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
 * @file simple_fsm.cpp
 * @brief Cheat-sheet showing all the syntax the framework can parse.
 *
 * Equivalent statechart (PlantUML):
 *
 *     [*] -> State1
 *     State1 --> State2 : event1 [guard1] / action1()
 *     State1 <-- State2 : event2
 *     State2 -> [*] : event6
 *
 *     State1 : entry / action7()
 *     State1 : exit  / action8()
 *     State1 : on event3 [guard3] / action3()
 *
 *     State2 : entry / action9()
 *     State2 : exit  / action10()
 *     State2 : on event5 / action5()
 */

#include <CppStatecharts/CppStatecharts.hpp>

#include <iostream>
#include <memory>

using namespace statechart;

namespace {

class FsmParameter: public Parameter
{
public:

    bool guard1 = true;
    bool guard3 = true;
};

#define DECLARE_EVENT(name)                                                    \
    class name##Event: public Event                                            \
    {                                                                          \
    public:                                                                    \
        name##Event() : Event(#name) {}                                        \
    }

DECLARE_EVENT(event1);
DECLARE_EVENT(event2);
DECLARE_EVENT(event3);
DECLARE_EVENT(event5);
DECLARE_EVENT(event6);

#undef DECLARE_EVENT

#define LOG_ACTION(name)                                                       \
    [](Metadata&, Parameter&) { std::cout << "  [ACTION " #name "]\n"; }

Guard guard1Active()
{
    return [](Metadata const&, Parameter const& p_param) {
        return static_cast<const FsmParameter&>(p_param).guard1;
    };
}

Guard guard3Active()
{
    return [](Metadata const&, Parameter const& p_param) {
        return static_cast<const FsmParameter&>(p_param).guard3;
    };
}

} // namespace

int main()
{
    auto chart = std::make_unique<Statechart>("SimpleFSM", 2, false);

    auto* start = chart->create<PseudoState>(
        "start", chart.get(), PseudoStateType::Start);
    auto* finalSt = chart->create<FinalState>("final", chart.get());

    auto* state1 = chart->create<State>(
        "State1", chart.get(), LOG_ACTION(action7), Action{}, LOG_ACTION(action8));

    auto* state2 = chart->create<State>(
        "State2", chart.get(), LOG_ACTION(action9), Action{}, LOG_ACTION(action10));

    chart->createTransition(start, state1);

    chart->createTransition(state1,
                            state2,
                            chart->createEvent<event1Event>(),
                            guard1Active(),
                            LOG_ACTION(action1));

    chart->createTransition(
        state2, state1, chart->createEvent<event2Event>());

    chart->createTransition(
        state2, finalSt, chart->createEvent<event6Event>());

    chart->createInternalTransition(state1,
                                    chart->createEvent<event3Event>(),
                                    guard3Active(),
                                    LOG_ACTION(action3));

    chart->createInternalTransition(
        state2, chart->createEvent<event5Event>(), LOG_ACTION(action5));

    FsmParameter parameter;
    Metadata data;

    std::cout << "=== SimpleFSM cheat-sheet ===\n";
    chart->start(data, parameter);

    event3Event e3;
    event1Event e1;
    event5Event e5;
    event2Event e2;
    event6Event e6;

    std::cout << "\n>> event3 (internal transition on State1)\n";
    chart->dispatch(data, &e3, parameter);

    std::cout << "\n>> event1 (State1 -> State2)\n";
    chart->dispatch(data, &e1, parameter);

    std::cout << "\n>> event5 (internal transition on State2)\n";
    chart->dispatch(data, &e5, parameter);

    std::cout << "\n>> event2 (State2 -> State1)\n";
    chart->dispatch(data, &e2, parameter);

    std::cout << "\n>> event1 then event6 to terminate\n";
    chart->dispatch(data, &e1, parameter);
    chart->dispatch(data, &e6, parameter);

    chart->shutdown();
    return 0;
}
