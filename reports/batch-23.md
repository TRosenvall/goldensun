# Batch 23 — 6 functions, and two blocker classes characterised properly

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–22 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean `make clean && make -j8 && make compare`, unassisted. Every address
below read back from the linked ELFs.

## Functions

| function | address | file | note |
|---|---|---|---|
| `Func_80cd508` | `0x080cd508` | `src/rom_c9000/rom_cd508_a_b.c` | indirect call, `void` pointer type |
| `Func_80ccbdc` | `0x080ccbdc` | `src/rom_c9000/rom_cc5d8_a_b.c` | indirect call, `int` pointer type |
| `Func_80c0098` | `0x080c0098` | `src/rom_b5000/rom_bffb8_a_a_a_b.c` | loop-invariant literal |
| `Actor_IsNotMoving` | `0x0800ca98` | `src/rom_9000/rom_ca6c_a_c_b.c` | join-operand naming |
| `OvlFunc_922_20085b8` | `0x020085b8` | `src/overlays/rom_7a8c8c/ovl_30_c_a_c_c_a_c_c_c_a.c` | whole-file |
| `OvlFunc_940_200816c` | `0x0200816c` | `src/overlays/rom_7c5974/ovl_30_c_c_a_c_c_c_c_a.c` | whole-file |

## Four levers, three of them new

**1. The pointer's return type is a per-call-site fact.** `Func_80cd508` and
`Func_80ccbdc` call the **same** callee, `Func_80008d4`, through function
pointers with **opposite** return types — and both byte-match. One wants r0
filled first and needs `void (*)(...)`; the other wants it last and needs
`int (*)(...)`. Read the order off the ROM; neither is a default. An earlier
note in this tree wrote the rule as "`int` gives the ROM's order", which was an
overgeneralisation from three functions that happened to agree.

**2. A loop-invariant value must be a LITERAL, not a named local** — the reverse
of the usual named-intermediate lever:

    v += 0x4040404;   ->  ldr r1, =0x4040404 hoisted above the loop  (ROM)
    v += step;        ->  ldr r1, =0x4040404 left inside the loop

Same instruction count either way; only placement moves. **And it runs both
ways**: in `Func_80064b8` and `Func_8012350` the ROM keeps the constant *inside*
the loop and a named local is what's needed. The direction has to be read off
the ROM each time.

**3. When a join block compares against a register whose contents differ per
predecessor, the source named the operand rather than repeating the constant.**
`Actor_IsNotMoving` joins two arms at one `cmp r2, r3` where r3 is
`a->targetX` on one path (already proved equal to `ACTOR_NO_TARGET`) and the
constant on the other. Spelling the join as `if (t != ACTOR_NO_TARGET)`
materialises the constant once *after* the join — one instruction short, on the
wrong side of the label. Carrying both sides in their own variables reproduces
it.

**4. Un-rotated loops need `goto`.** gcc rotates a counted loop whenever the
initial value provably satisfies the condition. Where the ROM tests at the top
and jumps into the test, no structured spelling reproduces it; writing the
control flow out with `goto` does. This is a prerequisite for several parked
functions rather than a fix on its own.

## Two blocker classes characterised

### The pre-header load merge — `src/non_matching/preheader_load_merge.c`

A spin-wait reads a value before the loop and again at the bottom of the body;
both reads reach the same test. gcc cross-jumps — the two predecessors end in
the same instruction, so it sinks that instruction into the shared successor.
**Three members, each short by exactly one instruction, always this one:**
`Func_80064b8` (24/25), `Func_8012350` (26/27), `OvlFunc_956_20081c8` (25/26,
the overlay member, so it is not bank-specific).

Two negatives worth more than the parks:

* **`volatile` does nothing.** Cross-jumping *relocates* the read rather than
  removing one, so exactly one read still happens per pass and the semantics are
  satisfied either way. This was the most promising-looking idea.
* **There is no compiler flag.** `-fno-crossjumping` does not exist in gcc-2.96
  (it arrives in 3.4); `-fno-thread-jumps` is accepted and gives byte-identical
  output; -O1 rotates the loop and is further out. Tested, not assumed.

### Address-register reuse

gcc loads a pointer into the same register that held the symbol address and
reuses it; the ROM allocates a fresh register. Four functions now sit on this,
either as a swapped pair (`NewActor`, 20 against 20, the entire diff) or as a
missing `mov` where the ROM stages through a scratch before moving to a
callee-saved register (`rom_5868.c`, `rom_91c44.c`, `rom_c00d8.c`).

It does **not** appear when the pointer must be callee-saved because it is live
across a call — `Func_8012350` has the same loop shape and both sides pick r5.
So it is specific to a pointer that stays in the caller-clobbered set.

## constant-CSE: the clearest specimen yet

`OvlFunc_899_200852c` is parked at **38 instructions against 36 — longer, not
shorter.** A flag id is read and then written on the same path; gcc hoists it
into a callee-saved register to save one pool load and pays a `push {r5}` and a
`pop {r5}` for it.

That is the sharpest available answer to why this class is a *defect* rather
than a preference: on a Thumb target with four low scratch registers, caching a
pooled constant across a call is a losing trade, and Camelot's compiler did not
make it.

The distinction that decides whether one of these is attemptable at all:
`OvlFunc_922_20085b8` in this batch repeats two flag ids too, but on **mutually
exclusive arms**, so gcc never has both live and no CSE arises. Same-path
repetition is the blocked case.

## On selection

Two rounds in this batch produced no elevations, from picking candidates blind
and landing on shapes already characterised as blocked. Changing the scan to
**loop-free, call-dense functions in small files, excluding `_call_via_rN`**
gives 232 candidates, and the two picked from it both matched. Recorded because
the failure was in candidate selection, not in technique.

## Counts

236 functions elevated in total. 3,063 hand-written functions remain in `asm/`
of 5,714. 93 parked.
