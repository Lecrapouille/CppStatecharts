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

#include <functional>

namespace statechart {

/**
 * @file forward.hpp
 * @brief Common forward declarations used to break circular dependencies
 *        between headers.
 *
 * Headers in this library never include each other directly when a forward
 * declaration is enough. This file centralizes those declarations so every
 * public header has a single, cheap include for them.
 */

class State;
class Context;
class Statechart;
class HierarchicalState;
class ConcurrentState;
class FinalState;
class PseudoState;
class Condition;
class Transition;
class InternalTransition;
class Event;
class TimeoutEvent;
class Parameter;
class Metadata;
struct StateRuntimedata;
class EventQueueEntry;

/**
 * @brief Action callback executed by a state on entry/do/exit, or by a
 *        transition when it triggers.
 *
 * @param p_data  The runtime data of the statechart.
 * @param p_param The parameter forwarded by the caller.
 */
using Action = std::function<void(Metadata& p_data, Parameter& p_param)>;

/**
 * @brief Guard callback evaluated by a transition to decide if it can fire.
 *
 * @param p_data  The runtime data of the statechart.
 * @param p_param The parameter forwarded by the caller.
 * @return @c true if the transition is allowed to fire, @c false otherwise.
 */
using Guard =
    std::function<bool(Metadata const& p_data, Parameter const& p_param)>;

} // namespace statechart
