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

#include "Statechart/Context.hpp"

namespace statechart {

/**
 * @brief UML OR-composite (hierarchical) state.
 *
 * Has at most one active substate at a time. Optionally tracks a history
 * pseudo-state used to restore the previously active substate on re-entry.
 */
class HierarchicalState: public Context
{
public:

    HierarchicalState(std::string p_name,
                      Context* p_parent,
                      Statechart* p_chart,
                      Action p_entryAction = {},
                      Action p_doAction = {},
                      Action p_exitAction = {});

    ~HierarchicalState() override = default;

    /** @brief Returns the configured history pseudo-state, if any. */
    PseudoState* history() const
    {
        return m_history;
    }

    void deactivate(Metadata& p_data, Parameter& p_param) override;
    bool
    dispatch(Metadata& p_data, Event* p_event, Parameter& p_param) override;

protected:

    PseudoState* m_history = nullptr;

    friend class PseudoState;
};

} // namespace statechart
