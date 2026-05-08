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

#include "CppStatecharts/Metadata.hpp"

#include "CppStatecharts/FinalState.hpp"
#include "CppStatecharts/PseudoState.hpp"
#include "CppStatecharts/State.hpp"
#include "CppStatecharts/Statechart.hpp"
#include "CppStatecharts/StatechartException.hpp"

#include <chrono>
#include <utility>

namespace statechart {

bool Metadata::isActive(State* p_state) const
{
    auto it = m_activeStates.find(p_state);
    if (it == m_activeStates.end())
    {
        return false;
    }
    return it->second->active;
}

bool Metadata::isActive(const std::string& p_name) const
{
    if (m_statechart == nullptr)
    {
        throw StatechartException(
            "Cannot resolve state name without an active Statechart");
    }
    return isActive(m_statechart->getStateByName(p_name));
}

StateRuntimedata* Metadata::getData(State* p_state)
{
    auto it = m_activeStates.find(p_state);
    if (it == m_activeStates.end())
    {
        return nullptr;
    }
    return it->second.get();
}

void Metadata::activate(State* p_state)
{
    if (auto* chart = dynamic_cast<Statechart*>(p_state))
    {
        m_statechart = chart;
    }

    auto it = m_activeStates.find(p_state);
    if (it == m_activeStates.end())
    {
        auto inserted = m_activeStates.emplace(
            p_state, std::make_unique<StateRuntimedata>());
        it = inserted.first;
    }
    StateRuntimedata* data = it->second.get();
    data->active = true;
    data->currentTime = std::chrono::steady_clock::now();
    data->currentState = nullptr;

    if (p_state->context() != nullptr)
    {
        auto parentIt = m_activeStates.find(p_state->context());
        if (parentIt != m_activeStates.end())
        {
            parentIt->second->currentState = p_state;
        }
    }

    notifyActivate(p_state);
}

void Metadata::deactivate(State* p_state)
{
    auto it = m_activeStates.find(p_state);
    if (it == m_activeStates.end())
    {
        return;
    }
    StateRuntimedata* data = it->second.get();

    // Keep history pseudo-states across deactivation to preserve their
    // recorded substate chain.
    if (auto* ps = dynamic_cast<PseudoState*>(p_state))
    {
        if (ps->type() == PseudoStateType::DeepHistory ||
            ps->type() == PseudoStateType::History)
        {
            data->active = false;
            return;
        }
    }

    data->timeoutEvents.clear();
    data->currentState = nullptr;
    m_activeStates.erase(it);
    notifyDeactivate(p_state);
}

StateRuntimedata* Metadata::createRuntimedata(State* p_state)
{
    auto it = m_activeStates.find(p_state);
    if (it == m_activeStates.end())
    {
        auto inserted = m_activeStates.emplace(
            p_state, std::make_unique<StateRuntimedata>());
        it = inserted.first;
    }
    return it->second.get();
}

void Metadata::reset()
{
    m_activeStates.clear();
    m_statechart = nullptr;
}

std::unordered_set<State*> Metadata::getActiveStates() const
{
    std::unordered_set<State*> result;
    result.reserve(m_activeStates.size());
    for (const auto& [state, _] : m_activeStates)
    {
        result.insert(state);
    }
    return result;
}

bool Metadata::isRunning() const
{
    bool running = true;
    for (const auto& [state, _] : m_activeStates)
    {
        if (dynamic_cast<Statechart*>(state) != nullptr)
        {
            continue;
        }
        if (dynamic_cast<PseudoState*>(state) != nullptr)
        {
            continue;
        }
        const bool isFinal = dynamic_cast<FinalState*>(state) != nullptr;
        running = running && !isFinal;
    }
    return running;
}

void Metadata::addActivateObserver(StateObserver p_observer)
{
    if (p_observer)
    {
        m_activateObservers.push_back(std::move(p_observer));
    }
}

void Metadata::addDeactivateObserver(StateObserver p_observer)
{
    if (p_observer)
    {
        m_deactivateObservers.push_back(std::move(p_observer));
    }
}

void Metadata::notifyActivate(State* p_state)
{
    for (const auto& obs : m_activateObservers)
    {
        obs(p_state);
    }
}

void Metadata::notifyDeactivate(State* p_state)
{
    for (const auto& obs : m_deactivateObservers)
    {
        obs(p_state);
    }
}

} // namespace statechart
