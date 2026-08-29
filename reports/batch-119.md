# Batch 119 — a class I had written off, reopened

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`. All 34
symbols read back out of `goldensun.elf` / the linked overlay ELFs with
`arm-none-eabi-nm`.

**34 elevated, 5 parked. 2366 → 2332 remaining.**

## The finding of the batch: `.call_via` is not a wall

Batch 118 recorded `.call_via rN` — `mov r12, pc / bx rN`, a Thumb-to-ARM call
into an IWRAM routine — as a **hard wall**, and removed **51 functions** from
the candidate pool on the grounds that gcc-2.96's machine description has
exactly one indirect-call pattern, `bl _call_via_rN`.

That was wrong twice over, and the user's objection is what forced the check:
*the bytes are in the ROM, so something produced them.*

1. **The machine description bounds the code GENERATOR, not the source.** This
   tree already reproduces sequences gcc would never choose, with inline asm —
   `include/dma.h` does it for the DMA registers.
2. **`tryc.py` was silently DROPPING the line.** `.call_via` begins with a dot,
   so the ref parser skipped it as a directive — but it is a *macro* from
   `include/macros.inc` expanding to two real instructions. Every one of the 51
   screened two instructions short per call site, which reads exactly like an
   unmatchable structural difference. **The evidence used to declare the class
   dead was produced by a tool that was not showing me the class.**

`Func_8097a10` is elevated and byte-exact:

```c
static inline int call_via_r4(int a, int b)
{
    register int (*_f)(int, int) __asm__("r4") = Func_8000888;
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile ("\t.align\t2, 0\n\tmov\tr12, pc\n\tbx\tr4"
                      : "=r" (_a) : "r" (_f), "0" (_a), "r" (_b)
                      : "memory", "lr", "r12");
    return _a;
}
```

Three refinements, all measured:

* **Bind the symbol; do not pass the callee as a parameter.** A parameter makes
  gcc materialise the address somewhere and copy it into the bound register —
  `ldr r5, =F / mov r4, r5` against the ROM's single `ldr r4, =F`. Binding it
  directly also freed r5 for the struct pointer, fixing a second difference.
* **Do NOT add `"r2"`/`"r3"` to the clobber list.** It looks obviously right and
  it is wrong: `Func_801cc50` went 53 differing / 56 lines to 61 / 62. Argument
  3 survives all three call sites in r3, which proves the routine does not
  clobber it. Where the ROM moves a value out of r2 it is to free r2 as the
  INDEX register for `ldrsh rD, [rB, rO]`.
* **Single call site is the reachable sub-shape.** Two or more sites sharing one
  pointer is harder — the ROM loads it *late*, after an intervening call,
  reusing a register that held something else. Parked:
  [80b84c0](../src/non_matching/rom_b5000/80b84c0.c),
  [801cc50](../src/non_matching/rom_15000/801cc50.c).

## Two more veneer labels, and two immediate matches

gcc names call veneers from `reg_names`: r10 → `sl`, r11 → `fp`, r12 → `ip`.
`src/lib/call_via.s` carried **only `_call_via_fp`**, so functions calling
through r10 or r12 screened one line dirty on a *symbol name* while being
byte-identical. Adding `_call_via_sl` turned two functions into matches
outright; `_call_via_ip` closes the remaining 3 call sites. r14 has none. A
label emits no bytes.

## A park that was wrong about its class

`OvlFunc_923_2009208` was parked at 67 of 80 on the *certain* zero-label
constant-CSE class — the one the tooling flags as definitionally dead. It
**matches outright**. Membership in that class is evidence, not proof.

Related and cheap: **perturb one copy of a CSE-able constant** before parking on
it. Change the second occurrence so gcc cannot common it, and screen. It turned
one park from "74 differing" into "2, and those two are the literals I changed",
and on another exposed a second blocker the CSE had been hiding.

## A precondition on a lever recorded one round earlier

Round 5 recorded "hoist the argument constants to top-of-function locals" as the
answer to the pool-loads-first shape. It is — **but only where the locals live
in a block that dominates the calls without being their own block.**
`OvlFunc_945_200bd10` is a single basic block; hoisting nine constants made them
simultaneously live, gcc spilled, and the function grew from 77 lines to 97
(13 differing → 96). In a straight-line function the lever is actively harmful.

Two functions this round are parked on that single-block variant
([200bd10](../src/non_matching/ovl_7cb2c0/200bd10.c),
[2009490](../src/non_matching/ovl_780898/2009490.c)), which is enough to call it
a class. Useful boundary alongside it: the **return-type lever** closed the
byte-identical shape on `OvlFunc_881_200b130` — where the racing operand was a
pool-loaded *symbol address* — and does nothing where it is a shifted *integer*.

## Housekeeping

Nine **stale park files** removed: parks for functions that had already been
elevated, several of them by me, where I wired the function and left the park
behind. A stale park inflates a blocked class and invites work that is finished.

`stage1.ld` was **never being committed**: `split_s.py` rewrites it for main-ROM
functions, and `git add asm src overlays` does not reach the repo root. Every
build passed because the change was in the working tree; git simply never saw
it. Any single earlier commit that elevated a main-ROM function is therefore not
independently buildable. HEAD is correct.
