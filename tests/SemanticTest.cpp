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

#include "Helpers.hpp"
#include "TestCharts.hpp"

#include "CppStatecharts/FinalState.hpp"
#include "CppStatecharts/Metadata.hpp"
#include "CppStatecharts/StateRuntimedata.hpp"
#include "CppStatecharts/Statechart.hpp"

#include <chrono>
#include <thread>

#include <gtest/gtest.h>

namespace statechart::tests {

namespace {

void waitForFinalState(Statechart& p_chart, Metadata& p_data)
{
    State* current = nullptr;
    while (current == nullptr || dynamic_cast<FinalState*>(current) == nullptr)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
        std::lock_guard<std::recursive_mutex> lk(p_data.mutex());
        StateRuntimedata* d = p_data.getData(&p_chart);
        current = d ? d->currentState : nullptr;
    }
}

} // namespace

TEST(Semantic, EventQueue)
{
    auto chart = TestCharts::t2();
    TestParameter parameter;
    Metadata data;

    auto e1 = std::make_shared<TestEvent>(1);
    auto e2 = std::make_shared<TestEvent>(2);
    auto param = std::make_shared<TestParameter>();

    chart->startAsynchron(data, param);
    chart->dispatchAsynchron(data, e1, param);
    chart->dispatchAsynchron(data, e2, param);
    waitForFinalState(*chart, data);
    chart->shutdown();
    EXPECT_EQ("D:start A:a D:a A:a D:a A:end", param->path);
}

TEST(Semantic, Test1)
{
    auto chart = TestCharts::t1();
    TestParameter parameter;
    Metadata data;
    EXPECT_TRUE(chart->start(data, parameter));
    EXPECT_EQ("D:start A:a D:a A:b D:b A:end", parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test2)
{
    auto chart = TestCharts::t2();
    TestEvent s1{1};
    TestEvent s2{2};
    TestParameter parameter;
    Metadata data;
    EXPECT_TRUE(chart->start(data, parameter));
    EXPECT_TRUE(chart->dispatch(data, &s1, parameter));
    EXPECT_TRUE(chart->dispatch(data, &s2, parameter));
    EXPECT_EQ("D:start A:a D:a A:a D:a A:end", parameter.path);

    parameter.path.clear();
    EXPECT_TRUE(chart->start(data, parameter));
    EXPECT_TRUE(chart->dispatch(data, &s1, parameter));
    EXPECT_TRUE(chart->dispatch(data, &s1, parameter));
    EXPECT_TRUE(chart->dispatch(data, &s2, parameter));
    EXPECT_EQ("D:start A:a D:a A:a D:a A:a D:a A:end", parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test3)
{
    // Java SemanticTest.testSemantics3 asserts a path through state "b" but
    // chart t2() has no such state; event 1 just self-loops on "a". The
    // assertion below reflects the actual chart behaviour.
    auto chart = TestCharts::t2();
    TestEvent s1{1};
    TestParameter parameter;
    Metadata data;
    EXPECT_TRUE(chart->start(data, parameter));
    EXPECT_TRUE(chart->dispatch(data, &s1, parameter));
    EXPECT_EQ("D:start A:a D:a A:a", parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test4)
{
    auto chart = TestCharts::t3();
    TestParameter parameter;
    Metadata data;
    EXPECT_TRUE(chart->start(data, parameter));
    waitForFinalState(*chart, data);
    EXPECT_EQ("D:start A:a D:a A:end", parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test5)
{
    auto chart = TestCharts::t4();
    TestParameter parameter;
    parameter.guardvalue = 0;
    Metadata data;
    EXPECT_TRUE(chart->start(data, parameter));
    EXPECT_EQ("D:start A:a D:a A:end", parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test6)
{
    auto chart = TestCharts::t4();
    TestParameter parameter;
    parameter.guardvalue = 1;
    Metadata data;
    EXPECT_TRUE(chart->start(data, parameter));
    EXPECT_EQ("D:start A:a D:a A:j1 D:j1 E:a1 A:b D:b A:end", parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test7)
{
    auto chart = TestCharts::t4();
    TestParameter parameter;
    parameter.guardvalue = 2;
    Metadata data;
    EXPECT_TRUE(chart->start(data, parameter));
    EXPECT_EQ("D:start A:a D:a A:j1 D:j1 E:a2 A:c D:c A:j2 D:j2 E:a3 A:j3 D:j3 "
              "E:a4 A:end",
              parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test8)
{
    auto chart = TestCharts::h1();
    TestParameter parameter;
    Metadata data;
    EXPECT_TRUE(chart->start(data, parameter));
    EXPECT_EQ("D:start A:p A:start p D:start p A:a D:a A:b D:b A:end p D:end p "
              "D:p A:end",
              parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test9)
{
    auto chart = TestCharts::h2();
    TestEvent event{1};
    TestParameter parameter;
    Metadata data;
    EXPECT_TRUE(chart->start(data, parameter));
    EXPECT_TRUE(chart->dispatch(data, &event, parameter));
    EXPECT_EQ("D:start A:p A:start p D:start p A:a D:a A:b D:b A:end p D:end p "
              "D:p A:end",
              parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test10)
{
    auto chart = TestCharts::h3();
    TestEvent event{1};
    TestParameter parameter;
    Metadata data;
    EXPECT_TRUE(chart->start(data, parameter));
    EXPECT_TRUE(chart->dispatch(data, &event, parameter));
    EXPECT_EQ("D:start A:p A:start p D:start p A:a D:a A:b D:b A:end p D:end p "
              "D:p A:end",
              parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test11)
{
    auto chart = TestCharts::h3();
    TestEvent event{2};
    TestParameter parameter;
    Metadata data;
    EXPECT_TRUE(chart->start(data, parameter));
    EXPECT_TRUE(chart->dispatch(data, &event, parameter));
    EXPECT_EQ("D:start A:p A:start p D:start p A:a D:a D:p A:end",
              parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test12)
{
    auto chart = TestCharts::h4();
    TestEvent e1{1};
    TestEvent e2{2};
    TestEvent e3{3};
    TestParameter parameter;
    Metadata data;
    EXPECT_TRUE(chart->start(data, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e1, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e2, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e1, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e2, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e1, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e2, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e3, parameter));

    std::string result =
        "D:start A:p A:start p D:start p U:history p A:a D:a "
        "D:p A:p A:start p D:start p A:a D:a A:b D:b D:p A:p "
        "A:start p D:start p A:b D:b A:a D:a D:p A:p A:start p "
        "D:start p A:a D:a A:b D:b A:end p D:end p D:p A:end";
    EXPECT_EQ(result, parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test13)
{
    auto chart = TestCharts::h5();
    TestEvent e1{1};
    TestEvent e2{2};
    TestEvent e3{3};
    TestEvent e4{4};
    TestParameter parameter;
    Metadata data;
    EXPECT_TRUE(chart->start(data, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e2, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e4, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e1, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e2, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e2, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e3, parameter));

    std::string result =
        "D:start A:p A:start p D:start p U:history p A:a D:a "
        "A:q A:start q D:start q A:b D:b A:c D:c D:q D:p A:p "
        "A:start p D:start p A:q A:c D:c D:q A:a D:a A:q "
        "A:start q D:start q A:b D:b D:q A:end p D:end p D:p A:end";
    EXPECT_EQ(result, parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test14)
{
    auto chart = TestCharts::h6();
    TestParameter parameter;
    Metadata data;
    EXPECT_TRUE(chart->start(data, parameter));
    EXPECT_EQ("D:start A:p A:q A:r D:r D:q D:p A:x A:y D:y D:x A:end",
              parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test15)
{
    auto chart = TestCharts::c1();
    TestParameter parameter;
    Metadata data;
    EXPECT_TRUE(chart->start(data, parameter));
    std::string result = "D:start A:p A:p-r1 A:start p-r1 D:start p-r1 A:a "
                         "A:p-r2 A:start p-r2 D:start p-r2 A:b D:a A:end p-r1 "
                         "D:b A:end p-r2 D:end p-r1 D:p-r1 D:end p-r2 D:p-r2 "
                         "D:p A:end";
    EXPECT_EQ(result, parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test16)
{
    auto chart = TestCharts::c2();
    TestParameter parameter;
    Metadata data;
    TestEvent e1{1};
    TestEvent e2{2};
    EXPECT_TRUE(chart->start(data, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e2, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e1, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e2, parameter));
    std::string result = "D:start A:p A:p-r1 A:start p-r1 D:start p-r1 A:a "
                         "A:p-r2 A:start p-r2 D:start p-r2 A:c D:a A:b "
                         "D:b A:end p-r1 D:c A:d D:d A:e D:e A:end p-r2 "
                         "D:end p-r1 D:p-r1 D:end p-r2 D:p-r2 D:p A:end";
    EXPECT_EQ(result, parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test17)
{
    auto chart = TestCharts::c2();
    TestParameter parameter;
    Metadata data;
    TestEvent e1{1};
    TestEvent e2{2};
    EXPECT_TRUE(chart->start(data, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e1, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e2, parameter));
    std::string result = "D:start A:p A:p-r1 A:start p-r1 D:start p-r1 A:a "
                         "A:p-r2 A:start p-r2 D:start p-r2 A:c "
                         "D:c A:d D:a A:b D:d A:e "
                         "D:b A:end p-r1 D:e A:end p-r2 D:end p-r1 D:p-r1 "
                         "D:end p-r2 D:p-r2 D:p A:end";
    EXPECT_EQ(result, parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test18)
{
    auto chart = TestCharts::c2();
    TestParameter parameter;
    Metadata data;
    TestEvent e1{1};
    TestEvent e3{3};
    EXPECT_TRUE(chart->start(data, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e1, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e3, parameter));
    std::string result = "D:start A:p A:p-r1 A:start p-r1 D:start p-r1 A:a "
                         "A:p-r2 A:start p-r2 D:start p-r2 A:c "
                         "D:c A:d D:a D:p-r1 D:d D:p-r2 D:p A:end";
    EXPECT_EQ(result, parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test20)
{
    auto chart = TestCharts::c4();
    TestParameter parameter;
    Metadata data;
    EXPECT_TRUE(chart->start(data, parameter));
    std::string result = "D:start A:p A:p-r1 A:start p-r1 D:start p-r1 A:a "
                         "A:p-r2 A:start p-r2 D:start p-r2 A:b D:a "
                         "D:p-r1 D:b D:p-r2 D:p A:end";
    EXPECT_EQ(result, parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test21)
{
    auto chart = TestCharts::c5();
    TestParameter parameter;
    Metadata data;
    TestEvent e1{1};
    TestEvent e2{2};
    EXPECT_TRUE(chart->start(data, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e1, parameter));
    EXPECT_FALSE(chart->dispatch(data, &e2, parameter));
    std::string result = "D:start A:p A:p-r1 A:start p-r1 D:start p-r1 A:a "
                         "A:p-r2 A:start p-r2 D:start p-r2 A:b D:a "
                         "D:p-r1 D:b D:p-r2 D:p A:end";
    EXPECT_EQ(result, parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test22)
{
    auto chart = TestCharts::c5();
    TestParameter parameter;
    Metadata data;
    TestEvent e1{1};
    TestEvent e2{2};
    EXPECT_TRUE(chart->start(data, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e2, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e1, parameter));
    std::string result = "D:start A:p A:p-r1 A:start p-r1 D:start p-r1 A:a "
                         "A:p-r2 A:start p-r2 D:start p-r2 A:b D:b A:c "
                         "D:a D:p-r1 D:c D:p-r2 D:p A:end";
    EXPECT_EQ(result, parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test23)
{
    auto chart = TestCharts::c6();
    TestParameter parameter;
    Metadata data;
    EXPECT_TRUE(chart->start(data, parameter));
    std::string result = "D:start A:fork D:fork A:p A:p-r1 A:a A:p-r2 A:b "
                         "D:a A:end p-r1 D:b A:end p-r2 D:end p-r1 D:p-r1 "
                         "D:end p-r2 D:p-r2 D:p A:end";
    EXPECT_EQ(result, parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test24)
{
    auto chart = TestCharts::c7();
    TestParameter parameter;
    parameter.guardvalue = 1;
    Metadata data;
    EXPECT_TRUE(chart->start(data, parameter));
    std::string result = "D:start A:fork D:fork A:p A:p-r3 A:c "
                         "A:p-r1 A:a A:p-r2 A:b D:a A:end p-r1 D:b A:end p-r2 "
                         "D:c A:end p-r3 D:end p-r1 D:p-r1 D:end p-r2 D:p-r2 "
                         "D:end p-r3 D:p-r3 D:p A:end";
    EXPECT_EQ(result, parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test25)
{
    auto chart = TestCharts::c7();
    TestParameter parameter;
    parameter.guardvalue = 0;
    Metadata data;
    EXPECT_TRUE(chart->start(data, parameter));
    std::string result =
        "D:start A:fork D:fork A:p A:p-r3 A:start p-r3 D:start p-r3 A:d "
        "A:p-r1 A:a A:p-r2 A:b D:a A:end p-r1 D:b A:end p-r2 "
        "D:d A:end p-r3 D:end p-r1 D:p-r1 D:end p-r2 D:p-r2 "
        "D:end p-r3 D:p-r3 D:p A:end";
    EXPECT_EQ(result, parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test26)
{
    auto chart = TestCharts::c8();
    TestParameter parameter;
    Metadata data;
    EXPECT_TRUE(chart->start(data, parameter));
    std::string result =
        "D:start A:p A:p-r1 A:start p-r1 D:start p-r1 A:a "
        "A:p-r2 A:start p-r2 D:start p-r2 A:b "
        "D:b A:c D:a D:p-r1 D:c D:p-r2 D:p A:join D:join A:end";
    EXPECT_EQ(result, parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test27)
{
    auto chart = TestCharts::c9();
    TestParameter parameter;
    Metadata data;
    EXPECT_TRUE(chart->start(data, parameter));
    waitForFinalState(*chart, data);
    std::string result = "D:start A:p A:p-r1 A:start p-r1 D:start p-r1 A:a "
                         "A:p-r2 A:start p-r2 D:start p-r2 A:b "
                         "A:p-r3 A:start p-r3 D:start p-r3 A:d "
                         "D:b A:c D:a D:p-r1 D:c D:p-r2 D:d D:p-r3 D:p "
                         "A:join D:join A:end";
    EXPECT_EQ(result, parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test28)
{
    auto chart = TestCharts::c10();
    TestParameter parameter;
    Metadata data;
    TestEvent e1{1};
    TestEvent e2{2};
    EXPECT_TRUE(chart->start(data, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e1, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e1, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e2, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e1, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e2, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e2, parameter));
    std::string result = "D:start A:c D:c "
                         "A:p A:p-r1 A:start p-r1 D:start p-r1 A:end p-r1 "
                         "A:p-r2 A:start p-r2 D:start p-r2 E:history A:a "
                         "D:a A:b "
                         "D:b D:p-r2 D:end p-r1 D:p-r1 D:p A:c "
                         "D:c "
                         "A:p A:p-r1 A:start p-r1 D:start p-r1 A:end p-r1 "
                         "A:p-r2 A:start p-r2 D:start p-r2 A:b "
                         "D:b D:p-r2 D:end p-r1 D:p-r1 D:p A:c "
                         "D:c A:end";
    EXPECT_EQ(result, parameter.path);
    chart->shutdown();
}

TEST(Semantic, Test30)
{
    auto chart = TestCharts::c11();
    TestParameter parameter;
    Metadata data;
    EXPECT_TRUE(chart->start(data, parameter));
    waitForFinalState(*chart, data);
    std::string result = "D:start A:p "
                         "A:p-r1 A:start p-r1 D:start p-r1 A:a "
                         "A:p-r2 A:start p-r2 D:start p-r2 A:b "
                         "D:b E:t2 A:end p-r2 "
                         "D:a E:t1 A:end p-r1 "
                         "D:end p-r1 D:p-r1 "
                         "D:end p-r2 D:p-r2 "
                         "D:p "
                         "A:end";
    EXPECT_EQ(result, parameter.path);
    chart->shutdown();
}

TEST(Semantic, InternalTransition)
{
    auto chart = TestCharts::t5();
    TestParameter parameter;
    Metadata data;
    EXPECT_TRUE(chart->start(data, parameter));
    TestEvent e1{1};
    TestEvent e2{2};
    TestEvent e3{3};
    EXPECT_TRUE(chart->dispatch(data, &e1, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e2, parameter));
    EXPECT_TRUE(chart->dispatch(data, &e3, parameter));
    std::string result = "D:start E:t1 A:a "
                         "D:a E:t2 A:a "
                         "E:t3 "
                         "D:a E:t4 A:end";
    EXPECT_EQ(result, parameter.path);
    chart->shutdown();
}

} // namespace statechart::tests
