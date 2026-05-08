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

#include "CppStatecharts/ThreadPool.hpp"

#include <utility>

namespace statechart {

ThreadPool::ThreadPool(std::size_t p_threads)
{
    if (p_threads < 1)
    {
        p_threads = 1;
    }
    m_workers.reserve(p_threads);
    for (std::size_t i = 0; i < p_threads; ++i)
    {
        m_workers.emplace_back([this] { workerLoop(); });
    }
}

ThreadPool::~ThreadPool()
{
    shutdown();
    awaitTermination(std::chrono::milliseconds{60'000});
    for (auto& t : m_workers)
    {
        if (t.joinable())
        {
            t.join();
        }
    }
}

bool ThreadPool::execute(std::function<void()> p_task)
{
    if (!p_task)
    {
        return false;
    }
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        if (m_shutdown.load(std::memory_order_acquire))
        {
            return false;
        }
        m_tasks.push(std::move(p_task));
    }
    m_condition.notify_one();
    return true;
}

void ThreadPool::shutdown()
{
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_shutdown.store(true, std::memory_order_release);
    }
    m_condition.notify_all();
}

bool ThreadPool::awaitTermination(std::chrono::milliseconds p_timeout)
{
    const auto deadline = std::chrono::steady_clock::now() + p_timeout;
    bool allFinished = true;
    for (auto& t : m_workers)
    {
        if (!t.joinable())
        {
            continue;
        }
        const auto now = std::chrono::steady_clock::now();
        if (now >= deadline)
        {
            allFinished = false;
            break;
        }
        // std::thread does not provide a timed join; we conservatively wait
        // for the deadline by polling the workers state via the condition
        // variable. The workers exit promptly once the queue is empty and
        // shutdown is set, so this is a tight loop in practice.
        std::unique_lock<std::mutex> lk(m_mutex);
        m_condition.wait_until(lk, deadline, [&] {
            return m_tasks.empty() &&
                   m_shutdown.load(std::memory_order_acquire);
        });
        lk.unlock();
        // Try to join; if the thread is still busy at this point, fall back
        // to a hard join (the destructor needs joinable workers).
        t.join();
    }
    return allFinished;
}

void ThreadPool::workerLoop()
{
    while (true)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lk(m_mutex);
            m_condition.wait(lk, [&] {
                return !m_tasks.empty() ||
                       m_shutdown.load(std::memory_order_acquire);
            });
            if (m_tasks.empty())
            {
                if (m_shutdown.load(std::memory_order_acquire))
                {
                    m_condition.notify_all();
                    return;
                }
                continue;
            }
            task = std::move(m_tasks.front());
            m_tasks.pop();
        }
        try
        {
            task();
        }
        catch (...)
        {
            // Mirror Java behaviour: a misbehaving task does not crash the
            // pool.
        }
    }
}

} // namespace statechart
