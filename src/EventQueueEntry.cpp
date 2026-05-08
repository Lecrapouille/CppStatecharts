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

#include "CppStatecharts/EventQueueEntry.hpp"

#include "CppStatecharts/Event.hpp"
#include "CppStatecharts/Metadata.hpp"
#include "CppStatecharts/Parameter.hpp"
#include "CppStatecharts/State.hpp"
#include "CppStatecharts/Statechart.hpp"

#include <sstream>

namespace statechart {

std::atomic<std::uint64_t> EventQueueEntry::s_idCounter{1};

EventQueueEntry::EventQueueEntry(Statechart* p_statechart,
                                 State* p_state,
                                 Metadata* p_data,
                                 std::shared_ptr<Event> p_event,
                                 std::shared_ptr<Parameter> p_param,
                                 std::int64_t p_timeoutMs)
    : m_statechart(p_statechart),
      m_state(p_state),
      m_data(p_data),
      m_event(std::move(p_event)),
      m_param(std::move(p_param)),
      m_relativeTimeoutMs(p_timeoutMs),
      m_deadline(std::chrono::steady_clock::now() +
                 std::chrono::milliseconds{p_timeoutMs}),
      m_id(s_idCounter.fetch_add(1, std::memory_order_relaxed))
{
}

void EventQueueEntry::run()
{
    if (m_invalid.load(std::memory_order_acquire))
    {
        return;
    }
    if (m_statechart == nullptr || m_data == nullptr || m_state == nullptr)
    {
        return;
    }
    if (!m_data->isActive(m_state))
    {
        return;
    }
    Parameter empty;
    Parameter& param = m_param ? *m_param : empty;
    m_statechart->dispatch(*m_data, m_event.get(), param);
}

std::string EventQueueEntry::toString() const
{
    std::ostringstream os;
    os << "EventQueueEntry [id=" << m_id
       << ", invalid=" << (m_invalid.load() ? "true" : "false")
       << ", relativeTimeoutMs=" << m_relativeTimeoutMs << "]";
    return os.str();
}

} // namespace statechart
