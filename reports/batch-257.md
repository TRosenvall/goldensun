# Batch 257 -- eight functions, five overlays, and a flags bug that read as a hard park

Gated on a clean `make clean && make -j8 && make compare`, green at the target
SHA1 `5c4695205413df7db52b9a184815a07783999971`. Every address below was checked
against the linked overlay ELF rather than against the source.

| | |
|---|---|
| elevated | **8** |
| parked | 2 |
| build-input changes | 2 (one Makefile rule, one `area.sym` symbol) |
| splits | 2 |
| whole-file conversions | 3 |

## What landed

| | function | address | source | landing |
|---|---|---|---|---|
| 1 | `OvlFunc_890_2009510` | `0x02009510` | [ovl_30_…_a_c_c.c](src/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_a_c_a_c_c.c) | file WHOLE |
| 2 | `OvlFunc_890_2009790` | `0x02009790` | [ovl_30_…_a_c_c.c](src/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_a_c_a_c_c.c) | file WHOLE |
| 3 | `OvlFunc_944_2008564` | `0x02008564` | [ovl_30_…_a_c_c.c](src/overlays/rom_7ca63c/ovl_30_c_c_a_c_c_a_c_c.c) | file WHOLE |
| 4 | `OvlFunc_944_20087b0` | `0x020087b0` | [ovl_30_…_a_c_c.c](src/overlays/rom_7ca63c/ovl_30_c_c_a_c_c_a_c_c.c) | file WHOLE |
| 5 | `OvlFunc_968_2009218` | `0x02009218` | [ovl_30_…_c_a_c.c](src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a_c_c_a_c.c) | WHOLE + explicit `-O2` rule |
| 6 | `OvlFunc_884_2009274` | `0x02009274` | [ovl_30_…_a_c_a.c](src/overlays/rom_784360/ovl_30_c_a_c_c_c_a_c_a.c) | split `_a`, + `_AREA_05` |
| 7 | `OvlFunc_884_20095b4` | `0x020095b4` | [ovl_30_…_a_c_a.c](src/overlays/rom_784360/ovl_30_c_a_c_c_c_a_c_a.c) | split `_a` |
| 8 | `OvlFunc_905_2008ecc` | `0x02008ecc` | [ovl_30_…_a_a_c_b.c](src/overlays/rom_799abc/ovl_30_c_c_a_a_a_c_b.c) | split `_b` |

Whole-object compares, each measured 3x after the merge:

    rom_78b2ac  OK WHOLE OBJECT -- 1352 bytes, 508 encodings and 152 relocations identical
    rom_7ca63c  OK WHOLE OBJECT -- 1312 bytes, 508 encodings and 135 relocations identical
    rom_7f2f14  OK WHOLE OBJECT --  732 bytes, 300 encodings and  57 relocations identical
    rom_799abc  OK WHOLE OBJECT --  444 bytes, 185 encodings and  31 relocations identical

`rom_784360` is the one exception and it is the expected one: ONE differing
encoding and ONE ours-only relocation, `ref 00000005 ours 00000000`, 144
relocations against 145. That is the recorded `_AREA_*` symbol tell -- the pool
word is the unrelocated placeholder -- and `make compare` is the authority. It
is green.

Both `asmfacts.py: 2 functions  split first` verdicts on the whole-file cases were
SUPERSEDED. That verdict is its answer for solving only the FIRST function; with
both closed the `.s` is replaced outright, the basename stays, and the
`overlay.ld` line stays verbatim with its `asm/` prefix.

## The result worth carrying forward

**A 157-instruction residue was a flags bug.** `OvlFunc_968_2009218` is the fifth
function caught by the mis-scoped `rom_7f2f14/ovl_30_c_a_c_a_c_a%` wildcard, which
applies `O1_CFLAGS`. At -O1 the finished source is 157 of 300 differing; at -O2 it
is exact.

> The size of an -O1 residue carries NO information about whether the cause is
> codegen or flags.

The failure mode is silent in the wrong direction: **the object screens GREEN
under `tools/tryc.py` and builds RED**, because the screen reads flags belonging to
a neighbouring TU that only shares a name prefix. Four sibling rules already
existed; this is the fifth, and one of the earlier five (`2009150`) had been
*parked* at 14 differing for the same reason. The wildcard is still not narrowed --
an explicit rule beats it without disturbing whatever genuinely needs -O1.

## Four more mechanisms, all recorded in docs/elevation.md

**gcc-2.96 never synthesizes a constant multiply at this tree's flags.** The ROM's
`n * 6553` is the classic `synth_mult` ladder; our gcc emits `ldr r3, =6553 / mul`.
Probed directly, and `n * 13` behaves the same, so it is not a threshold effect.
The ladder returns byte-for-byte only when the shifts and adds are written out in
the source. So a `mul` against a pooled constant where the ROM has a `lsl`/`add`
chain is a **source-level question, not a codegen one** -- a distinctive signature
worth re-screening existing parks against, since any park showing it is misfiled.

**One more callee-saved register than the ROM is a live-range symptom first.**
`OvlFunc_944_20087b0` opened `push {r5, r6, r7, lr}` against `push {r5, r6, lr}`,
with the ROM spending r5/r6/r8/r10 and genuinely skipping r7 -- the recorded shape
for `-ffixed-r7`. The flag measured **268 -> 45, size exact**. It was still wrong.
One local held the results of two `__MapActor_GetActor` calls, so its live range
spanned the function; giving the first call its own local is **45 -> 6 and drops r7
with no flag**. At the finished source the flag is INERT. This was one step from a
Makefile rule justified by a number good enough to ship.

**Transfer a file-mate's idiom, then re-derive its comparison.** The file-mate
heuristic paid twice more, but `OvlFunc_905_2008ecc` shows where verbatim is
exactly wrong: the sibling's `if (x <= (0 - 1))` compiles to four instructions and
a callee-saved register against the ROM's `cmp r3,#0 / bge`. Writing `if (x < 0)`
went **170 -> EXACT**.

**`local_alloc` runs before `global_alloc`, so block-local constants outbid
long-lived locals.** A stack-argument value in r8-r11 with a `mov` before every
`str rN,[sp]` reads as an ordering problem and is an allocation one. Giving the
pointer and two constants one variable each *spanning the branch* makes them
global allocnos; worth 292 -> 251 with prologue and epilogue becoming exact. Read
out of `.17.lreg`/`.18.greg` rather than inferred.

## Parked

* **`OvlFunc_954_2008540`** (`0x02008540`) -- 42 of 302 **at the ROM's exact length**, exact push
  mask, relocation order and pool order. A coupled pair that cancels in the
  length: the ROM spills the first actor to `[sp,#8]` where gcc has a genuinely
  free callee-saved register, and `spd` gets a pre-branch home write the ROM does
  not make. An exact length with a large differing count is a candidate for a
  cancelling pair, not evidence of being close. Discriminator recorded (+0-cost
  pre-branch reference for the 0xcccc allocno); still carries one ref-count
  adjuster as scaffolding, which is why it is parked rather than shipped.
* **`OvlFunc_905_2008bd0`** (`0x02008bd0`) -- 38 of 121, on a register-role alternation inside the
  now-correct multiply ladder. `OvlFunc_905_2008ce0` was not started and its tail
  is the same block; closing **both** would turn the rom_799abc landing from a
  split into a WHOLE conversion, so they are worth attempting together.

## Two build-input changes, both with precedent

**`Makefile`** -- a fifth explicit `-O2` rule, modelled on the four above it.

**`area.sym`** -- `_AREA_05 = 0x5;`. This one was nearly got wrong and the note
beside the symbol says why. `area.sym`'s header grounds the low ids (0x00, 0x02,
0x04, added in batch 68) in `__SetDestMap` destinations, and 0x05 is **not** a
`__SetDestMap` destination -- it reaches `__Func_8091f90`, a third consumer. The
checkable claim is about the *callee's signature*, and the census settles it:
seven elevated files already pass an area id to `__Func_8091f90`, one declares it
`void __Func_8091f90(int id, int b)`, and the ROM shape here (`ldr r5, =5` /
`mov r0, r5`) is the shape of `ldr r5, =0xbb` / `mov r0, r5` at
`rom_7f6e64/ovl_314_c_a_c_c_a_c_c.s:1372`, whose `_AREA_bb` is already in the
table.

## A tooling correction

`tools/funcindex.py:real_pins()` **undercounts when pins hide behind a macro.** It
counts `register ... __asm__("rN")` DECLARATIONS, so a file using

    #define PIN1 register int q0 __asm__("r0")

reads as exactly 4 pins whether the macros are invoked once, two hundred times, or
never. `rom_7ca63c` showed `real_pins() == 4` with **zero** pins inside either
function body -- it pins through 20+ macro invocations. For a fakematch decision
the question is whether the macros are *invoked*. The comment-stripping in
`real_pins()` remains correct and necessary; it is the macro indirection that
defeats the count. Five fakematch rows were added this batch on the corrected
check.

## State

| | |
|---|---|
| matched | **4,280** (75.7% of the 5,655 elevatable) |
| remaining, hand-written thumb | 1,375 |
| &nbsp;&nbsp;of which parked | 478 |
| &nbsp;&nbsp;of which UNATTEMPTED | 897 |
| ARM, never elevatable | 51 (excluded -- the ROM is `-mthumb`) |
| park files | 530, resolving to 500 distinct subjects |

**Correction.** An earlier draft of this table said "948 unattempted" and "75.0%".
Both were wrong: the 948 included the 51 ARM functions, which cannot have come
from C at all and are not work. The figures above exclude them.

Two older carried-forward numbers do not survive checking either, and are
retired here rather than propagated. "650 parked" was never a park-file count --
`src/non_matching` held 518 files at batch 254, 528 at the start of this round and
530 now, so it was most likely an over-count that included callees named inside
park prose, which is a mistake this log has recorded once before. "694
unattempted" plus that 650 sums to 1,344 against a measured 1,383 remaining at the
same commit, so the split was wrong even where the total was close.

The total itself is healthy and is the number to trust: **tracked hand-written
`.thumb_func_start` symbols went 1,383 -> 1,375 across this batch, exactly -8 for
the eight functions elevated.** Splits move functions between files and create
none, so the pool only ever shrinks by what is elevated.

The park-file arithmetic is worth reading once: 530 files, 500 distinct subjects,
478 of those still in `asm/`. So **22 park files describe functions that have since
been elevated** and are stale, and 30 files share a subject with another file.
`tools/funcindex.py --park` resolves all 530 with no unresolved entries, so this is
now a cheap standing check rather than a survey.

## Process note

Three agents ran concurrently and two of them wrote into the same
`scratch_elev/b259/a2/` and `a3/` paths with generic filenames (`ref.s`, `gen.py`,
`final.c`, `v1.c`), overwriting each other's working files mid-run. Nothing was
lost -- both agents noticed, moved to suffixed directories and re-verified from
there, and every result above was re-measured 3x by hand before landing. The
lesson is cheap to apply: **a concurrent agent's scratch directory must be named
for its target, not its slot.**
