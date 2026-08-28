# Batch 130 — two levers that recover functions I had written off

Verified on a clean `make clean && make compare` — `goldensun.gba: OK` — with
every address read out of the linked ELF, and the new absolute symbol
`_MSG_2399` confirmed present at 0x2399.

## Elevated (2224 → 2219)

| function | address | notes |
|---|---|---|
| `OvlFunc_950_20087b0` | 0x020087b0 | needed BOTH new levers |
| `OvlFunc_921_20082b8` | 0x020082b8 | **recovered from a park** |
| `OvlFunc_922_2008920` | 0x02008920 | first screen |
| `OvlFunc_922_20087f0` | 0x020087f0 | first screen |
| `OvlFunc_901_20089f8` | 0x020089f8 | twin; **closes a park** |

## The no-prototype lever

When the ROM sets up a two-`mov` argument block as `mov r1 / mov r0` and gcc
emits `mov r0 / mov r1`, **remove the callee's prototype**.

I parked this shape twice with reasoning that sounded solid — *neither argument
is a split build, so the interleave lever has nothing to work with*. The control
refutes it: **332 of 3428 solved functions** contain that order, and the smallest
example (`src/overlays/rom_78603c/ovl_30_c_c_b.c`) declares **nothing at all** —
four calls, no externs — producing `mov r1,#0 / mov r0,#16` from
`__Func_8093054(0x10, 0)`.

Applied to the parks: `OvlFunc_921_20082b8` went from 2 of 74 differing to exact
and its park is deleted; `OvlFunc_950_20087b0` from 2 of 61 to exact.

It is not universal. `OvlFunc_952_20085a4` calls the same function in **both
arms** of a branch and the ROM orders its arguments differently in each, so no
single declaration choice satisfies both — that is the recorded discriminator.
Cost: the file loses type checking for that callee, so remove only the prototype
the call needs.

## Symbol bases: gcc spends a register for a symbol, not for an integer

The ROM holds a message id in a callee-saved register and derives neighbours:

    ldr r5, =0x2399 ... add r0, r5, #1 ... add r0, r5, #2

Written `int m = 0x2399;`, gcc emits three independent pool loads and does not
keep `m` alive at all — rematerialising is cheaper than a push/pop pair, so it
never spends the register. Mutating the variable is worse: each arm folds
independently, 73 differing.

Declaring the base as an absolute symbol — `extern int _MSG_2399;` and
`m = (int)(&_MSG_2399);` — produces the register and both `add`s. **73 differing
to 3.**

This gives the batch-123 derived-constant rule the lever it lacked. That rule
said derivation is reachable when the base is *already* forced into a register;
this says how to force it when nothing else does. `message.sym` already carried
a section comment describing exactly this shape, so the entry went under it.
`stage1.o` has to be deleted by hand — `message.sym` is not a tracked dependency.

## A function with no conditional branch has only the flag group

Both source-level levers for constant placement need a dominating block: the
naming lever against constant CSE, and the interleave lever for argument order.

`OvlFunc_939_20095bc` is 23 calls, no memory operations and **no branch at all**.
gcc commons its one repeated shifted constant into a callee-saved register;
neither lever reaches it, and `-fno-rerun-cse-after-loop` does not either, so the
commoning is the main -O2 CSE rather than the rerun.

Check the branch count first. Four parks in this session were straight-line
cases where I tried the naming lever anyway — on one it cost nine instructions
and three extra pushes (36 lines to 49).

## Also this batch

- **`GetFlag(id)` guarding a block that ends `SetFlag(id)` means `CSE_CFLAGS`.**
  Now six cases to one, and visible in the ROM listing before writing any C.
- **Density, not size, is the selector.** Two rounds picking the smallest
  candidates produced zero matches across eleven functions; filtering by
  `calls*4 >= insns` gave three of four.
- **A detector returned 0 of 2227 twice** because I compared against a space
  where the assembly has a tab. When the first fix does not move a zero, suspect
  the harness rather than refining the pattern again.
- **Consume the pointer, do not index it** — `a += 0x62; *a = 0;` instead of
  `a[0x62] = 0;`, worth 28 differing to 7.

## Still owed

- 12 not-yet-elevated `.s` TUs inside Makefile wildcards.
- ~3,300 lines of duplicate Makefile rule blocks.
- 285 parks. Two were recovered this batch by levers found after they were
  written, which is the second and third time that has happened. A sweep is
  owed but is deliberately being left until the fresh pool thins.
