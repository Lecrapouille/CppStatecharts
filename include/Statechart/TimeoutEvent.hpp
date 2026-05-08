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

#include "Statechart/Event.hpp"

#include <cstdint>

namespace statechart {

/**
 * @brief Event that fires after a fixed delay relative to the activation of
 *        its source state.
 *
 * Each instance owns a unique identifier so that two timeout events with the
 * same delay are still distinguishable. The matching policy of @c equals
 * therefore compares unique ids rather than the event's textual id.
 */
class TimeoutEvent: public Event
{
public:

    /**
     * @brief Creates a timeout event.
     *
     * @param p_timeout Delay in milliseconds before the event fires.
     */
    explicit TimeoutEvent(std::int64_t p_timeout);

    /** @brief Returns the configured timeout in milliseconds. */
    std::int64_t timeout() const
    {
        return m_timeout;
    }

    /** @brief Sets a new timeout value (does not affect already-queued
     * entries). */
    void setTimeout(std::int64_t p_timeout)
    {
        m_timeout = p_timeout;
    }

    /**
     * @brief Compares two timeout events by their unique identifier.
     *
     * @param p_event The event being dispatched.
     * @param p_data  The runtime data of the statechart.
     * @param p_param The parameter forwarded by the caller.
     * @return @c true if @p p_event is the same @c TimeoutEvent instance
     *         as @c this (same UUID).
     */
    bool equals(const Event* p_event,
                Metadata& p_data,
                Parameter& p_param) const override;

private:

    //@brief The timeout value in milliseconds.
    std::int64_t m_timeout = 0;
    //@brief The high part of the unique identifier.
    std::uint64_t m_uuidHigh = 0;
    //@brief The low part of the unique identifier.
    std::uint64_t m_uuidLow = 0;
};

} // namespace statechart
