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

#include "CppStatecharts/forward.hpp"

#include <string>

namespace statechart {

/**
 * @brief Polymorphic base class for events dispatched into the statechart.
 *
 * An event carries an identifier used by the default @c equals implementation
 * to decide whether a transition's trigger matches the dispatched event.
 * Subclasses may override @c equals to implement domain-specific matching
 * (for example @c TimeoutEvent compares by UUID).
 */
class Event
{
public:

    Event() = default;
    explicit Event(std::string p_id);
    virtual ~Event() = default;

    Event(const Event&) = default;
    Event& operator=(const Event&) = default;
    Event(Event&&) noexcept = default;
    Event& operator=(Event&&) noexcept = default;

    /**
     * @brief Compares this event with @p p_event to decide if a transition
     *        triggered by @c this should consume @p p_event.
     *
     * The default behaviour matches by identifier string. Subclasses may
     * override this to implement custom matching semantics.
     *
     * @param p_event The event being dispatched (may be @c nullptr).
     * @param p_data  The runtime data of the statechart.
     * @param p_param The parameter forwarded by the caller.
     * @return @c true if the events should be considered equal.
     */
    virtual bool
    equals(const Event* p_event, Metadata& p_data, Parameter& p_param) const;

    /** @brief Returns the identifier of this event. */
    const std::string& id() const
    {
        return m_id;
    }

    /** @brief Returns the identifier as a @c std::string (parity with Java
     * toString). */
    std::string toString() const
    {
        return m_id;
    }

private:

    std::string m_id;
};

} // namespace statechart
