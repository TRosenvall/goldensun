# Batch 13 — 12 functions, all three GetEntrances arities finished

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–12 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean build, unassisted. Every address read back from the linked overlay
ELFs.

## The functions

| Function | Address | New source |
|---|---|---|
| `OvlFunc_895_2008030` | `0x02008030` | `src/overlays/rom_78dee8/ovl_30_a.c` |
| `OvlFunc_901_20087d4` | `0x020087d4` | `src/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_c_b.c` |
| `OvlFunc_911_200816c` | `0x0200816c` | `src/overlays/rom_79e5c0/ovl_30_a_c_b.c` |
| `OvlFunc_931_2008030` | `0x02008030` | `src/overlays/rom_7b8cb0/ovl_30_a.c` |
| `OvlFunc_931_200811c` | `0x0200811c` | `src/overlays/rom_7b8cb0/ovl_30_c_c_a_a_b.c` |
| `OvlFunc_937_2008030` | `0x02008030` | `src/overlays/rom_7c3044/ovl_30_a.c` |
| `OvlFunc_943_200b380` | `0x0200b380` | `src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_b.c` |
| `OvlFunc_947_2009544` | `0x02009544` | `src/overlays/rom_7d0e88/ovl_1528_a_a_a_c_b.c` |
| `OvlFunc_953_2008030` | `0x02008030` | `src/overlays/rom_7d95dc/ovl_30_a.c` |
| `OvlFunc_963_2008040` | `0x02008040` | `src/overlays/rom_7ec968/ovl_30_a_c.c` |
| `OvlFunc_963_20080e4` | `0x020080e4` | `src/overlays/rom_7ec968/ovl_30_c_c_a_a_b.c` |
| `OvlFunc_964_2009270` | `0x02009270` | `src/overlays/rom_7ed0a0/ovl_30_a_a_c_c_b.c` |

Several share `0x02008030` in different overlays — expected, since overlays
share a load base.

`unknown_id.sym` gains 12 entries.

## All three GetEntrances arities are finished

| Arity | Members | Done |
|---|---|---|
| two-way | 18 | 18 |
| three-way | 9 | 9 |
| four-way | 24 | 24 |

**51 functions across batches 08–13**, all descended from one parked function
whose note had diagnosed both of its diffs correctly and drawn the wrong
conclusion from them.

## The id namespace almost has a name

Two of the three-way functions return a table **your tree already names**:

    MapEntrance_ARRAY_895__02009cd4
    MapEntrance_ARRAY_937__020084a0

and the ROM annotation on the first reads *"area 0x13 -> .L1d04"*.

So two independent signals — your name for the data, and the annotation's word
for the operand — say these ids index **map entrance tables by area**. That
fits the contiguous per-area runs measured in batch 11 (`0x4d`–`0x57` for
overlay 932, `0x93`–`0x97` for 957, and so on, range `0x10`–`0xba`).

**We have not renamed `_ID_` to `_AREA_`.** The annotation corpus gets purpose
wrong often enough to matter, and renaming is deferred to the pass that has the
whole picture. But this reduces the standing question to something answerable
in one line: **is `MapEntrance` your name, and does the index mean what it
looks like?** If yes, `unknown_id.sym` can be retired and its ~55 entries given
real names.

## A block move that was the ABI, not the source

`OvlFunc_947_2009544` appeared to contain an open-coded copy:

    mov r2, sp / add r3, sp, #0x18 / ldmia r3!, {r0,r1} / stmia r2!, {r0,r1}
    ldr r0, [r5] / ldr r1, [r5,#4] / ldr r2, [r5,#8] / ldr r3, [r5,#0xc]

It is a six-word struct passed **by value** — four words in `r0`–`r3`, the last
two copied to the bottom of the stack as arguments five and six. The block move
is the calling convention; the source said `f(s)`. Written that way it matched
first try.

The struct is spelled `int w[6]` because only its size is visible from one call
site. Naming fields would look like knowledge we do not have.

## A scan of mine produced nine false family members

Worth recording because the failure is invisible without a screen.

After `OvlFunc_943_200b380` matched, I looked for its family with a substring
test — functions containing `__MapActor_GetActor`, `__Actor_SetSpriteFlags`,
`__Func_8092b08`, `#0x59` and `#0x23` — and got nine hits. **None were family
members.** They are functions of 450 to 1500 instructions that happen to
contain those calls somewhere inside.

Two files were split and three templated `.c` files written before
`tools/tryc.py` reported `rom 1526 lines, ours 25`. Both splits are reverted.

`tools/find_families.py` does this correctly: it compares whole shapes and
**bounds the body size**. A substring scan answers "which functions mention
these?", not "which functions *are* this?" Its docstring now says so, and the
three-way family in this batch was found with it.

## The 34-function `narrow-mask` blocker: two half-solutions that conflict

Not solved, but much better characterised than "ordering":

- **Field read first** gives an *exact* four-instruction prefix — sprite in
  `r0`, field in `r2`, `ldrb` in the right place, tail identical. It fails only
  because gcc folds the mask to `sub r3, #0x10`, since the `3` from
  `priority &= 3` is still live and `3 - 0x10 == ~0xc`.
- **Mask built before the `&= 3`** defeats that fold and produces the ROM's
  `mov`/`neg` pair — but pushes the sprite pointer out to `r4`.

The two pull opposite ways: the peephole fires exactly when the `3` is live,
and the `3` is live exactly when pressure is low enough to keep the sprite in
`r0`.

Also settled this batch: the combine is `and r3, r2`, i.e. `(m & f)`, not
`(f & m)`. Eight of the eleven earlier attempts varied statement order while
that single expression was wrong. And the `mov`/`neg` pair is **not inherently
hard** — `OvlFunc_924_200cf90` (batch 12) emits it unaided because its `-1` is
compared twice. The blocker is about constants used *once*.

Full detail in `src/non_matching/overlays/narrow_constant.c`.

## Still open, and still only answerable by you

- **Is `MapEntrance` your name for those tables, and is the index an area id?**
  One line retires `unknown_id.sym`.
- **Five ambiguous offsets in `actor.h`** (batch 03), documented rather than
  guessed.

## Reproducing the verification

    docker build -t goldensun-build -f tools/Dockerfile .
    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        sh -c 'make clean && make compare'
