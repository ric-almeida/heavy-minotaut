# Heavy MinOTAut

Heavy MinOTAut (Heavy Minimization Of Tree Automata) is a C++ command-line
tool for reducing nondeterministic tree automata. It implements the Heavy
minimization techniques based on lookahead downward and upward simulations,
with optional saturation-based reductions.

The program reads automata in Timbuk format and uses libvata for automaton
parsing, serialization, and core operations.

## Project status

This is research software accompanying work on tree-automata reduction. The
main algorithms are exposed through the CLI; the `testers/` directory contains
the processing and measurement routines used by those commands. See
[Publications](#publications) for the related research.

## Quick start

### Requirements

- CMake 3.20 or newer
- Git (to fetch libvata on the first configure)
- A C++ compiler with C++11 support
- Boost filesystem and system development libraries (version 1.54 or newer)
- Network access on the first configure

### Build

```sh
git clone https://github.com/ric-almeida/heavy-minotaut.git
cd heavy-minotaut
cmake -S . -B build
cmake --build build --parallel
```

If no build type is specified, CMake defaults to `Release`. With GCC and Clang,
Release builds use `-O3` optimisations.

Building in debug mode:

```sh
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug --parallel
```

Run the CLI help or the registered smoke test:

```sh
./build/minotaut -help
ctest --test-dir build --output-on-failure
```

To install the executable:

```sh
cmake --install build --prefix "$HOME/.local"
```

The build fetches libvata from its upstream Git repository at the immutable
revision pinned in [CMakeLists.txt](./CMakeLists.txt). CMake builds libvata's
library target directly.

## Command-line modes

| Command | Purpose |
| --- | --- |
| `minimize` | Apply Heavy using downward and upward lookahead. |
| `minimize_with_saturation` | Apply Heavy followed by saturation-based reductions. |
| `minimize_and_complement` | Minimize an automaton, then complement it. |
| `complement` | Complement an automaton without first minimizing it. |
| `compute_la-dw-sim` | Compute lookahead downward simulations for automata in directories. |

Pass `-help` to see the complete option list. A command takes one or more
input files by default; use `-input_format=dir` to process files in a directory.
The `-output=DIR` option writes transformed automata to that directory. Without
it, commands report measurements without writing automata.

## Examples

[`examples/sample.timbuk`](./examples/sample.timbuk) is a small Timbuk
automaton. From the repository root, apply Heavy with downward and upward
lookahead 1:

```sh
./build/minotaut minimize -output=out 1 1 examples/sample.timbuk
```

The minimized automaton is written under `out/` with a name similar to
`sample_minimized_with_Heavy(1,1).timbuk`. Omit `-output=out` to print the
measurements without saving the resulting automaton. The output directory is
created automatically when it does not exist.

To enable saturation-based reductions, provide the two additional saturation
lookahead values:

```sh
./build/minotaut minimize_with_saturation -output=out 1 1 1 1 examples/sample.timbuk
```

The four numbers are, in order, downward and upward lookahead for Heavy,
followed by downward and upward lookahead for saturation. The output is named
similar to `sample_minimized_with_Heavy(1,1)_with_Saturation(1,1).timbuk`.
The saturation version defaults to 2; use `-sat_version=N` to select another
version.

For a directory containing Timbuk files, add `-input_format=dir` and pass the
directory instead of an individual file:

```sh
./build/minotaut minimize -input_format=dir 1 1 path/to/automata/
```

## Timbuk input format

A Timbuk file declares symbols and their arities, states, final states, and
transitions. A transition has the form `symbol(child_state, ...) -> state`;
for a rank-zero symbol, use `symbol() -> state`. For example:

```text
Ops a:0 b:1 c:2

Automaton Sample
States q0 q1 q2
Final States q2
Transitions
a() -> q0
b(q0) -> q1
c(q1, q1) -> q1
c(q1, q1) -> q2
c(q2, q2) -> q2
```

The sample describes a rank-zero symbol `a`, a unary symbol `b`, and a binary
symbol `c`. State names and transition destinations are declared in the
automaton; final states determine acceptance.

## Repository layout

- `src/` - minimization algorithms and shared implementation
- `include/` - application entry-point declarations
- `testers/` - CLI processing and measurement routines
- `examples/` - small Timbuk inputs for trying the CLI
- `test-data/` - archived automata collection

## License

This project is distributed under the GNU General Public License, version 2;
see [LICENSE](./LICENSE).

## Publications

- R. Almeida, L. Holik, R. Mayr. "Reduction of Non-deterministic Tree Automata." *TACAS 2016*, pp. 717-735. Springer, 2016.
- R. Almeida. "Reducing Nondeterministic Tree Automata by Adding Transitions." Submitted to MEMICS 2016.
