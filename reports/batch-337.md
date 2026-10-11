# Batch 337

Five briefs, 20 targets, **7 landings** — six from the briefs, and the seventh
finished after the batch when the hold on it turned out to be unnecessary.
**682 → 675 park files**; census ROM functions landed 4,949 → **4,956**.

Landings: `Func_801fd34` and `HeightTile_A` (A), `Func_8006240` and
`Func_80c0130` (B), `OvlFunc_922_2008ed8` (C), `OvlFunc_932_20086a0` and
`OvlFunc_common1_588` (D).
Brief E held all four with several rungs closed.

Targets were drawn from the 49 parks claiming ≤10, **every one re-verified by
`parkcheck` at the head of the batch**, with pin counts from `shimcount.py` and
piece membership from `piece.py` pasted verbatim. 20 distinct pieces, no
collisions. Instruction counts were **withheld deliberately** — see below for
why that turned out to be right.

## `do { } while (0)` beats the barrier — a correction to batch 336

Batch 336's device table listed only the `__asm__` spellings. Brief A: for a
purely **scheduling** residue the `do { } while (0)` anchor is strictly better,
and the table now says to try it first.

- **Same reach, and stronger in the compiler.** `haifa-sched.c:3744-3757` adds a
  dependence on every register's last uses (`REG_DEP_ANTI`), last sets and last
  clobbers, **then calls `flush_pending_lists`** — so it orders memory too, which
  a bare `ASM_INPUT` does not. (Verified in the source, not taken on report.)
- **It emits nothing**, like the barrier.
- **`shimcount` does not count it**, so it is not fakematch-class and carries no
  depin debt.

`Func_801fd34` landed on one anchor: barrier-free floor **9** over 384 crossed
spellings, two asm barriers 2, one asm barrier 3, **one anchor 0**. And brief A
separately **removed** a device — swapping `DisplayMenuArrowCursor`'s batch-329
barrier for an anchor at the same figure of 5, taking its `shimcount` row from
`empty asm : 1` to nothing.

> **This is the rare lever that pays a figure and reduces pass-3 debt at the same
> time.** Worth retrofitting wherever a park already ships a bare barrier.

Use an `__asm__` form only for a reach the anchor lacks — the `cse` flush or the
`combine` fence, both of which need an `ASM_OPERANDS` pattern.

## Mechanisms

**A plain `unsigned char` struct-member store is a read-modify-write insert that
leaks a zero** (brief C, `OvlFunc_922_2008ed8` 8 → 0, byte-identical, pin-free).
`.00.rtl` shows `(old & ~fieldmask) | value` with `~fieldmask == 0`, so combine
holds a `(set (reg) 0)` nothing needs and **hands it to the next statement that
wants a zero** — stretching the zero's live range to insn 57–82, overlapping the
sprite pointer's 34–67, and the pointer lost `r1` to it.
`*(unsigned char *)&n->f9 = w;` takes the plain MEM path: **8 → 5 with all three
registers becoming the ROM's at once**. So the batch-329 diagnosis had the
causality backwards — the pointer was the victim, not the cause.

**The multiply operand copy is decided at expand** (brief A, `HeightTile_A`,
closing the 16-member `HeightTile_0..F` family). `*thumb_mulsi3`
(`arm.md:1116-1129`) always copies pattern operand 1, and `expand_binop`
(`optabs.c:647-669`) swaps when `op1` is a REG and `op0` is not — so
`expr * var` and `var * expr` both give `(mult var expr)`, and no pass after
`.00.rtl` touches the order. The fix is to stop the var being a bare pseudo:
`((a - b) * (t - 8))` rather than `a -= b;` then `(t - 8) * a`. Two batches had
gone into `REG_ALLOC_ORDER` and `find_reg`'s preference loop; **both registers
were already the ROM's.**

**A byte access through a struct member does not get alias set 0** — it gets the
struct's set, even for a plain `unsigned char` (brief A, probed: `a[8]` through
`unsigned char *` → 0, `a->f8` → 8, bitfield → 13). This refines batch 335's
`c-common.c` reading and retires a park's one-bit-field device as never needed.

**A fourth class-rung decision** (brief B, `Func_80c0130`): even granting the
park's stated escape at 3, the ROM's next insn is anti-dependent on the `strh`,
so `insn_cost` returns 0 (`haifa-sched.c:3105-3123` with `arm.c:2424-2427`) and
the candidate is class 2 against class 3 — unreachable a second time, for a
different reason than the one recorded.

**`gcse`'s cprop is NOT unstoppable by basic-block structure** (brief B). Batch
329's *"gcse's cprop is GLOBAL, so no basic-block boundary of any kind can stop
it"* is false, and the park's own batch-327 body is the counter-example:
rebuilt, reproduced at 7 of 119, and `.07.gcse` has **zero `CONST-PROP:` lines**
while its set table holds both sets. The gate is `cprop_insn`'s
`oprs_not_set_p` test (`gcse.c:3826-3828`), which is **per basic block**, plus
the `PARALLEL` skip at `:3792-3797`.

**`allocno_compare`'s third factor is `size`** (brief E, `global.c:607-614`),
unnamed by four batches and now closed on its target. Brief E also validated the
priority model end to end by running `allocno_compare` over all twelve allocnos
and reproducing `.18.greg`'s order **twelve for twelve**, which refuted two of
batch 331's claims: the chain order *is* sufficient alone here, and the real
live lengths are 46 and 24 rather than the inferred "L+2 and L with L≈30".

## -O1 for `OvlFunc_common1_588`

Added **explicitly, not as a pattern**, beside the Makefile's own record of five
mis-scoped `-O1` wildcards that each caught a split product sharing a prefix. At
-O2 gcc if-converts a three-way message-id pick (`find_if_case_1`,
`ifcvt.c:1651`; `.14.ce` reports "2 IF blocks converted"), hoisting each pool
load above its own compare. Precedent is exact:
`src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c` was parked on the same
speculative literal hoist and matched at -O1 with no source change.

The inversion worth keeping: **the parked body is the only one of ten in that
corpus group that does not improve at -O1**, so a flag sweep run on the park
alone finds nothing. Brief D got there through the labelled corpus — 394
generated hits for the needed shape, narrowed to the 15 with a plain-numeric
then-block pool word, and the `.c` beside one names this exact blocker.

`ovl_common` is **not local**: that split rewrote **three** overlay linker
scripts (`rom_7db0c8`, `rom_7ddb88`, `rom_7e0928`), as brief D predicted, and all
gated green.

## Held for the owner

**`Func_80c0130` reaches 0 of 37** (from 6) but needs a bare
`__asm__ volatile ("")` **inside `include/dma.h`'s inline helper** — new for a
shared header, since every future caller of `DMA0_SET_OFS` would inherit a
scheduling device. The helper promotion itself has precedent (`DMA3_CLEAR_OFS`,
`DMA3_FILL_OFS`, `DMA3_SET_RW`, all promoted in batch 323 for a single function
each; the four register pins are that header's house pattern for
`stmia r3!, {r0, r1, r2}`).

Ladder: neither barrier **6**, call-site only **5**, header only **3**, both
**0** — so no route to 0 avoids it. Parked at 6 device-free, candidate kept,
finding preserved in the park header, booked in `docs/owner-decisions.md`.

> **RESOLVED — the question was withdrawn, not ruled on.** Brief A's anchor
> finding landed *after* brief B had finished, so nobody had tried substituting
> it. Measured in all four combinations, and **every one matches** at 84 bytes,
> 37 encodings and 1 relocation:
>
> | header | call site | result |
> |---|---|---|
> | barrier | barrier | OK |
> | anchor | barrier | OK |
> | barrier | anchor | OK |
> | **anchor** | **anchor** | **OK** |
>
> `Func_80c0130` is **landed** at `src/rom_b5000/rom_bffb8_a_a_a_c_b.c` with
> **no device anywhere**: `shimcount` prints no rows on it, where the inlined
> control printed `empty asm : 2`. `include/dma.h` gained `DMA0_SET_OFS`
> carrying an anchor rather than a barrier, so no caller inherits a device, and
> no `fakematch.txt` row was needed. The helper's four register pins live in the
> header as its house pattern for `stmia r3!, {r0, r1, r2}`, exactly as
> `DMA0_SET` has them.
>
> **The general lesson is worth more than the function: when a hold is about a
> DEVICE, test a device-free construct before asking for a ruling.** A precedent
> for devices in shared headers was nearly set for a function that did not need
> one.

## Data quality: park headers are putting the stream count on the instruction line

This now matters because the selection criterion **is** the instruction counts.

- Brief C: **three of its four** park headers do it — 179 encodings is 159
  instructions + 20 32-bit entries; 221 = 165 + 56; 41 = 38 + 3.
- Brief E: two more — `StartRain` claims "103 instructions" (it is **84**),
  `8096810` claims 122 (it is **75**), both predating `objcmp` printing them
  separately.

**Withholding the instruction counts from the briefs was the right call** — the
parks themselves carry the confusion, and every brief re-derived both numbers.
All 20 targets turned out to be **true distances** (equal counts), so the
length-gap criterion had nothing to find on any of them this batch.

## Tooling, and two mistakes of mine

`tools/piece.py` gained a **bodyless-park fallback**. It indexed parks by the
function they *define*, so a park with no body — **144** of this tree's parks are
prose/triage/class parks — reported "no park found" for a function that is
parked. Brief D caught one (`src/non_matching/ovl_common/4cc.c`, 61 lines of
header, and its filename also breaks the `<addr>.c` convention so no name match
could rescue it). The fallback reads the header **subject, first line only**,
which is tight enough not to re-introduce the prose-as-machinery bug this index
has already had once.

**I then broke that convention while fixing it.** My first repoint note was
*prepended* to `4cc.c`, which moved the subject off line 1 and defeated the
fallback I had just added. Notes go **under** the subject line; the file now says
so. I also claimed its scratch candidate was stale and it is not —
`scratch/N04cc_best.c` exists, checked.

Three park recipes orphaned by this batch's splits were repointed. `4cc.c`
needed it by hand: it names its `.s` with no `--func`, so
`install_batch.py`'s automatic repoint cannot act on it and correctly flagged it
for manual handling.

## Open

- **`OvlFunc_common1_4cc`** carries the *same* blocker as the `-O1` landing with
  the same message ids, so -O1 is the indicated remedy; its park is cross-linked.
  Brief D notes a third park on the same blocker.
- `Func_80216b4` 9 → **7**, with an instrument at **4 of 24** from a three-way
  cross (struct store + `volatile` + an r2 pin) shipped device-free at 7.
  Corpus check: zero instances of the ROM's window in 4,527 generated `.s`, so
  the next move is shape evidence rather than spelling.
- `Func_800bfa4` 8 → **6**; `OvlFunc_970_2008da4` 6 → **5**.
- `Func_80b6d30` holds at 4; residue is now a **cse2-only** question with the one
  satisfying construction derived and costed at +2.
- `OvlFunc_927_200a1b0`'s pinned-exact claim is **verified** → pass-3 depin.
- `OvlFunc_931_2008904` holds at 4 on a perfect plateau (nine spellings, all 4 at
  index 167); one door left, `find_reloads`' commutative handling
  (`reload.c:2880-2930`), reachable only with a byte load that does not die at
  the third IOR.
- Brief E's `e_sched.sh` means scheduler priorities never need hand-derivation
  again — `-fsched-verbose=6` writes a `prio` column into `.23.sched2`.
