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

#include <vector>

namespace statechart {

/**
 * @brief UML AND-composite (concurrent) state composed of several regions.
 *
 * Each direct @c HierarchicalState child registers itself as a region in
 * its constructor. When activated, every region is activated; when an
 * event is dispatched, every region tries to consume it.
 */
class ConcurrentState: public Context
{
public:

    ConcurrentState(std::string p_name,
                    Context* p_parent,
                    Statechart* p_chart,
                    Action p_entryAction = {},
                    Action p_doAction = {},
                    Action p_exitAction = {});

    ~ConcurrentState() override = default;

    /** @brief Registers a hierarchical region. Called by @c HierarchicalState.
     */
    void addRegion(HierarchicalState* p_region);

    /** @brief Returns the list of regions of this concurrent state. */
    const std::vector<HierarchicalState*>& regions() const
    {
        return m_regions;
    }

    bool activate(Metadata& p_data, Parameter& p_param) override;
    void deactivate(Metadata& p_data, Parameter& p_param) override;
    bool
    dispatch(Metadata& p_data, Event* p_event, Parameter& p_param) override;

private:

    /** @brief Returns @c true if every region has reached a @c FinalState. */
    bool finished(Metadata& p_data) const;

private:

    std::vector<HierarchicalState*> m_regions;
};

} // namespace statechart
