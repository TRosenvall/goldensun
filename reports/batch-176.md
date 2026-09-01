# Batch 176

Five functions and nine parks, over five rounds. Two of those rounds returned
one elevation and zero — the batch's real subject is what ran out and what
replaced it.

## Function breakdown

| # | function | address | file | screens | what it took |
|---|---|---|---|---|---|
| 1 | `Func_807a550` | `0x0807a550` | [rom_79460_c_c_c_c_a_c_c_c_c_b.c](src/rom_77000/rom_79460_c_c_c_c_a_c_c_c_c_b.c) | 4 | named store addresses; accumulator birth; `GCSE_CFLAGS` |
| 2 | `Func_80c1fa8` | `0x080c1fa8` | [rom_c1a34_a_a_a_b.c](src/rom_b5000/rom_c1a34_a_a_a_b.c) | 8 | initialiser permutation; multiply operand order; named byte offset |
| 3 | `OvlFunc_880_20082f4` | `0x020082f4` | [ovl_30_c_c_a_a_c_b.c](src/overlays/rom_7795e8/ovl_30_c_c_a_a_c_b.c) | 3 | store-and-return arms |
| 4 | `OvlFunc_945_2009894` | `0x02009894` | [ovl_30_c_c_c_c_c_c_a_a_a_a_a_a_c_a.c](src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_a_c_a.c) | **1** | nothing |
| 5 | `OvlFunc_921_2008384` | `0x02008384` | [ovl_30_c_c_c_a_a_c_a_c.c](src/overlays/rom_7a7298/ovl_30_c_c_c_a_a_c_a_c.c) | 7 | pointer advanced in place; named stored value; **the basic-block lever** |

All five verified in the linked ELF with a `.gcc2_compiled.` symbol at the
address after a clean `make clean && make && make compare`.

## THE ARG-INTERLEAVE WALL IS REACHABLE

This is the batch's main result, and it retires a class that has been parked on
repeatedly.

`OvlFunc_921_2008384` reached the ROM's exact 133 lines with eight differing,
and all eight were four copies of one shape:

```
    rom    mov r1, #0x80 / mov r0, #0x8 / lsl r1, #0x1 / mov r2, #0x28
    ours   mov r1, #0x80 / lsl r1, #0x1 / mov r0, #0x8 / mov r2, #0x28
```

Batch 42 read `local-alloc.c` and worked out where the split comes from:
`update_equiv_regs` declines to keep a constant equivalence when the pseudo is
live in more than one basic block, so **assigning the constant in a block that
DOMINATES the call forces gcc to rebuild it split at the use**. Every function
parked on this class since has been straight-line, and
[`200bdec.c`](src/non_matching/ovl_7cb2c0/200bdec.c) states plainly that a call
does not create the block the lever needs.

This function has two early-return `if`s before the affected calls, so the entry
block genuinely dominates them. Four locals assigned at the top, passed to the
four calls — eight differing to zero.

> **The wall is not the interleave. It is the absence of a dominating branch.**
> Read the control-flow graph before parking anything on this class.

It does not contradict batch 172's "a named constant cannot create register
pressure." That entry is about a ROM holding a literal in a register for its own
sake; this is about where gcc is willing to REBUILD one.

## WHEN A BAND EMPTIES, RAISE THE CEILING BEFORE CHANGING METHOD

Two rounds of this batch returned one elevation and then none. The cause was
measurable: of 1,332 unelevated functions, 465 sit in the 20–120 instruction
band, 137 of those avoid r8–r12/r14, and **only 8 of those 137 avoid a repeated
expensive constant** — four of them m4a. The band was worked out.

Raising the ceiling to 260 instructions returned **thirteen fresh clean
candidates**, most of them zero-callee-saved cutscene scripts, and the first one
tried matched.

> A 130-instruction script with 41 calls is 41 easy transcriptions and one or
> two real questions. **Work scales with the number of DISTINCT residues, not
> with size.**

## A CHAIN OF ARMS THAT EACH STORE AND RETURN IS NOT ONE THAT ASSIGNS AND JOINS

`OvlFunc_880_20082f4` maps a character code through fifteen arms and writes one
byte. The ROM is a plain chain with a single `strb` at a join, and written the
way that reads — `v = c + 0x41` per arm, `out[0] = v` once — gcc comes out at
**58 lines against 86**. It speculates each arm's computation above its test,
which is legal because an assignment has no side effect.

Putting the store inside each arm matches exactly. A store cannot be speculated
above a test, and cross-jumping then merges the fifteen identical
`strb / pop / bx` tails back into the one the ROM shows.

> A ROM whose arms all branch to a single store does NOT mean the source had a
> join variable. The discriminator is whether the arm's COMPUTATION sits before
> or after its test.

An explicit `goto` chain reproducing the ROM's control flow is byte-identical to
the join form — gcc canonicalises it — so this is reachable only by giving each
arm a side effect. Six flag groups inert, including `-fno-thread-jumps`.

The follow-up matters too: `OvlFunc_945_2009894`'s message-id chain is the same
shape, but each arm CALLS `__MessageID`, so the plain if/else chain is already
correct. **An arm with a side effect needs no rewriting.**

## TWO PASSES THAT SHARE A FLAG NAME

Batch 175 parked three functions on duplicate-constant CSE with `-fno-gcse`
inert. `Func_807a550` matched **with** `-fno-gcse`. Both are right:

| symptom | pass | `-fno-gcse` |
|---|---|---|
| one constant materialised once and kept in a register | `cse.c`, local | inert |
| a redundant LOAD sunk onto the only path that needs it | global CSE / PRE | **fixes it** |

`Func_807a550` re-reads a loop bound through a pointer each iteration because a
byte store inside the loop may alias it. At -O2 gcc is *smarter* than the
original build — it sinks the reload onto the branch where the store happens and
skips it otherwise. Read the diff before reaching for the flag.

A third direction turned up in `Func_8011164`: a pool load HOISTED out of a loop
the ROM reloads in. `-fno-gcse` is inert there because that motion is `loop.c`'s
invariant hoisting, which gcc-2.96 has no switch for. The recorded GCSE case
(`rom_f0254_a_b.c`) is the mirror image — a load SUNK into a loop — and the flag
works there.

## POOL SCAFFOLDING, AND A WAY TO REMOVE IT

`Func_80f6148` is two lines SHORT of its ROM because the ROM contains two `b`
instructions to the immediately-following label — jumps over a literal pool
emitted mid-function. `Func_801bd98` is one line LONG for the same reason with
the sign flipped: ours has the `b`.

> A one-line difference in EITHER direction, adjacent to a `b` whose target is
> the next label, is constant-pool placement.

And `OvlFunc_921_2008384` shows it can be fixed from the source, indirectly.
Batch 175 recorded that gcc pools a small literal stored through a halfword
pointer; here the ROM does not:

| spelling | result |
|---|---|
| `*(short *)a = 0xa;` | `ldr r3, =0xa`, 135 lines against 133, 66 differing |
| `*(unsigned short *)a = 0xa;` | identical |
| `n = 0xa; *(short *)a = n;` | **`mov r3, #0xa`, 133 lines, 10 differing** |

The two-line gain is not the `mov`/`ldr` swap. Removing the pool ENTRY removed a
whole pool, and with it the skip jump. **A pooled constant costs more than its
own instruction.**

## OTHER LEVERS CONFIRMED OR SHARPENED

**Naming the SCALED BYTE offset flips a register+register load's operands.**
`ldr r0, [r6, r3]` and `ldr r0, [r3, r6]` are different bytes, and that was the
only differing line in an otherwise exact 42 for `Func_80c1fa8`. `base[idx]`,
`*(base + idx)` and `*(int *)((char *)base + idx * 4)` are all byte-identical
and put the scaled index in Rn; `k = idx * 4; *(int *)((char *)base + k)` puts
the pointer there.

**Naming is a floor, not a ceiling.** `Func_801d014`'s residue is that gcc forms
a copy's destination address before its source, five times over. Naming the
loaded byte costs eight lines; naming the source address costs five. Naming can
stop gcc sinking something past a point; it cannot stop it hoisting something
above one.

**An accumulator's initialiser wants to be as early as the source allows** —
twice more, for two reasons. `Func_807a550` needs `count = 0;` above the *call*
so its live range crosses it, with batch 173's misleading tell (the ROM's
`mov r6, #0x0` sits *after* the `bl`). `Func_80c1fa8` has no call at all and
moving `n = 0;` above an `if` went 17 differing to 10 as pure statement order.

**A run of separate constant ANDs on one loaded byte is the bitfield tell.**
`Func_801bd98` does one `ldrb`, four ANDs with `~0xc`/`~0x20`/`~0x10`/`0x3f`, and
one `strb`. Four `&=` statements fold to a single `& 3`; four assignments to
four bitfields do not. The same struct shows gcc using the smallest load
covering each field, which is why one declaration produces a mix of byte and
halfword accesses.

**The addressing forms are a preference, not a guarantee.** `Func_8011f54` needs
a named pointer to get `add / ldrb` instead of a reg+reg load. `TestCollision`
reads the same two tables with the identical spelling and gcc folds it back —
register pressure is higher there. When a spelling that worked goes inert on a
near-identical function, look at what else is competing for registers.

## A CORRECTION

Batch 175's three duplicate-constant parks all had callee-saved registers
committed, which suggested gcc shares a repeated constant only when one is
spare. [`OvlFunc_953_200a904`](src/non_matching/ovl_7d95dc/200a904.c) was
screened to test that — ten calls, `push {lr}` alone — and **gcc adds r5, its
push and its pop** to avoid rebuilding a two-instruction constant three times.
The length comes out identical. A zero-pressure push list is no protection, and
the reject is right to be a hard skip in every ranking.

## TOOLS

- [`tools/lowpressure.py`](tools/lowpressure.py), new. Every other ranking
  scores shape, adjacency or lever-applicability; none scores register pressure,
  which is what the surviving walls are made of. Ranks by push width, rejects
  r8–r12/r14 outside the push/pop, prefers few calls. It also rejects bodies
  that read r4–r7 before writing them and never push — the `asm/rom_f9000` m4a
  engine, which has a private calling convention and can never be C.
- [`tools/family_siblings.py`](tools/family_siblings.py) gained the DUP-CONST
  hard-skip column, which disqualified eight of its top eleven candidates on
  the first run.

## Parks

Nine: `rom_9000/8011f54.c`, `rom_77000/8077cb8.c`, `rom_9000/80120dc.c`,
`rom_9000/8011164.c`, `rom_15000/801d014.c`, `rom_f6000/80f6148.c`,
`rom_c9000/80cd358.c`, `ovl_7d95dc/200a904.c`, `rom_15000/801bd98.c`.

`80f6148.c` is worth singling out: it has NO CALLS AT ALL, so nothing forces
callee-saved registers, and every one of its 71 differing lines is the same
three values allocated to r0/r4/r5 by gcc and r5/r6/r7 by the original build.
No spills, no other residue. It is the cleanest specimen of the REG_ALLOC_ORDER
hypothesis in HANDOFF, and the one to re-screen first if anyone ever rebuilds
gcc with the allocation order starting at r4.
