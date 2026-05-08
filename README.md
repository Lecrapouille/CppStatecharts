# CppStatecharts — UML statechart framework for C++

[![License: LGPL v2.1](https://img.shields.io/badge/License-LGPL_v2.1-blue.svg)](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.en.html)

[CppStatecharts](https://github.com/Lecrapouille/CppStatecharts) is a C++20 port of the [Java UML statechart framework](https://github.com/klangfarbe/UML-Statechart-Framework-for-Java) originally written by Christian Mocek (License: LGPL v2.1) implementing the semantics and syntax of UML statecharts. [PlantUMLStatecharts](https://github.com/Lecrapouille/PlantUMLStatecharts) generates from [PlantUML statecharts](https://plantuml.com/en/state-diagram) code using this library.

## Compilation

```sh
git clone https://github.com/Lecrapouille/CppStatecharts
cd CppStatecharts
make -j8

# Optional: make tests -j8
sudo make install
```

In your code:

```cpp
#include <CppStatecharts/CppStatecharts.hpp>
```

Use `-I…/include` pointing at this repo (or the installed prefix) so the compiler finds the `CppStatecharts/` directory.

You can test the example:

```sh
./build/GettingStarted
```

## Documentation

| Document | Description |
| --- | --- |
| [Statecharts](doc/statecharts.md) | Introduction to UML statechart concepts (states, transitions, hierarchy, concurrency, history, fork/join). |
| [Getting started](doc/getting-started.md) | Step-by-step C++ walkthrough mirroring [getting-started.cpp](doc/examples/getting-started.cpp). |
| [References](doc/references.md) | Bibliography and further reading. |

## Credits and license

- Original Java implementation: Christian Mocek (2006-2013).
- C++20 port and PlantUML translator integration: Quentin Quadrat (2026).
- Released under the GNU Lesser General Public License v2.1 (see source headers).
