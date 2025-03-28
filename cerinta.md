# Homework 1

## Assignment: Simple C-to-Assembly Compiler

**Deadline soft**: 6 aprilie 2025, 11:55PM

**Deadline hard**: 11 aprilie 2025, 11:55PM

**Authors**

* Robert Grancsa
* Adelina Alexe

## Introduction
![](banner.webp)

In a neon-lit room filled with vintage circuit boards and digital memes, Alex—a modern TikTok brain rot specialist—devised a daring plan to escape Instagram’s endless scroll. Determined to reclaim his mind, he crafted a groundbreaking compiler that transformed addictive algorithms into streams of artful data. With every line of code, toxic noise became immersive stories, intricate music, and thought-provoking visuals, forging a digital sanctuary where creativity reigned. As the algorithmic chains broke, Alex emerged as a quiet revolutionary—a cyber alchemist turning the toxic into transcendent.

### Objective

Develop a small compiler or translator that converts simple C-like code snippets into basic assembly instructions. The primary goal is to familiarize yourself with assembly language mnemonics and to understand how high-level constructs map to low-level operations. This assignment is intentionally minimalistic to ease you into both assembly language and compiler design. If we actually want to be pedantic, this is actually a [transpiler](https://en.wikipedia.org/wiki/Source-to-source_compiler) implementation.

### Theme

_Simple C Statements → Real Assembly_
The translation should be as simple as possible while covering basic arithmetic operations, register usage, data movement, and control flow constructs. On an lower level, this is what happens in the background when compiling a C program, but it is simplified for easier implementations. If you are really intersted about how a compiler works in reality, we recommend you the 4th year course of [Compilers](https://gitlab.cs.pub.ro/Compilatoare)

## Conventions and Guidelines

Because we want to prevent you from having to juggle with registers, we'll use some simple conventions.

- **Basic Register Mapping:**
  - `A` → `eax`
  - `B` → `ebx`
  - `C` → `ecx`
  - `D` → `edx`
- **Data types**
  - We'll assume all of the data types are **4 bytes**
  - When you see a number, we will treat it as a int (4 bytes)
  - When handling pointers, those will also be stored as 4 bytes

## Instructions

### MOV

The mov instructions is the simplest of all, and as the name says, it moves the data from one place to another. More details here

#### Usage

`MOV destination, source`

#### C - ASM translation

| **C Code** 	| **ASM Code**   	|
|------------	|----------------	|
| `a = 1;`   	| `MOV eax, 1`   	|
| `b = a;`   	| `MOV ebx, eax` 	|

**Note**: There will be no `MOV 2, eax` as that is an invalid operation

### Logical operations

#### Usage

`AND destination, source`
`OR destination, source`
`XOR destination, source`

#### C - ASM translation

| **C Code** 	    | **ASM Code**    |
|------------	    |---------------- |
| `a = a & 0xFF;`   | `AND eax, 0xFF` |
| `b = b \| a`      | `OR ebx, eax`   |
| `c = a ^ c;`      | `XOR ecx, eax`  |

### Arithmetic operations

There are 4 aritmetic operations, but we will take them 2 by two, as there are differences between them. The first two, add and sub, are for + and - operations. These work the same way as the MOV instructions.

#### Usage - add, sub

`SUB destination, source`

#### C - ASM translation

| **C Code** 	    | **ASM Code**   	|
|------------   	|----------------	|
| `a = a + 5;`   	| `ADD eax, 5`   	|
| `b = b - a;`   	| `SUB ebx, eax` 	|

#### Usage - mul, div

`MUL source2`
`DIV divisor`

You might be wondering, why only one operand? Here we have a special rule, and goes something like this:

When multiplying with MUL, the multiplication will actually take EAX and source2 as the multiplication values, and set them in 2 registers -> EDX and EAX. EDX will store the higher values, and EAX the lower one, acting as a big 8 byte register, because after multiplying two 32 bit number, we might have an overflow. Looking at the table will make it a bit clearer.

Similar to MUL, DIV works with EAX as the primary operand, but it also considers EDX as part of the dividend. This means the division is performed using a 64-bit value (EDX:EAX) divided by the given divisor. The result is stored in EAX, while the remainder is placed in EDX.

| **C Code** 	    | **ASM Code**   	|
|------------   	|----------------	|
| `a = a * 3;`   	| `MUL 3`   	    |
| `b = b * c;`    | `MOV eax, ebx`  |
|                 | `MUL ecx`       |
|                 | `MOV ebx, eax`  |
| `a = a / 3;`    | `MOV eax, a`    |
|                 | `DIV 3`         |
|                 | `MOV a, eax`    |
| `b = b / c;`    | `MOV eax, ebx`  |
|                 | `DIV ecx`       |
|                 | `	MOV ebx, eax` |

**Note**: For simplicity, we will never use eax as source2, and we will never never use the value from EDX.

If you use the *DIV* instruction with a **divisor of 0**, the processor will generate a **"Divide Error" exception**, and the program will stop if it is not handled.

Since division requires **EDX** to be set up properly, we should always **zero** it before performing DIV if we are working with unsigned values.

### Shift operations

SHL (Shift left) and SHR (Shift right) are bitwise shift instructions.

#### Usage - shl, shr

`SHL destination, no_bits_shifted`

`SHR destination, no_bits_shifted`

- SHL:  Moves bits to the left, filling the rightmost bits with zeros. Each shift effectively **multiplies** the value by 2.
  - Example: `00001100` (`12` in decimal) shifted left by 1 becomes `00011000` (`24` in decimal).

- SHR: Moves bits to the right, filling the leftmost bits with zeros. Each shift effectively **divides** the value by 2.
  - Example: `00001100` (`12` in decimal) shifted left by 1 becomes `00000110` (`6` in decimal).

| **C Code**       | **ASM Code**    |
|-------------     |---------------- |
| `a = a << 1`     | `SHL eax, 1`    |
| `b = b >> 2`     | `SHR ebx, 2`    |

### Conditional Statements

#### CMP Instrunction

`CMP` instruction compares two values by substracting the second operand from the first. It updates CPU flags, but does not store the result.

#### Usage - cmp

`CMP operand1, operand2`

- Example:

  `MOV eax, 3`

  `CMP eax, 3` ; Compares eax with 3

  `JE equal_label` ; Jumps to label "equal_label" if eax == 3 (Zero flag is set)


### Jumps

Here's a short description of each conditional jump:

- **JMP** (Jump): Jumps to the given label.
- **JE** (Jump if Equal): Jumps to the given label if the two compared values are equal (e.g.: `CMP` sets the Zero Flag).
- **JNE** (Jump if Not Equal): Jumps if the two values are not equal (Zero Flag is not set).
- **JG** (Jump if Greater): Jumps if the first value is greater than the second (Signed comparison).
- **JGE** (Jump if Greater or Equal): Jumps if the first value is greater than or equal to the second.
- **JL** (Jump if Less): Jumps if the first value is less than the second.
- **JLE** (Jump if Less or Equal): Jumps if the first value is less than or equal to the second.

#### Usage

`JMP label`

`JE label`

`JNE label`

`JG label`

`JL label`

#### C - ASM translation

| **C Code**                  | **ASM Code**            |
|-------------                |----------------         |
| `if (a == b) {`             | `CMP eax, ebx`          |
|                             | `JNE else_label`        |
|     `// some code here`     | `; some code here`      |
|                             | `JMP end_if`            |
| `} else {`                  | `else_label:`           |
|     `some other code here`  | `; some other code here`|
| `}`                         | `end_if:`               |

### Loops - for, while

#### `for` loop

| **C Code**                    | **ASM Code**          |
|-------------                  |----------------       |
| `for (a = 0; a < 10; a++) {`  | `MOV eax, 0`          |
|                               | `start_loop:`         |
|                               | `CMP eax, 10`         |
|                               | `JGE end_loop`        |
| `// some code here`           | `; some code here`    |
|                               | `ADD eax, 1`          |
|                               | `JMP start_loop`      |
| `}`                           | `end_loop:`           |

#### `while` loop

| **C Code**          | **ASM Code**                                          |
|-------------        |----------------                                       |
| `while (b < 5) {`   | `start_loop:`                                         |
|                     | `CMP ebx, 5`                                          |
|                     | `JGE end_loop`                                        |
| `// some code here` | `; some code that makes b greater than or equal to 5` |
|                     | `JMP start_loop`                                      |
| `}`                 | `end_loop:`                                           |        

### Coding style

Coding style can be run directly in the checker, by pressing `C`, or by using the
`cs.sh` executable from `checker/cs`. The points are the folowing:

- \>= 10 of `CHECK` => -5 points
- \>= 5 of `WARNING` => -5 points
- \>= 1 of `ERROR` => -10 points

## Notes

- The implementation can be done in any file, and the executable must be named 
  `transpiler`, situated in the root of the folder, same as the initial makefile does
- For sending the homework, you can use the `make pack` rule, which automatically creates
  a zip file with the necesarry content already in it. Don't forget to change your name first

## Checker

> ! Atention, the checker doesn't run valgrind by default, but you must pass all the test
with valgrind in order to get the score. To activate valgrind on your checker, press
`v`, making sure there is a red border which will show that it is on.

### Instalation steps

You can use the `./install.sh` to install all the dependencies and the checker.
If you find any errors along this process, try to follow the steps below:

1. Download rustup using
`curl https://sh.rustup.rs -sSf | sh -s -- -y`. If you get any errors
after this step, add cargo to the PATH variable using `source "$HOME/.cargo/env"`.

2. Run the following command to install the checker
```bash
$ cargo install hw_checker
```

After install, you can use the `hw_checker` command to run the checker.

### Instructiuni checker

For a list of commands and guides for the checker, please read the README from the
`src/` directory. If you have any problems running the GUI, you can use the
`hw_checker --legacy` command to run them in text only mode.
