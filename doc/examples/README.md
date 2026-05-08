# CppStatecharts examples

This folder contains C++ implementations of statechart examples ported from
the [PlantUMLStatecharts](https://github.com/Lecrapouille/PlantUMLStatecharts)
collection. Each example is a standalone executable demonstrating one or more
features of the library.

All examples include the umbrella header only:

```cpp
#include <CppStatecharts/CppStatecharts.hpp>
```

Pass `-I` *to the parent of the `CppStatecharts` folder* (typically the project's
`include` directory) so this path resolves.

## How to build and run

From the repository root:

```sh
make -j$(nproc)
```

Binaries are produced in `build/`. Each example can then be launched
independently:

```sh
./build/Motor
./build/Gumball
./build/RichMan
# ...
```

To rebuild a single example:

```sh
cd doc/examples/<ExampleName>
make
```

## Examples overview

### Non-hierarchical state machines (FSM)

| Example | Demonstrates | PlantUML source |
| --- | --- | --- |
| [GettingStarted](GettingStarted) | All features in one chart (hierarchy, concurrency, timeout, junction) | — |
| [Motor](Motor) | Entry/exit/internal actions, guards, multiple events | [`Motor.plantuml`](../../../PlantUMLStatecharts/examples/Motor.plantuml) |
| [Gumball](Gumball) | Initial state with guards (junction at boot) | [`Gumball.plantuml`](../../../PlantUMLStatecharts/examples/Gumball.plantuml) |
| [RichMan](RichMan) | Multiple outgoing transitions with guards | [`RichMan.plantuml`](../../../PlantUMLStatecharts/examples/RichMan.plantuml) |
| [SimpleFSM](SimpleFSM) | Cheat-sheet covering events, guards, actions, internal transitions | [`SimpleFSM.plantuml`](../../../PlantUMLStatecharts/examples/SimpleFSM.plantuml) |
| [DigitalWatch](DigitalWatch) | `do` actions, modulo arithmetic in actions | [`DigitalWatch.plantuml`](../../../PlantUMLStatecharts/examples/DigitalWatch.plantuml) |
| [LaneKeeping](LaneKeeping) | Realistic FSM with shared state flags | [`LaneKeeping.plantuml`](../../../PlantUMLStatecharts/examples/LaneKeeping.plantuml) |
| [EthernetBox](EthernetBox) | Realistic FSM with internal transitions | [`EthernetBox.plantuml`](../../../PlantUMLStatecharts/examples/EthernetBox.plantuml) |
| [Triggers](Triggers) | Two transitions sharing the same event with disjoint guards | [`Triggers.plantuml`](../../../PlantUMLStatecharts/examples/Triggers.plantuml) |
| [SelfParking](SelfParking) | Park-Assistant FSM (large flat state machine) | [`SelfParking.plantuml`](../../../PlantUMLStatecharts/examples/SelfParking.plantuml) |
| [InfiniteLoop](InfiniteLoop) | Infinite-loop detection (trigger-less cycle) | [`InfiniteLoop.plantuml`](../../../PlantUMLStatecharts/examples/InfiniteLoop.plantuml) |

### Hierarchical state machines (HSM)

| Example | Demonstrates | PlantUML source |
| --- | --- | --- |
| [SimpleComposite](SimpleComposite) | One nested level of composite state | [`SimpleComposite.plantuml`](../../../PlantUMLStatecharts/examples/SimpleComposite.plantuml) |
| [ComplexComposite](ComplexComposite) | Three nested levels of composite state | [`ComplexComposite.plantuml`](../../../PlantUMLStatecharts/examples/ComplexComposite.plantuml) |
| [SimpleOrthogonal](SimpleOrthogonal) | Concurrent regions inside a composite state | [`SimpleOrthogonal.plantuml`](../../../PlantUMLStatecharts/examples/SimpleOrthogonal.plantuml) |
| [Pompe](Pompe) | Hierarchical state + timeouts + multiple emergency exits | [`Pompe.plantuml`](../../../PlantUMLStatecharts/examples/Pompe.plantuml) |

## Detailed descriptions

### Motor controller

A simple motor controller. Initially `Idle`, transitions to `Start` then
`Spinning` based on speed setpoints. The `Spinning` state declares an internal
transition that updates the reference speed without exiting/re-entering. A
`halt` event always returns to `Idle`.

Features: entry/exit actions, guards on event arguments, internal transitions.

### Gumball distributor

Inspired from the *Head First Design Patterns* book. The distributor starts
either in `NoQuarter` (if there is stock) or `OutOfGumballs` (sold out). After
each sale, a junction reroutes the chart depending on the remaining stock.

Features: junction (`Condition`) at startup and after sale, guards driving the
flow, action decrementing a counter on transition.

### RichMan

A punk collects quarters on the street. Once he gets one, he says "Yes!" — the
counter rolls up to 10. This example shows multiple outgoing transitions with
guards on the same source state.

Features: guards `[quarters < 10]` and `[quarters == 1]`, transitions with
actions resetting state.

### SimpleFSM

Cheat-sheet showing the full set of features the framework can handle:

- entry/exit actions on every state
- internal transitions on both states
- transitions with events, guards and actions
- transition to a `[*]` final state

### Digital watch

Models a watch where pressing `set` cycles between displaying time, setting
hours and setting minutes. Hours and minutes increment modulo their max
values via internal transitions.

Features: junction on initial transition (`SettingHours` if time is zero,
`Idle` otherwise), `do` actions reading the parameter, internal transitions
with arithmetic actions.

### Simple composite (hierarchical)

Showcases a single nested composite state with two leaves (ON / OFF). An
external `disable`/`enable` event exits/enters the entire composite.

### Complex composite (deep hierarchy)

Three levels of nested states (`State0` ⊃ `State1` ⊃ `State11` and
`State0` ⊃ `State2` ⊃ `State21` ⊃ `State211`). The transition `State1 -> State211`
crosses a deep boundary, validating the LCA computation.

### Simple orthogonal (concurrent)

Demonstrates a composite state with two parallel regions, each ending in its
own final state. When all regions finish, the parent moves to `DisableSystem`.

### Lane Keeping System (LKS)

Realistic vehicle FSM. The driver toggles the LKS button and the system
transitions across `LKSModeOff`, `LKSModeOn`, `DetectLane`, `FollowLane` and
`WaitDetect`, updating LED and servo flags on every transition.

### Ethernet box

Pairing manager for a Wi-Fi-enabled Ethernet box: discoverable mode, paired,
disconnected, etc. Internal transition handles "short button press" while in
`WifiConnected` to launch the home screen without changing state.

### Pump dead-man controller (Pompe)

Hierarchical FSM modelling a pump that runs only while the operator holds the
HM (Homme Mort) lever. Releasing it triggers a chain of timeouts (40s, 20s,
5s) and alarms before stopping the pump. Emergency buttons (AU, BPA, DPP)
short-circuit any active state and return to the idle state.

Features: hierarchical composition, timeouts via `TimeoutEvent`, multiple
group-level transitions.

For a fast demo run all timeouts are scaled down by a factor of 10 so the
40s/20s/5s become 4s/2s/0.5s. The scenario performs:

1. Press `BPM` (HM held) — pump starts.
2. Wait ~20 s of simulated time, then press `BPA` to stop.
3. Press `BPM` again to restart.
4. Release HM (`falling HM`) — alarm type 2 fires.
5. Press `BPM` once more (no effect, we are already in `AttenteHM`).
6. Press HM again (`rising HM`) — pump resumes.

### Triggers

Demonstrates how the framework picks among several transitions sharing the
same trigger event. The example has three outgoing transitions from `A`:

- `A -> B` on event `e` and guard `[x == 10]`
- `A -> C` on event `e` (no guard)
- `A -> D` on guard `[x > 10]` only (trigger-less)

Three runs show how the same model behaves with `x = 0`, `x = 10` and
`x = 20`.

### Self-Parking assistant

Realistic Park-Assistant FSM. The driver activates the turning indicator,
the system either scans for a free slot (driving in) or computes a leaving
trajectory. The scenario then drives along the trajectory until the
controller can no longer drive, ending in `TRAJECTORY_DONE`.

Features: multiple guarded outgoing transitions, scanner sub-status,
re-using the FSM after a complete cycle.

### Infinite-loop detection

Three states `State1 -> State2 -> State3 -> State1` connected with
trigger-less transitions form an infinite loop. The example lowers the
detection threshold to 10 (instead of the default 1000) so the run
terminates quickly and shows the framework throwing an
`InfiniteLoopException` after a few cycles.

The threshold can be tuned per-chart with `Statechart::setInfiniteLoopThreshold()`.

### GettingStarted

The original example from the README; combines hierarchy, concurrency,
junctions, timeouts, internal transitions and asynchronous parameter
mutation.

## Notes about the port

These examples are translations of the PlantUML diagrams to the
`statechart::` C++ API. The PlantUML annotations starting with `'[code]`,
`'[header]`, etc. are PlantUMLStatecharts-specific and have been translated
to the equivalent C++ idioms (custom `Parameter` subclass, `Event` subclasses,
`Action`/`Guard` lambdas).

Each example follows the same structure:

1. Define a `Parameter` subclass holding domain data.
2. Declare `Event` subclasses for each named trigger.
3. Express `Action`s and `Guard`s as lambdas capturing/casting the parameter.
4. Build the chart with `chart->create<...>(...)` and `chart->createTransition(...)`.
5. Drive the chart with `chart->dispatch(data, &event, parameter)`.
