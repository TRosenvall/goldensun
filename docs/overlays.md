# The overlay corpus

96 overlays, 3475 functions, 282,223 Thumb instructions. That is roughly 1.5x
everything annotated in the ROM proper, so before starting we measured what
kind of work it actually is.

The short answer: **it is bulk, not depth.** The overlays are per-map scripted
content written against a runtime we have already read and named. There is very
little new machinery in them.


## Every overlay is the same kind of thing

Each overlay's `imports.s` is dominated by `rom_8a000` -- the cutscene and
dialogue layer -- with `rom_9000` (entities) and `rom_c0` (math, DMA, OAM)
behind it. That holds for all 96, from the 26-function ones to the 116-function
ones. There is no second category hiding in here: no overlay is a battle
engine, a menu system, or a sound driver. They are maps and the scenes that
play on them.

The six-slot export contract (documented in `overlays/rom_779188/ovl_30.s`)
holds everywhere, which is why so many overlays open with a run of two-
instruction `ldr r0, =table; bx lr` stubs.


## The calls land almost entirely in known territory

Of 55,776 call sites in overlay code:

| target | calls | share |
|---|---:|---:|
| ROM function we have annotated | 50,752 | 91.0% |
| another function in the same overlay | 4,932 | 8.8% |
| `call_via` veneer | 56 | 0.1% |
| long branch within the overlay | 36 | 0.1% |
| **unresolved** | **0** | — |

Not "mostly known" -- *all* of it. Every ROM function the overlays call has
already been read and annotated.

The vocabulary is also tiny and very top-heavy. Only **336 distinct ROM
functions** are called from the entire overlay corpus, and they concentrate
hard:

| coverage of all call sites | callees needed |
|---|---:|
| 50% | 11 |
| 75% | 24 |
| 90% | 49 |
| 95% | 72 |
| 99% | 159 |

Eleven functions account for half of every call in 282k instructions of overlay
code. `Func_9163c` (DialogueWait) alone is 6,076 calls, 11% of the total.

The practical consequence: annotating an overlay is mostly *recognition*. The
same dozen routines recur on nearly every page, and their meaning is already
written down one directory over.


## Duplication is real but smaller than it looks

Overlays are built from shared templates, so the same routine appears many
times under different addresses. Normalising away address-derived names
(`.L1658`, `OvlFunc_3cc`):

| | unique | share |
|---|---:|---:|
| function bodies | 2,626 of 3,475 | 75.6% |
| instructions | 253,084 of 282,223 | 89.7% |

Blanking operands as well (mnemonic skeleton only -- the upper bound on "same
routine, different constants") gets to 58.8% of bodies but still 85.8% of
instructions.

So duplication removes only about 10-14% of the reading. It is concentrated in
tiny stubs: the single most-replicated body is a 2-instruction slot stub with
236 copies.

The one exception worth exploiting is a **family of ~18 overlays** (headed by
`rom_780898`) that share a large identical core -- bodies of 172, 193, 302 and
139 instructions each appear 17-18 times. Annotating one member covers roughly
1,100 instructions in each of the others.

Function sizes across the corpus:

| size | count |
|---|---:|
| 1-2 (slot stub) | 349 |
| 3-9 | 390 |
| 10-49 | 1,566 |
| 50-199 | 882 |
| 200+ | 288 |

Half the corpus is under 50 instructions.


## What this means for sequencing

1. **The shared family first.** One pass over `rom_780898` carries into ~17
   siblings.
2. **The top-50 vocabulary is already done.** No prerequisite reading is
   needed; the annotations in `rom_8a000` and `rom_9000` are the glossary.
3. **Expect breadth, not difficulty.** The risk here is fatigue and drift in
   naming, not getting stuck. Consistency of vocabulary matters more than
   depth per function.

Two overlays are already annotated: `overlays/common/` (65 functions) and
`overlays/rom_779188/` (10 functions, plus the slot-contract header).


## Reproducing these numbers

The survey scripts are not checked in -- they are throwaway analysis over the
`.s` files. Each one walks `overlays/*/ovl_*.s` for `.thumb_func_start` /
`.func_end` pairs and cross-references `rom_*/src/*.s`.

Two traps worth remembering if you rebuild them:

- Annotations sit in a comment block **above** `.thumb_func_start`, not inside
  the body. Scanning for `@` within the body reports ~22% annotated instead of
  100%.
- Function starts come in three spellings: `.thumb_func_start`,
  `.arm_func_start` (53 functions), and `.thumb_func_start_noalign`. Missing
  the last two makes the sin/cos pair `Func_2322` / `Func_231c` and the ARM
  helpers look like unresolved calls.
