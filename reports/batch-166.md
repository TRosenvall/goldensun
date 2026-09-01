# Batch 166

Five elevated. Verified after a clean `make clean && make compare`; SHA1
`5c4695205413df7db52b9a184815a07783999971`. Every address checked against the
linked images.

| function | address | image |
| --- | --- | --- |
| Func_80b27b0 | 0x080b27b0 | goldensun.elf |
| Func_80b19cc | 0x080b19cc | goldensun.elf |
| Func_80a8034 | 0x080a8034 | goldensun.elf |
| OvlFunc_971_2009294 | 0x02009294 | overlays/rom_7fb4a8/overlay.elf |
| OvlFunc_886_20090c0 | 0x020090c0 | overlays/rom_786f0c/overlay.elf |

Three of the five continued the `multi` population that batch 164 opened.
The last two came from `tools/filtered.py`, which was down to two candidates at
the documented thresholds — that pool is now empty, and the round had to relax
the filter to find anything. Two functions were parked, both at or inside one
line of the ROM's length.

## An address local's BIRTH STATEMENT decides whether it gets its own register

`OvlFunc_886_20090c0` sat at 4 differing of 60 with everything aligned except
one scratch register. The ROM builds `0xc0 << 13` in **r2** and then reuses r2
for a byte pointer; ours built the constant in r1, because r2 was already
claimed by the pointer.

Three placements of `q = a + 0x64;`, changing nothing else:

| where `q` is assigned | differing |
| --- | --- |
| before the `+0xc` update | 4 |
| **between the two int stores** | **1** — and that one is the linker alias |
| after both int stores | 10 |

The middle row is exactly where the ROM has `mov r2, r5`.

This is finer than the recorded rule that pointer birth ORDER decides which
register each pointer gets — that one is about several pointers relative to each
other. Here there is one pointer, and what matters is the **statement gap** it
is born in. Born too early it holds a register through the constant's live range
and pushes the constant elsewhere; born too late gcc rebuilds the address after
the stores instead of before them.

**When a diff is "one scratch register is wrong and nothing else is", move the
address local's assignment one statement at a time before concluding it is
allocation.**

## A third signature for the aliasing class: an address computation that FLOATS

The two forms on record are a reload that vanished and a load that sank. This
round adds one where nothing is missing at all.

With strict aliasing on, gcc knows the later `*(short *)q` stores cannot touch
the `int` fields at +0xc and +0x3c, so it is free to hoist `add r2, #0x64` above
them; the ROM keeps it below. `-fno-strict-aliasing` reinstates the dependence
and the `add` lands where the ROM has it — 7 differing to 4, no source change.
`-fno-schedule-insns`, `-fno-schedule-insns2`, `-fno-rerun-cse-after-loop` and
`-fno-gcse` all leave it alone or make it worse.

So the tell is also **an address computation scheduled across stores of a
different width**. `tools/aliastell.py` cannot find this form for the same reason
it cannot find the sunk load: there is no re-read to key on, and nothing in the
listing looks anomalous.

## The interleave is narrower than "this ROM likes r0 early"

`OvlFunc_927_200a1b0` is a 108-instruction cutscene script that comes out at
exactly 108 with six differing, all one shape: `mov r0, #0x12` has to land
before the `lsl` finishing r1's split build.

The useful observation is what does **not** differ. The same ROM issues four
calls to a four-argument callee with the same split-constant arguments, and
every one puts `mov r0, #0x12` last, after all the shifts — as ours does. Only
the two- and three-argument calls interleave.

That rules out "compiled with a different scheduler" and points at argument
loading order, which is consistent with six flags leaving it at 6 and only
`-fno-schedule-insns2` moving it (to 52, worse). It also confirms the
guarded/straight-line split from the other side: this function has no
conditional branch, so the naming lever has nothing to rematerialise across and
is exactly inert.

**`tools/filtered.py` now reports a branch count** and marks candidates with no
guard. It offered this function; the filter counted calls and instructions and
said nothing about guards. Not made a reject — plenty of straight-line functions
have been elevated — but it is now readable before the work starts.

## Levers that landed as documented, with no new reading

Worth listing because together they were most of the round's edits, and each was
found by looking the residue up rather than by experiment:

* `pop {r1} / bx r1` in a void-looking function → `int` with no return
  statement. Two of `OvlFunc_971_2009294`'s four first-screen differences.
* The slot initialiser assigned **after** the modulo call, not before it — the
  build-a-constant-after-the-call rule, worth 1 line and the register.
* An **unsigned** switch value, or the decision tree compares with `bgt` where
  the ROM has `bhi`.
* **Int intermediates for halfword literals** — the halfword exception running
  in the ROM-has-`mov`-and-we-pool direction, on `0x19` and `0x80`.
* An **int mask** against width narrowing: `q[9] & ~0xc` stored back into a byte
  lets gcc truncate to `mov r3, #0xf3`, one instruction shorter than the ROM's
  `mov r3, #0xd / neg r3, r3`.
* The **merge lever**, again — `OvlFunc_921_20095b4` spends four callee-saved
  registers with a separate `short *` and `int *` and three with one variable
  playing both roles; 106 lines to 101.

## Parks

`src/non_matching/ovl_7a7298/20095b4.c` — `OvlFunc_921_20095b4` at 101 lines of
102, 65 differing, on the register-role swap. Everything else is exact. It is a
near-twin of the solved `Func_80993b0`, and the reason that one matches with
separate locals is that its first block sits under a guard, so the store to `f8`
does not dominate the re-read; without the guard this one needs `ALIAS_CFLAGS`
and has a tighter register budget. A good specimen for the swap class, because
nothing else is outstanding.

`src/non_matching/ovl_7b4558/200a1b0.c` — `OvlFunc_927_200a1b0` at the ROM's
exact 108 with six differing, all straight-line interleave. Not a candidate.

## Where the candidate pools stand

`tools/solved_twins.py` returns **0** across 0 templates — exhausted.
`tools/filtered.py` at its documented thresholds returned **2**, both of which
this round consumed, and now returns 0. Relaxing to 25–110 instructions, ≥4
calls and at least one conditional branch returned **3**, two of which are the
elevations above. The next round should expect to widen further or to work the
park corpus, which `tools/park_retry.py` still ranks 112 entries deep.
