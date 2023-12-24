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

#include "Statechart/Statechart.hpp"

#include <memory>

namespace statechart::tests {

/**
 * @brief Factory functions producing the same statecharts used by the
 *        Java @c TestCharts.java reference.
 */
struct TestCharts
{
    static std::unique_ptr<Statechart> t1();
    static std::unique_ptr<Statechart> t2();
    static std::unique_ptr<Statechart> t3();
    static std::unique_ptr<Statechart> t4();
    static std::unique_ptr<Statechart> t5();
    static std::unique_ptr<Statechart> h1();
    static std::unique_ptr<Statechart> h2();
    static std::unique_ptr<Statechart> h3();
    static std::unique_ptr<Statechart> h4();
    static std::unique_ptr<Statechart> h5();
    static std::unique_ptr<Statechart> h6();
    static std::unique_ptr<Statechart> c1();
    static std::unique_ptr<Statechart> c2();
    static std::unique_ptr<Statechart> c3();
    static std::unique_ptr<Statechart> c4();
    static std::unique_ptr<Statechart> c5();
    static std::unique_ptr<Statechart> c6();
    static std::unique_ptr<Statechart> c7();
    static std::unique_ptr<Statechart> c8();
    static std::unique_ptr<Statechart> c9();
    static std::unique_ptr<Statechart> c10();
    static std::unique_ptr<Statechart> c11();
};

} // namespace statechart::tests
