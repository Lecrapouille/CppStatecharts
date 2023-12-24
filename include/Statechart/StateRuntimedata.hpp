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

#include "Statechart/forward.hpp"

#include <chrono>
#include <vector>

namespace statechart {

/**
 * @brief Holds the runtime-specific data for an active state.
 *
 * One instance is allocated per active state inside a @c Metadata object and
 * is destroyed when the corresponding state is deactivated. Plain data: the
 * struct intentionally has no methods beyond defaults.
 */
struct StateRuntimedata
{
    /** @brief Wall-clock time at which the state became active. */
    std::chrono::steady_clock::time_point currentTime{};

    /** @brief Whether the state is currently active. */
    bool active = false;

    /** @brief The currently active substate, or @c nullptr. */
    State* currentState = nullptr;

    /**
     * @brief Generic set of states used for history storage and region
     *        activation bookkeeping.
     */
    std::vector<State*> stateset;

    /**
     * @brief Set of timeout entries currently queued for this state.
     *        Pointers into a @c Statechart-owned queue; not owned here.
     */
    std::vector<EventQueueEntry*> timeoutEvents;
};

} // namespace statechart
