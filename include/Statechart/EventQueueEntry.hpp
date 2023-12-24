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

#include <atomic>
#include <chrono>
#include <cstdint>
#include <memory>
#include <string>

namespace statechart {

/**
 * @brief Queued unit of work used by @c Statechart for both async dispatch
 *        and timeout-driven dispatch.
 *
 * Unlike the Java original which uses inheritance from @c Runnable and
 * @c Delayed, the C++ port keeps the entry as a passive data carrier and
 * exposes a @c run() method invoked by the @c ThreadPool.
 */
class EventQueueEntry
{
public:

    /**
     * @brief Creates a new queue entry.
     *
     * @param p_statechart The statechart owning this entry.
     * @param p_state      The state this entry is bound to (used to verify
     *                     the entry is still relevant when it fires).
     * @param p_data       The metadata; may be @c nullptr for the sentinel
     *                     entry pushed during shutdown.
     * @param p_event      The event to dispatch; ownership is shared.
     * @param p_param      The parameter; ownership is shared.
     * @param p_timeoutMs  Delay before the entry becomes ready, in
     *                     milliseconds.
     */
    EventQueueEntry(Statechart* p_statechart,
                    State* p_state,
                    Metadata* p_data,
                    std::shared_ptr<Event> p_event,
                    std::shared_ptr<Parameter> p_param,
                    std::int64_t p_timeoutMs);

    EventQueueEntry(const EventQueueEntry&) = delete;
    EventQueueEntry& operator=(const EventQueueEntry&) = delete;
    EventQueueEntry(EventQueueEntry&&) = delete;
    EventQueueEntry& operator=(EventQueueEntry&&) = delete;

    ~EventQueueEntry() = default;

    /**
     * @brief Marks the entry as invalid, preventing it from dispatching
     *        when the worker picks it up.
     */
    void markInvalid()
    {
        m_invalid.store(true, std::memory_order_release);
    }

    /** @brief Returns whether the entry has been invalidated. */
    bool invalid() const
    {
        return m_invalid.load(std::memory_order_acquire);
    }

    /** @brief Returns the absolute deadline at which the entry becomes ready.
     */
    std::chrono::steady_clock::time_point deadline() const
    {
        return m_deadline;
    }

    /** @brief Returns a stable id used to break ties in queue ordering. */
    std::uint64_t id() const
    {
        return m_id;
    }

    /** @brief Returns the contained event (may be @c nullptr). */
    Event* event() const
    {
        return m_event.get();
    }

    /** @brief Returns the bound metadata (may be @c nullptr). */
    Metadata* data() const
    {
        return m_data;
    }

    /** @brief Returns a debug string mirroring the Java toString. */
    std::string toString() const;

    /**
     * @brief Dispatches the contained event into the statechart, unless the
     *        entry has been invalidated or the bound state is no longer
     *        active.
     */
    void run();

private:

    static std::atomic<std::uint64_t> s_idCounter;

    Statechart* m_statechart = nullptr;
    State* m_state = nullptr;
    Metadata* m_data = nullptr;
    std::shared_ptr<Event> m_event;
    std::shared_ptr<Parameter> m_param;
    std::int64_t m_relativeTimeoutMs = 0;
    std::chrono::steady_clock::time_point m_deadline;
    std::uint64_t m_id = 0;
    std::atomic<bool> m_invalid{false};
};

} // namespace statechart
