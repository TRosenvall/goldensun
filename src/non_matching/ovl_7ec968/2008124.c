/* OvlFunc_963_2008124  --  0x02008124  [asm/overlays/rom_7ec968/ovl_30_c_c_a_a_c_a.s]
 *
 * NON-MATCHING, 33 of 149 encodings  (MEASURED batch 347 brief B, production flags).
 *   Instruction counts AGREE at 119/119 and SIZE agrees at 356/356, so 33 IS a
 *   true distance.  Was 143 of 149 at ref 119 / ours 117 -- a -2 INSTRUCTION GAP.
 *   RELOCATIONS differ by three words only: _AREA_69, _AREA_a9 and _AREA_aa read
 *   0 in our object and link to their area.sym literals, which is the accepted
 *   .sym class (area.sym is INCLUDEd at stage1.ld:17).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7ec968/2008124.c \
 *     asm/overlays/rom_7ec968/ovl_30_c_c_a_a_c_a.s --func OvlFunc_963_2008124
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/ovl_7ec968/2008124.c \
 *     asm/overlays/rom_7ec968/ovl_30_c_c_a_a_c_a.s OvlFunc_963_2008124
 *   -> 33 of 149, aligned residue 31 in 13 hunks.
 *
 * THE HEADER THIS REPLACES SAID "the figure IS a distance" AND IT WAS NOT.
 * It also, four lines earlier, said the opposite -- "COUNT DIFFERS (ref 149, ours
 * 150) -- so this positional figure measures MISALIGNMENT" -- and even that used
 * the STREAM length (149/150) where the 16-bit INSTRUCTION counts were 119/117.
 * The batch-322 re-classification sentence overwrote a correct warning with a
 * false claim, and the false claim is the one every later reader acted on: it is
 * why the park's three problems were ranked by residue size rather than by which
 * of them changed the LENGTH.
 *
 * WHAT THE FALSE CLAIM COST.  The park's own "problem 3" -- `ldr r3, =0xa9 /
 * cmp r6, r3` where gcc writes `cmp r6, #0xa9` -- WAS THE GAP, twice over, and
 * the park dismissed it: "gcc is right and the ROM went the long way round".
 * The park then wrote "NEXT: problem 1 is the one to solve first".  Problem 1 was
 * count-neutral.
 *
 * THE GAP, PER REGION: TWO SITES OF 1, same class, both pooled comparisons.
 *     ldr r3, =0xa9 / cmp r6, r3   vs ours  cmp r6, #0xa9    -1
 *     ldr r3, =0xaa / cmp r6, r3   vs ours  cmp r6, #0xaa    -1
 * 0xa9 and 0xaa are inside the #imm8 range, so by const.sym's own criterion --
 * gcc-2.96 never pools what an eight-bit `mov` can build -- the ROM named
 * SYMBOLS.  The compared halfword is `*(short *)(gState + 0xe0 * 2)` =
 * gState+0x1C0, which is the exact field area.sym's header names as the current
 * area, and `_AREA_a9 = 0xa9` (area.sym:218) and `_AREA_aa = 0xaa` (area.sym:132)
 * WERE ALREADY IN THE FILE.  Nothing had to be added.
 *
 * FOUR MORE MOVES, EACH MEASURED (ndiff / ins ref:ours / size ref:ours / aligned):
 *     park body                            143  119:117  356:364   100 in 25
 *     + _AREA_a9, _AREA_aa                 151  119:119  356:380   100 in 26
 *     + int carriers for the 1 and 5
 *       halfword stores                    136  119:118  356:360    80 in 31
 *     + `int` return type and `return 0;`  136  119:119  356:360    78 in 29
 *     + (int)&_AREA_69 for the 0x69 store  140  119:118  356:352    57 in 23
 *     + `p = h + 0xe3 * 2;` own statement   56  119:119  356:356    57 in 20
 *     + `p = g + 0x242;` own statement,
 *       both blocks                         33  119:119  356:356    31 in 13   <- kept
 *
 * WHY EACH ONE WORKS, with citations:
 *   - THE 0x69 STORE WAS COSTING THREE MID-FUNCTION POOL DUMPS.  Written as a
 *     literal into `*(short *)`, gcc took `*thumb_movhi_insn` alternative 1,
 *     whose `pool_range` is 64 (config/arm/arm.md:4353) against SImode's 1020
 *     (arm.md:3856).  A 64-byte reach forces create_fix_barrier to emit a `b`
 *     over a pool dump; ours had THREE pools and the ROM has ONE at the end, and
 *     that offset shift alone was most of the 100-instruction aligned residue.
 *     _AREA_69 = 0x69 (area.sym:106) makes the operand a pooled SYMBOL address --
 *     an SImode word load, 1020 range -- and all three dumps collapse into the
 *     ROM's single tail pool.  The ROM's own `ldr r3, =0x69` for a value `mov`
 *     can build is the same .sym signature as the two comparisons.
 *   - THE `1` AND `5` STORES had the same HImode pool defect; an `int` carrier
 *     gives the ROM's `mov r3, #1 / strh r3, [r2]`.  The park had already used
 *     exactly this trick for the 0xa store (`t = 0xa;`) and had not extended it.
 *   - THE FUNCTION RETURNS int.  The ROM's tail is `mov r0, #0 / add sp, #8 /
 *     pop {r5, r6} / pop {r1} / bx r1`; a void body gives no `mov r0, #0` and
 *     pops into r0.  `int` + `return 0;` produces both.
 *   - THE DERIVED OFFSETS.  gcc reached 0xe3*2 as 0xe2*2 + 2 (`adds r0, #2`) and
 *     0x242 as 0x90*4 + 2 (`adds r1, #2`); the ROM builds both from scratch.
 *     Naming the OFFSET in its own statement does not stop it (measured: -3 and
 *     -2, aligned 59 and 60, both worse) -- the park measured that too and was
 *     right about it.  Naming the ADDRESS does: `p = g + 0x242;` as its own
 *     statement, with the store through `*(short *)p`, took 57 -> 33 and 20
 *     hunks -> 13.  That is the deciding move of this round and it is NOT the
 *     lever the park had tried.
 *
 * THE PARK'S OTHER TWO PROBLEMS, re-measured:
 *   - problem 1 (the base register call-clobbered in the ROM, callee-saved in
 *     ours) NO LONGER APPEARS.  r5 holds gState in both streams.
 *   - problem 2 (derived second offset) is real and is closed, above.
 *
 * OPEN RESIDUE -- ONE ALLOCATION CHOICE, REPEATED SEVEN TIMES, and it is now
 * nearly all of the 33:
 *     ROM   mov r0, #0xe0 / lsl r0, #1 / add r3, r5, r0
 *     ours  mov r2, #0xe0 / lsl r2, #1 / add r3, r5, r2
 * at 0xe2*2, 0xe0*2, 0xe1*2 (x3) and 0x90*4 (x2).  REG_ALLOC_ORDER is
 * { 3, 2, 1, 0, 12, 14, ... } (config/arm/arm.h:989-995), so the allocator tries
 * r3, then r2, then r1, then r0: ours takes the SECOND choice and the ROM takes
 * the FOURTH, which means r3, r2 and r1 were all excluded for that quantity in
 * the original.  Plus two stack-argument register roles (`mov r2, #0x1b` and
 * `mov r2, #0x38` scheduled before `str`, in r2, where ours uses r3 later).
 * MEASURED INERT OR WORSE on this body: one shared `int off` for all seven sites
 * (-8, aligned 67); both cheap axes -- all eight flag groups and all 28 anchor
 * placements -- closed by tools/autosweep.py, best 140 against base 140.
 *
 * NEXT: this is a local-alloc question, not a priority one.  Check whether the
 * offset pseudo is in `.18.greg`'s allocno list at all before diagnosing it --
 * batch 345 brief D found two of four such gaps owned by
 * local-alloc.c:1495-1497's QTY_CMP_PRI, and a single-block pseudo appears in
 * neither the greg allocno list nor its dispositions.
 */
extern unsigned char gState[];
extern int _AREA_69;
extern int _AREA_a9;
extern int _AREA_aa;

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

int OvlFunc_963_2008124(void)
{
    unsigned char *g;
    unsigned char *h;
    unsigned char *a;
    int v;
    int t;
    unsigned char *p;

    if (__GetFlag(0x89f) != 0) {
        h = gState;
        *(short *)(h + 0xe2 * 2) = (int)&_AREA_69;
        p = h + 0xe3 * 2;
        t = 0xa;
        *(short *)p = t;
    }
    g = gState;
    v = *(short *)(g + 0xe0 * 2);
    if (v == (int)&_AREA_a9) {
        if (__GetFlag(0x897) != 0)
            __MapActor_SetPos(0xa, 0, 0);
        if (*(short *)(g + 0xe1 * 2) == 3) {
            if (__GetFlag(0x8fb) != 0) {
                *(short *)(g + 0x90 * 4) = v;
                p = g + 0x242;
                t = 1;
                *(short *)p = t;
            }
            if (__GetFlag(0x8fc) != 0) {
                *(short *)(g + 0x90 * 4) = v;
                p = g + 0x242;
                t = 5;
                *(short *)p = t;
            }
            __ClearFlag(0x12f);
        }
        g = gState;
        if (*(short *)(g + 0xe1 * 2) == 1) {
            __SetFlag(0x8fb);
            if (__GetFlag(0x96f) == 0)
                __Func_8010704(6, 0, 2, 1, 8, 0x1b);
        }
        if (*(short *)(g + 0xe1 * 2) == 5)
            __SetFlag(0x8fc);
    } else if (v == (int)&_AREA_aa) {
        __MapActor_SetAnim(8, 4);
        __MapActor_SetAnim(9, 4);
        __MapActor_SetAnim(0xa, 3);
        __MapActor_SetAnim(0xb, 4);
        __MapActor_SetAnim(0xc, 3);
        a = __MapActor_GetActor(0xf);
        *(int *)(a + 0x1c) = 0x19999;
        __Func_8010704(0x6c, 0x26, 1, 1, 0x66, 0x38);
    }
    return 0;
}
