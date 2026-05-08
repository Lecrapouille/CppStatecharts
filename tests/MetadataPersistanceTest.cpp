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

#include "CppStatecharts/Metadata.hpp"
#include "CppStatecharts/Statechart.hpp"

#include <gtest/gtest.h>

namespace statechart::tests {

TEST(MetadataPersistance, RestoreState)
{
    auto chart = TestCharts::h5();
    State* s3 = chart->getStateByName("c");
    State* h1 = chart->getStateByName("p");
    State* h2 = chart->getStateByName("q");

    TestParameter parameter;
    Metadata data;

    chart->restoreState(s3, data, parameter);
    EXPECT_TRUE(data.isActive(s3));
    EXPECT_TRUE(data.isActive(h2));
    EXPECT_TRUE(data.isActive(h1));
    EXPECT_TRUE(data.isActive(chart.get()));

    EXPECT_EQ("A:p A:q A:c", parameter.path);

    EXPECT_FALSE(chart->restoreState(s3, data, parameter));

    chart->shutdown();
}

} // namespace statechart::tests
