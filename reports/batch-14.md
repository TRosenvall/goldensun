# Batch 14 — 8 functions, and a 17-member family reduced to one register

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–13 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean build, unassisted. Every address read back from the linked overlay
ELFs.

## The functions

A seven-member three-message prompt family, complete:

| Function | Address | New source |
|---|---|---|
| `OvlFunc_887_2008118` | `0x02008118` | `src/overlays/rom_787e04/ovl_30_c_a_a_c_c_a_a.c` |
| `OvlFunc_951_2008074` | `0x02008074` | `src/overlays/rom_7d6418/ovl_30_c_c_a_a_a_c_b.c` |
| `OvlFunc_951_20080bc` | `0x020080bc` | `src/overlays/rom_7d6418/ovl_30_c_c_a_a_a_c_c.c` |
| `OvlFunc_961_2008068` | `0x02008068` | `src/overlays/rom_7ebdfc/ovl_30_c_c_a_c_b.c` |
| `OvlFunc_961_20080b0` | `0x020080b0` | `src/overlays/rom_7ebdfc/ovl_30_c_c_a_c_c.c` |
| `OvlFunc_962_20081d4` | `0x020081d4` | `src/overlays/rom_7ec19c/ovl_30_c_a_c_b.c` |
| `OvlFunc_966_20080c4` | `0x020080c4` | `src/overlays/rom_7f148c/ovl_30_c_c_a_c_b.c` |

And one single:

| Function | Address | New source |
|---|---|---|
| `OvlFunc_936_2009ea4` | `0x02009ea4` | `src/overlays/rom_7c097c/ovl_30_c_c_c_a_c_c_c_b.c` |

`message.sym` gains 7 entries — these are `__MessageID` operands, so they belong
there rather than in `unknown_id.sym`.

## The prompt family needed both declaration rules at once

Each stub says an opening line, runs a check, and delivers one of two follow-ups
at `base+1` or `base+2`.

**The base has to be a symbol.** The ROM holds it in `r5` across the whole
function and computes the follow-ups with `add r0, r5, #1`. Written as a
literal, gcc folds each use into its own pool load and the function comes out an
instruction short. Only a link-time address survives as a register value gcc
will add to.

**And one callee must stay undeclared.** The ROM fills `__Func_8092c40`'s `r0`
last, which is the implicitly-declared shape from batch 07. Adding a prototype
flips the pair and costs the match.

That rule has now been used in **both** directions — adding a prototype to get
`r0` first, withholding one to get `r0` last. Batch 07 only established the
first half.

## A signedness detail that would have compiled either way

`OvlFunc_936_2009ea4`'s `lsl r3, r0, #2 / add r3, r0 / lsr r3, #12` is a
multiply by five and a **logical** right shift, so the intermediate is
`unsigned`. An `int` emits `asr` and silently changes the result for a negative
return from `__Random`.

Same class as the signed comparison noted in batch 06: the C reads as
equivalent, the machine code does not, and only one of them is what the game
does.

## `FindEntityAtPosition` — 17 functions behind one register exchange

The largest unsolved family in the overlays. Parked, but much closer than that
sounds, and worth someone's fresh eyes.

Splitting the table's *address* from its *dereference* reproduces the ROM's
prologue exactly:

    rom    ldr r3,=0x3001ebc / mov r4,r0 / ldr r2,[r3] / ldr r3,[r4] / mov r1,r2
    ours   ldr r3,=0x3001ebc / mov r1,r0 / ldr r2,[r3] / ldr r3,[r1] / mov r4,r2

Forty instructions against forty, same instructions in the same order, loop body
identical. **The only difference in the entire function is that `r4` and `r1`
are exchanged** between the position argument and the table pointer. Both are
live across the whole loop, so it is purely which pseudo the allocator sees
first.

Six formulations have failed, including declaration order and copying the
argument into its own local. Neither reaches the allocator once the statement
shape is right. Detail in `src/non_matching/ovl_780898/200806c.c`.

### Its annotation is wrong, and all 17 share it

    r0 = an {x, y, z} triple, r1 = the entity to skip (the caller itself).

`r1` is overwritten by `mov r1, r2` before it is ever read. There is no second
argument. This is exactly the failure `docs/attribution.md` records — mechanism
right, purpose wrong — and anyone attempting any of the seventeen will read that
line first and write a two-argument function.

The rest of the annotation is good, including a real observation: the axis
comparison is deliberately mismatched, x and z at whole-tile resolution
(`asr #20`) but y at 1/16 (`asr #16`), which is what stops an object on a ledge
blocking one on the floor below.

## A lever that does not generalise

`OvlFunc_common2_254` is parked. Naming an intermediate — which solved a folded
byte offset in batch 12 and a split `mov`/`lsl` pair in batch 10 — makes this
one **worse**: taking `&p` and `&q` into locals to force the ROM's
pointer-register addressing produced 29 instructions against 23 and spilled
`r8`.

So that technique applies to *computed values*, not to *stack-object addresses*.
Recorded so it is not tried a fourth time on the wrong shape.

Note also that this TU drops `-mthumb-interwork` — all fourteen `common2`
functions return `pop {pc}` rather than the `bx` form, and there is a Makefile
rule for the whole stem. A screen at default flags is meaningless there.

## Still open, and still only answerable by you

- **Is `MapEntrance` your name for those tables, and is the index an area id?**
  (batch 13) One line retires `unknown_id.sym` and its ~55 placeholder entries.
- **Five ambiguous offsets in `actor.h`** (batch 03), documented rather than
  guessed.

## Reproducing the verification

    docker build -t goldensun-build -f tools/Dockerfile .
    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        sh -c 'make clean && make compare'
