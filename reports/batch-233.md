# Batch 233 — four levers that fire backwards, and a sweep that reports "worse" and is wrong

Eight functions and a park closed. Nearly every result in this batch is a
*correction* to something already written down, which is the pattern worth
noticing: the notebook's failures now cost more than its gaps.

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_968_200a2c8` | `0x0200a2c8` | [ovl_30_c_c_a_a_a_a.c](src/overlays/rom_7f2f14/ovl_30_c_c_a_a_a_a.c) |
| 2 | `OvlFunc_932_20088d4` | `0x020088d4` | [ovl_30_…_a_c.c](src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_c_c_c_c_c_c_a_c.c) |
| 3 | `Func_80b8fd4` | `0x080b8fd4` | [rom_b8228_…_a_b.c](src/rom_b5000/rom_b8228_c_a_c_c_a_c_a_b.c) |
| 4 | `OvlFunc_961_2008194` | `0x02008194` | [ovl_30_c_c_c_a.c](src/overlays/rom_7ebdfc/ovl_30_c_c_c_a.c) |
| 5 | `OvlFunc_961_2008120` | `0x02008120` | [ovl_30_c_c_c_a.c](src/overlays/rom_7ebdfc/ovl_30_c_c_c_a.c) |
| 6 | `Func_80200cc` | `0x080200cc` | [rom_1fe2c_c_c_b.c](src/rom_15000/rom_1fe2c_c_c_b.c) |
| 7 | `Func_801ff58` | `0x0801ff58` | [rom_1fe2c_c_a_b.c](src/rom_15000/rom_1fe2c_c_a_b.c) |
| 8 | `StartThunder` | `0x08095160` | [rom_944ec_…_c_b.c](src/rom_8a000/rom_944ec_a_a_a_a_c_c_b.c) |

Row 5 closes `src/non_matching/ovl_7ebdfc/2008120.c` after four prior attempts —
the second park refuted today.

## A one-lever-at-a-time sweep can report WORSE and be wrong

The most useful finding here, because it invalidates a stopping rule rather than
adding a lever.

At an interleave site the callee's **return type** and the **pinned fill** are
one lever, and each half measures *actively worse alone*. From the parked source
at 2 differing:

| | differing |
|---|---|
| pins alone | 3 |
| `void` prototype alone | 3 |
| **both together** | **exact** |

So the sweep does not report "inert" — it reports **worse**, the strongest stop
signal we have, and it is mistaken. That is traceably how batch 94 (prototype,
no pins) and batches 196 and 207 (pins, no prototype) each correctly concluded
their own lever was dead while the pair was the answer.

**Rule: when an interleave residue survives a full pin sweep, re-run the sweep
under the opposite return type before recording a blocker.**

Related tooling gap: `tools/protolever.py` screens with prototypes **deleted**
only, so it structurally cannot reach a cure that is an *addition*. Two named
past casualties are worth one screen each.

## Three more levers that fire backwards

**The recorded `check_dbra_loop` cure is wrong for `Func_801ff58`.** The doc
prescribes a backward `goto`; here that costs **57 differing**, because it also
destroys the giv reduction the ROM depends on. `-fno-strength-reduce` is not a
substitute either — it kills both passes, 60. The discriminator:

- `goto` when you want **no** loop optimisation;
- `-fno-rerun-loop-opt` when you want **pass 1 but not pass 2** — the ROM has
  strength-reduced pointers *and* an un-reversed counter.

That is the batch's one new flag group. It also bounds a recorded cure exactly:
`do { } while (i != N)` is valid only when the ROM's exit test is `bne`; against
this `ble` it stalls at 2, and an unsigned counter at 1.

**A family template's levers are sufficient, not necessary — four times over.**
`Func_80b8fd4`'s family asserts "the zero is named" in two sibling headers; here
the named and bare spellings are **byte-identical**, so the tell says a named
zero is *safe*, not *required*. `StartThunder`'s sibling records "y before buf";
the opposite order is required, 36 of 67 the sibling's way. Add the store-block
pin that cost 14 differing and the twin whose pins were all inert, and the rule
is simply: **re-measure, never transplant**.

**"r0 last" is a return-type question, not an interleave question.**
`StartThunder`'s interleave at all four sites is bought by the callee return
type alone — and for `StartTask` the correct type was already in
`include/task.h`. Recorded practice reaches for named arguments in a dominating
block, a far more expensive answer to the same tell.

## Two compiler facts, read at source rather than inferred

**gcc's thumb `mov`+`lsl` split takes the smallest shift, not the trailing-zero
count.** The `define_split` at `arm.md:3881` scans for the smallest `i` where
`0xff << i` still covers the value. Verified two ways — reading the split in the
build image, and reimplementing the loop against the function's own constants:

| value | gcc emits | trailing-zero guess |
|---|---|---|
| `0x4000` | `mov #0x80 / lsl #7` | `lsl #14` |
| `0x10000` | `mov #0x80 / lsl #9` | `lsl #16` |
| `0x1f40000` | `mov #0xfa / lsl #17` | `lsl #18` |

**Consequence: a `mov`+`lsl` pair whose immediate looks "wrong" is usually not
evidence of a source-level split.** Write the value whole and let gcc choose.

**A `str`/`ldr` pair bracketing a `bl` is caller-save, not a fifth argument.**
Read as an argument it costs 4 bytes of frame — the 5-arg call reserves an
outgoing word *and* gcc allocates a separate save slot. `-fcall-used-r4` is why
a pointer in r4 needs saving at all. The recorded stack-argument entries all
assume the store *is* an argument.

## Naming a carried value, twice more

`Func_80200cc` gives the cleanest number yet: naming its two loop-invariant
constants costs **50 differing and eight extra bytes**, because pinned they stop
being hoistable invariants and are rebuilt every iteration. Left bare, gcc
carries them in r9/r10 for free.

`Func_801ff58` is the same rule one level up, where the carried value is an
**induction variable**: subscripts let gcc's own strength reduction emit the
ROM's pointer bumps, while hand-written `pos++` measures 37 differing.

## A caution about my own targeting

I aimed four agents at the interleave bucket this batch, and three reported the
silhouette matching while the cause did not — one was a duplicated-value CSE
case, one needed nothing at all, one was a loop-reversal problem.

**The bucket scan finds a shape, and shape is not diagnosis.** The hit rate is
real, but I had been describing it as though the classifier identified the
defect, and it does not. Worth stating plainly so the next reader does not
inherit the overclaim.

Two smaller process notes. A screening run caught that its *predecessor's*
headline finding was not new — the doc already stated it and named that very
function — which is the first time the correction came from the screening side
rather than from me. And linker-script line numbers are not stable while work is
in flight: cite them by content, not by number.
