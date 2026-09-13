# Batch 258 -- ten functions, and three scoring defects that were hiding them

Gated on a clean `make clean && make -j8 && make compare`, green at the target
SHA1 `5c4695205413df7db52b9a184815a07783999971`. Every address below was checked
against the linked ELF with `tools/checkaddr.py`.

| | |
|---|---|
| elevated | **10** |
| parks retired | 6 |
| whole-file conversions | 4 |
| splits | 3 |
| build-input changes | 2 Makefile rules (the sixth and seventh) |
| fakematch debt added | 2 functions |

Nine of the ten came out of a **six-agent round**; the tenth was solo. **Six were
parks** -- functions the corpus had already given up on once -- and **four were
file-mates** picked up for free alongside a named target, which is the sixth round
running that heuristic has paid.

## What landed

| | function | address | source | note |
|---|---|---|---|---|
| 1 | `OvlFunc_968_2009808` | `0x02009808` | [ovl_30_…_a_a_c_b.c](src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c_a_a_c_b.c) | split; fakematch |
| 2 | `GetMercuryDjinni` | `0x080965a8` | [rom_944ec_a_c_a_c_c_c.c](src/rom_8a000/rom_944ec_a_c_a_c_c_c.c) | WHOLE; fakematch |
| 3 | `OvlFunc_927_2009078` | `0x02009078` | [ovl_30_c_c_a_c_a_a.c](src/overlays/rom_7b4558/ovl_30_c_c_a_c_a_a.c) | WHOLE; +`-O2` rule |
| 4 | `Func_801ec6c` | `0x0801ec6c` | [rom_1de5c_…_a_c_c.c](src/rom_15000/rom_1de5c_c_c_c_a_a_c_c.c) | WHOLE (file-mate) |
| 5 | `Func_801ed40` | `0x0801ed40` | [rom_1de5c_…_a_c_c.c](src/rom_15000/rom_1de5c_c_c_c_a_a_c_c.c) | WHOLE |
| 6 | `OvlFunc_905_2008bd0` | `0x02008bd0` | [ovl_30_c_c_a_a_a_c_a.c](src/overlays/rom_799abc/ovl_30_c_c_a_a_a_c_a.c) | WHOLE |
| 7 | `OvlFunc_905_2008ce0` | `0x02008ce0` | [ovl_30_c_c_a_a_a_c_a.c](src/overlays/rom_799abc/ovl_30_c_c_a_a_a_c_a.c) | WHOLE (file-mate) |
| 8 | `OvlFunc_907_2008db4` | `0x02008db4` | [ovl_30_c_c_a.c](src/overlays/rom_79b154/ovl_30_c_c_a.c) | WHOLE (file-mate) |
| 9 | `OvlFunc_907_2008ed8` | `0x02008ed8` | [ovl_30_c_c_a.c](src/overlays/rom_79b154/ovl_30_c_c_a.c) | WHOLE |
| 10 | `Func_80b09fc` | `0x080b09fc` | [rom_b0070_…_a_a_b.c](src/rom_b0000/rom_b0070_a_a_c_c_a_a_b.c) | 3-way split (file-mate) |

## THE THEME: three of these were hidden by a number nobody re-measured

This is the batch's real result, and it is uncomfortable. Six parks closed, and in
**three** of them the recorded score was wrong in a way that had stopped the work.

**1. `park_status.py` read lever prose as a score.** It took the MINIMUM over every
`"N differing"` match anywhere in a park file, so a lever note reading *"Worth 0
differing on its own"* -- a statement that a lever is INERT -- scored that park at
**0**. `GetMercuryDjinni` was selected for this batch as an already-matching
function that merely needed landing. It was 17 differing. The tool's own header
records getting this wrong twice before; this is the third time, and the cause was
the same each time: reading lever prose as a score. Fixed to scan the header block
and skip lines containing worth/on its own/alone/buys/costs/gains.

**2. `tryc.py` normalises pool words, so it scores a pool defect as solved.**
`Func_80b0a20`'s park recorded **1 differing**. `objcmp` says **26, and four bytes
short**. The blocker IS a pool-order defect, so the screen erased exactly the
evidence it was measuring, and `park_status` then inherited the number from the
note. `docs/elevation.md` already documents that `tryc` is blind to pool words;
this is what that blindness costs once a note is believed.

**3. A park asserted the wrong `-O` level as correct.** `OvlFunc_968_2009808`'s
note said *"THIS TU BUILDS AT -O1 and must be screened with `--O1`"* and named the
wrong wildcard. It then built an entire blocker class on the `-O1` output -- a
theory about `sub sp, #8` placement, cross-references to two other parks, three
measured fill orders. At `-O2` the residue is **2**.

> A recorded score is a measurement someone took once, with a tool that had a
> blind spot, under flags they may have inherited. Re-measure before reasoning
> from it. Every agent this round was told to verify the park's number first, and
> in half the cases that instruction is what produced the close.

The corollary bit twice more: `OvlFunc_927_2009078` was parked at **72 of 80**
because `tryc` and `objcmp` both read a mis-scoped `-O1` wildcard. At `-O2` it is
32 and the length is exact; one structural change then closed it. That is the
**inverse** of the documented screens-green/builds-red hazard -- here the screen
was RED and a default screen could never have found it.

## Mechanisms worth keeping

**A `CODE_LABEL` between two constant sets defeats `reload_cse_move2add`.** The
park on `Func_801ed40` said "gcc DERIVES a pooled constant the ROM loads fresh --
there is no lever in that direction". RTL dumps show the constant surviving cse,
gcse, loop, cse2, combine and lreg and dying at `18.greg`, in
`reload_cse_move2add` (`reload1.c:8840`), whose guard is
`reg_set_luid > last_label_luid`. Rewriting the nested `if` as an `else if` chain
puts a join label between the two pool loads; gcc straightens the branches back
afterwards, but the label existed AT RELOAD TIME. First screen, exact. Writing the
`goto` and label explicitly does *not* work -- gcc deletes them in `02.jump`,
before reload. This is spelling-independent, so every "gcc derives / the ROM loads
fresh" park is worth retrying this way.

**Check which allocator pass owns a quantity before computing `QTY_CMP_PRI`.**
`local_alloc` DECLINES a quantity that dies in more than one place, so a
block-local that "dies in 2 places" falls through to `global_alloc` and is ranked
against long-lived quantities it cannot outscore. On `OvlFunc_927_2009078` the
delta needed **eleven more references** to win that auction -- five or six
`__asm__` adjusters. Splitting one twice-assigned `int` into two names does not
win the auction; it **leaves** it. `local_alloc` runs first, takes both halves,
and the ROM's roles fall out for free. Cheaper and stronger than any ref-count
adjustment, and it needs no inline asm. The tell is `.17.lreg`'s "dies in N
places" plus absence from the `;; Register N in M.` list.

**A thumb HImode pool load DISASSEMBLES as `ldr`.** Thumb-1 has no PC-relative
`ldrh`, so GNU `as` encodes gcc's HImode pool load as the identical two bytes of a
word load -- a disassembly cannot tell the two apart, and `tryc` reports
"rom `ldr` / ours `ldrh`" as a difference when the bytes are the same. The park on
`2008ed8` had read `ldr r3, =0` feeding a `strh` as the `const.sym` symbol tell,
reasoning that SImode `0` satisfies constraint `I` and so can never be pooled.
Sound reasoning, false premise. Which form you get is a source question:
`L1d88[0] = 0` gives `mov r3, #0`; `short *p = L1d88; p[0] = 0` gives the ROM's
bytes. Exact inverse of the recorded "`int` intermediate" rule.

**Two levers can each measure WORSE alone and win together.** On `2008bd0` the
synth_mult ladder needs compound assignment (`t <<= 2; t += n`) for the ROM's
destructive two-operand `add`, AND per-arm locals:

| | shared locals | per-arm locals |
|---|---|---|
| expression form | **38** (the parked number) | 40 |
| compound form | 40 | **20** |

A one-at-a-time sweep discards both. That is how it stayed parked.

**A pooled constant can carry a NEIGHBOURING field store's mode.** gcc-2.96
expands an ordinary struct-field store as a mode-typed read-modify-write bitfield
insert, so a halfword field store creates `(set (reg:HI) (const_int 0))` as the
insert's MASK and CSE reuses that HImode zero for an unrelated byte store. The
pooled zero is a leftover mask, not the source's zero -- which is why no spelling
or placement of the visible statement moves it. Confirmed by bisection on
`Func_80b0a20`.

Also: **a flag that fixes MOST of a two-arm diff is a claim about control flow**
before it is a flag candidate (`-fno-gcse` reading 20 -> 5 was gcse commoning
across a block boundary that should not have existed -- nesting the `if` was
20 -> 0); and **reading a global inside the store expression** rather than into a
temp lengthens its live range past the destination's, which is a concrete cure for
the "ROM has more registers" direction (33 -> 8 -> exact).

## A false claim retracted from docs/elevation.md

The file asserted:

> *"Corpus test: 0 of the generated `.s` files contain two consecutive
> `neg rN, rN`. So two `-1` arguments to one call is unreachable for this compiler
> -- not merely unreached. Check that before spending screens."*

Re-measured: **105** such pairs in our current output. A table further down the
same file already said "2 of 2987" and the contradiction had sat unnoticed.
`OvlFunc_905_2008ce0` calls the very function named beside that claim with
`(-1, -1, 0xe666)` and matches exactly. The agent that closed it reported it would
have parked the function on the strength of that paragraph.

> A corpus count is evidence about what the compiler has been ASKED to emit so
> far. "Unreachable -- not merely unreached" converts a sampling observation into
> a proof, and nothing in a corpus licenses that. Where this file calls a shape
> unreachable it must name the mechanism that forbids it, as the
> `move2add`/`CODE_LABEL` and `expand_assignment` entries do.

One related amendment, NOT a retraction: the pool-order section said a HImode
`CONST_INT` is pooled "sign-extended". The sign follows the C **type**, not the
mode. The passage's operative clause -- *the value alone does not tell you the
mode* -- was always correct and is unchanged.

## Not closed

`Func_80b0a20`, the one named target that did not close, is at **16 of 34 under
`-fno-strict-aliasing`** with size and encoding count exact, down from 26 and four
bytes short. The flag is a fifth `ALIAS_CFLAGS` candidate with the cleanest
signature yet: a pointer re-read after a narrow store through that pointer. The
remaining 16 is one thing, read straight out of the `.26.mach` minipool fixup
list -- the `0xffff` entry is SImode in ours and must be HImode. Park rewritten
with the real number and a 30-row measured table.

## Fakematch debt

Two functions added: `OvlFunc_968_2009808` (one `__asm__ volatile ("" ::: "memory")`,
zero instruction cost, listed in `reports/fakematch-worklist.md` with the specific
thing that would retire it) and `GetMercuryDjinni` (two bare register pins). Both
were previously parked, so both are a net reduction in unconverted assembly -- but
it is debt, and seven of the ten landed here needed none.

One catch worth recording: the agent wrote `GetMercuryDjinni`'s pins as
`asm("r5")`. **No tracked file in the tree uses that spelling** -- every one uses
`__asm__` -- and `funcindex.py:real_pins()` only matches the latter, so as written
the pins were invisible to our own tooling. Normalised before landing.

## State

| | |
|---|---|
| matched | **4,290** (75.9% of the 5,655 elevatable) |
| remaining, hand-written thumb | 1,365 |
| &nbsp;&nbsp;parked | 470 |
| &nbsp;&nbsp;UNATTEMPTED | 895 |
| ARM, never elevatable | 51 (excluded -- the ROM is `-mthumb`) |
| park files | 524, resolving to 494 distinct subjects |

Tracked `.thumb_func_start` symbols went **1,375 -> 1,365** across batches 257 and
258, which is exactly -10 for the ten functions elevated. 524 park files resolve
to 494 distinct subjects of which 470 are still in `asm/`, so 24 park files
describe already-elevated functions and are stale.

## Process

Six agents ran concurrently with scratch directories named for their TARGET rather
than their slot -- the fix for batch 257's collision, where two agents overwrote
each other's generic `final.c`. **No collisions this round.** Every agent was
barred from touching `src/`, `asm/`, the Makefile and the linker scripts, and from
running `make`; all ten landings were done here, each gated, with a
`grep -q 'Generated by gcc'` guard on every staged `.s` so no compiler output can
be committed.
