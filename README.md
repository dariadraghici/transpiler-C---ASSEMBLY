# C to x86 Assembly Transpiler

This project is the first homework assignment for IOCLA (Computer Operation Engineering and Assembly Language). It implements a transpiler that converts a simplified subset of C source code into x86 Assembly output.

## Overview

The goal of the project is to transpile, not compile, C code into an equivalent assembly representation. The transpiler works line by line on the input, recognizing a small set of constructs and emitting the corresponding assembly instructions. It supports:

- Assignment statements, including compound expressions with a single operator.
- Conditional statements (`if`).
- Loops (`while` and `for`).
- Basic arithmetic and bitwise operators: `+`, `-`, `*`, `/`, `&`, `|`, `^`, `<<`, `>>`.
- Comparison operators: `<`, `>`, `<=`, `>=`, `==`.

## Project Structure

```
├── cerinta.md
├── README.md
└── src
    ├── Makefile
    ├── main.c
    └── main.o
```

## Build

From the `src` directory:

```bash
make
```

## Usage

The program reads C-like source code from standard input, line by line, and writes the generated assembly to standard output:

```bash
./transpiler < input.c > output.asm
```

Each input line is expected to hold a single statement. Leading tabs are stripped before a line is classified and processed.

## Registers

The transpiler works with four single-letter variables, mapped directly to x86 32 bit registers:

| Source variable | Register |
|------------------|----------|
| `a`               | `eax`    |
| `b`               | `ebx`    |
| `c`               | `ecx`    |
| `d`               | `edx`    |

## Supported Constructs

### Assignment

Handles lines of the form `reg = value;` or `reg = value op operand;`, where `value` or `operand` can be either a numeric literal or another register.

- A plain assignment (no operator) becomes a `MOV` instruction.
- An arithmetic or bitwise operator is mapped to its assembly mnemonic: `+` to `ADD`, `-` to `SUB`, `*` to `MUL`, `/` to `DIV`, `&` to `AND`, `|` to `OR`, `^` to `XOR`, `<<` to `SHL`, `>>` to `SHR`.
- Since `MUL` and `DIV` operate implicitly on `eax` in x86, the transpiler inserts extra `MOV` instructions to move the target register's value into `eax`, perform the operation, and move the result back, whenever the destination register is not already `eax`.
- Division by the literal `0` is detected and reported as an error instead of generating code.

### If Statement

Parses a condition of the form `if (reg op value)`, emits a `CMP` instruction comparing the register to the operand, and then emits the inverse conditional jump to `end_label` so that the block is skipped when the condition is false. For example, a `<=` condition emits `JG end_label`, and a `>=` condition emits `JL end_label`.

### While Loop

Emits a `start_loop:` label, then parses the condition the same way as the `if` statement, emitting a `CMP` and the corresponding inverse conditional jump to `end_label`.

### For Loop

Parses a loop of the form `for (reg = start; reg op bound; ...)`. It emits:

1. A `MOV` instruction to initialize the register with the starting value.
2. A `start_loop:` label.
3. A `CMP` instruction against the bound value.
4. The inverse conditional jump to `end_label`.

The register used in the loop is remembered so that the closing brace can generate its increment automatically.

### Closing Brace

When a `}` is encountered, the transpiler closes whichever block was last opened:

- If a `for` loop is active, it emits an increment instruction (`ADD reg, 1`) for the loop variable.
- If a loop (`while` or `for`) is active, it emits a `JMP start_loop` to repeat the condition check.
- It then emits the appropriate closing label, `end_loop:` for a `for` loop or `end_label:` otherwise, and resets the internal loop state.

## Notes and Limitations

- The parser relies on fixed character offsets within each line, so it expects a consistent formatting style (spacing and structure) matching the subset described in `cerinta.md`.
- Only single-operator expressions are supported on the right-hand side of an assignment; chained operations are not parsed.
- The `for` loop's increment step is assumed to always be `+1` on the loop variable; arbitrary increment expressions are not evaluated.
- Nested loops and nested conditionals are not explicitly tracked, since only one loop state is kept at a time.
