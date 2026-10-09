# Heavy MinOTAut

Heavy Minimization algorithms for Tree Automata

Copyright (c) 2014-16 Ricardo Almeida (LFCS - University of Edinburgh)

## Downloading

```sh
git clone https://github.com/ric-almeida/heavy-minotaut.git
```

## Prerequisites

- CMake 3.20 or newer
- A C++ compiler with C++11 support
- Boost filesystem and system development libraries (version 1.54.0 or newer)

## Building

From the repository root:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

The first configure fetches libvata from its upstream Git repository, so Git and
network access are required. CMake builds libvata's library target directly,
without its separate CLI and test suite. The dependency is pinned to a specific
upstream revision in [CMakeLists.txt](./CMakeLists.txt), rather than tracking the
upstream default branch.

Run the command-line smoke test with:

```sh
ctest --test-dir build --output-on-failure
```

Install the executable under a chosen prefix with:

```sh
cmake --install build --prefix /your/install/prefix
```

## Input format

libvata supports the Timbuk format of tree automata. Its grammar is specified
below, with `<file>` as the start symbol:

```text
<file>            : 'Ops' <label_list> <automaton>

<label_list>      : <label_decl> <label_decl> ... // a list of label declarations

<label_decl>      : string ':' int // a symbol declaration (the name and arity)

<automaton>       : 'Automaton' string 'States' <state_list> 'Final States' <state_list> 'Transitions' <transition_list>

<state_list>      : <state> <state> ... // a list of states

<state>           : string // the name of a state

<transition_list> : <transition> <transition> ... // a list of transitions

<transition>      : <label> '(' <state> ',' <state> ',' ... ')' '->' <state> // a transition

<label>           : string // the name of a label
```

For example:

```text
Ops a:0 b:1 c:2

Automaton A
States q0 q1 q2
Final States q2
Transitions
a() -> q0
b(q0) -> q1
c(q1, q1) -> q1
c(q1, q1) -> q2
c(q2, q2) -> q2
```

## Usage

Run `build/minotaut -help` for command-line options.

## Publications

- R. Almeida, L. Holik, R. Mayr. "Reduction of Non-deterministic Tree Automata." *TACAS 2016*, pp. 717-735. Springer, 2016.
- R. Almeida. "Reducing Nondeterministic Tree Automata by Adding Transitions." Submitted to MEMICS 2016.
