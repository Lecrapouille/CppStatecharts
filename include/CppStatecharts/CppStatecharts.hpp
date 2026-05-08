/*
 * UML statechart framework (https://github.com/Lecrapouille/CppStatecharts)
 * Copyright (c) 2026 Quentin Quadrat (lecrapouille@gmail.com)
 *
 * Based on the Java UML statechart framework
 * (https://github.com/klangfarbe/UML-Statechart-Framework-for-Java)
 * Copyright (C) 2006-2013 Christian Mocek (christian.mocek@gmail.com)
 */

#pragma once

/**
 * @file CppStatecharts.hpp
 * @brief Umbrella header including every public API header of this library.
 *
 * Client code may depend on this single include:
 *
 * @code
 * #include <CppStatecharts/CppStatecharts.hpp>
 * @endcode
 */

#include <CppStatecharts/forward.hpp>
#include <CppStatecharts/StatechartException.hpp>
#include <CppStatecharts/Action.hpp>
#include <CppStatecharts/Guard.hpp>
#include <CppStatecharts/Event.hpp>
#include <CppStatecharts/TimeoutEvent.hpp>
#include <CppStatecharts/Parameter.hpp>
#include <CppStatecharts/StateRuntimedata.hpp>
#include <CppStatecharts/Metadata.hpp>
#include <CppStatecharts/State.hpp>
#include <CppStatecharts/FinalState.hpp>
#include <CppStatecharts/PseudoState.hpp>
#include <CppStatecharts/Condition.hpp>
#include <CppStatecharts/Context.hpp>
#include <CppStatecharts/HierarchicalState.hpp>
#include <CppStatecharts/ConcurrentState.hpp>
#include <CppStatecharts/Transition.hpp>
#include <CppStatecharts/InternalTransition.hpp>
#include <CppStatecharts/EventQueueEntry.hpp>
#include <CppStatecharts/DelayQueue.hpp>
#include <CppStatecharts/ThreadPool.hpp>
#include <CppStatecharts/Statechart.hpp>
