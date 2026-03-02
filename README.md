# 16-bit CPU Simulator in C

I stumbled upon a post on X about building a 16-bit CPU in C and got curious — how does a CPU actually work at the low level? I always thought it was some incredibly complex black box. Turns out it's not. It's actually surprisingly simple and elegant once you understand it. So I built one from scratch.

This is a fully working 16-bit CPU simulator written in C, complete with its own assembler. You can write assembly programs, run them, and see the results — all on a CPU I built myself.

---

## What I Learned

Before this project I had no idea how instructions actually execute inside a CPU. I knew high level stuff like "registers are fast" but I didn't know *why* or *how*. After building this:

- I understand exactly how a CPU fetches, decodes and executes instructions
- I know why registers exist and why there are so few of them
- I understand how jumps and loops work at the hardware level — it's literally just changing a number (the Program Counter)
- I understand how function calls work — CALL saves your place on a stack and jumps, RET just comes back
- I know what flags are and how conditions like `if (a == b)` translate to actual hardware

The thing that surprised me most — I always assumed CPUs were insanely complex. But the core idea is dead simple. The worker reads a task, does it, moves to the next task. That's it. Everything else is just details.

---

## Architecture

```
┌─────────────────────────────────────────────┐
│                  THE OFFICE                 │
│                                             │
│  ┌───────────┐    ┌───────────────────────┐ │
│  │  FILING   │    │       THE DESK        │ │
│  │ CABINET   │    │                       │ │
│  │           │    │  R0  R1  R2  R3       │ │
│  │  65,536   │    │  R4  R5  R6  R7       │ │
│  │   bytes   │    │   (8 Registers)       │ │
│  │ (Memory)  │    │                       │ │
│  └───────────┘    │  [ALU]  [PC]  [SP]    │ │
│                   └───────────────────────┘ │
│  ┌───────────┐                              │
│  │  STATUS   │                              │
│  │  BOARD    │                              │
│  │ [Z][N][O] │                              │
│  └───────────┘                              │
└─────────────────────────────────────────────┘
```

- **8 registers** (R0-R7) — temporary fast storage, like sticky notes on a desk
- **64KB memory** — the filing cabinet, holds both program and data
- **Program Counter (PC)** — the checklist, always points to next instruction
- **Stack Pointer (SP)** — inbox tray, used for function calls and temporary saves
- **Flags (Z, N, O)** — status board updated after every calculation
- **ALU** — the calculator, handles all math and logic

---

## Instruction Set

23 instructions total:

| Category | Instructions |
|----------|-------------|
| Data Move | LOAD, MOV, LDR, STR |
| Arithmetic | ADD, SUB |
| Logic | AND, OR, XOR, NOT, SHL, SHR |
| Compare | CMP |
| Jump | JMP, JZ, JNZ, JN |
| Stack | PUSH, POP |
| Function | CALL, RET |
| Control | NOP, HALT |

---

## How Instructions Are Encoded

Every instruction is exactly 16 bits. The bits are packed like this:

```
|15      11|10      8|7       5|4        0|
|  opcode  |   dst   |   src   |   imm5   |
|  5 bits  |  3 bits |  3 bits |  5 bits  |
```

5 bits for opcode supports up to 32 different instructions. 3 bits for each register supports R0-R7. This is how you fit an entire instruction into just 2 bytes.

---

## Project Structure

```
16-bit-CPU-Simulator/
├── cpu.h           → CPU struct (registers, memory, flags, pc, sp)
├── opcodes.h       → All 23 opcodes as an enum
├── instruction.h   → Macros to encode and decode instructions
├── memory.c        → Read and write 16-bit values to byte memory
├── alu.c           → All arithmetic and logic operations + flag updates
├── cpu_execute.c   → The fetch-decode-execute loop
├── assembler.c     → Two-pass assembler (assembly text → machine code)
└── main.c          → Loads a program and runs it
```

---

## The Hardest Part — The Assembler

Honestly the CPU itself was straightforward. The assembler was the hardest part.

The problem is **forward references** — what if you write `JMP loop` but `loop` is defined later in the code? You don't know the address yet.

The solution is a **two-pass assembler**:
- **Pass 1** — scan through all lines, just record where every label lives in memory
- **Pass 2** — go through again, encode every instruction, look up labels from pass 1

Same technique used by the earliest real assemblers ever built.

---

## How to Build and Run

**Requirements:** GCC (any version)

```bash
gcc main.c alu.c assembler.c memory.c cpu_execute.c -o cpu
```

**Run on Windows:**
```bash
.\cpu.exe
```

**Run on Linux/Mac:**
```bash
./cpu
```

---

## Example — Sum of first 100 numbers

```asm
LOAD R0, 0       ; sum = 0
LOAD R1, 1       ; counter = 1
LOAD R2, 101     ; limit
LOAD R3, 1       ; increment
loop:
ADD R0, R1       ; sum += counter
ADD R1, R3       ; counter++
CMP R1, R2       ; reached limit?
JNZ loop         ; if not, keep going
HALT
```

Output:
```
CPU halted after 404 cycles.
R0 = 5050
```

---

## Example — Fibonacci(13)

```asm
LOAD R0, 0
LOAD R1, 1
LOAD R2, 13
LOAD R3, 0
LOAD R4, 1
loop:
CMP R3, R2
JZ end
MOV R5, R1
ADD R1, R0
MOV R0, R5
ADD R3, R4
JMP loop
end:
HALT
```

Output:
```
R0 = 233  (F13 = 233) ✓
```

---

## Example — Find Maximum in Array

```asm
LOAD R0, 200
LOAD R1, 91
STR R0, R1
; ... store array elements ...
; after running:
; R4 = 91 (maximum value found)
HALT
```

---

## Why Build This?

Because reading about CPUs and actually building one are completely different things. After this project, every time I write a for loop or call a function in C, I know exactly what's happening at the hardware level underneath. That mental model is worth more than any textbook.

---

## Author

**Deepak** — 3rd year BTech CSE student  
Built this after seeing a post on X and getting curious about how CPUs actually work at the lowest level.

> "I used to think CPU internals were incredibly complex. After building one I realised it's actually simple and elegant. The CPU just reads a task, does it, and moves to the next one. Everything else is details."
