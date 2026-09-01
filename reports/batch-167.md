# Batch 167

Seven elevated. Verified after a clean `make clean && make compare`; SHA1
`5c4695205413df7db52b9a184815a07783999971`. Every address checked against the
linked images.

| function | address | image |
| --- | --- | --- |
| Func_80b26cc | 0x080b26cc | goldensun.elf |
| Func_8019a54 | 0x08019a54 | goldensun.elf |
| DeleteActor | 0x0800c0f4 | goldensun.elf |
| Func_8005810 | 0x08005810 | goldensun.elf |
| TextBox | 0x080187ac | goldensun.elf |
| OvlFunc_924_2009bf0 | 0x02009bf0 | overlays/rom_7ac2d8/overlay.elf |
| OvlFunc_926_2008484 | 0x02008484 | overlays/rom_7b2078/overlay.elf |

The batch took five rounds, and **two of them produced nothing**. That is the
most useful thing in it, because the fix was not a new codegen lever.

## The headline: SELECTION is the lever

Rounds three and four picked candidates the way the tooling had always ranked
them — instruction count, call count, branch count — and screened eight
functions. Every one reached exact length. **None matched.** All eight stopped
on register allocation or post-reload scheduling.

The rounds that *did* produce elevations had a different thing in common, and it
was not size. `DeleteActor` matched on the first screen with no iteration
because its own `.s` already held the solved `Actor_SetAnimAndSpeed` with the
identical opening — a null guard, then `switch (*(u8 *)(e + 0x54) & 0xf)`.

So the fifth round ranked on that instead.

**First, the exact-skeleton tools are exhausted, and it is now measured rather
than assumed.** `tools/match_shapes.py` reports 0 leads; `--near 1`, `--near 2`
and `--near 3` all report 0; `tools/solved_twins.py` reports 0 across 0
templates. Every remaining function that is a constants-only variant of an
elevated one has been taken. That is worth stating plainly so the next round
does not re-run them hopefully.

**Second, "no exact twin" is not "no usable template".**
`tools/fuzzy_solved.py` (new) ranks every remaining function by the best difflib
ratio between its skeleton and any solved function's, bucketed by length with a
mnemonic-overlap prefilter so the quadratic part stays cheap. It reports **26
leads at ratio >= 0.80**.

The top three were taken in one round and **all three matched on the first
screen**:

| function | ratio | exemplar |
| --- | --- | --- |
| `TextBox` | 0.986 | `DialogueBox`, same `.s` stem — same body, one call argument differs |
| `OvlFunc_924_2009bf0` | 0.977 | `OvlFunc_924_2009420`, same overlay — same cutscene, different slot and constants |
| `OvlFunc_926_2008484` | 0.971 | `OvlFunc_939_2008764`, a different overlay |

> Size, call count and branch count predict how hard a function is to **read**.
> Similarity to a solved function predicts whether the answer already **exists**.
> A ratio above 0.95 is worth more than any tractability heuristic.

The ratio is a lead and not a proof. `OvlFunc_926_2008484` needed its branch
polarity inverted against the exemplar, and each exemplar's `.c` had to be read
first. That is minutes, against a round.

It also carried its exemplar's recorded oddity across intact: the two identical
counter increments are spelled *differently* on purpose — offset-first in one,
the pointer walk in the other — and copying both spellings was part of why it
matched cold.

## Levers that closed the earlier rounds

**A symbol-relative load needs BOTH halves named.** `Func_80b26cc` reads a
record out of the file-local table `.Lb41ac`:

| source | result |
| --- | --- |
| `*(short *)(Lb41ac + off + 0x30)` | 37 lines, 27 differing |
| `n = off + 0x30; ... (Lb41ac + n)` | 39 lines, 8 |
| `base = Lb41ac;` as well | 39 lines, **2** |

Naming the complete offset stops gcc reassociating to `(Lb41ac + 0x30) + off`
and pooling the folded symbol; naming the base is what puts the symbol in the
addressing *base* position rather than the offset one. The named base is then
reused for the second address the ROM builds, so one local fixes two sites.

**`do { } while (i != N)` stops gcc reversing a counted loop.** `Func_8019a54`
written `for (i = 0; i < 3; i++)` gets `mov r6, #2 / sub r6, #1 / bge` because
the index is dead in the body; the ROM counts up. Indexing the array with `i`
does not prevent the reversal — only the `!=` exit test does.

**The merge lever chains.** `Func_8005810` runs one register through four roles
— loop counter, result, modulo result, final array read. Merging them one at a
time: 35 differing → 21 → 21 → 13 → **match**. And `r = Random() % cnt;
r = v[r];` matches while naming the index in its own local, which is the ROM's
`mov r5, r0` read literally, is 26 — *worse* than not naming it.

**A named base pointer for a stack array costs a callee-saved register.** The
same function, written with an explicit walking pointer, makes gcc keep the
frame address in a callee-saved register and walk with a copy. `sp` is free to
rematerialise, so naming it only creates a pseudo competing for r4–r7 — the
opposite of the heap and global case.

## Two process findings, both from the dry rounds

**A ref built from a line range can be short, and it lies quietly.**
`Func_8006088`'s reference was cut with `sed -n '225,265p'`, which is shorter
than the function, so tryc compared against a truncated reference and reported a
five-instruction excess that does not exist. Two spellings were screened against
that phantom. Use the function markers:

    awk '/^\.thumb_func_start NAME/,/^\.func_end NAME/' file.s

**Two defects can cancel, and then the length agrees for the wrong reason.** On
the corrected reference that function screens 45 lines — the ROM's exact length
— and 12 differing with the *wrong* three-instruction bit extraction, against 44
lines and 21 differing with the ROM's actual two-shift spelling. The wrong
extraction's extra instruction cancels a `mov` gcc never emits. A better count
from a compensating pair of errors is not progress.

## Parks

Six, all at or within a few lines of the ROM's length, all carrying full
measurement tables including flag probes:
`rom_c0/8006088.c` (44 of 45, one register copy),
`rom_8a000/80936a0.c` (16 of 43, register rotation),
`rom_b5000/80bf4c4.c` (25 of 47, copy-then-modify reachable at one site only),
`rom_a1000/80a3354.c` (27 of 56),
`rom_8a000/8095fcc.c` (**3 of 54**, post-reload scheduling),
`rom_b5000/80bac6c.c` (34 of 62, a counter-example to the `goto`-loop lever).

`8095fcc` is the one to look at first: the `gState` offset build alone took it
from 53 differing to 3, and four independent spellings plus `-fno-schedule-insns`
all sit at exactly 3.

## What the next round should do

Run `tools/fuzzy_solved.py` first. Twenty-three leads remain above 0.80, and the
three taken so far cost one screen each.
