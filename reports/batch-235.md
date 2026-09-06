# Batch 235 — a split that had no more reason to exist

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `Func_801b4ec` | `0x0801b4ec` | [rom_1aeec_…_a_b.c](src/rom_15000/rom_1aeec_a_a_c_a_c_c_a_b.c) |
| 2 | `OvlFunc_882_200c0f0` | `0x0200c0f0` | [ovl_30_c_c_c_c_a_a_c_c_c.c](src/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_c_c_c.c) |
| 3 | `OvlFunc_952_2008af8` | `0x02008af8` | [ovl_30_c_a_a_c_c_c_c_c_a_a.c](src/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c_c_c_a_a.c) |
| 4 | `OvlFunc_955_2009898` | `0x02009898` | [ovl_30_c_c_c_c_c_c_c_a.c](src/overlays/rom_7ddb88/ovl_30_c_c_c_c_c_c_c_a.c) |
| 5 | `Func_80b0574` | `0x080b0574` | [rom_b0070_a_a_c_a_c_a_c.c](src/rom_b0000/rom_b0070_a_a_c_a_c_a_c.c) |
| 6 | `OvlFunc_925_200af18` | `0x0200af18` | [ovl_314_c_c_c_a_c_c_c.c](src/overlays/rom_7b0400/ovl_314_c_c_c_a_c_c_c.c) |
| 7 | `OvlFunc_925_200b060` | `0x0200b060` | [ovl_314_c_c_c_a_c_c_c.c](src/overlays/rom_7b0400/ovl_314_c_c_c_a_c_c_c.c) |
| 8 | `OvlFunc_965_2009b10` | `0x02009b10` | [ovl_30_a_c_c_c_c_c_c_a_a.c](src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c_c_a_a.c) |

Entries 6 and 7 landed here behind a split. Batch 237 collapses that split and
the file above is where they live now — the path in this table is the current
one, not the one the landing commit created.

## A SPLIT IS SCAFFOLDING, AND SCAFFOLDING COMES DOWN

`OvlFunc_965_2009b10` is 914 instructions, 2396 bytes, 925 encodings — the
second of the two functions the batch-231 split left behind. The split existed
for exactly one reason: so its file-sibling `OvlFunc_965_2009238` could land
while this one was still assembly. With both elevated that reason is gone.

So the landing is not "one more function in the directory". It is one
translation unit byte-identical to the WHOLE original two-function `.s` — 4660
bytes, 1791 encodings, 519 relocations — and the two `overlay.ld` lines fold
back into the single line they replaced.

`objcmp` could not have proved that on its own: its `--func` cannot isolate a
function inside a multi-function candidate, so it compares a one-function
reference against a whole compiled candidate. The proof is a whole-object
comparison (`collapse.py`), and the collapse check has to be written before it
can be trusted.

## A STALE FLAG WARNING, CAUGHT BY SCREENING THROUGH THE REAL `asm/` PATH

The sibling's header says landing this function "requires a fourth such
override" for the `rom_7ef4f4/ovl_30_a_c_c_c_c_c%` `-O1` wildcard. That is
false, and it was false when written: commit `fe6b158f` narrowed the wildcard
at source **in the same batch** instead of adding the override. The note
describes a proposal that was never taken.

What caught it was screening the candidate through the real `asm/` path as well
as the scratch copy. The two agree exactly, which is the check that would have
caught the trap originally, and it costs one extra command.

## THE LENGTH TELL FIRED SHORT HERE AND LONG ON THE SIBLING

Plain C is 2356 bytes against the ROM's 2396 — **forty bytes short**, because
the constants `cse_main` commons are worth more than the three-register
prologue costs. The sibling, same family and same file, measured **+4 long** on
exactly this transformation.

Two of the recorded tell's three outcomes, on two functions of one family in
one file. Length is not evidence. Read the diff text.

## THE FIRST-USE PIN RULE IS ABOUT REGIONS, NOT ABOUT THE FUNCTION

`0x80 << 6` has four uses — sites 128, then 168, 191 and 197 — with an
if/else diamond at site 155 between the first and the rest.

| spelling | result |
|---|---|
| pin 168 only | exact |
| pin 128 only | 307 differing, `push {r5,lr}` |
| pin both | exact — so 128 is inert scaffolding, dropped |

The recorded reversal of this polarity was a pure ORDERING pin either side of a
join, where neither site had CSE work to do. This one is the opposite: the pin
at 168 is doing real constant-CSE work for the group 168/191/197, while the
lone use at 128, upstream of the diamond, needs nothing.

So the rule reads **pin the first use IN EACH REGION the diamonds cut the
function into** — and the earliest use in the function may not be in a region
that needs a pin at all.

## The sweep, and what it cost

76 pin candidates, **65 required**. Every site stripped individually under
`objcmp`, greedily, re-testing after each drop, run to a fixpoint FROM BOTH
ENDS — both directions surviving the same 65. Eleven fall. Eight of those are
the recorded "one pin at the first use covers the later ones"; two are the
region reversal above; and one is the plain "or nothing" job — `0xa0 << 7` has
two uses in one region and gcc rematerialises it unaided. Being repeated and
expensive makes a value a CANDIDATE, not a requirement.

Ten-row measured-worse table in the file header. The two rows that matter most
are the two that are not close: the ROM's own emitted argument order, not
ascending, is 94 differing; and no pins at all is 834.

## Method note

The other seven functions are written up in their own headers and in the
commits that landed them. The one thing worth lifting out is `Func_80b0574`,
which `objcmp` reported as differing and `make compare` proved byte-identical —
the authority tool is the build, and the batch gate exists because screens lie
in both directions.
