# Batch 317 — the nineteen smallest parked functions

The owner's framing: *"Every one of those functions is there for a reason. They
made it into the game, so they need to be able to be elevated."*

**Twelve landed. One closed as unmatchable on its own evidence. Six improved.**
The 1–20 instruction band went from **19 parked to 6**, and five of the six now
sit at 8 encodings or fewer.

State: **4,819 / 5,710 (84.4%)**, 891 remaining, 799 parked, 0 available.

## Landings

| function | from | mechanism |
|---|---|---|
| `Func_80b06c0` | 2 of 21 | the **aggregate lever**, two words, no flag |
| `OvlFunc_933_2009874` | 2 of 20 | sched2 **LUID** tie-break — `{q0→r0}` alone moved it, `{q1→r1}` alone *exactly inert*, together 0 |
| `Func_8078ad0` | 3 of 17 | real callee arity (forward the 2nd arg) × result initialised at its first declaration |
| `OvlFunc_common1_16cc` | 5 of 21 **+ RELOCDIFF** | `while` for `do-while` (a **label collision**) × the mask written inline |
| `Func_80a3d6c` | 6 of 22 | the counter's zero initialiser moved to its first declaration |
| `OvlFunc_973_20080c0` | 7 of 20 **+ RELOCDIFF** | cse1 commons the frame address; `static inline` **parameter** lever at **both** sites |
| `Func_8015e8c` | 7 | **pre-cross-jump duplication** (1 shim) |
| `Func_80a65e4` | 10 of 21 | in-place `g += k` × a volatile store that un-folds the address add (1 shim) |
| `OvlFunc_945_200b7b4` | 10 of 17 | **three** causes crossed, two of which *cancelled in the count* |
| `OvlFunc_936_20095b4` | 14 of 18 | per-file `CSE_CFLAGS` — **park body unchanged** |
| `Func_80f7e34` | 20 of 21 | gcse's **PRE** sank the load, not cse — `-fno-gcse` **crossed with** distinct reload locals |
| `Func_801d94c` | "5 of 17" | variable offset is a legal Thumb `(plus reg reg)` load mode |

Three carry one documented shim each and have `fakematch.txt` rows
(`Func_8015e8c`, `Func_80a65e4`, `OvlFunc_933_2009874`). Pass 3 is depinning; the
pin-free `Func_8015e8c` body at 2 of 23, residue fully diagnosed, is preserved at
`docs/repro-b317/8015e8c_pinfree.c`.

## Closed

`free` (FreeScratch, 6 instructions) — enrolled in `unmatchable.txt` as
`thumb-regalloc`. Reached **exactly**: 6 of 6 instructions in the ROM's order and
operand shape, size and relocations exact, differing only in register assignment,
which reduces to **one condition — r3 unavailable**. `-ffixed-r3` matches it
exactly and is refuted as a build flag because the shared pool word proves `gfree`
is in the *same object* and `gfree` uses r3 three times. Corpus: **0 of 4,807
generated thumb functions use r4 without r3** (654 use r4).

**And it had inherited a verdict it did not earn.** No park ever named `free` as
its subject — it was counted parked because the word "free" appears in another
park's prose, and it carried `gfree`'s unmatchability by sharing a file. The
batch-265 shared-pool argument does not apply to it: `gfree`'s `ldr` reaches
*forward* 32 bytes across `free`'s whole body to that word, while `free`'s own
per-function pool word lands exactly where the ROM's is. **An impossibility
argument does not transfer to a file-mate.**

## Still parked, all six with real figures now

| park | figure | what remains |
|---|---|---|
| `OvlFunc_971_2008128` | **2 of 9** | sched2 dependent-count tie |
| `OvlFunc_881_20082cc` | **3 of 16** | one pooled-zero symbol (withheld) + pool word order |
| `OvlFunc_881_20082f0` | **5 of 17** (2 with a pin) | same dependent-count wall |
| `Func_80ab1f4` | **4 of 19** | one sched2 rung; the epilogue blocker is closed |
| `Func_801c154` | **8 of 17** | narrow-mode pool load; declined with a corpus result |
| `Func_80270ac` | **18 of 20**, body exact | a whole-file nested-function job |

## The document this batch refutes

`tiny_reg_order.c` argued these were *harder* than the 30-instruction ones — "the
levers that carry a 30-line function have nothing to bite on. **Each was tried on
each of these and none moved a single one**" — and advised against clearing the
small band. Its *observations* reproduce; its *verdict* was a measurement
artefact, for two reasons. It predates every pass-two lever. And it **proves the
cross-the-lists law on its own targets and then does not apply it**: on
`GetFlagByte` it records that the result "needs a SPECIFIC pairing… neither half
is sufficient alone", while every other entry is a one-at-a-time list.

> "Each was tried on each of these and none moved a single one" is a statement
> about how they were tested, not about a floor.

The arithmetic is the other half: at nine instructions a two-encoding residue is
22% of the function, so the figures look dreadful while one or two decisions
remain. **Rank the small band by the number of named causes, never by the figure.**

## Not one of the nineteen had a recipe naming it

Zero of 19. Every checkable inherited figure was wrong: 20 of 21 against a
claimed 15; 14 of 18 against 7; 2 of 20 against "19 against 19"; 8 of 17 against
"13 lines against 15". Several also carried RELOCDIFF, so they were never
distances. Two were prose figures with no body to measure at all.

**Seventeen park diagnoses refuted, one survived** — and the survivor had its
mechanism exactly right while missing that the tree already carried its cure four
times, including for the same overlay family and the same flag id `0x200`. Its
body is installed **unchanged** under a per-file flag rule. Two agents again
declined to close where they lacked a reachability proof either way.

## A park with no body is a lost candidate

Found **twice in one batch**, both as the *second* park for a function whose
first park had the code. `Func_80f7df0`'s second park claimed **18 of 30** —
better than the park that has a body — in 34 lines of pure comment compiling to a
**zero-byte TU**, which `objcmp` will still print a number against.
`Func_8078ad0`'s second park was the same shape, its candidate "at
scratch/L78ad0.c". Both retired; the recoverable findings preserved verbatim in
the park that has the body.

> Two parks for one function is not redundancy — it is a race in which the file
> nobody compiles wins the headline.

## New method

- **The aggregate lever is size-gated at two words** (`int sv[1]` and a
  one-member struct are *exactly inert*), it is allocated as a **consecutive
  hard-register pair**, so it is a **positional** lever — you pick which two
  registers by picking which two values share it — and it **defeats dead-store
  elimination for its unused half**, which can be held alive *for free* when its
  value is an identity for the operator reading it. That is a new, zero-cost
  route into the "dead callee-saved register" class both `tiny_reg_order.c` and
  `HANDOFF.md` call unreachable.
- **A pre-cross-jump duplication is an allocator lever**: duplicating a store
  into both arms raises `REG_N_REFS` before flow1 measures it, and cross-jumping
  merges the duplicate out after reload at no cost. Batch 316's pin-enables-a-
  cross-jump finding, read in reverse.
- **The declaration-initialiser lever acts on L, not R** — and both parks that
  screened "declaration order: inert" had tested it **on a different variable
  from the one that moves**.
- **New bounds**: a register pin cannot defeat cse1 on an array-decay address
  (`expand_expr` forces the address into a fresh pseudo first, erasing the pin);
  `const` is *bit-identically inert* on the dependence table despite looking like
  the exact cure; and alias-set-0 edges on a byte access are **unremovable**,
  because every one-byte C type is a character type so `DIFFERENT_ALIAS_SETS_P`
  can never fire.
- **local-alloc's walk is `{3,2,1,0,4,5,…}`, shortest-lived-first** (probed), which
  makes the register-permutation residue class calculable rather than guessed.
- **A park body can be a wrong program via a `.L` label collision** — gcc names
  its own loop label `.L6`, gas resolves `.L` references locally, and `--func`
  will not show you that the pool word points at the loop top.

## Tooling and discipline

**The fifth measurement device, and the first caught by its own author.** An
agent reached 2 of 16 with a placeholder symbol whose only purpose was to put a
relocation where the ROM has one — and labelled it, unprompted, as a device that
must not ship. Removed and re-measured: **3 of 16**, the honest improvement from
12 *with size wrong*. The 2 is kept, stated as what it is: a figure about the
remaining blocker, not a distance. **A device is permitted as an instrument and
forbidden as a result.**

`parkcheck.py` gained a **fourth** claim phrasing, deliberately **last in the
chain**. Nineteen parks were reporting `NO CLAIM` on `NOT MATCHING, 4 of 19` and
`DOES NOT LAND -- 8 of 17`. Measured before adding: 19 gain verification, 162
agree with the existing match, and **11 disagree** — so promoting it would have
silently rewritten eleven parks' verified figures. **"It matches more" is not
"it is right more".**

Two traps paid for: **`tryc.py` is blind to pool order** (it cost a 60-variant
round), and **a corpus scan grepping `.thumb_func_start` in generated `.s`
measures ~137 functions instead of 4,807** — one brief's first result, "0 of
4,469", was vacuous, and it kept the broken scan beside the fixed one.

Five splits, each gated byte-neutral before any `.c` was written. Two per-file
Makefile rules, both with existing precedent groups. Five recipes repointed, all
invalidated by this batch's own splits.
