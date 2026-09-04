# Batch 215

Five elevated, one parked. The batch's centre of gravity is one function that
turned out not to be a spelling problem at all: **a third shape for the
`-fno-gcse` rule**, and the discipline for telling a flag from a spelling
before adding one.

## Function breakdown

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_954_2008840` | `0x02008840` | [ovl_30_c_c_a_c_a.c](src/overlays/rom_7db0c8/ovl_30_c_c_a_c_a.c) |
| 2 | `OvlFunc_955_2008a1c` | `0x02008a1c` | [ovl_30_…_c_c_c.c](src/overlays/rom_7ddb88/ovl_30_c_c_c_a_c_c_c_c_c_c_c_c_c.c) |
| 3 | `OvlFunc_932_20087e8` | `0x020087e8` | [ovl_30_…_c_a_b.c](src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_c_c_c_c_c_c_a_b.c) |
| 4 | `OvlFunc_945_200812c` | `0x0200812c` | [ovl_30_a_c_c_a_a_b.c](src/overlays/rom_7cb2c0/ovl_30_a_c_c_a_a_b.c) |
| 5 | `OvlFunc_971_200808c` | `0x0200808c` | [ovl_30_a_c_c_c_a_a_a_a.c](src/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_a_a_a.c) |

Parked: `OvlFunc_960_20089cc` (115 of 128, and four instructions SHORT — the
shortfall is now identified exactly).

Gated on a clean `make clean && make compare`, every address verified against
the linked ELF with `tools/checkaddr.py`.

## A THIRD `-fno-gcse` SHAPE: A LOAD HOISTED IN FRONT OF A SWITCH

`200812c` is a thirteen-way switch on an actor's step counter. Each arm re-loads
the counter to bump it. At `-O2` the global pass loads it ONCE in the entry
block, above the switch, and every arm after it shifts by one instruction —
**147 of 157 lines differ for that single hoisted load**.

The two `-fno-gcse` rules already in the Makefile are a *shared constant* hoist
and a *sunk load*. This is neither: it is a load HOISTED to a dominator, in
front of a jump table. Same flag, third symptom.

**What makes it a flag and not a spelling.** Six source shapes were measured
against the hoist and none of them moved it: a separate tail per case instead of
a shared `goto`; an inverted guard in the one arm that reaches the tail without
an intervening call; recomputing the counter pointer inside each arm; a
`__asm__ volatile ("" ::: "memory")` immediately before the switch; and the
counter typed both `short` and `unsigned short`. That is the bar. A single
failed spelling is not evidence for a flag — the point of trying six is that a
spelling problem yields to at least one of them.

`-O1` also removes the hoist, and the tree's own note warns that reaching for it
is how mis-scoped wildcards get written. It is the wrong answer here on the
measurements: `-O1` disagrees with the ROM in the entry block and in the
jump-table setup and settles **eight lines worse** than `-fno-gcse` does. Six
further flags (`-fno-rerun-cse-after-loop`, `-fno-strict-aliasing`,
`-fno-strength-reduce`, `-fno-expensive-optimizations`, `-fno-cse-follow-jumps`,
`-fno-cse-skip-blocks`) were each tried on top and changed nothing, which is
what pins the choice to `-fno-gcse` alone.

## A NARROW STORE'S POOLED CONSTANT IS REACHABLE — WITH A PIN

`docs/elevation.md`'s narrow-store table ends with a "where it does not reach"
note: a block where the ROM pools a value we `mov` and `mov`s a value we pool.
`200812c` has exactly that. Its shared tail stores a zero into a byte field and
the ROM emits `ldr r2, =0x0`; case 12 stores a zero into a halfword and the ROM
emits `mov r3, #0`. One function, two zeros, opposite choices.

Twelve spellings were measured on the pooled one: the bare literal; casts
through `char *`, `unsigned char *` and a `volatile unsigned char *`; named
locals of type `int`, `char`, `unsigned char` and `short`, each with and without
a barrier; a local assigned in a dominating block; a local assigned in every
predecessor of the join; and a `static const`. Eleven give `mov`.

The twelfth is exact: **a `short` local PINNED to the ROM's register**.

    register short hz __asm__("r2");
    ...
    hz = 0;
    __asm__ volatile ("" : : "r" (hz));
    p3 = a + 0x62;
    *p3 = hz;

The pin is what does it, not the width — the same `short` unpinned still `mov`s,
and an `int` pinned to the same register also `mov`s. So the table gains a
fourth row rather than losing its exclusion: the exclusion was about the
UNPINNED spellings, and it still holds for those.

## A PIN CAN REPLACE THE CROSSED-SHIFT BARRIER — AND SOMETIMES CANNOT

`20087e8` passes `0x20000` twice to one call. Written as one expression gcc
commons it and copies (`mov r0, r1`); the ROM materialises it twice. Two locals
pinned to `r0` and `r1` rematerialise both — and, unasked, ALSO produced the
ROM's crossed order, `mov r0 / mov r1 / mov r2` against `lsl r2 / lsl r0 /
lsl r1`, with no barrier anywhere. The whole seven-instruction argument fill
went exact in one step.

`200812c` has the same crossed shape in its two `__Actor_TravelTo` fills and
pins are NOT enough there: gcc pairs each `mov` with its own `lsl` and the
barrier is still required. The difference is what sits inside the crossing. In
`20087e8` every crossed value is a call argument being built. In `200812c` a
plain register copy — `mov r0, r5`, the actor pointer — is wedged between the
first shift and the second, and a pin cannot order a value that is merely
*copied* against values that are *built*.

So the order to try is: **pins first, barrier only if the pairs re-fuse.** That
is cheaper than the recorded "barrier, then measure" and it is strictly safer,
since pins have no side effects on the schedule elsewhere.

## KEEPING A POINTER COPY NEEDS BOTH ENDS PINNED

`20087e8` ends with the ROM materialising a data label into `r2`, storing a zero
through `r2`, and only then copying it to `r5` for the loop that follows:

    ldr r2, =.L5238 / ldr r3, =0x0 / strh r3, [r2] / mov r5, r2

A single pointer local is loaded straight into `r5` and the copy disappears —
one instruction short, and everything after it shifts. Two locals, `t` and `q`,
with `t` pinned to `r2` and `q` pinned to `r5`, keep it.

**Both pins are load-bearing.** Leaving either end free lets gcc coalesce the
pair back into one pseudo, and the copy goes again. Read this beside batch 214's
`20089c0` park, which recorded that "a pin says *where* a value lives; it cannot
say that it must be copied rather than used in place". That is still true of ONE
pin. Two pins, naming both ends of the copy, do say it.

## BLOCK LAYOUT FOLLOWS SOURCE ORDER, SO THE SOURCE HAS TO NAME THE BLOCKS

`200808c` is a small link-cable check whose arithmetic came out right almost
immediately and which still missed by 45 of 64. Every one of the 45 was layout.
The ROM's block order is

    entry -> [set arm] -> [BODY] -> [clear arm] -> [join] -> return 0

with the join's `bne` reaching BACKWARD into the body. No arrangement of nested
`if`/`else` produces that, because gcc lays blocks out in the order the source
expands them and the body is textually last. Writing the two arms and the body
as `goto` targets in the ROM's own order took 45 to 27.

The remaining 27 were two entries already in the tree, both worth re-stating
because they cost more than their size suggests:

  * `if (x == y) return 1; return 0;` is the **return-a-boolean** idiom and gcc
    if-converts it into seven branchless instructions (`eor / neg / orr / lsr /
    sub`). Inverting to `if (x != y) goto out0; return 1;` restores
    `cmp / bne / mov #1`.
  * Every zero return must reach ONE label. Separate `return 0` statements make
    gcc hoist a `mov r0, #0` above the first test so one path can fall through
    with the value already set — the same single-exit discipline that overshot a
    function by 86 instructions in an earlier batch, seen from the other side.

With both applied the function closed exactly on the next screen.

## THE PARK: FOUR MISSING INSTRUCTIONS, IDENTIFIED

`20089cc` sits at 115 of 128 and is **four instructions short**, which is the
useful part. The ROM duplicates its distance test's add-and-compare into both
arms of the sign fix-up where gcc emits one copy joined afterwards:

    sub r3,r0,r1 / cmp r3,#0 / blt .L3
      add / mov / lsl / cmp / blt .L4 / b .L5
    .L3:
      sub r3,r1,r0 / add / mov / lsl / cmp / bge .L5

`add / mov / lsl / cmp` appears twice — the exact shortfall — and the branch
asymmetry (`blt .L4 / b .L5` against a bare `bge .L5`) is the signature of a
duplicated rather than a shared block. Two levers landed before that was even
visible and are recorded in the park: `gState` needs the array idiom or the base
and offset fold into one pool word, and the parameter needs a pinned
callee-saved copy because gcc gives it the register the ROM uses for the fetched
actor. Together those moved the first divergence from instruction 1 to 12.

## HOUSEKEEPING

`tools/checkaddr.py` gates on a report file, not on a symbol list — worth
recording because it was invoked with bare symbol names first and threw. Write
the report, then check it.
