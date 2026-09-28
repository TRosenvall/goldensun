# Batch 291 — seventeen functions, and a verification tool that had been answering "clean" to everything

Gated on `make -j8 && make compare` → `goldensun.gba: OK` at
`5c4695205413df7db52b9a184815a07783999971`. All 17 landings verified at their exact ROM
addresses. 23 parks all verified by `parkcheck`. 10 commits.

The largest local batch so far, and the one where a **tool** was the most valuable find.

| | |
|---|---|
| elevated | **17** |
| parked | **23 new** (one advanced 10 → 2 while being parked) |
| unopened | 12 |
| `.sym` entries added | **2** (`_MSG_66`, `_MSG_2930`) |
| whole-file conversions | **6 `.s` files / 9 functions** |
| splits | 6 (2 text/data, 4 text-only) |
| tool fixes | **1, load-bearing** (`tools/datacheck.py`) |
| agents | 10 briefs at 5 targets each = 50 |

**38 assigned targets resolved + 12 unopened = 50.** The 40 outcomes above (17 + 23)
include 2 bonus functions that were in no brief: agent G's fifth, and agent J's
`Func_80c24b0`, an unassigned third function in a file whose stated count was wrong.
Twenty-two consecutive batches now reconcile exactly.

| brief | bank | landed | parked | unopened |
|---|---|---|---|---|
| G | overlays | **5** (3 `.s` whole, one a bonus) | 1 | 0 |
| F | overlays 925 / 971 | **4** (1 whole, 2 via splits) | 0 | 1 |
| I | `rom_f0000`, `rom_f6000` | **2** (1 whole) | 0 | 3 |
| J | `rom_77000`, `rom_b5000` | **2** (1 whole, 1 bonus) | 2 | 2 |
| B | `rom_c9000` | **1** | 3 | 1 |
| C | `rom_15000` | **1** | 4 | 0 |
| E | `rom_a1000` | **1** | 2 | 2 |
| H | `rom_c0`, `rom_f2000` | **1** (whole) | 3 | 1 |
| A | `rom_c9000` | 0 | 3 | 2 |
| D | `rom_9000` | 0 | 5 | 0 |

**Agents D and I were killed mid-write by the session rate limit.** Neither report ever
arrived. Nothing was actually lost: both had written their analysis into the candidate
*files*, including full drop ladders, so agent I's two landings came in complete with
their reasoning and agent D's five parks were placed as written. The lesson is cheap and
worth keeping — **an agent that writes its findings into the artifact survives its own
death; one that saves them for the report does not.**

## The headline is a tool, not a function

`tools/datacheck.py` exists to catch exactly one failure: converting a function out of a
`.s` that *also* carries data deletes the data with it. Neither `tryc` nor `objcmp` can
see it — both compare a single function and both report a clean exact match on a
candidate that will not link. Batch 267 lost `.L79b0`/`.L79b8` that way.

Its guard for "is this file gcc's own output?" was `".gcc2_compiled." in text` — a
**substring** search. Hand-written disassembly in this tree discusses that string in its
`@` annotation prose ("text through `Func_1e7c0` and `.gcc2_compiled.`"), so the guard
called those files generated, examined no data sections, and exited 0.

**Nineteen hand-written `.s` files carrying `.rodata` were silenced** — nine in
`rom_c9000`, four in `rom_15000`, two each in `rom_a1000` and `rom_b5000`, one each in
`rom_77000` and `rom_9000`. One was live: `asm/rom_15000/rom_23178_c_c_c.s`, holding
`Debug_IconTest` — which measured **exact this batch and was queued to land on that
verdict**.

`filtered.py` had already found and fixed this exact trap. Its `generated` docstring
records 72 files and 126 functions lost to the substring test across `templated.py`,
`census.py` and `split_s.py`, and states the correct predicate: gcc identifies itself by
the banner on line 1 and by `.gcc2_compiled.:` as a **label at line start**, neither of
which prose can imitate. **The fix never reached `datacheck`.** So the repair was to
import that function rather than re-derive the test — one predicate, one place.

Two process notes, both mine:
- My first attempt to measure the blast radius compared against a copy of the old tool at
  `/tmp/datacheck.bak`, which computes `ROOT` from its own path, walked `/asm`, and found
  nothing. The "83 → 0" that produced was an artefact of where I put the file. Measured
  properly in place, the answer is 83 files reported now, **19** of them previously
  silenced — which is exactly what the agent had said.
- A looser scan for any `.word <1-255>` returns 80 hits and looks like a refutation of the
  corpus control for `_MSG_66`. It is not: those are `.data`/`.rodata` tables, not pool
  words. **A difference in what a scan matches is not a difference in the world**, and the
  precise test — a `.word` reached by an SImode `ldr rX, .Lnn` — returns zero.

## `.global` under-counting is now measured in both directions

Batch 290 recorded that a split must export every label crossing the new boundary.
Batch 291 shows **`datacheck`'s EXPORTS line cannot tell you which those are**, because it
lists the labels that are *already* `.global`:

- `rom_23178_c_c_c.s` — sixteen listed, and **five more needed** (`.L37440`, `.L37448`,
  `.L37450`, `.L37458`, `.L37460`), all inside the same `.rodata`, none `.global`, all
  read only by `Debug_IconTest`. Without them the link fails with five undefined
  references.
- `rom_a4f08.s` — **no** EXPORTS line at all, and exactly **one** export needed
  (`.Laf08c`); the other target in the same file needs none.
- `rom_cc5d8_c_c.s` — three listed, and **five more** needed to convert
  `BaseAnim_Tentacle`; converting either of its file-mates needs **zero**.

The rule the tool cannot express: **read definitions against references across the
intended boundary, in both directions.** `split_s.py` enforces the order — export first,
gate that, then split, then write the `.c` — so a layout mistake can never be confused
with a bad decompilation. Every split this batch followed it and every intermediate gate
was green.

## Mechanisms established

**A commutative `mul`'s source operand order picks the tied destination.**
`*thumb_mulsi3` ties operand 0 to operand 1 with `%0`, and canonicalisation does not
erase the order: `(a->f30 + 0x1c) * __cos(ang)` is 2 differing, `__cos(ang) * (a->f30 +
0x1c)` is exact. This is the **counter-case** to the standing "all four
pointer-arithmetic operand orders are inert" — for `mult` the order is real, and the
ROM's `mul rD,rS` destination is the readout.

**A `"+r"` barrier goes on the FIRST use of a repeated pool constant, not the second, and
the pass is cse2.** Barrier on the first site: exact. On the second: 71 differing,
completely inert. Pinned from the other side too —
`-fno-rerun-cse-after-loop` alone reproduces the ROM's stream while `-fno-gcse` and
`-fno-cse-follow-jumps` do not, and it cannot be gcse because `want_to_gcse_p` returns 0
for `CONST_INT`. **Because a source route exists, no per-file CSE flag row is warranted**,
and `OvlFunc_959_2009528` — where such a row looked necessary — should be re-screened with
the barrier on its first site before any row is added.

**That barrier then unblocked a park two banks away, which is the batch's best result.**
`ActorCmd_Wander` was parked at 10 of 187 with a complete diagnosis: byte-exact under
`-fno-rerun-cse-after-loop`, cse pass 1 producing the ROM's destructive-add shape and cse
pass 2 undoing it via `make_regs_eqv` — but **unusable**, because the flag is per-TU and
the file-mate `ActorCmd_Unk9` needs the opposite (40 of 367 becomes 50 under the flag, and
the ten extra are the same two coordinate pairs). Its own closing note was that a
source-level route must exist and had not been found. **It had been found, that same
batch, by a different agent for a different symptom.** The barrier is per-site where the
flag is per-file: **10 → 2**, size and instruction count still identical, relocations still
identical. Neither agent could have used its own half.

The residue is the same copy direction on the `x` pair alone, and two of the four rejected
routes are worth recording as shapes: the divide-first ordering that was exact *under the
flag* is 59 and +4 bytes *with* the barrier — the ordering and the barrier are
**alternative routes to the same swap, not additive** — and naming the div temp
explicitly, which is literally the ROM's shape, is 71 with barriers, because naming it
gives it its own quantity and cse1 then has nothing to swap.

**The allocation priority formula read as an instruction rather than as a wall.**
`Func_80a4f08`'s `win` and `i` have fourteen real refs each, so `allocno_compare`'s
`floor_log2(n_refs) * n_refs` is 42 for both and only `live_length` separates them.
Writing `i = a;` *above* the `state` load makes `i`'s live range a strict superset of
`win`'s, so `win` ranks first and takes r7: **68 → 19, and the whole residue went with
it.** Two earlier batches used this formula to prove parks unreachable; this is the first
time it said what to move.

**A 2-bit `unsigned char` bitfield copy is the only route to an SImode `~0xc`.** The ROM's
`mov r3,#13 / neg r3,r3 / and r3,r1 / orr r3,r2 / strb r3,[r5,#9]` is unreachable from any
masked-merge spelling: combine's `simplify_and_const_int` masks `-13` by the zero-extended
byte's `nonzero_bits` and gives `mov r3,#0xf3`. Seven spellings measured, all 48. The
bitfield reproduces it register for register, because `store_fixed_bit_field` builds its
mask in the container mode and never reaches `force_to_mode`.

**The `int`-return lever ran backwards twice in one batch, in two different banks.**
`DrawLine` must be `void` in `rom_c9000`; `UIDrawText` and `Func_801ea08` must be `void`
in `rom_15000` because the ROM writes r0 first among the argument moves (44 → 28). **The
lever's direction is per callee — not per bank, and not fixed.**

**Argument precompute's order is the lever, not its presence.** Only one of four orders is
exact (x,y,z exact; y,x,z 7; y,z,x 45; z,y,x 45; call-site expressions 46). The tell is
the ROM's *second* low-register copy of a high-register pointer. And separately: **at a
two-register-argument call, reach for the callee's return type before the precompute** —
block-scoped precompute locals were fully inert at three sites where declaring the callee
`int` went 9 → 3.

**A `goto` out of an if-arm only places the block late if the arm does not fall through.**
The bare `goto` is right for a long arm (`goto Lc74`, 154 → 9) and wrong for a short one,
where gcc inverts the test and pulls the block inline; writing the other arm as the `then`
is worth 20.

**`extern int L1f4c __asm__(".L1f4c");` needs no linker alias.** GAS emits
`R_ARM_ABS32 .L1f4c` straight from the C declaration and it links because the defining
`.s` already has `.global`. So `label.sym` / `_TBL_` aliases are needed **only** where the
label is not already global — which `label.sym`'s header overstates.

## Symbols

Two admitted, both with mechanical rather than aesthetic control.

**`_MSG_66 = 0x0066`** — the base of eight starting-PC name strings, for `ResetPCs`.
gcc-2.96 never pools an SImode constant it can build with a single `mov` (imm8 covers
0..255), and scanning every tracked `.s` bearing gcc's banner for an `ldr rX, .Lnn` whose
pool word is a plain decimal 1–255 gives **zero hits in 4,282 files**. The ROM pools 0x66
anyway, so the operand cannot have been a literal. In-function control three ways: the
same function's `0x1ff` pools (over imm8), `0x4000` builds as `mov #128 / lsl #7`, and 0
pools as a HImode store constant. Namespace decided by use, not value — 0x66 also exists
as `_AREA_66` and `_FILE_66`, but `area.sym`'s space is a halfword that 61 functions
*compare*, and this value is never compared.

**`_MSG_2930 = 0x2930`** — base of a nine-message run in `OvlFunc_971_2008b94`. **The
control is an encoding limit, which makes it a prediction rather than an observation.** The
ROM derives eight ids as `add r0, r6, #K` for K in 1..7 and reaches the ninth — 0xd past
the base — as its own `ldr r0, =0x293d`, same function, same `__MessageID`. Thumb's
`add rd, rn, #imm3` encodes only 0..7: **every id the short form can reach is derived, and
the single id it cannot reach is pooled.** The partition falls exactly where the
instruction encoding falls. As a plain integer the function is 177 of 175 differing at
+5 instructions and +24 bytes.

## Corrections made to agents' own reports

- **A reported lever did not hold.** `Func_80c24b0`'s 0x23c offset was written
  `t = 0x8f; t <<= 2;` to mirror the ROM's `mov / lsl`, and reported as surviving the drop
  ladder. It does not: `g += 0x23c` compiles byte-for-byte identically, because
  `thumb_shiftable_const` already builds it that way unasked. Both measured side by side.
  **A drop-ladder claim is only as good as the drop actually being tried**, and contrived
  arithmetic that buys nothing is worse than the constant it replaces.
- **Three briefs stated the wrong function count for their own file** — `rom_c1a34_a_c.s`
  is 3 not 2, `rom_a5534_c_a_a.s` is 6 not 2, `rom_a4f08.s` is 4 not 2 — and one named the
  wrong bank entirely (targets 4/5 are overlay 971 in `rom_7fb4a8`, not `rom_7fa4ec`).
  Agents caught all four themselves. `grep -ci func_start` before trusting a brief.
- **`AnimEnd.c`'s claim about the `AnimStart` pair fails on three counts** — wrong file
  (never repointed after a split), figures matching nothing measurable, and a dependency
  claim that is simply false: `AnimStart`'s queue push is not `AnimEnd`'s inline (one reads
  the count as `s32` and guards `< 32`, the other uses `ldrh` and `cmp #0x1f / bgt`). They
  do not block each other.
- **`repoint_parks.py` must run after the parks are placed, not before.** Three parks came
  back UNCHECKABLE on the first `parkcheck` pass because their recipes named `.s` files
  split in this same batch; repointing worked once the files existed. And of 162 dangling
  park references swept tree-wide, only **1** was a verify recipe — the other 161 are prose
  `Source asm:` lines, which `parkcheck` does not read. That is documentation staleness for
  the next reader, **not** tool breakage, and it is not what drives the UNCHECKABLE count.

## State

`census` TOTAL and `funcindex`'s still-in-asm figure agree at **1,005**, which is the
cross-check `census.py`'s own docstring requires.

| | |
|---|---|
| functions elevated | **4,705 of 5,710 (82.4%)** |
| still in `asm/` | 1,005 |
| — parked | 611 |
| — **unattempted and attemptable** | **304** |
| — hand-written assembly (never was C) | 76 |
| — ARM (no ARM compile path in this build) | 14 |

The unattempted 304 are now almost entirely large: **303 of them are over 100
instructions** and 253 are over 200. The 1–100 bands hold just **1** available function
between all four of them. The single full pass is close to done, and what remains of it is the big end.

## Open

- **`datacheck`'s EXPORTS line should compute the exports a split will REQUIRE**, not list
  the ones that exist. Three independent hits this batch; the logic is a definitions-vs-
  references diff across a proposed boundary and is mechanical.
- `InitRenderTilemapBG1` at **2 of 129** and `ActorCmd_Wander` at **2 of 187** are the two
  closest parks in the tree. Wander's residue is one copy direction; the barrier is the
  right tool and has not yet been aimed at what still drives it.
- `Func_80a5788` (464 instructions, `rom_a1000`) is **unattempted, unparked and absent from
  the index** — found by an agent reading a file whose brief said it held two functions.
- The `make clean` churn (720 then 1,135 tracked `.s` files gaining a duplicated
  `.text / .align 2, 0` tail, additions-only and byte-neutral) still reproduces and is
  still worth ten minutes in the Makefile.
- ~469 parks carry no `Verify with:` recipe at all, which is what `parkcheck` reports as
  UNCHECKABLE. The 161 stale prose references are a separate, smaller job.
- Decisions still waiting on the owner: `-ffixed-r7` for `CamelotLogo` (closes a
  4-instruction prologue gap but leaves 109 differing — likely withhold);
  `OvlFunc_930_20091b0`'s three; `-fcall-saved-r4` for `OvlFunc_970_2008f80`;
  `ALIAS_CFLAGS` for `OvlFunc_882_200c41c`; a `DMA3_SET` `"r2"`-clobber variant for
  `dma.h`; and the withheld symbols `_MSG_26fa`, `_MSG_2850`, `_MSG_ae0`, `_FILE_18`,
  `_FILE_19`, `_SIZE_80b5138`.
