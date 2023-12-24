/*
 * UML statechart framework (https://github.com/Lecrapouille/CppStatecharts)
 * Copyright (c) 2026 Quentin Quadrat (lecrapouille@gmail.com)
 *
 * Based on the Java UML statechart framework
 * (https://github.com/klangfarbe/UML-Statechart-Framework-for-Java)
 * Copyright (C) 2006-2013 Christian Mocek (christian.mocek@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

#pragma once

#include "Statechart/Transition.hpp"

namespace statechart {

/**
 * @brief Self-transition that runs an action without exiting/re-entering
 *        the source state.
 *
 * Internal transitions must always carry a triggering event. They never
 * touch the entry/exit actions of the state, only the supplied action.
 */
class InternalTransition: public Transition
{
public:

    InternalTransition(State* p_state, Event* p_event, Action p_action);
    InternalTransition(State* p_state,
                       Event* p_event,
                       Guard p_guard,
                       Action p_action);

    ~InternalTransition() override = default;

    bool execute(Event* p_event, Metadata& p_data, Parameter& p_param) override;
    bool allowed(Metadata& p_data, Parameter& p_param) override;
};

} // namespace statechart
