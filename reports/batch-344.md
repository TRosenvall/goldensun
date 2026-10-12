# Batch 344 — the figure was never a distance

**5 briefs, 20 targets, 3 landings** — and the reason this batch matters is not
the landings but the pool they came from.

## The finding

Every targeting instrument in this tree ranks parks by their **figure**: the
count of differing encodings in the object stream. That figure is a
**positional** count. When the two sides' 16-bit instruction counts differ, the
streams are compared **out of phase** — from the first missing or extra
instruction onward, every later slot is offset by one and counts as differing.
The figure stops measuring distance and starts measuring **offset**.

A park one instruction from matching can read 109, 143, 159.

Measured over all 558 parks carrying a verify recipe (`tools/gapscan.py`, raw
data at `reports/data/gapscan-344.tsv`):

| | parks |
|---|---|
| equal instruction length | 200 |
| **out of phase** | **358** |
| — within 1 instruction | **100** |
| — within 2 | 156 |
| — within 3 | 192 |
| — within 5 | 244 |

**192 parks are within three instructions and their median figure is 134**
(range 17 to 1934). A hundred are at a one-instruction gap.

Those parks had sorted to the bottom of every shortlist here while being
structurally the closest work available. That is expensive because the
**length-gap criterion is this tree's most productive targeting rule by a wide
margin** — 8 of 15, 7 of 25, 8 of 25, 10 of 32 across four batches, and it
*locates* the missing instruction on essentially every target. It had only ever
been pointed at parks whose figure was already small, which is a tiny and (as of
343) exhausted slice of the parks it fits.

## Results

Twenty targets drawn from the new pool, all at ±1 or ±2, split into briefs by gap
**direction**.

### Landings — three, all pin-free and device-free

| function | before | counts | how |
|---|---|---|---|
| `Func_80c0228` | 21 (phase) | 50/50 | **a crossing of two of the park's own recorded negatives** |
| `Func_80a355c` | 50 (phase) | 69/69 | two levers, **either alone inert** |
| `Func_80a8b10` | 51 (phase) | 58/58 | loop.c **biv elimination**; refutes a standing bound |

- **`Func_80c0228`.** The park's question (`local-alloc.c:1633-1636`) was exactly
  right. The divide result must be block-local so `combine_regs` ties it to the
  dying dividend temp — that is what occupies r0–r4 and forces the pooled base
  onto r5, which *is* the missing `pop {r5}` — **and** `row` must be assigned in
  both fill blocks so `combine_regs` declines to tie block 2's copy. The park's
  eight-combination sweep varied `{r, i, d}` and never touched `row`.
- **`Func_80a355c`.** Reuse `off` as the index (the ROM holds `off` and `idx` in
  one hard register, r6) *and* set it before the call that feeds the multiply.
  Reusing `off` but setting it after the call is gap +5, which is what proves
  both halves load-bearing. The park's `allocno_compare` arithmetic was right and
  its verdict wrong: the priority contest it described does not exist, because
  the two values are **one variable**. Fifth consecutive batch whose landing came
  from a park whose arithmetic was correct.
- **`Func_80a8b10`.** Needed a text split, gated alone before the `.c` was
  written. See the refutation below.

### Length gaps closed without landing — five

Each of these gives a park its first figure that is a real distance.

| park | before | after | counts |
|---|---|---|---|
| `OvlFunc_967_2008308` | 56 (phase) | **2** | 60/60 |
| `Func_8021d88` | 40 (phase) | **7** | 47/47 |
| `StartEarthquake` | 51 (phase) | **14** | 66/66 |
| `OvlFunc_971_2008e10` | *no figure* | **29** | — |
| `StartThunder2` | *no figure* | **39** | 68/68 |

## A standing `docs/elevation.md` bound is retired

"**A copy of an UNCHANGING value is unreachable**" cited `Func_80a8b10` as its
evidence. The function now lands in **five** spellings.

The ROM's `mov r12, r5` is **loop.c biv elimination**: write the clearing loop as
an *index* loop and strength reduction materialises the giv's `i == 0` value as a
fresh preheader pseudo, which coalescing cannot touch; `r12` follows from
`REG_ALLOC_ORDER` (`arm.h:989`). The whole gap is in the **preheader**, which is
why every probe aimed at the loop body measured inert.

The entry had generalised a correct observation about one body into a bound over a
class — the failure mode this project keeps paying for.

## Mechanisms added

- **`flow.c:645-665`** appends a `(use (const_int 0))` to any basic block ending
  in a CALL and closed by a label. `find_cross_jump` (`jump.c:1444-1470`) walks
  back skipping only NOTEs and CODE_LABELs, so that nop fails the cross-jump at
  `:1469-1470`. Cure: duplicate the shared tail in source and let jump2 merge it.
- **A HImode pooled mask forces `create_fix_barrier` to emit the ROM's `b`.**
  `*thumb_movhi_insn` alt 1 carries `pool_range` **64** against SImode's **1020**
  (`arm.md:4353` vs `:3856`). **The sign is load-bearing**: `0xffffc000` is
  `unsigned` in C and narrows to `49152` (figure 3); `~0x3fff` narrows to
  `-16384` (figure 2). Zero of the tree's 4,562 generated `.s` files hold this
  shape. Read in reverse it is what closed `StartThunder2`.
- **Check the reference's argument registers before diagnosing register
  pressure.** `Func_8021d88` was parked on "nothing source-level"; the reference's
  first argument to `Func_8021cb8` is `a`, not `p`. One token, 40 → 11, gap
  closed. A function holding an extra long-lived value because its *source* names
  one is indistinguishable at the prologue from one where the allocator found a
  spare register — the disambiguator is which register the ROM feeds each call,
  not anything in the allocator dumps. Four parks in that class are unchecked.
- **`volatile` is the only escape from the halfword narrowing on Thumb**
  (`combine.c:5921-5938`, `arm.h:2287-2290`).
- **`Func_80167e0`'s pass attribution was wrong in three successive headers**:
  counting `ior` insns per dump shows **gcse pass 07**, not loop.c, hoists
  `ctl | cnt` — which is why the hard-register instrument measured inert.

## Two instruments were manufacturing false leads

- **`autosweep`'s flag axis had no phase guard.** The anchor axis got a
  `dsize`/`first` guard in 343; the flag axis never did and kept ranking on raw
  ndiff. Brief E measured the cost: **three of its four targets carried a false
  "BETTER" flag row** — on T1, two rows reading 37 against a base of 49 both
  *raised* the real residue from 9 to 12. Fixed, and a row failing the guard no
  longer enters `BEST`, which is the part that stops propagation.
- **`gapscan` was blind to byte size.** Brief A's p1 is **equal at 102/102
  instructions and two bytes long** — one of the reference's 16-bit `b`s is a
  32-bit `bl` on our side. An equal instruction count is not an equal length.
- **And `gapscan`'s own first version reported the pool as 99 when it is 192.**
  Its separator class was `[\s\\]+` for a verify command parks write across
  several lines of a **block comment**, so the continuation begins
  `" *     src/..."`. It matched the 56 parks that fit the command on one line and
  **silently skipped 550** — better than 90% of the tree.

That makes **six instruments, six clean negatives** that were really unmeasured
axes (`piece`, `parkcheck`, `directory`, `shimcount`, `autosweep`, `gapscan`).
A tool that reports nothing is making a claim, and the claim needs the same audit
as a figure.

## Open

- `OvlFunc_888_200b1b8` (brief A) and `OvlFunc_882_200bc48` (brief E) are both
  bounded pin-free with citations; both wait on standing owner decisions.
- Brief A's open question: which first-jump-pass transform moves a
  singly-referenced `goto` target's block to a conditional jump's fallthrough.
  Not `jump.c:434-476`, not bb-reorder (`flag_reorder_blocks` is 0 at
  `toplev.c:443`). Dumps left in `dumps/t1_v1/`.
- Brief E found `Func_80a355c`'s named twin **`rom_a1000/80a602c` goes 72 → 38**
  on the half of the lever that generalises. Not installed — it needs a rewrite,
  not a figure patch.
- Brief C: one pin on r0 alone makes `OvlFunc_967_2008308` byte-identical; the
  device-free question now has its precondition stated (`update_equiv_regs` needs
  `REG_BASIC_BLOCK < 0`).
