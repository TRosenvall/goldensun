# Batch 238 — levers that are only correct together, and a third false negative

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_947_2009268` | `0x02009268` | [ovl_314_c_a_c_c.c](src/overlays/rom_7d0e88/ovl_314_c_a_c_c.c) |
| 2 | `OvlFunc_947_20093b0` | `0x020093b0` | [ovl_314_c_a_c_c.c](src/overlays/rom_7d0e88/ovl_314_c_a_c_c.c) |
| 3 | `OvlFunc_954_2008db8` | `0x02008db8` | [ovl_30_c_c_c_c_a_a_b.c](src/overlays/rom_7db0c8/ovl_30_c_c_c_c_a_a_b.c) |
| 4 | `OvlFunc_924_200bc48` | `0x0200bc48` | [ovl_35b8_a_a_c_a_c_c_a_b.c](src/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_c_a_b.c) |
| 5 | `OvlFunc_899_2008b48` | `0x02008b48` | [ovl_30_a_c_c_c_a_a_b.c](src/overlays/rom_794ac0/ovl_30_a_c_c_c_a_a_b.c) |
| 6 | `OvlFunc_942_2008e40` | `0x02008e40` | [ovl_30_c_c_c_c_c_b.c](src/overlays/rom_7c6bac/ovl_30_c_c_c_c_c_b.c) |

## Parked

| function | address | file | blocker |
|---|---|---|---|
| `OvlFunc_955_2008160` | `0x02008160` | [2008160.c](src/non_matching/ovl_7ddb88/2008160.c) | `precompute_register_parameters` in a straight-line block + an allocno-priority rotation |

## THREE LEVERS THAT ARE ONLY CORRECT TOGETHER

`2008e40`'s last three levers were measured as a full 2×2×2 rather than one at
a time:

| r | s | one | differing |
|---|---|---|---|
| – | – | – | 21 |
| ✓ | – | – | **156** |
| – | ✓ | – | **159** |
| – | – | ✓ | 19 |
| ✓ | ✓ | – | 2 |
| ✓ | – | ✓ | 156 |
| – | ✓ | ✓ | 159 |
| ✓ | ✓ | ✓ | **0** |

A one-lever-at-a-time sweep reports two of the three as ACTIVELY HARMFUL and is
wrong about both. The notebook records the subtractive mirror of this
("individually-inert pins are not jointly removable") and the cancellation case
("a change that helps and one that hurts cancel"). This is non-additivity in
ADDITION, which is the case that costs you the function, because the natural
response to +135 differing is to revert.

Mechanism: `r` and `s` are two halves of ONE statement about live ranges. The
ROM keeps one actor pointer live across three re-reads, so the pointer that is
not re-read must be a different pseudo from the one that is. Either half alone
hands local-alloc an inconsistent assignment, and it spends a call-saved
register to resolve it.

## A PIN SET CAN NEED A HOLE, AND THE HOLE IS THE POINT

`2008db8` takes 18 pins from 92 candidate sites — and the five
`__MapActor_SetPos` calls must be left BARE. That region is where the ROM
ITSELF commons, so pinning there destroys the CSE the ROM performs.

The notebook records this polarity for NAMING ("name the ones gcc should NOT
hoist"). The pin form, at region granularity, was not recorded.

Nothing in this function is diagnosable one lever at a time:

| spelling | differing |
|---|---|
| no pins at all | 396 |
| the five SetSpeed pins ALONE | **401 — worse than plain** |
| all 92 pins | 347 |
| 87 pins carved, halfword bare | 321 |
| final: 18 pins + carve + typed halfword | **0** |

It confirms the recorded "a pin set that gets worse is not evidence pins are
wrong — extend it", and adds the second half: extend, THEN carve out the ROM's
own CSE region.

## NO STRUCT TAG SUBSTITUTES FOR `-fno-strict-aliasing`, AND THAT BOUNDS A LEVER

`20093b0` was screened in batch 235 and left at ~17 differing on what that
screen called pure load scheduling, with a note that neither source ordering
nor either scheduler flag reaches it. The premise was right; the cause was not.

Deleting three `int` locals takes 20 differing to 8 — the recorded "naming a
value gcc already carries destroys the carry", in a form that reads as a
scheduling problem. Thumb cannot `ldr` into a high register, so reload emits
`ldr rLOW / mov rHIGH, rLOW` and sched2 pulls the load into the previous
component's gap IF the scratch is free. With the locals, reload hands it r3 —
the register the position chain is already using — and nothing can move.

`-fno-strict-aliasing` takes the remaining 8 to matched. `-fsched-verbose=8`
reads out why: the store and the load TIE on priority, `reload_completed` has
killed the pressure test, and `rank_for_schedule` falls through to dependent
count — 5 against 4 — so the load wins. The missing dependent is the memory
dependence strict aliasing deletes.

**No struct-tag spelling substitutes, and this bounds a recorded lever.** Alias
sets can only REMOVE dependences — `true_dependence` consults them before
`memrefs_conflict_p` — and two `(plus (reg) K)` addresses are provably distinct
regardless. So "give each store its own struct tag" buys SEPARATION, which is
the opposite of what this wants; measured inert. The only source-level escape
is `volatile` on BOTH sides of each pair, which short-circuits above the alias
test. That spelling is also byte-identical and is recorded in the file, but the
flag is the honest description and gets the Makefile rule.

The file's other function was re-measured UNDER the flag rather than assumed
unaffected. Still exact — which is the check that makes a per-TU flag rule
safe, since a TU takes one flag group.

## objcmp: A THIRD FALSE NEGATIVE, AND THE OBVIOUS FIX WOULD HAVE BEEN A FALSE POSITIVE

`_alias_addrs` built ONE flat name→address map for the whole tree, main ROM
first, filled with `setdefault`. Every overlay that divides carries
`__divsi3 = _divsi3_RAM;` in its own `.ld` — but the flat map had already bound
`__divsi3` to the main ROM's copy, and `setdefault` never let the overlay
rebind it. So **every overlay function that divides reported
`XX RELOCATIONS differ`, with SIZE and ENCODINGS silent, on a byte-identical
object.**

That is the third shape of "objcmp can report a byte-identical match as
failing", and the worst-looking: it is indistinguishable from a real park.

The obvious fix is wrong. Pooling all addresses per name and asking whether the
sets intersect would find the alias — and would also declare two unrelated
symbols one symbol, because **overlays share address space**. Keeping one map
PER ELF asks the only question worth asking: did some single link resolve both
names to one address?

Tested four ways. `__divsi3 ~ _divsi3_RAM` now true; `__divsi3 ~ __modsi3`
false; an absent name false; and a REAL cross-overlay collision found by
search — `__start_overlay` and `_OvlFunc_879_2008054`, both at `02008000` in
different overlays — correctly false. That fourth test is the one the
flat-with-sets version would have failed.

**Audited for false parks, which was the reason to care: none.** No park in
`src/non_matching/` states relocations as its blocker. Of the ten mentioning
`divsi3` at all, eight mention it in passing; `rom_f2000/80f2ebc.c` is blocked
on it CORRECTLY — it is a main-ROM function, one link, 110 asm files reference
`__divsi3` directly, so the overlay remedy genuinely does not transfer — and
`ovl_793768/2009754.c` is blocked on scheduling for 6 of its 8 differing.

That last park records an owed item this fix does NOT close:
`overlays/rom_793768/overlay.ld` and `overlays/rom_7bc690/overlay.ld` are both
missing the alias their own `imports.s` already exports.

## "A POINTER-RETURNING CALL WHOSE RESULT DIES IMMEDIATELY MUST NOT BE NAMED" IS TOO STRONG

The anonymous form is 580 differing in `2008e40`, while the recorded
`p += K; *p op= C;` — which keeps the name and still consumes the pointer — is
exact. What the ROM asks for is CONSUMPTION, not anonymity.

## ROM ADJACENCY DOES NOT CARRY A SPELLING OVER

`200bc48`'s decisive template was the function 116 bytes earlier in the same ROM
region, which handed over the struct and the prototype verbatim — and whose
bias placement is the OPPOSITE. It shifts then subtracts; this one subtracts
inside the parenthesis and shifts the difference. 48 differing for the wrong
choice, and four of six relocations move.

The 0.80-scored template offered by `templated.py` could not have expressed the
function at all: its struct pads over the two offsets this ROM stores to. The
score ranks callee-set overlap, which is a good filter and not a layout check.

## Two `.data` lines for sections that do not exist

Two of this batch's four splits had an `overlay.ld` `.data` entry for an object
with no `.data` section, and both had to be remapped anyway — the recorded
"remap EVERY section, not the one that errors" trap, twice in one batch.
`split_s.py` handles both sections itself; the trap is only live when a split is
done by hand.

## The park

`2008160` reaches 17 differing of 107 with the instruction stream 1:1 against
the ROM — only placement differs — and both residues trace to a named pass.
Thirteen lines are an allocno-priority rotation, confirmed causal rather than
guessed: deleting two stores from inside the losing allocno's live range flips
the register to the ROM's. Four lines are
`precompute_register_parameters` copying arguments whose `rtx_cost > 2` ahead of
the hard-register load, in the entry block with no dominating branch — the
documented straight-line argument-interleave blocker. Thirty-one flags, nine
return-type variants and the full spelling sweep are recorded in the park.

One operational note from it: `asm/**/*.s` sitting beside a solved `src/**/*.c`
is **gcc's own output for thousands of solved TUs, and it is greppable**. Scanning
it for the residue's shape returned 20 hits and produced the templates that
supplied the working lever. A `solved_twins.py` zero is not the end of the
search.
