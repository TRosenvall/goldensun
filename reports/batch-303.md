# Batch 303 — one landing, and a seventeen-function lever sitting at 7 encodings

Three agents, nine targets, all unattempted — seven of them recovered by batch 302's census
fix and never offered to anyone before. Gate green at
`5c4695205413df7db52b9a184815a07783999971` at every commit.

**1 landed, 8 newly parked.** Every target reached a measured result; nothing was left
unaccounted.
State: **4,766 of 5,710 elevated (83.5%)** — 104 available, 749 parked, 76 hand-written asm,
14 ARM, 1 unmatchable.

Reconciliation, exact: remaining 945 → 944 (**1 landed**), parked 741 → 749 (**+8**),
available 113 → 104 (**9 left the pool**), and 1 + 8 = 9.

## Landed

| function | result | shims |
|---|---|---|
| `OvlFunc_969_200871c` | **byte-exact at plain -O2** — 376 bytes, 149 encodings, 34 relocations | 0 |

No split, no `.global`, no Makefile row.

## The lever that landed it, and it went three for three

**The gState offset as a named local, built in its own statement.** gcc folds
`SYMBOL_REF + CONST_INT` into one pool word; the ROM pools the bare symbol and materialises
the offset separately, so any large-offset global read comes out short:

    int k = 0x1c2;  *(short *)((char *)gState + k)

- `OvlFunc_969_200871c` — landed byte-exact on it
- `OvlFunc_956_200804c` — 137 → 81
- `OvlFunc_959_200d0e4` — 169 → 213 aligned in one edit, taking size **and** count to exact

On the third the offset must be built in **two** statements (`off = 0xe1; off <<= 1;`), and
what it repairs includes the **jump table's alignment `.short`** — three instructions plus an
alignment directive, exactly the observed gap. This is now the first thing to try on any
large-offset global read.

**A sibling's shim was actively wrong for the same fold.** `200cda0.c`, the first function in
the *same file* as `200d0e4`, solves this fold with a `"+r"` barrier and books 15 pins. Adding
it here measures **worse** — 145 aligned against 213 of 214 — because that function uses the
offset **twice** and cse chains them, where this one uses it once. A sibling's shim is
evidence about the sibling, not about the construct.

## Seventeen functions behind seven encodings

The tree's largest remaining duplicate group is **seventeen functions at 132 instructions, all
identical modulo relocated symbols**. The recipe parks at **7 of 135 with size (284), count
and all relocations exact**, pin-free, and explicitly **not** a `CSE_CFLAGS` candidate — every
flag group is worse than plain `-O2`.

**The transfer is proven, not predicted.** All seventeen were generated from one recipe and
each measures the same figure with *the same three hunks and same seven encodings*. So closing
those seven lands seventeen functions — the highest-leverage item in the project.

`tools/dupfuncs.py` calls this group **x15**, because it demands byte identity and does not
canonicalise relocated symbol or label names. Its summary — *8 groups, 21 functions would come
free* — is therefore a **lower bound**.

### I tried to close it and failed; here is what that rules out

The residue is that the ROM loads the camera value **into its own address register** and shifts
it straight out to a fresh one, where we load to a spare and shift in place four slots later:

    ref   681b  ldr  r3, [r3, #0]   /  151d  asrs r5, r3, #20
    ours  681a  ldr  r2, [r3, #0]   /  ... 1512  asrs r2, r2, #20   (four slots later)

- **"Don't share a base" is wrong.** Dropping the shared `cam` local and reading the global at
  both sites reads **131 of 135 and is two instructions SHORTER** — gcc commons the two reads
  regardless, since they are 4 bytes apart. The ROM both keeps a base and clobbers it.
- **"Born earlier gets its own register" is wrong.** Three earlier placements of the pair cost
  **+6, +2 and +6** instructions (137, 56, 139). Every earlier birth lengthens the live range
  and costs instructions.
- The count is **already exact at 135**, so nothing is missing or extra. This is purely which
  register the load targets and when the shift issues. The scheduler is ruled out by flag, and
  declaration order by exhaustion — sixteen orders, byte-identical, because the one aggregate
  is the only memory local so there are no spill slots for the `expand_decl` lever to move.

It also **supersedes an older park** at 101 of 142 for the same routine
(`StampPlayerFootprintSolid`), whose real leads — the model id is a double indirection, the
search table is indexed by a mutated offset — are carried forward, and whose group figure and
claimed larger x18/x17 groups are stale (those are elevated and gone).

## A second near-landing

`OvlFunc_959_200d0e4` parks at **3 of 214** — size exact (576), count exact, **all 70
relocations exact**, 99.5% aligned, pin-free, no flag row. The blocker is sched2: one
`mov r0,#0x19` two slots late in an argument setup. Nine spellings measured, 3 is the floor,
pins inert, and `-fno-schedule-insns2` is the only flag that moves the site and ruins it.

## A real gcc-2.96 ICE

    Internal compiler error in decode_rtx_const, at varasm.c:3421

Fires when **one offset local is live into both arms** of an `if`/`else` reading `gState`, and
**only under `-fno-rerun-cse-after-loop`** — plain `-O2` compiles the same source. Six
candidates hit it. So a crash under a per-file flag is not necessarily a bad candidate. The
per-arm assignment is both the workaround and, independently, the better allocation.

## Parked

| function | figure | note |
|---|---|---|
| `OvlFunc_959_200d0e4` | **3 of 214** | size, count and all 70 relocations exact; pin-free |
| `OvlFunc_905_20088c0` | **7 of 135** | size, count, relocations exact — and transfers x17 |
| `OvlFunc_913_20088c0` | 7 of 135 | same recipe, same three hunks |
| `OvlFunc_923_2008ba4` | 7 of 135 | same recipe, same three hunks |
| `OvlFunc_956_200804c` | 81 of 145 | saturated; 94 aligned. Blocker is sched1, not allocation |
| `OvlFunc_897_20090c4` | 268 of 295 | saturated; 215 aligned (72.9%). 2 pins in `call_via` |
| `OvlFunc_959_200938c` | 137 of 135 | 49 under `-fno-rerun-cse-after-loop`; 118 aligned |
| `Anim_Douse` | 384 of 602 | saturated; 419 aligned. One extra `_call_via_r6` = the one extra insn |

## Negatives worth as much as the levers

- **`CSE_CFLAGS` does not apply to `OvlFunc_897_20090c4`**, and the reasoning is the useful
  part: it reads save bit `0x246` exactly **once**, so the documented two-part precondition is
  absent. `-fno-rerun-cse-after-loop`, `-fno-gcse` and `-fno-cse-follow-jumps` are all
  byte-identical to the default. No Makefile row should be written. Check the precondition
  before reaching for the flag.
- **A pin measured worse for the eighth and ninth time** (the camera register on the x17
  family; the sibling's barrier on `200d0e4`). It remains a per-function hypothesis, never a
  family default.
- **Only the width of the variable reaches a narrowing**: `OvlFunc_897_20090c4`'s `s[9]`
  read-modify-write must flow through an `int` local, because gcc narrows `~0xc` to the byte
  `0xf3`. Spelling the mask `-13` is inert.
- **A counter shared across a loop-nest split is expensive**: on `Anim_Douse` one counter
  straddling the outer sweep and the inner loop pushed the index off a global register onto
  `r4`, which `-fcall-used-r4` makes call-clobbered — 18 spill/reload sites and two frame
  slots. One counter for the six disjoint outer loops and a second for the two inner ones is
  the shape (382 → 416).

## Two park defects, both caught by parkcheck rather than by reading

- **A claim line quoted a flag figure.** `OvlFunc_959_200938c` claimed **49**, its figure under
  `-fno-rerun-cse-after-loop`, where production flags measure **137**. `parkcheck` re-measures
  the claim line at production flags, so N must be that number — fourth occurrence, after three
  batch-301 drafts put their aligncmp figure there. The park had even documented that parkcheck
  would read 137, and still wrote 49.
- **My own addendum broke three sound parks.** Prepending it pushed the `Verify with:` recipe
  out of the first comment block — all `parkcheck` reads — turning them UNCHECKABLE. I had
  recorded that hazard and walked into it anyway. Addenda now merge into the first block.

## Deferred

The `OvlFunc_897_20090c4` park carries 2 pins and an agent asked for a `fakematch.txt` row.
**Not added:** `fakematch.txt` maps functions to **landed** `src/` paths, and this is a park.
The row belongs with the landing.

## Open

- **Closing the x17 family's 7 encodings is the single highest-value action in the tree.** The
  count is exact and two readings are eliminated; what remains is a load-destination and
  issue-time question.
- `OvlFunc_959_200d0e4` at 3 of 214 is the next closest after it and `Anim_Froth`.
- `tools/dupfuncs.py` should canonicalise relocated symbol and label names; until it does,
  treat its group sizes and its "21 would come free" as lower bounds.
- **Host disk at 100%, ~1.9 GB free.** Unresolved.
