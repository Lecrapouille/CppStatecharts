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

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace statechart {

/**
 * @brief Minimal fixed-size thread pool used by @c Statechart.
 *
 * Replaces the Java @c ExecutorService used in the original implementation.
 * Tasks are arbitrary @c std::function<void()> callables; the pool keeps
 * @c size workers alive until @c shutdown() is called or the pool is
 * destroyed.
 */
class ThreadPool
{
public:

    /**
     * @brief Creates a pool with @p p_threads worker threads.
     *
     * @param p_threads Number of workers (clamped to at least 1).
     */
    explicit ThreadPool(std::size_t p_threads);

    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    /**
     * @brief Schedules @p p_task for asynchronous execution.
     *
     * @return @c true if the task was accepted, @c false if the pool has
     *         been shut down.
     */
    bool execute(std::function<void()> p_task);

    /**
     * @brief Returns @c true if @c shutdown() has been called.
     */
    bool isShutdown() const
    {
        return m_shutdown.load(std::memory_order_acquire);
    }

    /**
     * @brief Refuses new tasks and signals workers to exit once the queue
     *        is drained.
     */
    void shutdown();

    /**
     * @brief Waits for the workers to finish, up to @p p_timeout.
     *
     * @return @c true if all workers terminated within the timeout.
     */
    bool awaitTermination(std::chrono::milliseconds p_timeout);

private:

    void workerLoop();

    //@brief Mutex protecting the task queue.
    mutable std::mutex m_mutex;
    //@brief Condition variable to notify workers of new tasks.
    std::condition_variable m_condition;
    //@brief Queue of tasks to be executed.
    std::queue<std::function<void()>> m_tasks;
    //@brief Vector of worker threads.
    std::vector<std::jthread> m_workers;
    //@brief Atomic flag to indicate if the pool has been shut down.
    std::atomic<bool> m_shutdown{false};
};

} // namespace statechart
