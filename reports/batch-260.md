# Batch 260 -- ten functions, and a flag bug that scored four parks against a prologue gcc could not emit

Gated on a clean `make clean && make -j8 && make compare`, green at the target
SHA1 `5c4695205413df7db52b9a184815a07783999971`. Every address checked against the
linked ELF with `tools/checkaddr.py`.

| | |
|---|---|
| elevated | **10** |
| parks retired | 6 (+1 stale park retired separately) |
| whole-file conversions | 4 |
| splits | 2 (one of them at a DATA boundary) |
| build-input changes | 0 Makefile rules |
| fakematch debt added | 3 functions |

A six-agent round. Every one of the six closed. **Six of the ten came out of the
park pile and four were file-mates picked up free** -- the eighth round running
that heuristic has paid.

## What landed

| | function | address | source |
|---|---|---|---|
| 1 | `OvlFunc_957_2008de8` | `0x02008de8` | [ovl_30_…_a_c_c.c](src/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_c_a_c_c.c) |
| 2 | `OvlFunc_942_20087dc` | `0x020087dc` | [ovl_30_…_c_a_c.c](src/overlays/rom_7c6bac/ovl_30_c_c_a_c_c_c_c_a_c.c) |
| 3 | `Func_8028808` | `0x08028808` | [rom_23178_…_c_c_c.c](src/rom_15000/rom_23178_a_a_a_a_c_c_c_c.c) |
| 4 | `Func_80288a8` | `0x080288a8` | [rom_23178_…_c_c_c.c](src/rom_15000/rom_23178_a_a_a_a_c_c_c_c.c) |
| 5 | `Func_8028920` | `0x08028920` | [rom_23178_…_c_c_c.c](src/rom_15000/rom_23178_a_a_a_a_c_c_c_c.c) |
| 6 | `OvlFunc_common2_0` | `0x02009158` | [common2_a_a.c](src/overlays/common/common2_a_a.c) |
| 7 | `OvlFunc_common2_254` | `0x020093ac` | [common2_a_a.c](src/overlays/common/common2_a_a.c) |
| 8 | `OvlFunc_947_2009fd4` | `0x02009fd4` | [ovl_1528_a_a_c_b.c](src/overlays/rom_7d0e88/ovl_1528_a_a_c_b.c) |
| 9 | `OvlFunc_924_200ae6c` | `0x0200ae6c` | [ovl_2dcc_c_a.c](src/overlays/rom_7ac2d8/ovl_2dcc_c_a.c) |
| 10 | `OvlFunc_924_200af68` | `0x0200af68` | [ovl_2dcc_c_a.c](src/overlays/rom_7ac2d8/ovl_2dcc_c_a.c) |

## The tool bug

`COMMON2_CFLAGS` makes **two** changes:

```make
$(subst -fcall-used-r4,-fcall-saved-r4,
  $(filter-out -mthumb-interwork,$(GCC296_CFLAGS)))
```

`tryc.makefile_flags()` recorded only `"no-interwork"`, and both `tryc` and
`objcmp` acted only on that. The r4 substitution was silently dropped -- and since
`-fcall-used-r4` means **gcc never pushes r4**, every common2 function whose ROM
pushes r4 screened ~27 differing with the first diff at **index 0**:
`push {r4,…}` against `push {r5,…}`.

> The screen was scoring these against a prologue gcc could not emit under the
> flags it was using.

Four parks were affected. Both tools fixed; the other three are worth re-screening.
The wildcard itself is correct for the family, measured on the whole object:
`no-interwork + -fcall-saved-r4` EXACT, adding interwork back 33 differ, keeping
tree-default `-fcall-used-r4` **310** differ.

That is the third tool defect in three batches — `park_status` (four separate
failure modes), the reference-filtering scope trap, and now this. All the same
shape: **a number produced by a tool whose contract did not match the question it
was being asked.**

## A ceiling that has now failed four times

*"A straight-line/unguarded site cannot fix an argument interleave, because the
dominating-local lever needs a dominating block."* Disproved in batch 259 and
falsified again on both interleave parks here — one of which had measured its
naming lever at **99 of 110** before concluding it was unreachable. A bare
`register int q __asm__("r0") = …;` closed it on the first try.

The refinement that came with it is the useful part:

> A cheap constant argument cannot be repositioned ALONE, but it can be
> repositioned **as a pair**. Pinning r0 alone measured 6, and **8 when applied at
> both sites — worse than doing nothing**. Pinning r0 *and* r1, declared in
> argument order, was exact.

The cheap argument cannot be given a lower LUID than the expensive one unless the
expensive argument's own materialisation is also pulled out of the precompute
path. One pin has nothing to be ordered against. And on the 763-instruction
function, the r0 pin **alone did not kill repeated-constant CSE at all** (15
differing before and after) — the pair did.

## Mechanisms

**`andsi3` and `iorsi3` are not symmetric in `arm.md`.** Under `TARGET_THUMB`,
`andsi3` calls `force_reg` for any non-`CONST_INT` operand 2; `iorsi3` calls it
only in the `CONST_INT` branch. A QI-narrowed mask arrives as
`(subreg:SI (reg:QI n) 0)`, so the `and` gets a plain REG and the `ior` keeps the
subreg — and `regmove_optimize` opens with `if (GET_CODE (src) != REG) continue;`.
That single asymmetry is why `&= mask` and `|= mask` on the same byte compile with
different operand roles from symmetric C. Reordering the C cannot fix it:
`fold_rtx` canonicalises a known constant to operand 2, and `block_alloc` can only
tie the destination to operand 1.

**A pool-load order residue is usually LUID, not pool arithmetic.** Batch 259 made
pool arithmetic readable, and `Func_8028920` is the case where it looks applicable
and is not: the `.26.mach` fixup list showed both entries SImode and **already in
the ROM's order**. The real question was *when* the load is emitted. Read the fixup
list before reaching for `add_minipool_forward_ref`.

**The alias device raises a PRIORITY as well as delaying a ready time.**
`2008d0c` and `2008de8` wanted opposite orderings and the same device delivered
both, which retires the first one's "the pairing is the finding" reading. An
anti-dependence has **link cost zero**, so promoting that same edge to a true
dependence raises the *producer's* priority by the link cost.

**A single adjacent pair can tie on DEPENDENT COUNT, before LUID runs.** On
`common2_254` the stores tie on priority and class and are settled 6-to-5 on
dependent count — and LUID order was already the ROM's, so reaching LUID would have
won. A store of a hard-register argument keeps an anti-dependence on every later
**call** until an explicit set of that register kills `reg_last_uses`.

**A text+data `.s` cannot be elevated in place.** `asm/%.o: src/%.c` builds from
the `.c`, so the blob must move to its own stem first, its `.L` labels must be
exported `.global`, and the layout must gate green alone before the `.c` lands.
The export is not cosmetic: before it, every encoding byte matched and two
relocations were still wrong.

**Per-loop locals decide high-register allocation, and the push list does not warn
you.** Three loops sharing one local set put the counter in r10 where the ROM has
r8 — with a *matching* push list, so the recorded "count your locals" diagnostic
never fired. The tell is in the ROM: different registers for the same role in
different loops means distinct allocnos, which means distinct source variables.

**A pin exact in isolation can be globally harmful.** Pinning both r1 and r2 at one
site forced r4 into the allocation and took a function from 6 differing to **610**.
Probe pins in the real function, never only in a standalone probe.

## Two more parks whose class was wrong

`2008de8` said "pre-reload scheduling" — the pass order here is
`20.ce2 → 23.sched2`, so that names nothing. `Func_8028920` said "pool load order"
when the pool was innocent. Both counts were right. That is now **seven** parks in
three batches with a correct number and a wrong class, against **three** with a
wrong number.

> The recorded count is usually reliable. The recorded *class* is not, and it is
> the class that decides which levers get tried.

## State

| | |
|---|---|
| matched | **4,306** (76.1% of the 5,655 elevatable) |
| remaining, hand-written thumb | 1,349 |
| &nbsp;&nbsp;parked | 459 |
| &nbsp;&nbsp;UNATTEMPTED | 890 |
| park files | 512, resolving to 482 distinct subjects |

Pool went **1,359 → 1,349**, exactly -10.

## Also this batch

**The generated-`.s` tracking contradiction, found and fixed.** `CLAUDE.md`'s Never
list said *"commit compiler-generated `.s` files from `asm/`"*; `docs/elevation.md`
says the opposite and explains why, and the tree carries 3,914 tracked `.s` files
bearing gcc's banner. Following the CLAUDE.md wording through batches 257–259 left
**17 elevated files without their generated `.s`** — every other elevated file in
the tree has one. The 17 are now tracked and the CLAUDE.md line rewritten.

**One stale park retired.** `OvlFunc_899_2008428` is already elevated;
`funcindex` resolved its park to a `.s` holding zero `.thumb_func_start`, which is
the tell. First of the 24 stale park files to be retired rather than counted.
