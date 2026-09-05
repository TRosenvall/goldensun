# Batch 220

Five elevated, one parked, one existing park deepened — and **one park that
turned out to be wrong**. The batch's theme is that several residues came down
to *which C variable holds which value*, or to a type or mode the source never
names explicitly, rather than to any expression shape.

## Function breakdown

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_957_200ba30` | `0x0200ba30` | [ovl_30_c_c_c_c_b_b.c](src/overlays/rom_7e3e08/ovl_30_c_c_c_c_b_b.c) |
| 2 | `OvlFunc_956_20093c0` | `0x020093c0` | [ovl_30_…_a_c_a_b.c](src/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_a_b.c) |
| 3 | `OvlFunc_971_2008d68` | `0x02008d68` | [ovl_30_…_c_c_b.c](src/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_c_c_b.c) |
| 4 | `Func_8098a84` | `0x08098a84` | [rom_97b54_a_c_a_c_c_b.c](src/rom_8a000/rom_97b54_a_c_a_c_c_b.c) |
| 5 | `Func_809a3c4` | `0x0809a3c4` | [rom_97b54_c_c_c_b.c](src/rom_8a000/rom_97b54_c_c_c_b.c) |

Parked: `OvlFunc_888_200b1b8` (79 of 75, one extra callee-saved register).
Deepened: `OvlFunc_879_2008454`, from 66 differing to one extra pool dump.

Gated on a clean `make clean && make compare`, every address verified against
the linked ELF with `tools/checkaddr.py`. Every function was verified with
`tools/objcmp.py` before the build was touched, and integrated **one at a
time**.

## A PARK'S CONCLUSION WAS WRONG, BY ONE TOKEN

`Func_809a3c4` sat parked at 16 of 62 asserting the ROM's hoisted pool load was
unreachable:

> gcc prices `mov`+`lsl` (two instructions, no pool entry) below a pool load,
> and nothing in the C reaches that decision.

Three placements of the step constant had been measured — hoisted to the entry
block, above the guard, inside it — all tying at 16. **All three named it**, and
that was the whole error:

    int step; ... step = 0x2000; ... *(u16 *)(p + 6) += step;   16 of 62
    *(u16 *)(p + 6) += 0x2000;                                  EXACT

A NAMED loop-invariant is cheap for gcc to rematerialise per iteration, so it
does. An UNNAMED one becomes a pool reference, and `loop.c` hoists pool loads
out of loops. The park's reasoning about instruction pricing was correct and led
to the wrong conclusion, because that choice is not made until *after* the
constant has become a pool entry — and whether it does is decided by naming.

**How it was found matters more than the fix.** Its twin `Func_8098a84` was
elevated first in the same batch, and its nine measured spellings made the
pattern visible. A park is a hypothesis; the cheapest thing that can refute one
is a sibling landing.

Two consequences for how parks should be read:

  * **A park resting on a `tryc` score alone has not been checked.** Both twins
    screen DIRTY at 11 differing — gcc emits an extra label at the same address
    as another, and the whole tail of the diff is one phantom line shift. Only
    `objcmp` shows them exact.
  * A park that measured N spellings has only ruled out what those N shared.
    Here all three shared "the invariant must be named".

## THE VARIABLES *ARE* THE ALLOCATION

`OvlFunc_956_20093c0`'s entire residue was which C variable held which value.
The ROM's `mov r5, r0` after the first call is not scheduling: global-alloc
gives one pseudo a callee-saved register because THAT PSEUDO's live range
crosses calls, even though its first sub-range dies immediately.

Writing two logically-distinct reads as the **same** variable, with a third
re-read as a separate short-lived one, produced both the copy in the first block
and no copy in the second. The other way round costs the copy and comes out a
line short (74 differing); naming a counter temp to force it only *relocates*
the copy (49). Pinning is much worse (84 of 86).

gcc-2.96 has no live-range splitting, so **which C variable holds which value is
directly observable in the register class**. That makes variable identity a
first-class lever, not a stylistic choice.

## A SHARED *MIDDLE* MUST BE SHARED IN SOURCE

This is the counter-case to batch 219's "let gcc cross-jump a shared tail".

On `OvlFunc_971_2008d68`, writing both arms out in full lets gcc
constant-propagate a message id and it never produces the ROM's register phi at
all (62 differing). `jump.c` merges *identical* tails; a shared **middle**
carrying a **differing constant** is not identical and cannot be produced. Write
it once.

And one `goto` places the last arm's early return after the shared block. With
both inline, gcc cross-jumps the two *early returns* together and leaves the
shared block its own tail copy (54). **Which of two candidate pairs gets merged
is decided entirely by source block order.** Note the `goto` here is on the
UNSHARED arm, which is what keeps this consistent with the tail rule.

## A BYTE-WIDE `-1` IS A MODE ARTEFACT

On `OvlFunc_957_200ba30`, with a `char *`, `*p = *p - 1` compiles the `-1` in
QImode and gcc canonicalises it to `add r3, #0xff` where the ROM has
`sub r3, #0x1`. **Four syntactically distinct decrements** — `*p = *p - 1`,
`(*p)--`, `*p -= 1`, `*p = *p + -1` — all lower to the same QImode `plus -1` and
all give the same wrong instruction. An `int`-typed name for the loaded byte
forces SImode and yields `sub`.

The **asymmetry** is the informative part: the increment needs no such
treatment, because `+1` survives QImode intact. So this is not "narrow
arithmetic needs a name" — it is specifically that a negative addend is
canonicalised and a positive one is not.

## AND ONE FINDING THAT WAS NOT ONE

The epilogue tell — `pop {r1} / bx r1` rather than `pop {r0} / bx r0` meaning
the return value is live — was reported as new and **is already on file**, in
`src/non_matching/rom_15000/8019908.c`, which calls it "a reliable void/non-void
tell", and used by a second park. It is recorded here as confirmation on a third
function, not as discovery. Checking the parks for a tell before writing it up
costs one grep.

## THE PARK, AND A DEEPENED ONE

`OvlFunc_888_200b1b8` sits at 79 of 75, and **all four extra lines are one extra
callee-saved register** — two in the prologue, two in the epilogue. gcc parks
the map actor in `sl` and pays an extra copy; the ROM keeps it in r6 and covers
five values with four registers by **sharing one between two values of
different types** whose ranges never overlap.

Six spellings, none moving the length. Two are informative: pinning the actor to
the very register the ROM uses makes the function LONGER (the batch-210 drop
hazard again — a pin cannot ask for an allocation that spans a call), and naming
a mask constant put the CONSTANT in a callee-saved register where the ROM uses a
scratch. The park names the shared assumption: every attempt varied *how values
are declared*, none changed the *set* of values that must be live.

`OvlFunc_879_2008454` went from 66 differing to a single extra pool dump, on a
new lever: **a pin on the CALL-CLOBBERED argument register defeats a
constant-address CSE**, because gcc `force_reg`s a call argument into a pseudo
that CSE then commons, whereas a hard argument register is invalidated by the
call. Its remaining 4 bytes are code, not pool — both objects hold sixteen pool
words.

## PROCESS

Two class-level parks were checked and ruled out rather than left as open leads:
`preheader_load_merge.c` is a different mechanism from the naming finding, and
`tiny_reg_order.c` already records statement order, named locals and
constant-as-destination as tried on all five of its functions. Its opening
argument is worth re-reading before any round that plans to "do the small ones
first" — at five to ten instructions there is no structure left to get right,
and arrangement is the whole function.
