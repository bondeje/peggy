# Example use of <b>peg4c</b> parser generater: a REPL for <math.h> parser

This is a basic calculator application that provides a REPL to the `math.h` library, but also supports vector-based calculations. Unfortunately, I have no implemented a history or memory yet so the typing is tedious.

NOTE: this leaks a little memory so do not leave the process open. It is memory that needs to remain for when a "history" is implemented

## How to build
1) build <b>peg4c</b> at the top level of the repo
2) in this directory, run one of the following
    - `make` - will build `calculator` binary in `/path/to/peg4c/bin`

## How to use

- run `/path/to/peg4c/bin/calculator` and the help information will be printed

![example calculator](../../images/calculator.jpg)