# Batch 127 — a 248-function class opened by a control that refuted three parks

Verified on a clean `make clean && make compare` — `goldensun.gba: OK` — with
every address read out of the linked ELF. (`OvlFunc_common1_3e4` links at
0x02009c7c; the `common1_` names are label indices into the shared overlay, not
addresses.)

## Elevated (2241 → 2236)

| function | address | notes |
|---|---|---|
| `OvlFunc_971_200853c` | 0x0200853c | needed its **return type** |
| `OvlFunc_909_2008338` | 0x02008338 | first elevation with the new lever |
| `OvlFunc_883_2008ba8` | 0x02008ba8 | needed `CSE_CFLAGS` |
| `OvlFunc_938_2008184` | 0x02008184 | sibling of 2008338, first screen |
| `OvlFunc_common1_3e4` | 0x02009c7c | first screen |

## I parked three functions on a claim that was false

Batches 125–126 parked `OvlFunc_899_20099a4`, `OvlFunc_901_2008a80` and
`OvlFunc_911_20082b4` on the same thing: the ROM emits

    mov r1,#0x80 / mov r2,#0x80 / mov r0,#0x0 / lsl r1,#8 / lsl r2,#7
    bl  __MapActor_SetSpeed

with the zero first argument **inside** the split build of the other two, and
`f(0, 0x80 << 8, 0x80 << 7)` puts the `mov r0, #0` last. I wrote that gcc always
emits it last.

Before publishing the sizing I ran the control. **144 of 3,411 solved functions
have gcc placing it non-last.** The claim was false, and it had been repeated
across three park files.

### The detector was wrong twice, in different ways

The first pass asked only "is the zero not last in the block" — 450 remaining /
144 solved. Both inflated by trivial `mov r0,#0 / mov r1,#5` pairs where r0-first
is simply the natural order. Worse than overcounting, it pointed me at a
two-instruction example with nothing to learn from.

The signature that matters is the zero **interleaved into a split build**: a
`mov rX,#imm` before it and an `lsl rX` after it. That gives **248 remaining /
15 solved** — and those 15 located the construct.

### The construct

From `src/overlays/rom_79c738/ovl_30_c_c_c_c_c_c_c_b.c`, already in the tree:

    c = 0x80 << 9;
    d = 0x80 << 8;
    if (__GetFlag(0x84e) != 0) return;     /* the guards are load-bearing */
    if (__GetFlag(0x322) == 0) return;
    ...
    __MapActor_SetSpeed(0, c, d);

Named locals at the top **with a branch between the assignments and the call**.
gcc will not keep the constants live across the guards, so it rematerialises each
at its use, and the rematerialised sequence interleaves the way the ROM's does.

**The branch is not optional.** The same source in a straight-line function makes
gcc keep the values live and is strictly worse — 33 → 39 lines on
`OvlFunc_911_20082b4`, 26 → 29 on `OvlFunc_899_20099a4`. So the three parks stand,
but for a narrower and correct reason: no dominating block, not an impossible
instruction order. All three files are corrected.

**Sizing: 248 of 2,236 carry the shape. 150 have a guard before the site and are
worth the lever; 98 are straight-line.** The count is a floor — it only detects
the `lsl` form (see below).

### Validated, and it covers `neg` too

`OvlFunc_909_2008338` was the first elevation from it: naming the two shifted
constants at the top, with the function's existing guards in between, reproduced
the interleave exactly and took the first fifty instructions to identical.

The remaining six differences were the same shape around a different build —
`mov r2,#0x10 / mov r0,#0 / mov r1,#0 / neg r2,r2` — and the same lever closed
them. `mov`+`neg` is a split build exactly as `mov`+`lsl` is. Three more
functions followed, two on the first screen.

## Two other results

**A redundant-looking copy before a loop can mean the function returns that
value.** `OvlFunc_971_200853c` copies its clamped count into a second register
before the loop and never reads the original again. Declaring the function `int`
and returning the count was exact — r0 has to survive, which forces the counter
elsewhere.

**An added push holding a commoned constant is a flag tell, not a source
problem.** `OvlFunc_883_2008ba8` loads the flag id 0x807 twice; at `-O2` the
rerun-CSE pass commons the two pool loads into r5 and adds a `push {r5}` the ROM
does not have. I tried two source spellings first — separately named locals (the
trick that works on the `-1` triple) and a local inside the guarded block —
before `-fno-rerun-cse-after-loop` turned out to be exact. **Read the push list
first**; it is in the first two lines of the screen, and it is now the second
diagnostic that lives in the prologue rather than the body.

## Still owed

- 12 not-yet-elevated `.s` TUs inside Makefile wildcards.
- ~3,300 lines of duplicate Makefile rule blocks.
- 267 parks. Three were corrected this batch rather than closed; nothing
  systematically re-checks parks against new results, and batches 123–127 have
  changed the reasoning behind several.
