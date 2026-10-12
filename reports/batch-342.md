# Batch 342

Five briefs, 20 targets, **6 landings**. **641 → 633 park files**; census ROM
functions landed 4,978 → **4,984**.

Landings: `Func_801e3c8` and `Func_80b2720` (A), `Func_807977c` (B),
`Func_8006088` and `DecodeMetatileset` (C), `OvlFunc_924_2009db4` (D). Brief E
held all four of the depin targets.

## Three landings came from flag groups their parks had ruled out

- **`OvlFunc_924_2009db4`** is byte-identical under `-fno-rerun-loop-opt` **with
  the park's body unchanged** — the flag *is* the landing. Its park recorded "NO
  FLAG GROUP HELPS", a bound stated from six of the eight groups. Autosweep's
  flag axis found it in one command, and the park's *diagnosis* of the residue
  was correct; only its bound was wrong.
- **`Func_80b2720`** — `-fno-gcse` takes the parked body 23 → 11 alone.
- **`DecodeMetatileset`** is the instructive one: a **four-way crossing** where
  each half is worthless. `-fno-gcse` is *bit-identically inert* on the parked
  body; the source change alone measures **42 of 73**, worse than the 24 the park
  held; **together they are 0.**

> That crossing is why the flag sat recorded as inert since batch 335:
> `sweep_variants.py` cannot cross a flag group against a source change, so
> nothing in the toolchain could have found it. **A flag measured inert on the
> parked body is not a closed axis.**

## Five routes into the allocator, and two do not touch priority

Two new ones this batch, bringing the total to five — and they are not
interchangeable, which matters because parks keep testing one, finding it inert,
and generalising it into a bound the others defeat.

| # | route | what it changes |
|---|---|---|
| 1 | delete/split a reused local | makes the value **block-local** |
| 2 | move a definition before first use | shortens `live_length` |
| 3 | **add** a definition | the value **leaves the race** |
| 4 | move a statement across a **block boundary** | a range stops **spanning** it |
| 5 | **lengthen** a range to overlap a hard register | **prunes its own preference** |

**Route 4** landed `Func_807977c` on a crossed triple its park had half of — the
park's own "NOT YET TRIED" lever is **inert alone at 22** and a required
prerequisite, and the deciding move was putting `p++` above the `if` so a range
stops spanning the branch, after which `global.c:617-619` breaks the tie on
allocno *number* (32 < 36).

**Route 5** landed `Func_80b2720` without touching priority at all — the priority
order is **identical** in both `.18.greg` dumps. `return n` gives the allocno a
preference for r0 (`global.c:1630-1642`), `prune_preferences` deletes a preference
for any register it hard-conflicts with (`:897-909`), and `find_reg`'s pass 0
excludes whatever a lower-priority conflicting allocno prefers (`:1016`) — so
**lengthening** the range until it overlaps the incoming argument frees r0. All
three priority levers were inert at 11.

> **So "no priority lever reaches this" is not a bound.**

## "Two is not N"

All three of brief C's movers were *one shared name → N distinct names*, and in
every case the park had tested the **two**-name version and recorded it inert or
worse. That matches batch 340's `Func_80a33d4`, where one local reassigned per
loop measured 42 and **three distinct locals were 0** — and whose park had
recorded both rows as the same figure.

## Two citation corrections, both verified in the source

- **`REG_N_REFS` is loop-depth weighted at `flow.c:5114-5116`** —
  `+= (optimize_size ? 1 : pbi->bb->loop_depth + 1)`, so **an in-loop reference
  counts two**. This tree had cited `flow.c:4948`, which carries the same
  expression but is **only the auto-increment path**; read that way the weighting
  looks specific to increments. Brief A confirmed the corrected model with probes
  separating a depth-0 edit (does *not* flip the allocation) from a depth-1 one
  (**does**).
- **"A tie is a win" needs its direction.** `global.c:617-619` breaks on
  *ascending* allocno number, so a tie wins **only for the lower-numbered
  allocno**. Stated without that it is a coin flip, which is how one park hedged.

## A device class that was invisible

Brief E found a park reported as **0 pins and 0 devices** carrying
`extern int _FILE_18;` read as `(int)&_FILE_18`, with `_FILE_18` defined
**nowhere** — a stand-in for a pooled constant **worth 160 encodings**.
`shimcount` counted register-asm, `.equ` shims and empty `asm volatile`, not that.
It now does, and **26 parks carry one**; every name is a withheld symbol-table
candidate (`_MSG_*`, `_FILE_*`, `_SIZE_*`, `_CONST_50`), exactly the class the
owner's symbol ruling governs.

Getting the detector right took two corrections of its own. Checking `.sym`
definitions alone flagged **26 landed files**; adding asm labels and C definitions
left **5**, and those are defined through assembler *macros*
(`_FIXUP_RAM_CODE_SIZE` comes from `.func_end_emit_size`). Chasing definition
syntaxes was the wrong move — **a landed file's externs resolve by construction
because the build links**, so the class applies to parks only and the link is the
authority. Tree-wide now: 0 landed, 26 parks.

## Brief E's depin results

Nothing could have landed — two targets have **pin-independent floors of 2**, one
is blocked on a pooled-constant identity, one on a local-alloc conflict. What it
produced instead:

- **Three dead pins, two of them free**: a single r6 pin byte-for-byte inert
  (1 → 0), a second `PIN1` block inert (20 → 19), an `r11` pin inert (10 → 9).
  All three parks called them load-bearing.
- **The pin-first method reproduced, with its control.** One park records
  `-fno-rerun-cse-after-loop` as "2, inert" — measured *with the pin in*, where
  the flag has nothing left to do. On the pin-free body it is worth **142
  encodings** (187 → 45). **But a second target is the control and says the
  inversion is not general** — its pin-free body unlocks no flag at all. So the
  rule is "an inherited control measured with the pin in is **untrusted**", not
  "wrong".
- A park's "one escape left" closed by measurement: a hard-r0 `args[0].value`
  *is* constructible in C and measures **8** against a base of 2.

## A duplicate park I created

`CutsceneStart` had two park files. Batch 338's scratchfill filled
`80916b0.c` — then a prose park with no body — from a `scratch/` candidate,
without noticing `916b0.c` was already the measured park for the same function.
Re-measured, the older body is **152 bytes against the reference's 160** while the
recovered one is **156**, so the recovered body is closer by four and its
"60 of 69 encodings" claim no longer reaches the size check. Merged into the
conventional name, the other retired with its text preserved.

`piece.py` now reports two parks sharing one subject. Getting *that* right needed
the right discriminator: a **count** cannot distinguish a duplicate from a shared
helper, because `call_via_r3` appears in three parks and `QueuePush1..4` in two.
The test is whether the name is the **subject** of more than one park. It also
separates **address collisions across overlays** — overlays share address space,
so `overlays/2009818.c` and `ovl_7b4558/2009818.c` may be different functions
declaring one name, which is a naming question rather than a stale copy.

## Other mechanisms worth promoting

- **A scratch residue that *alternates* is one cursor phase, not N choices.**
  `allocate_reload_reg` has no per-site preference — it takes the next register
  off a round-robin cursor (`reload1.c:5003`, `:4937`, reset per function at
  `:821`). With `{r2,r3}` the period is **two**, so one extra reload anywhere
  earlier inverts every later site at once. Two parks had been decomposed into
  five and six "independent" choices, and that framing is what made them look
  unreachable.
- **A three-operand shift/add in the ROM where ours is two-address is a
  coalescing tell** — the refusal gate is `local-alloc.c:1633-1636` (the dying
  input must be block-local and die once), so the lever is **block membership**,
  not naming.
- **`nonzero_bits` is a union over *all* sets** (`combine.c:907`, full-mask
  fallback at `:916`), so the "gcc knows the loaded value's range" class is **not**
  closed — one unbounded set defeated a blocker its park had written off.
- **An anchor can live in the sibling arm of an `if`/`else`** — one landing's
  missing instruction was in arm 2 and the fix was an anchor at the end of arm 1.
  But **an anchor inside a switch arm's `if` body cost 105 encodings**, so batch
  339's "sweep anchors downstream" needs the qualifier *within the same basic
  block*.
- **Count distinct registers per side before describing a difference.** Two of
  brief D's parks share a shape — the ROM spends one more register and we reuse a
  dead one — which presents as a rename and sends you after the wrong thing.

## Open

- `autosweep` cannot be aimed at a candidate, only an installed park, so
  "re-cross the flags after a source change" still has to be hand-rolled. A
  `--base <file>` flag would fold in two briefs' helpers.
- `tryc.py` prints an ~11-line keyhole and showed 3 of 20 differing lines on two
  targets; and `tryc.py --full` is unsafe for localising when label counts differ
  (104 against objcmp's 23). Both briefs wrote index-aligned replacements.
- A park carries the `ldrh`/`ldr` dialect trap **as its stated residue** —
  verified in-container, both assemble to `4900`.
- Eighth and ninth instances of a park quoting the **stream count on the
  instruction line**.
