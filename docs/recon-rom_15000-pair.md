# Batch 307 brief F -- RECON ONLY for Func_8025200 and Func_802592c
# NO FIGURE IS CLAIMED FOR EITHER.  Neither was compiled, so there is nothing to
# measure and nothing is invented.  Deliberately NOT written as PARK_*.c: a park
# file whose first comment block carries a claim line gets re-measured by
# parkcheck, and a park with no candidate behind it would be a false entry of
# exactly the kind batch 305 spent the batch cleaning up.

## 1. THE SPLIT SHAPE -- VERIFIED BY DRY-RUN, AND ONE SPLIT SERVES BOTH

Both targets live in `asm/rom_15000/rom_23178_a_a_a_a_c_a_a_a.s`, which holds FOUR
functions and NO data section:

    line    7  .thumb_func_start Func_8025200   (ends 875)
    line  880  .thumb_func_start Func_802592c   (ends 1767)
    line 1774  .thumb_func_start Func_8026080   (ends 3564)
    line 3571  .thumb_func_start Func_8026e80   (ends 3716)

`split_s.py --dry-run` for **Func_802592c** gives the shape that serves BOTH
targets at once, because Func_8025200 is the file's FIRST function and so falls
out whole as the `_a` part:

    asm/rom_15000/rom_23178_a_a_a_a_c_a_a_a_a.s   1 function,  873 lines  <- Func_8025200
    asm/rom_15000/rom_23178_a_a_a_a_c_a_a_a_b.s   1 function,  892 lines  <- Func_802592c
    asm/rom_15000/rom_23178_a_a_a_a_c_a_a_a_c.s   2 functions, 1949 lines <- 8026080, 8026e80
    REMOVE asm/rom_15000/rom_23178_a_a_a_a_c_a_a_a.s ; rewrite stage1.ld

So the two .c files would be `src/rom_15000/rom_23178_a_a_a_a_c_a_a_a_a.c` (8025200)
and `..._b.c` (802592c).  DO NOT run the single-target split for Func_8025200 --
it produces only `_b` (the target) + `_c` (the other three), which puts 802592c
into a 3-function `_c` and forces a SECOND split later.  Sequence the 802592c
split once and both targets are ready.

Per the tool's own instructions: run the split, confirm `make compare` is still
GREEN before any .c is written, because a layout mistake and a bad decompilation
look identical at the end.

## 2. THE DUPLICATE HYPOTHESIS IS **NEGATIVE** -- measured, not assumed

Both recon comments say "same construction" (802592c's says "same construction as
Func_25200"), which raised the batch-303/304 possibility that one recipe lands
both, as it did for the 17-function 132-instruction group.  IT DOES NOT HOLD:

    opcode-sequence ratio (difflib, opcodes only, operands ignored):  47.2%
    8025200 = 770 instructions, 802592c = 794

47% on opcodes ALONE -- an upper bound, since operands are ignored -- is nowhere
near the byte-identity-modulo-relocations that made the 132-instruction group
transfer.  Budget them as two separate reconstructions.

Shared callees, 19 of them: AllocUploadSpriteGFX CloseUIBox CreateUIBox
Func_800352c Func_8003dec Func_8003f3c Func_8016498 Func_8016738 Func_8017aa4
Func_8019000 Func_801965c Func_801e7c0 Func_8022768 Func_80251d4 SetTextColor
UploadSprite2 WaitFrames _PlaySound __divsi3
Only in 8025200: Func_8021af0 Func_8025180 _GetItemInfo  (an ITEM screen)
Only in 802592c: Func_801e9d4 Func_80218dc LoadMoveIcon _GetMoveInfo _GetUnit
                                                        (a MOVE/PSYNERGY screen)

## 3. WHAT **DOES** TRANSFER: A 94-INSTRUCTION IDENTICAL RUN

    8025200[37:131]  ==  802592c[40:134]   len 94   <- the screen construction
    8025200[409:461] ==  802592c[468:520]  len 52
    8025200[292:326] ==  802592c[353:387]  len 34
    8025200[463:494] ==  802592c[522:553]  len 31
    8025200[598:629] ==  802592c[615:646]  len 31
    8025200[136:159] ==  802592c[136:159]  len 23   (same index in both)

~265 of 770 instructions are in shared runs.  THE 94-RUN IS THE HIGHEST-VALUE
TARGET IN THIS BRIEF: it begins immediately after the prologue and the first
CreateUIBox, and getting it right is worth ~12% of each function and is the same
source in both.  Write it ONCE and paste it into both reconstructions.

## 4. STRUCTURAL RECON OF THE 94-RUN (read off 8025200 lines 1-140)

Frame `sub sp, #0x130`.  Three incoming parameters are spilled immediately, which
PUBLISHES them as declared variables: r0 -> sp+0x50, r1 -> sp+0x4c, r2 -> sp+0x48.

    s = iwram_3001e8c                      -> sp+0x44   (a pointer)
    sel = -1                               -> sp+0x40
    row = -1                               -> sp+0x3c   (two -1 sentinels, one
                                              `mov r2,#1 / neg r2,r2` reused for BOTH)
    gfx = AllocUploadSpriteGFX(0x80)       -> sp+0x38
    boxA = CreateUIBox(0, 5, 0x1e, 4, 0x2a)            -> sp+0x34   (5th arg via sp+0)
    0                                      -> sp+0x30
    v = *(iwram_3001e8c + 0xa8)            ; then r9 = v->f34, r8 = v->f30,
                                             sp+0x2c = v->f38
    boxB = CreateUIBox(0xf, 9, 0xf, 0xb, 6) -> r11      (5th arg via sp+0)

Then the FIRST loop, the one to write carefully -- five OBJ entries of stride 0xc
built in a local array at sp+0x54, `for (i = 0; i <= 4; i++)`:

    obj = (char *)sp+0x54 ; sp+0x10 = obj   (the BASE is kept in its own slot)
    r6 = 0x80 << 23 = 0x40000000            (built, not pooled)
    r12 = 0xfffffe00, r10 = 0xfffffc00      (two masks held in HIGH registers
                                             across the whole block)
    each iteration:
        *(int *)(p + 4) = 0x40000000
        *(int *)(p + 8) = 0
        *(u16 *)(p + 6) = (*(u16 *)(p + 6) & 0xfffffe00)
                        | (((boxB->f0c << 3) + 8) & 0x1ff)
        *(u8 *)(p + 4)  = ((i * 2 + boxB->f0e) << 3) + 4
        p += 0xc
    loop control: `add r7,#1 / cmp r7,#4 / ble` -- an ASCENDING `<= 4`, so five
    iterations, and the giv is the pointer `p` (`add r4,#0xc`), NOT a subscript.

Then the SECOND loop, a COUNTDOWN from 4 (`sub r7,#1 / cmp r7,#0 / bge`, five
iterations), which allocates and uploads five sprites:

    q = sp+0x90 ; sp+0x1c = q ; sp+8 = q    (the SAME address in TWO slots --
                                             one is the walking copy, `stmia r2!,{r0}`
                                             then `str r1,[sp,#8]` writes it back,
                                             i.e. a POINTER SPILLED AND RELOADED
                                             EVERY ITERATION)
    r6 = sp+0x10's value (the obj base), r5 = 8 stepping by 0xc
    each iteration:
        g = AllocUploadSpriteGFX(0x80)
        *q++ = g
        h = UploadSprite2(g, -1)
        *(u16 *)(r5 + r6) = (*(u16 *)(r5 + r6) & 0xfffffc00) | (h & 0x3ff)
        r5 += 0xc

    NOTE the `ldrh r3,[r5,r6] / strh r3,[r5,r6]` REGISTER+REGISTER form on the
    halfword -- this is EXACTLY the shape batch 305 solved on Func_8018efc, where
    a SEPARATE INDEX LOCAL for the halfword store gives `[r5,r2]` and the array
    form gives the operands the other way round.  Per that park: THE LOCAL IS THE
    LEVER, NOT THE POINTER SPELLING.  Write the 0xc-stride offset into its own
    declared `int` and index the base with it.

Then four calls to Func_80251d4 with a pooled 0xf018 held in r5 and then `add r5,#1`:

    Func_80251d4(0xf018, 0x200)   ; 0x80 << 2, BUILT
    Func_80251d4(0xf018, 0x201)   ; pooled
    Func_80251d4(0xf019, 0x210)   ; 0x84 << 2, BUILT
    Func_80251d4(0xf019, 0x211)   ; pooled
    -- `add r5, #1` between the pairs says 0xf018 and 0xf019 are ONE named base
    plus one, not two independent constants (batch 298's rule: gcc never chains
    plain CONST_INTs, so a ROM deriving a constant from a held value identifies a
    NAMED BASE).  0xf018 is a very strong candidate for a symbol; check
    include/*.sym before spelling it as a literal.

Then the main loop preheader and `.L25344`, the screen's input loop, whose exit
test is the pair of -1 sentinels:

    if (r9 == sel && r8 == row) break;      /* both unchanged -> .L2552c */

## 5. LEVERS TO TRY FIRST, IN THIS ORDER (from batch 307 brief F's own set)

1.  THE BANK LEAD IS DIRECTLY APPLICABLE.  `src/non_matching/rom_15000/8018efc.c`
    went 17 -> 2 on TWO levers and BOTH shapes are present in the 94-run above:
    a base pointer and a walking pointer that are ONE VARIABLE advanced in place,
    and a separate index local for a reg+reg halfword store.  The second loop's
    `sp+0x1c` / `sp+8` pair is the first shape almost verbatim -- but note the ROM
    keeps TWO slots here, so read it carefully before collapsing them.
2.  Lever 8, THE SPILL-SLOT RULE, is unusually informative on this function
    because the frame is 0x130 and TWELVE distinct slots are in use.  Declared
    locals take HIGH offsets in DECLARATION ORDER, so the slot order
    0x2c,0x30,0x34,0x38,0x3c,0x40,0x44,0x48,0x4c,0x50 read DOWNWARD publishes the
    declaration order, and the arrays at 0x54 and 0x90 sit above the scalars.
    Use it to fix the declaration order BEFORE measuring anything else -- it is
    free information and it moves `allocno_compare`'s third input.
3.  The first loop is an ascending `i <= 4` with a pointer giv; the second is a
    countdown `bge 0`.  DO NOT normalise them to the same form -- brief F's own
    Func_80bd898 result showed the countdown form is worth 18 objcmp and 10 hunks
    when the ROM has `subs/cmp #0/bne`, and the ascending form is worth the same
    in reverse when the ROM has `add/cmp #4/ble`.
4.  Two masks (0xfffffe00, 0xfffffc00) are held in r12 and r10 ACROSS the whole
    block.  High registers holding loop-invariant constants is the signature of
    named locals declared OUTSIDE the loop, not of inline literals.

## 6. WHY NO CANDIDATE WAS WRITTEN

Brief F asked for three targets and said to prefer two well-understood functions
over three shallow ones.  Func_80bd898 was taken to a verified, pin-free park at
737 of 838 with six levers measured, one measured-inert family recorded and a real
reconstruction bug found and fixed.  The remaining context did not permit a
second 770-instruction reconstruction of comparable quality, and a shallow
candidate would have produced a SATURATED figure that cannot rank -- which, per
batch 305, is worse than no figure, because a claim line that nobody can act on
still gets re-measured forever.  The split shape, the negative duplicate result
and the 94-run above are the parts of that work that do not have to be repeated.
