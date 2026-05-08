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

#include "CppStatecharts/StateRuntimedata.hpp"
#include "CppStatecharts/forward.hpp"

#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace statechart {

/**
 * @brief Holds runtime-specific data for a statechart instance.
 *
 * The framework allows several @c Metadata objects to be evaluated in
 * parallel by a single @c Statechart, each carrying its own set of active
 * states and per-state runtime information.
 *
 * The class also exposes a recursive mutex used by @c Statechart::dispatch
 * to serialize event handling on a given metadata, which is the C++
 * equivalent of the Java @c synchronized(data) blocks.
 */
class Metadata
{
public:

    /** @brief Callback type for activate/deactivate observers. */
    using StateObserver = std::function<void(State* p_state)>;

    Metadata() = default;
    ~Metadata() = default;

    Metadata(const Metadata&) = delete;
    Metadata& operator=(const Metadata&) = delete;
    Metadata(Metadata&&) = delete;
    Metadata& operator=(Metadata&&) = delete;

    /** @brief Returns @c true if @p p_state is currently active. */
    bool isActive(State* p_state) const;

    /**
     * @brief Returns @c true if the state named @p p_name is currently active.
     *
     * @throws StatechartException if the name is unknown to the statechart.
     */
    bool isActive(const std::string& p_name) const;

    /**
     * @brief Returns the runtime data for @p p_state, or @c nullptr if the
     *        state is not active.
     */
    StateRuntimedata* getData(State* p_state);

    /** @brief Resets the metadata so it can be reused. */
    void reset();

    /** @brief Returns the set of currently-active states. */
    std::unordered_set<State*> getActiveStates() const;

    /** @brief Returns @c true if any non-final, non-pseudo, state is active. */
    bool isRunning() const;

    /** @brief Registers an observer invoked on each state activation. */
    void addActivateObserver(StateObserver p_observer);

    /** @brief Registers an observer invoked on each state deactivation. */
    void addDeactivateObserver(StateObserver p_observer);

    /**
     * @brief Returns the recursive mutex serializing dispatch on this
     *        metadata. Used by @c Statechart::dispatch.
     */
    std::recursive_mutex& mutex() const
    {
        return m_mutex;
    }

    /**
     * @brief Activates @p p_state and refreshes the runtime data.
     * @note Framework-internal.
     */
    void activate(State* p_state);

    /**
     * @brief Deactivates @p p_state, freeing its runtime data when allowed.
     * @note Framework-internal.
     */
    void deactivate(State* p_state);

    /**
     * @brief Returns the runtime data for @p p_state, creating one if
     *        none exists yet (used by fork/concurrent activation).
     * @note Framework-internal.
     */
    StateRuntimedata* createRuntimedata(State* p_state);

private:

    void notifyActivate(State* p_state);
    void notifyDeactivate(State* p_state);

private:

    Statechart* m_statechart = nullptr;
    std::unordered_map<State*, std::unique_ptr<StateRuntimedata>>
        m_activeStates;
    std::vector<StateObserver> m_activateObservers;
    std::vector<StateObserver> m_deactivateObservers;
    mutable std::recursive_mutex m_mutex;

    friend class State;
    friend class PseudoState;
    friend class HierarchicalState;
    friend class ConcurrentState;
    friend class Transition;
    friend class InternalTransition;
    friend class Statechart;
    friend class EventQueueEntry;
};

} // namespace statechart
