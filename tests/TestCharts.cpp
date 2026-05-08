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

#include "TestCharts.hpp"
#include "Helpers.hpp"

#include "CppStatecharts/ConcurrentState.hpp"
#include "CppStatecharts/Condition.hpp"
#include "CppStatecharts/FinalState.hpp"
#include "CppStatecharts/HierarchicalState.hpp"
#include "CppStatecharts/InternalTransition.hpp"
#include "CppStatecharts/PseudoState.hpp"
#include "CppStatecharts/State.hpp"
#include "CppStatecharts/Statechart.hpp"
#include "CppStatecharts/TimeoutEvent.hpp"
#include "CppStatecharts/Transition.hpp"

namespace statechart::tests {

namespace {

inline State*
state(Statechart& chart, const char* name, Context* parent, const char* aname)
{
    return chart.create<State>(name,
                               parent,
                               makeTestAction(aname, "A"),
                               Action{},
                               makeTestAction(aname, "D"));
}

inline PseudoState*
pstart(Statechart& chart, const char* name, Context* parent, const char* aname)
{
    auto* p = chart.create<PseudoState>(name, parent, PseudoStateType::Start);
    p->setEntryAction(makeTestAction(aname, "A"));
    p->setExitAction(makeTestAction(aname, "D"));
    return p;
}

inline FinalState*
pfinal(Statechart& chart, const char* name, Context* parent, const char* aname)
{
    auto* p = chart.create<FinalState>(name, parent);
    p->setEntryAction(makeTestAction(aname, "A"));
    p->setExitAction(makeTestAction(aname, "D"));
    return p;
}

inline PseudoState* pjunction(Statechart& chart,
                              const char* name,
                              Context* parent,
                              const char* aname)
{
    auto* p =
        chart.create<PseudoState>(name, parent, PseudoStateType::Junction);
    p->setEntryAction(makeTestAction(aname, "A"));
    p->setExitAction(makeTestAction(aname, "D"));
    return p;
}

inline HierarchicalState*
phier(Statechart& chart, const char* name, Context* parent, const char* aname)
{
    return chart.create<HierarchicalState>(name,
                                           parent,
                                           makeTestAction(aname, "A"),
                                           Action{},
                                           makeTestAction(aname, "D"));
}

inline ConcurrentState*
pconc(Statechart& chart, const char* name, Context* parent, const char* aname)
{
    return chart.create<ConcurrentState>(name,
                                         parent,
                                         makeTestAction(aname, "A"),
                                         Action{},
                                         makeTestAction(aname, "D"));
}

} // namespace

std::unique_ptr<Statechart> TestCharts::t1()
{
    auto chart = std::make_unique<Statechart>("t1", 10, false);
    auto& c = *chart;
    auto* s1 = state(c, "a", &c, "a");
    auto* s2 = state(c, "b", &c, "b");
    auto* p1 = pstart(c, "start", &c, "start");
    auto* p2 = pfinal(c, "end", &c, "end");
    c.createTransition(p1, s1);
    c.createTransition(s1, s2);
    c.createTransition(s2, p2);
    return chart;
}

std::unique_ptr<Statechart> TestCharts::t2()
{
    auto chart = std::make_unique<Statechart>("t2", 10, false);
    auto& c = *chart;
    auto* s1 = state(c, "a", &c, "a");
    auto* p1 = pstart(c, "start", &c, "start");
    auto* p2 = pfinal(c, "end", &c, "end");
    c.createTransition(p1, s1);
    c.createTransition(s1, s1, c.createEvent<TestEvent>(1));
    c.createTransition(s1, p2, c.createEvent<TestEvent>(2));

    s1->setName("s1");
    p1->setName("p1");
    p2->setName("p3");
    return chart;
}

std::unique_ptr<Statechart> TestCharts::t3()
{
    auto chart = std::make_unique<Statechart>("t3", 10, false);
    auto& c = *chart;
    auto* s1 = state(c, "a", &c, "a");
    auto* s2 = state(c, "b", &c, "b");
    auto* p1 = pstart(c, "start", &c, "start");
    auto* p2 = pfinal(c, "end", &c, "end");
    c.createTransition(p1, s1);
    c.createTransition(s1, s2, c.createEvent<TestEvent>(1));
    c.createTransition(s1, p2, c.createEvent<TimeoutEvent>(1000));
    c.createTransition(s2, p2);
    return chart;
}

std::unique_ptr<Statechart> TestCharts::t4()
{
    auto chart = std::make_unique<Statechart>("t4", 10, false);
    auto& c = *chart;
    auto* s1 = state(c, "a", &c, "a");
    auto* s2 = state(c, "b", &c, "b");
    auto* s3 = state(c, "c", &c, "c");
    auto* p1 = pstart(c, "start", &c, "start");
    auto* p2 = pfinal(c, "end", &c, "end");
    auto* j1 = pjunction(c, "j1", &c, "j1");
    auto* j2 = pjunction(c, "j2", &c, "j2");
    auto* j3 = pjunction(c, "j3", &c, "j3");

    c.createTransition(p1, s1);
    c.createTransition(s1, p2, makeTestGuard(0));
    c.createTransition(s1, j1);
    c.createTransition(j1, s2, makeTestGuard(1), makeTestAction("a1", "E"));
    c.createTransition(j1, s3, makeTestGuard(2), makeTestAction("a2", "E"));
    c.createTransition(s2, p2);
    c.createTransition(s3, j2);
    c.createTransition(j2, j3, makeTestAction("a3", "E"));
    c.createTransition(j3, p2, makeTestAction("a4", "E"));
    return chart;
}

std::unique_ptr<Statechart> TestCharts::t5()
{
    auto chart = std::make_unique<Statechart>("t1", 10, false);
    auto& c = *chart;
    auto* s1 = state(c, "a", &c, "a");
    auto* p1 = pstart(c, "start", &c, "start");
    auto* p2 = pfinal(c, "end", &c, "end");

    c.createTransition(p1, s1, makeTestAction("t1", "E"));
    c.createTransition(
        s1, s1, c.createEvent<TestEvent>(1), makeTestAction("t2", "E"));
    c.createInternalTransition(
        s1, c.createEvent<TestEvent>(2), makeTestAction("t3", "E"));
    c.createTransition(
        s1, p2, c.createEvent<TestEvent>(3), makeTestAction("t4", "E"));
    return chart;
}

std::unique_ptr<Statechart> TestCharts::h1()
{
    auto chart = std::make_unique<Statechart>("h1", 10, false);
    auto& c = *chart;
    auto* h = phier(c, "p", &c, "p");
    auto* s1 = state(c, "a", h, "a");
    auto* s2 = state(c, "b", h, "b");
    auto* p1 = pstart(c, "start", &c, "start");
    auto* p2 = pfinal(c, "end", &c, "end");
    auto* p3 = pstart(c, "start p", h, "start p");
    auto* p4 = pfinal(c, "end p", h, "end p");
    c.createTransition(p1, h);
    c.createTransition(h, p2);
    c.createTransition(p3, s1);
    c.createTransition(s1, s2);
    c.createTransition(s2, p4);
    return chart;
}

std::unique_ptr<Statechart> TestCharts::h2()
{
    auto chart = std::make_unique<Statechart>("h2", 10, false);
    auto& c = *chart;
    auto* h = phier(c, "p", &c, "p");
    auto* s1 = state(c, "a", h, "a");
    auto* s2 = state(c, "b", h, "b");
    auto* p1 = pstart(c, "start", &c, "start");
    auto* p2 = pfinal(c, "end", &c, "end");
    auto* p3 = pstart(c, "start p", h, "start p");
    auto* p4 = pfinal(c, "end p", h, "end p");
    c.createTransition(p1, h);
    c.createTransition(h, p2);
    c.createTransition(h, p2, c.createEvent<TestEvent>(1));
    c.createTransition(p3, s1);
    c.createTransition(s1, s2, c.createEvent<TestEvent>(1));
    c.createTransition(s2, p4);
    return chart;
}

std::unique_ptr<Statechart> TestCharts::h3()
{
    auto chart = std::make_unique<Statechart>("h3", 10, false);
    auto& c = *chart;
    auto* h = phier(c, "p", &c, "p");
    auto* s1 = state(c, "a", h, "a");
    auto* s2 = state(c, "b", h, "b");
    auto* p1 = pstart(c, "start", &c, "start");
    auto* p2 = pfinal(c, "end", &c, "end");
    auto* p3 = pstart(c, "start p", h, "start p");
    auto* p4 = pfinal(c, "end p", h, "end p");
    c.createTransition(p1, h);
    c.createTransition(h, p2);
    c.createTransition(h, p2, c.createEvent<TestEvent>(2));
    c.createTransition(p3, s1);
    c.createTransition(s1, s2, c.createEvent<TestEvent>(1));
    c.createTransition(s2, p4);
    return chart;
}

std::unique_ptr<Statechart> TestCharts::h4()
{
    auto chart = std::make_unique<Statechart>("h4", 10, false);
    auto& c = *chart;
    auto* h = phier(c, "p", &c, "p");
    auto* s1 = state(c, "a", h, "a");
    auto* s2 = state(c, "b", h, "b");
    auto* p1 = pstart(c, "start", &c, "start");
    auto* p2 = pfinal(c, "end", &c, "end");
    auto* p3 = pstart(c, "start p", h, "start p");
    auto* p4 = pfinal(c, "end p", h, "end p");
    auto* hi = c.create<PseudoState>("history p", h, PseudoStateType::History);
    hi->setExitAction(makeTestAction("history p", "U"));
    c.createTransition(p1, h);
    c.createTransition(h, h, c.createEvent<TestEvent>(1));
    c.createTransition(h, p2);
    c.createTransition(p3, hi);
    c.createTransition(hi, s1);
    c.createTransition(s1, s2, c.createEvent<TestEvent>(2));
    c.createTransition(s2, s1, c.createEvent<TestEvent>(2));
    c.createTransition(s2, p4, c.createEvent<TestEvent>(3));
    return chart;
}

std::unique_ptr<Statechart> TestCharts::h5()
{
    auto chart = std::make_unique<Statechart>("h5", 10, false);
    auto& c = *chart;
    auto* h1 = phier(c, "p", &c, "p");
    auto* h2 = phier(c, "q", h1, "q");
    auto* s1 = state(c, "a", h1, "a");
    auto* s2 = state(c, "b", h2, "b");
    auto* s3 = state(c, "c", h2, "c");
    auto* s4 = state(c, "d", &c, "d");
    auto* p1 = pstart(c, "start", &c, "start");
    auto* p2 = pfinal(c, "end", &c, "end");
    auto* p3 = pstart(c, "start p", h1, "start p");
    auto* p4 = pfinal(c, "end p", h1, "end p");
    auto* hi =
        c.create<PseudoState>("history p", h1, PseudoStateType::DeepHistory);
    auto* p5 = pstart(c, "start q", h2, "start q");
    hi->setExitAction(makeTestAction("history p", "U"));
    (void)s4;
    (void)p5;
    c.createTransition(p1, h1);
    c.createTransition(h1, h1, c.createEvent<TestEvent>(1));
    c.createTransition(h1, p2);
    c.createTransition(p3, hi);
    c.createTransition(hi, s1);
    c.createTransition(s1, h2, c.createEvent<TestEvent>(2));
    c.createTransition(h2, s1, c.createEvent<TestEvent>(2));
    c.createTransition(h2, p4, c.createEvent<TestEvent>(3));
    c.createTransition(p5, s2);
    c.createTransition(s2, s3, c.createEvent<TestEvent>(4));
    c.createTransition(s3, s2, c.createEvent<TestEvent>(4));
    c.createTransition(s3, s4, c.createEvent<TestEvent>(5));
    c.createTransition(s4, h1);
    return chart;
}

std::unique_ptr<Statechart> TestCharts::h6()
{
    auto chart = std::make_unique<Statechart>("h6", 10, false);
    auto& c = *chart;
    auto* h1 = phier(c, "p", &c, "p");
    auto* h2 = phier(c, "q", h1, "q");
    auto* h3 = phier(c, "x", &c, "x");
    auto* s1 = state(c, "a", h2, "r");
    auto* s2 = state(c, "b", h3, "y");
    auto* p1 = pstart(c, "start", &c, "start");
    auto* p2 = pfinal(c, "end", &c, "end");
    c.createTransition(p1, s1);
    c.createTransition(s1, s2);
    c.createTransition(s2, p2);
    return chart;
}

std::unique_ptr<Statechart> TestCharts::c1()
{
    auto chart = std::make_unique<Statechart>("c1", 10, false);
    auto& c = *chart;
    auto* n1 = pconc(c, "p", &c, "p");
    auto* h1 = phier(c, "p-r1", n1, "p-r1");
    auto* h2 = phier(c, "p-r2", n1, "p-r2");
    auto* s1 = state(c, "a", h1, "a");
    auto* s2 = state(c, "b", h2, "b");
    auto* p1 = pstart(c, "start", &c, "start");
    auto* p2 = pfinal(c, "end", &c, "end");
    auto* p3 = pstart(c, "start p-r1", h1, "start p-r1");
    auto* p4 = pfinal(c, "end p-r1", h1, "end p-r1");
    auto* p5 = pstart(c, "start p-r2", h2, "start p-r2");
    auto* p6 = pfinal(c, "end p-r2", h2, "end p-r2");
    c.createTransition(p1, n1);
    c.createTransition(n1, p2);
    c.createTransition(p3, s1);
    c.createTransition(s1, p4);
    c.createTransition(p5, s2);
    c.createTransition(s2, p6);
    return chart;
}

std::unique_ptr<Statechart> TestCharts::c2()
{
    auto chart = std::make_unique<Statechart>("c2", 10, false);
    auto& c = *chart;
    auto* n1 = pconc(c, "p", &c, "p");
    auto* h1 = phier(c, "p-r1", n1, "p-r1");
    auto* h2 = phier(c, "p-r2", n1, "p-r2");
    auto* s1 = state(c, "a", h1, "a");
    auto* s2 = state(c, "b", h1, "b");
    auto* s3 = state(c, "c", h2, "c");
    auto* s4 = state(c, "d", h2, "d");
    auto* s5 = state(c, "e", h2, "e");
    auto* p1 = pstart(c, "start", &c, "start");
    auto* p2 = pfinal(c, "end", &c, "end");
    auto* p3 = pstart(c, "start p-r1", h1, "start p-r1");
    auto* p4 = pfinal(c, "end p-r1", h1, "end p-r1");
    auto* p5 = pstart(c, "start p-r2", h2, "start p-r2");
    auto* p6 = pfinal(c, "end p-r2", h2, "end p-r2");
    c.createTransition(p1, n1);
    c.createTransition(n1, p2);
    c.createTransition(n1, p2, c.createEvent<TestEvent>(3));
    c.createTransition(p3, s1);
    c.createTransition(s1, s2, c.createEvent<TestEvent>(2));
    c.createTransition(s2, p4);
    c.createTransition(p5, s3);
    c.createTransition(s3, s4, c.createEvent<TestEvent>(1));
    c.createTransition(s4, s5, c.createEvent<TestEvent>(2));
    c.createTransition(s5, p6);
    return chart;
}

std::unique_ptr<Statechart> TestCharts::c3()
{
    auto chart = std::make_unique<Statechart>("c3", 10, false);
    auto& c = *chart;
    auto* n1 = pconc(c, "p", &c, "p");
    auto* h1 = phier(c, "p-r1", n1, "p-r1");
    auto* h2 = phier(c, "p-r2", n1, "p-r2");
    auto* s1 = state(c, "a", h1, "a");
    auto* s2 = state(c, "b", h2, "b");
    auto* p1 = pstart(c, "start", &c, "start");
    auto* p2 = pfinal(c, "end", &c, "end");
    auto* p3 = pfinal(c, "end p-r1", h1, "end p-r1");
    auto* p4 = pstart(c, "start p-r2", h2, "start p-r2");
    auto* p5 = pfinal(c, "end p-r2", h2, "end p-r2");
    c.createTransition(p1, s1);
    c.createTransition(n1, p2);
    c.createTransition(s1, p3);
    c.createTransition(p4, s2);
    c.createTransition(s2, p5);
    return chart;
}

std::unique_ptr<Statechart> TestCharts::c4()
{
    auto chart = std::make_unique<Statechart>("c4", 10, false);
    auto& c = *chart;
    auto* n1 = pconc(c, "p", &c, "p");
    auto* h1 = phier(c, "p-r1", n1, "p-r1");
    auto* h2 = phier(c, "p-r2", n1, "p-r2");
    auto* s1 = state(c, "a", h1, "a");
    auto* s2 = state(c, "b", h2, "b");
    auto* s3 = state(c, "c", h2, "c");
    auto* p1 = pstart(c, "start", &c, "start");
    auto* p2 = pfinal(c, "end", &c, "end");
    auto* p3 = pstart(c, "start p-r1", h1, "start p-r1");
    auto* p4 = pstart(c, "start p-r2", h2, "start p-r2");
    c.createTransition(p1, n1);
    c.createTransition(p3, s1);
    c.createTransition(s1, p2);
    c.createTransition(p4, s2);
    c.createTransition(s2, s3);
    return chart;
}

std::unique_ptr<Statechart> TestCharts::c5()
{
    auto chart = std::make_unique<Statechart>("c5", 10, false);
    auto& c = *chart;
    auto* n1 = pconc(c, "p", &c, "p");
    auto* h1 = phier(c, "p-r1", n1, "p-r1");
    auto* h2 = phier(c, "p-r2", n1, "p-r2");
    auto* s1 = state(c, "a", h1, "a");
    auto* s2 = state(c, "b", h2, "b");
    auto* s3 = state(c, "c", h2, "c");
    auto* p1 = pstart(c, "start", &c, "start");
    auto* p2 = pfinal(c, "end", &c, "end");
    auto* p3 = pstart(c, "start p-r1", h1, "start p-r1");
    auto* p4 = pstart(c, "start p-r2", h2, "start p-r2");
    c.createTransition(p1, n1);
    c.createTransition(p3, s1);
    c.createTransition(s1, p2, c.createEvent<TestEvent>(1));
    c.createTransition(p4, s2);
    c.createTransition(s2, s3, c.createEvent<TestEvent>(2));
    return chart;
}

std::unique_ptr<Statechart> TestCharts::c6()
{
    auto chart = std::make_unique<Statechart>("c6", 10, false);
    auto& c = *chart;
    auto* n1 = pconc(c, "p", &c, "p");
    auto* h1 = phier(c, "p-r1", n1, "p-r1");
    auto* h2 = phier(c, "p-r2", n1, "p-r2");
    auto* s1 = state(c, "a", h1, "a");
    auto* s2 = state(c, "b", h2, "b");
    auto* p1 = pstart(c, "start", &c, "start");
    auto* p2 = pfinal(c, "end", &c, "end");
    auto* p3 = pfinal(c, "end p-r1", h1, "end p-r1");
    auto* p4 = pfinal(c, "end p-r2", h2, "end p-r2");
    auto* p5 = c.create<PseudoState>("fork", &c, PseudoStateType::Fork);
    p5->setEntryAction(makeTestAction("fork", "A"));
    p5->setExitAction(makeTestAction("fork", "D"));
    c.createTransition(p1, p5);
    c.createTransition(p5, s1);
    c.createTransition(p5, s2);
    c.createTransition(s1, p3);
    c.createTransition(s2, p4);
    c.createTransition(n1, p2);
    return chart;
}

std::unique_ptr<Statechart> TestCharts::c7()
{
    auto chart = std::make_unique<Statechart>("c7", 10, false);
    auto& c = *chart;
    auto* n1 = pconc(c, "p", &c, "p");
    auto* h1 = phier(c, "p-r1", n1, "p-r1");
    auto* h2 = phier(c, "p-r2", n1, "p-r2");
    auto* h3 = phier(c, "p-r3", n1, "p-r3");
    auto* s1 = state(c, "a", h1, "a");
    auto* s2 = state(c, "b", h2, "b");
    auto* s3 = state(c, "c", h3, "c");
    auto* s4 = state(c, "d", h3, "d");
    auto* p1 = pstart(c, "start", &c, "start");
    auto* p2 = pfinal(c, "end", &c, "end");
    auto* p3 = pfinal(c, "end p-r1", h1, "end p-r1");
    auto* p4 = pfinal(c, "end p-r2", h2, "end p-r2");
    auto* p5 = c.create<PseudoState>("fork", &c, PseudoStateType::Fork);
    auto* p6 = pstart(c, "start p-r3", h3, "start p-r3");
    auto* p7 = pfinal(c, "end p-r3", h3, "end p-r3");
    p5->setEntryAction(makeTestAction("fork", "A"));
    p5->setExitAction(makeTestAction("fork", "D"));
    c.createTransition(p1, p5);
    c.createTransition(p5, s1);
    c.createTransition(p5, s2);
    c.createTransition(p5, s3, makeTestGuard(1));
    c.createTransition(s1, p3);
    c.createTransition(s2, p4);
    c.createTransition(p6, s4);
    c.createTransition(s3, p7);
    c.createTransition(s4, p7);
    c.createTransition(n1, p2);
    return chart;
}

std::unique_ptr<Statechart> TestCharts::c8()
{
    auto chart = std::make_unique<Statechart>("x8", 10, false);
    auto& c = *chart;
    auto* n1 = pconc(c, "p", &c, "p");
    auto* h1 = phier(c, "p-r1", n1, "p-r1");
    auto* h2 = phier(c, "p-r2", n1, "p-r2");
    auto* s1 = state(c, "a", h1, "a");
    auto* s2 = state(c, "b", h2, "b");
    auto* s3 = state(c, "c", h2, "c");
    auto* p1 = pstart(c, "start", &c, "start");
    auto* p2 = pfinal(c, "end", &c, "end");
    auto* p3 = pstart(c, "start p-r1", h1, "start p-r1");
    auto* p4 = pstart(c, "start p-r2", h2, "start p-r2");
    auto* p5 = c.create<PseudoState>("join", &c, PseudoStateType::Join);
    p5->setEntryAction(makeTestAction("join", "A"));
    p5->setExitAction(makeTestAction("join", "D"));
    c.createTransition(p1, n1);
    c.createTransition(p3, s1);
    c.createTransition(p4, s2);
    c.createTransition(s2, s3);
    c.createTransition(s1, p5);
    c.createTransition(s3, p5);
    c.createTransition(p5, p2);
    return chart;
}

std::unique_ptr<Statechart> TestCharts::c9()
{
    auto chart = std::make_unique<Statechart>("c9", 10, false);
    auto& c = *chart;
    auto* n1 = pconc(c, "p", &c, "p");
    auto* h1 = phier(c, "p-r1", n1, "p-r1");
    auto* h2 = phier(c, "p-r2", n1, "p-r2");
    auto* h3 = phier(c, "p-r3", n1, "p-r3");
    auto* s1 = state(c, "a", h1, "a");
    auto* s2 = state(c, "b", h2, "b");
    auto* s3 = state(c, "c", h2, "c");
    auto* s4 = state(c, "d", h3, "d");
    auto* p1 = pstart(c, "start", &c, "start");
    auto* p2 = pfinal(c, "end", &c, "end");
    auto* p3 = pstart(c, "start p-r1", h1, "start p-r1");
    auto* p4 = pstart(c, "start p-r2", h2, "start p-r2");
    auto* p5 = c.create<PseudoState>("join", &c, PseudoStateType::Join);
    auto* p6 = pstart(c, "start p-r3", h3, "start p-r3");
    p5->setEntryAction(makeTestAction("join", "A"));
    p5->setExitAction(makeTestAction("join", "D"));
    c.createTransition(p1, n1);
    c.createTransition(p3, s1);
    c.createTransition(p4, s2);
    c.createTransition(s2, s3, c.createEvent<TimeoutEvent>(2000));
    c.createTransition(s1, p5);
    c.createTransition(s3, p5);
    c.createTransition(p5, p2);
    c.createTransition(p6, s4);
    return chart;
}

std::unique_ptr<Statechart> TestCharts::c10()
{
    auto chart = std::make_unique<Statechart>("c10", 10, false);
    auto& c = *chart;
    auto* n1 = pconc(c, "p", &c, "p");
    auto* h1 = phier(c, "p-r1", n1, "p-r1");
    auto* h2 = phier(c, "p-r2", n1, "p-r2");
    auto* s1 = state(c, "a", h2, "a");
    auto* s2 = state(c, "b", h2, "b");
    auto* s3 = state(c, "c", &c, "c");
    auto* p1 = pstart(c, "start", &c, "start");
    auto* p2 = pfinal(c, "end", &c, "end");
    auto* p3 = pstart(c, "start p-r1", h1, "start p-r1");
    auto* p4 = pfinal(c, "end p-r1", h1, "end p-r1");
    auto* p5 = pstart(c, "start p-r2", h2, "start p-r2");
    auto* hi = c.create<PseudoState>("history", h2, PseudoStateType::History);
    hi->setExitAction(makeTestAction("history", "E"));
    c.createTransition(p1, s3);
    c.createTransition(s3, n1, c.createEvent<TestEvent>(1));
    c.createTransition(s3, p2, c.createEvent<TestEvent>(2));
    c.createTransition(p3, p4);
    c.createTransition(p5, hi);
    c.createTransition(hi, s1);
    c.createTransition(s1, s2, c.createEvent<TestEvent>(1));
    c.createTransition(s1, s3, c.createEvent<TestEvent>(2));
    c.createTransition(s2, s1, c.createEvent<TestEvent>(1));
    c.createTransition(s2, s3, c.createEvent<TestEvent>(2));
    return chart;
}

std::unique_ptr<Statechart> TestCharts::c11()
{
    auto chart = std::make_unique<Statechart>("c10", 10, false);
    auto& c = *chart;
    auto* n1 = pconc(c, "p", &c, "p");
    auto* h1 = phier(c, "p-r1", n1, "p-r1");
    auto* h2 = phier(c, "p-r2", n1, "p-r2");
    auto* s1 = state(c, "a", h1, "a");
    auto* s2 = state(c, "b", h2, "b");
    auto* p1 = pstart(c, "start", &c, "start");
    auto* p2 = pfinal(c, "end", &c, "end");
    auto* p3 = pstart(c, "start p-r1", h1, "start p-r1");
    auto* p4 = pfinal(c, "end p-r1", h1, "end p-r1");
    auto* p5 = pstart(c, "start p-r2", h2, "start p-r2");
    auto* p6 = pfinal(c, "end p-r2", h2, "end p-r2");

    c.createTransition(p1, n1);
    c.createTransition(p3, s1);
    c.createTransition(
        s1, p4, c.createEvent<TimeoutEvent>(500), makeTestAction("t1", "E"));
    c.createTransition(p5, s2);
    c.createTransition(
        s2, p6, c.createEvent<TimeoutEvent>(250), makeTestAction("t2", "E"));
    c.createTransition(n1, p2);
    return chart;
}

} // namespace statechart::tests
