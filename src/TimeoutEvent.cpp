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

#include "CppStatecharts/TimeoutEvent.hpp"

#include <random>

namespace statechart {

namespace {

/**
 * @brief Generates a fresh 128-bit value used as a UUID equivalent.
 *
 * The Java implementation uses @c java.util.UUID.randomUUID(); we mimic the
 * same property of "extremely unlikely to collide" with two thread-local 64-bit
 * Mersenne twisters seeded from @c std::random_device.
 */
void generateUuid(std::uint64_t& p_high, std::uint64_t& p_low)
{
    thread_local std::mt19937_64 rng{std::random_device{}()};
    p_high = rng();
    p_low = rng();
}

} // namespace

TimeoutEvent::TimeoutEvent(std::int64_t p_timeout)
    : Event("TimeoutEvent"), m_timeout(p_timeout)
{
    generateUuid(m_uuidHigh, m_uuidLow);
}

bool TimeoutEvent::equals(const Event* p_event,
                          Metadata& /*p_data*/,
                          Parameter& /*p_param*/) const
{
    const auto* other = dynamic_cast<const TimeoutEvent*>(p_event);
    if (other == nullptr)
    {
        return false;
    }
    return other->m_uuidHigh == m_uuidHigh && other->m_uuidLow == m_uuidLow;
}

} // namespace statechart
