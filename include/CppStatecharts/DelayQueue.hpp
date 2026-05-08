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

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <optional>
#include <vector>

namespace statechart {

/**
 * @brief Time-ordered blocking queue, equivalent to Java's
 *        @c java.util.concurrent.DelayQueue.
 *
 * Elements are stored in a vector kept ordered by deadline (smallest first).
 * @c take() blocks until the head of the queue is past its deadline or
 * @c stop() is called.
 *
 * The element type @c T must expose:
 *   - @c std::chrono::steady_clock::time_point deadline() const;
 *
 * The queue is a thin facade over @c std::vector with custom ordering;
 * the implementation favours simplicity (linear @c remove) over a heap
 * because the original Java code also performs explicit removals which
 * are awkward with @c std::priority_queue.
 */
template <typename T>
class DelayQueue
{
public:

    DelayQueue() = default;
    ~DelayQueue() = default;

    DelayQueue(const DelayQueue&) = delete;
    DelayQueue& operator=(const DelayQueue&) = delete;
    DelayQueue(DelayQueue&&) = delete;
    DelayQueue& operator=(DelayQueue&&) = delete;

    /** @brief Returns @c true if the queue currently holds no element. */
    bool empty() const
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        return m_items.empty();
    }

    /** @brief Returns the number of queued elements. */
    std::size_t size() const
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        return m_items.size();
    }

    /**
     * @brief Adds @p p_value to the queue.
     *
     * The queue keeps items sorted by deadline; insertion is logarithmic
     * for the search and linear for the actual move.
     */
    void add(T p_value)
    {
        {
            std::lock_guard<std::mutex> lk(m_mutex);
            const auto deadline = p_value->deadline();
            auto it = std::lower_bound(
                m_items.begin(),
                m_items.end(),
                deadline,
                [](const T& p_a,
                   const std::chrono::steady_clock::time_point& p_b) {
                    return p_a->deadline() < p_b;
                });
            m_items.insert(it, std::move(p_value));
        }
        m_condition.notify_all();
    }

    /**
     * @brief Removes the first occurrence of @p p_value from the queue,
     *        comparing with operator==.
     */
    void remove(const T& p_value)
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        for (auto it = m_items.begin(); it != m_items.end(); ++it)
        {
            if (*it == p_value)
            {
                m_items.erase(it);
                return;
            }
        }
    }

    /** @brief Removes all queued elements. */
    void clear()
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_items.clear();
    }

    /**
     * @brief Blocks until an element's deadline expires or the queue is
     *        stopped, then returns it.
     *
     * @return The expired element, or @c std::nullopt when the queue has
     *         been stopped while waiting.
     */
    std::optional<T> take()
    {
        std::unique_lock<std::mutex> lk(m_mutex);
        while (true)
        {
            if (m_stop)
            {
                return std::nullopt;
            }
            if (m_items.empty())
            {
                m_condition.wait(lk);
                continue;
            }
            const auto now = std::chrono::steady_clock::now();
            const auto deadline = m_items.front()->deadline();
            if (now >= deadline)
            {
                T value = std::move(m_items.front());
                m_items.erase(m_items.begin());
                return value;
            }
            m_condition.wait_until(lk, deadline);
        }
    }

    /**
     * @brief Wakes up @c take() callers and forces them to return
     *        @c std::nullopt regardless of pending elements.
     */
    void stop()
    {
        {
            std::lock_guard<std::mutex> lk(m_mutex);
            m_stop = true;
        }
        m_condition.notify_all();
    }

    /** @brief Resets the stop flag so the queue can be reused. */
    void resume()
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_stop = false;
    }

private:

    mutable std::mutex m_mutex;
    std::condition_variable m_condition;
    std::vector<T> m_items;
    bool m_stop = false;
};

} // namespace statechart
