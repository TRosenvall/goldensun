# Batch 264 -- four functions, 31 stale parks retired, and three findings

Gated on a clean `make clean && make -j8 && make compare`, green at the target
SHA1 `5c4695205413df7db52b9a184815a07783999971`. All four addresses checked
against the linked ELF — and **all four are named symbols**, the case
`tools/checkaddr.py` exists for.

| | |
|---|---|
| elevated | **4** |
| stale parks retired | **31** |
| new parks | 1 (plus additions to 4 existing) |
| whole-file conversions | 1 (a twin pair) |
| splits | 2 (one at a `.rodata` boundary) |
| build-input changes | 0 |
| fakematch debt added | 2 functions |

Four solo rounds, no agents.

## What landed

| | function | address | source |
|---|---|---|---|
| 1 | `ModifyHP` | `0x080783a4` | [rom_77320_c_c_c_a.c](src/rom_77000/rom_77320_c_c_c_a.c) |
| 2 | `ModifyPP` | `0x080783dc` | [rom_77320_c_c_c_a.c](src/rom_77000/rom_77320_c_c_c_a.c) |
| 3 | `Func_80108c4` | `0x080108c4` | [rom_10424_c_b.c](src/rom_9000/rom_10424_c_b.c) |
| 4 | `Func_80a3ce4` | `0x080a3ce4` | [rom_a1814_…_a_c_b.c](src/rom_a1000/rom_a1814_c_a_c_c_c_c_a_c_b.c) |

## 31 stale parks retired

Batch 258 *counted* 24 park files describing already-elevated functions. Counting
them is not finding them. The detector is three lines:

```python
idx = funcindex.index(force=True)
s   = funcindex.park_subject(park)
if idx.get(s, {}).get("kind") == "c":   # already elevated -> stale
```

`_build()` marks a name `kind="c"` when the `.s` defining it has a `.c` sibling —
and an elevated function's `.s` is regenerated from that `.c`, so its symbol
arrives via `.type NAME,function` instead of `.thumb_func_start`. **A park whose
reference `.s` holds zero `.thumb_func_start` is the tell.**

31 files retired, 29 distinct subjects, every one verified present in a linked ELF
first. Park files **501 → 470**; the "≤6 differing" list **56 → 48**.

**Nine of the 31 went stale in the preceding two batches**, when the flag-accessor
family closed. This is a standing check after any batch that closes a family, not
a one-off.

> The first verification of those 29 symbols was a **false green** — I wrote the
> symbol list to `/tmp` on the host while the check ran inside the container, so
> the loop never executed and reported "0 missing". Re-run with the list inside
> the mount: 29 checked against 11,132 ELF symbols, 0 missing.

## Three findings

**A register-role swap can be decided by pass OWNERSHIP, not priority.**
Batch 258 recorded a quantity *demoted* into `global_alloc`, losing an auction.
`ModifyHP` is the mirror: a quantity *promoted* into `local_alloc`, taking the
register before any auction happens.

```
.17.lreg: Register 36 used 3 times across 4 insns IN BLOCK 0; set 1 time
.18.greg: ;; Register 36 in 3.        <- local_alloc took it
```

> When two locals exchange registers, check whether one is **block-local**. If it
> is, priority never runs — local_alloc already spent the register.

That also refines batch 258's "pin either member is too loose". True for a
**priority** swap. For an **ownership** swap either works — both pins measured
exact, because the pin removes that variable from the contest entirely.

**A `cmp #C / blt` that resists every `<` spelling is a switch range test.**
`Func_80a3ce4`'s second test is `cmp r0, #0xc1 / blt` and no comparison reaches
it — `id < 0xc1`, `id <= 0xc0`, `0xc1 > id`, `!(id >= 0xc1)` all canonicalise to
`cmp #0xc0 / ble`. With the structure otherwise instruction-identical, only that
pair differed at 2 of 10. A switch over four consecutive cases is exact, because
`expand_switch` emits the bounds check itself rather than folding a user
comparison. Four cases cost nothing — the same two compares, no table.

**What the source controls is which value is still unfinished.** `Func_80108c4`
had 7 of 14 at exact length, all one defect: two constants in opposite registers
because gcc hoisted a pool load above a `mov`. Moving the shift *one statement
earlier* is exact — and the source order is **not** the ROM's order. The ROM emits
the pool load *between* the `mov` and the `lsl`, and writing it that way is
exactly what produces the 7.

Also recorded: **`goto` into a loop's bottom test reproduces gcc's unrotated
`while`** (16 → 7 on `NewActor`), and **twins cost barely more than one function**
— identical ranker score *and* shape counts *and* file is a twin tell.

## Parks

`NewActor` parked at **2 of 19** — two adjacent instructions, the same
cheap-constant shape as `2008e0c`: `mov r0,#0` costs nothing, so nothing hoists it
and no pin has anything to rank it against. Ten forms measured; every pin
combination inert.

Additions to four existing parks. Two matter:

* **`2008e0c`** — `-fno-schedule-insns2` is **inert**, so the swapped pair is
  emitted before scheduling runs. That rules out the entire adjacent-pair toolkit
  at a stroke, since the alias device, promote-the-producer and add-a-dependent
  all operate on sched2's ready list. Eighteen spellings measured. It also bounds
  batch 260's pin-pair rule: **that lever needs one EXPENSIVE argument.** Two
  cheap arguments are a different shape — the pinned assignments get coalesced
  into the argument stores and the generated `.s` is identical to the unpinned
  form.
* **`Func_80198dc`** — a second defect the prior note did not cover: the two
  stores go through *different* pointer types, so they sit in different alias sets
  and sched2 may order them freely. That is the recorded alias device running
  **backwards** — they already have distinct tags, and that is the problem.

## A process failure worth recording

I wrote fresh park files for `Func_80198dc` and `Func_8019d0c` before noticing git
reported them **modified, not added**. Both already existed with substantial prior
work — a register-rotation table and two rejected-lever lists, one of which I had
just re-derived from scratch. Restored from `HEAD` and re-applied as prepended
additions; nothing was lost.

> Read `git diff --cached --name-status` before every park write, not only before
> a landing. A park file you did not know existed looks exactly like a new one.

## State

| | |
|---|---|
| matched | **4,332** (76.6% of the 5,655 elevatable) |
| remaining, hand-written thumb | 1,323 |
| &nbsp;&nbsp;parked | 438 |
| &nbsp;&nbsp;UNATTEMPTED | 885 |
| park files | 471, resolving to 441 distinct subjects |

Pool went **1,327 → 1,323**, exactly -4.

## Note on throughput

Batches 258–262 were six-agent rounds delivering 6–10 functions each. These four
rounds were solo and delivered 5, 0, 3 and 1. The difference is not the method —
it is that the top of the park list is now mostly **proven walls and lost bodies**,
and the ranker's cheap single-function targets are thinning. `tools/elevation_candidates.py`
remains the better source: three of this batch's four came from it, and the twin
pair came from reading its score column rather than its top row.
