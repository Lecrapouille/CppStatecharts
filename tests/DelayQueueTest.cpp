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

#include "CppStatecharts/DelayQueue.hpp"

#include <chrono>
#include <gtest/gtest.h>
#include <memory>
#include <thread>

namespace statechart::tests {

struct TestDelayedItem
{
    int value;
    std::chrono::steady_clock::time_point m_deadline;

    explicit TestDelayedItem(
        int v,
        std::chrono::milliseconds delay = std::chrono::milliseconds(0))
        : value(v), m_deadline(std::chrono::steady_clock::now() + delay)
    {
    }

    std::chrono::steady_clock::time_point deadline() const
    {
        return m_deadline;
    }

    bool operator==(const TestDelayedItem& other) const
    {
        return value == other.value;
    }
};

using TestItem = std::shared_ptr<TestDelayedItem>;

// WHEN a DelayQueue is default-constructed
// EXPECT it to be empty
TEST(DelayQueueTest, DefaultConstructor)
{
    DelayQueue<TestItem> queue;

    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0u);
}

// WHEN an item is added to the queue
// EXPECT the size to increase
TEST(DelayQueueTest, AddItem)
{
    DelayQueue<TestItem> queue;
    auto item = std::make_shared<TestDelayedItem>(1);

    queue.add(item);

    EXPECT_FALSE(queue.empty());
    EXPECT_EQ(queue.size(), 1u);
}

// WHEN multiple items are added to the queue
// EXPECT the size to reflect the number of items
TEST(DelayQueueTest, AddMultipleItems)
{
    DelayQueue<TestItem> queue;

    queue.add(std::make_shared<TestDelayedItem>(1));
    queue.add(std::make_shared<TestDelayedItem>(2));
    queue.add(std::make_shared<TestDelayedItem>(3));

    EXPECT_EQ(queue.size(), 3u);
}

// WHEN an item is removed from the queue
// EXPECT the size to decrease
TEST(DelayQueueTest, RemoveItem)
{
    DelayQueue<TestItem> queue;
    auto item1 = std::make_shared<TestDelayedItem>(1);
    auto item2 = std::make_shared<TestDelayedItem>(2);

    queue.add(item1);
    queue.add(item2);

    EXPECT_EQ(queue.size(), 2u);

    queue.remove(item1);

    EXPECT_EQ(queue.size(), 1u);
}

// WHEN the queue is cleared
// EXPECT it to become empty
TEST(DelayQueueTest, ClearQueue)
{
    DelayQueue<TestItem> queue;

    queue.add(std::make_shared<TestDelayedItem>(1));
    queue.add(std::make_shared<TestDelayedItem>(2));
    queue.add(std::make_shared<TestDelayedItem>(3));

    EXPECT_EQ(queue.size(), 3u);

    queue.clear();

    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0u);
}

// WHEN an expired item is taken from the queue
// EXPECT take() to return the item immediately
TEST(DelayQueueTest, TakeExpiredItem)
{
    DelayQueue<TestItem> queue;
    auto item =
        std::make_shared<TestDelayedItem>(42, std::chrono::milliseconds(-100));

    queue.add(item);

    auto result = queue.take();

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ((*result)->value, 42);
    EXPECT_TRUE(queue.empty());
}

// WHEN take() is called on an empty queue and then stop() is called
// EXPECT take() to return nullopt
TEST(DelayQueueTest, TakeOnStoppedQueue)
{
    DelayQueue<TestItem> queue;

    std::thread taker([&queue]() {
        auto result = queue.take();
        EXPECT_FALSE(result.has_value());
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    queue.stop();

    taker.join();
}

// WHEN the queue is stopped and then resumed
// EXPECT it to be usable again
TEST(DelayQueueTest, StopAndResume)
{
    DelayQueue<TestItem> queue;

    queue.stop();
    queue.resume();

    auto item =
        std::make_shared<TestDelayedItem>(10, std::chrono::milliseconds(-50));
    queue.add(item);

    auto result = queue.take();

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ((*result)->value, 10);
}

// WHEN items are added with different deadlines
// EXPECT take() to return them in deadline order
TEST(DelayQueueTest, OrderByDeadline)
{
    DelayQueue<TestItem> queue;

    auto item1 =
        std::make_shared<TestDelayedItem>(1, std::chrono::milliseconds(300));
    auto item2 =
        std::make_shared<TestDelayedItem>(2, std::chrono::milliseconds(100));
    auto item3 =
        std::make_shared<TestDelayedItem>(3, std::chrono::milliseconds(200));

    queue.add(item1);
    queue.add(item2);
    queue.add(item3);

    auto result1 = queue.take();
    ASSERT_TRUE(result1.has_value());
    EXPECT_EQ((*result1)->value, 2);

    auto result2 = queue.take();
    ASSERT_TRUE(result2.has_value());
    EXPECT_EQ((*result2)->value, 3);

    auto result3 = queue.take();
    ASSERT_TRUE(result3.has_value());
    EXPECT_EQ((*result3)->value, 1);
}

// WHEN take() is called on a non-empty queue with future deadlines
// EXPECT take() to block until the earliest deadline
TEST(DelayQueueTest, TakeBlocksUntilDeadline)
{
    DelayQueue<TestItem> queue;
    auto item =
        std::make_shared<TestDelayedItem>(99, std::chrono::milliseconds(150));

    queue.add(item);

    auto start = std::chrono::steady_clock::now();
    auto result = queue.take();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ((*result)->value, 99);
    EXPECT_GE(elapsed.count(), 100);
}

// WHEN a non-existent item is removed
// EXPECT the queue to remain unchanged
TEST(DelayQueueTest, RemoveNonExistentItem)
{
    DelayQueue<TestItem> queue;
    auto item1 = std::make_shared<TestDelayedItem>(1);
    auto item2 = std::make_shared<TestDelayedItem>(2);

    queue.add(item1);

    EXPECT_EQ(queue.size(), 1u);

    queue.remove(item2);

    EXPECT_EQ(queue.size(), 1u);
}

} // namespace statechart::tests
