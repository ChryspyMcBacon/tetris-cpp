# Tetris in C++ — recursive piece parser & game engine

A simplified Tetris engine written in modern C++ (C++17) as the final project for **Programming and Laboratory** (Ca' Foscari University of Venice, A.Y. 2024/2025).

The focus is not the graphics but the core logic: parsing pieces from a compact **recursive (quadtree) format**, placing them on the board, cutting full rows and letting the remaining pieces fall — all with **manual memory management** and no STL containers.


## Features

**`piece`** — a square boolean grid (side = power of 2) with a color
- Recursive parser (`operator>>`) for the quadtree format, whitespace-tolerant, with input validation via `tetris_exception`
- Recursive serializer (`operator<<`) that writes the most compact representation (`()` full, `[]` empty quadrants)
- 90° clockwise rotation, row cutting with downward shift
- Rule of five: deep copy, move semantics, destructor on a dynamically allocated `bool**`

**`tetris`** — the game board
- Pieces stored in a hand-written **singly linked list**
- `containment` check for collisions and bounds (pieces can have negative x)
- `insert`: drops a piece, cuts full rows, removes emptied pieces and shifts the others down until the board is stable; updates the score
- Forward `iterator` / `const_iterator`
- Save/load the whole game state through streams

## Recursive piece format

```
side color grid
```
where a quadrant is `()` (full), `[]` (empty) or `(TL TR BL BR)` (four sub-quadrants). Example:

```
4 75 (([]()[]())(()[]()[])([]()()())(()[]()()))
```
```
- X X -
- X X -
- X X -
X X X X
```

## Project structure

```
.
├── src/
│   └── tetris.cpp     # my implementation
├── docs/
│   ├── assignment.md  # original assignment text (by the course instructors)
│   └── figures/
└── README.md
```

## Notes on building

Only the implementation file is included. As required by the assignment, `tetris.cpp` has no `main` and relies on the header `tetris.hpp` (class declarations) provided by the course, which is not part of this repository. The code was evaluated by the instructors with their own test suite, compiled with `-std=c++17`.

## What I learned

- Recursive descent parsing and quadtree representations
- Manual memory management, RAII and the rule of five
- Implementing a linked list and STL-style iterators from scratch
- Debugging with sanitizers and Valgrind

## Credits

The assignment, the header `tetris.hpp` (not included) and the figures were provided by the course instructors of *Programmazione e Laboratorio* at Ca' Foscari. The implementation in `tetris.cpp` is my own.
