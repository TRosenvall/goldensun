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


## Two kinds of annotation

Overlay annotations come from two sources, and it is worth knowing which you
are reading.

**Hand-read.** Someone read the function and wrote prose about it. These name
the function, explain why it does what it does, and call out anything
surprising. Every module header is hand-read, as is all of `rom_780898`,
`rom_78dee8`, `rom_7a67d8`, `rom_7fc720` and the other overlays listed as
complete below.

**Pattern-derived.** By the time a few dozen overlays had been read, the same
half-dozen shapes accounted for most functions in the rest. A classifier now
recognises those shapes and states the concrete facts: which table a slot
returns, which save bits a selector tests, which message ids a villager
speaks, which shop a counter opens. These annotations are shorter and never
speculate about intent.

The classifier extracts facts by tracking r0-r3 through the body and recording
the arguments at each call site -- a number is a message id because it was in
r0 at a `Func_92b94` call, not because it looked like one. An earlier version
guessed from magnitude instead and was wrong in both directions: save-bit
indices and message ids share a numeric range, so it reported ewram offsets as
save bits and labelled a 241-instruction cutscene a "talk handler". If you
extend it, keep the tracking and resist the shortcut.

It refuses more than it accepts (roughly 2000 functions come back unclassified)
and that is the intended bias: a wrong annotation is far worse than none. Two
guards matter in particular -- a straight-line-only register trace that resets
at every label, since a branch may arrive with different values; and a
vocabulary check on talk handlers, because a short function containing a
message id may still be a small scene doing several other things.

## Progress

**All 3540 overlay functions across all 97 directories are annotated**, and
with them every function in the repository -- 5642 of 5642.

Roughly a quarter were hand-read; the rest carry pattern-derived annotations
or, where no shape matched, a labelled call trace. Coverage is not the same as
understanding: a call trace tells you what a function reaches for, not what it
means. Treat the traces as a starting point for reading, not a substitute.

The last stragglers were worth the individual attention. Among them:
`Crc16Ccitt` in `rom_7795e8`, where the polynomial is hidden as an ADD of
0xFFFFEFDF (that is -0x1021 in two's complement, so the constant to recognise
is the CCITT 0x1021 and not the literal in the source); `FindRegionContaining`
in `rom_7a37f0`, whose region records use a flag at +0x06 to choose which axis
gets a three-unit extension, so one record shape describes both wide and tall
regions; and a family of palette fades that step each 5-bit BGR555 channel
toward 0x1F and count the entries already saturated so the caller can tell when
the fade is done.

- `overlays/common/` -- 65 functions
- `overlays/rom_779188/` -- 10 functions, plus the six-slot contract header
- `overlays/rom_780898/` -- the push-log core in full, plus this map's slots
- 23 sibling overlays -- 153 functions carrying the shared core's names

### The push-log block

`OvlFunc_30 .. OvlFunc_8c0` in `rom_780898` is the pushable-log puzzle: the
long logs and pillars a player shoulders one tile at a time. It is worth
knowing by sight because it recurs, byte-identical, in 23 other overlays.

What makes it map code rather than an entity script is the third job it does.
Besides finding the log and animating the push, it rewrites the map's collision
attributes -- `OvlFunc_244` stamps 0xFF over the log's footprint at the
destination and 0 at the origin, on layers 0 and 2 -- so the log actually
obstructs movement where it comes to rest.

Two details worth remembering:

- A log can only be pushed **broadside**. `OvlFunc_34c` rejects a match when the
  player's coordinate along the log's own axis equals the log's, so facing down
  a log's length finds nothing. Bit 0 of the model index doubles as the axis
  flag, since the six pushable models in the table alternate orientation.
- `OvlFunc_474` stops its outward walk only on a Func_120dc result of 2
  ("no tile / blocked"). Results of 1 and -1, the step-too-high and drop-too-far
  rejections, do not stop it -- so a log will be pushed down a ledge it could
  never be pushed up.

Sibling overlays carry the function names and a one-line summary pointing back
here. Their local table labels differ (label names are address-derived) and are
resolved per overlay in the annotation; the underlying table bytes were checked
identical.


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
