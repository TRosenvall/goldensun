/* OvlFunc_946_2008f70  [overlays/rom_7ced6c]
 *
 * NON-MATCHING, still PARKED.
 *   79 of 283 encodings, improved from 102 of 283.  Batch 345 brief E.
 *   INSTRUCTIONS ref 227, ours 228 -- GAP +1 (ours LONGER).  Stream 283 == 283
 *   and BYTES EQUAL (dsize 0), because a pool pad absorbs the extra
 *   instruction, so the positional figure is partly PHASE.
 *   ALIGNED residue (tools/aligncmp.py) 219 -> 238 of 283, 77.4% -> 84.1%.
 *   Under ALIAS_CFLAGS: 75 of 283, aligned 239 (84.5%), same gap, same size.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7ced6c/2008f70.c \
 *     asm/overlays/rom_7ced6c/ovl_30_c_c_a_a_c_c_a.s --func OvlFunc_946_2008f70
 *
 * PINS: 0.  DEVICES: 0.  FAKEMATCH: no.  SPLIT: none (1 function in the .s).
 *
 * ===== THE "NOT A DISTANCE" BANNER IS WRONG AND IS RETIRED =====
 *
 * The old header opened with "*** RELOCATIONS DIFFER IN THEIR SYMBOLS, NOT ONLY
 * THEIR OFFSETS -- _AREA_71, _AREA_7b, _AREA_7d, _AREA_7e, _AREA_86 SO THIS
 * FIGURE IS NOT A DISTANCE ***".  Its own FACTS section then says those five are
 * "the linker symbols _AREA_71 / _7b / _7d / _7e / _86 (all in area.sym)".  A
 * symbol defined BY VALUE in a .sym file links to the literal, so those pool
 * words are correct and the relocations are the EXPECTED consequence of writing
 * them as symbols -- landed files in this tree already do exactly this, and
 * tools/autosweep.py now prints that rule beside the warning.  The reference's
 * pool words read `.word 0x0000007b` / `0x0000007d` where ours read
 * `.word 0x00000000` + a relocation; that is five encodings of the figure which
 * are ARTEFACT, not defect.  The figure IS readable.  What makes it only partly
 * a distance is the +1 instruction, not the relocations.
 *
 * ===== WHAT MOVED IT 102 -> 79, AND IT IS THE OPPOSITE OF TARGET 1's LEVER ===
 *
 * 1. `k` MUST NOT EXIST.  The old body named `k = 0xe0 << 1;` and used it twice,
 *    leaving the other four `(0xe0 << 1)` / `(0xe1 << 1)` sites as literals.
 *    Deleting `k` entirely and writing the literal at BOTH of its sites is worth
 *    102 -> 88 and aligned 219 -> 234.  The ROM rebuilds `movs r2,#224 /
 *    lsls r2,#1 / adds r3,r6,r2` at each use; a single name lets cse hold 448 in
 *    a register and the ROM's three instructions collapse to one `adds`.
 *    *** AND ONE NAME PER USE SITE IS MUCH WORSE HERE: 263. ***  That is the
 *    reverse of the batch-105 basic-block lever that works on
 *    Task_BlitAnim/Task_BlitAnim_BG1Wide, because those constants are LIVE
 *    ACROSS A CALL and these are not -- without a call there is nothing for
 *    reload to rematerialise against, so N names just add N copies.
 * 2. TWO NAMES FOR EACH REPEATED ARGUMENT CONSTANT THAT IS LIVE ACROSS A CALL.
 *    `0xa0 << 17` appears at two __Func_8012078 calls in ONE basic block (the
 *    flag-0x243 arm) and `0xb8 << 17` at two in different arms.  Naming each
 *    pair `a1`/`a2` and `b1`/`b2` AT THE TOP of the function is worth 88 -> 79.
 *    This IS the batch-105 lever, and it applies here exactly where a call
 *    separates the two uses -- which is the discriminator between item 1 and
 *    item 2 and the reusable rule from this park.
 *    Measured: `a` pair alone 81, `b` pair alone 86, BOTH 79, and adding the
 *    `0xd8 << 16` pair as well is WORSE at 83.  A composition cliff on a set of
 *    three pairs; "two is not N" in the other direction.
 *
 * ===== WHERE THE ONE EXTRA INSTRUCTION IS =====
 *
 * Every length-imbalanced hunk in the aligned diff is a pure reorder (a delete
 * at ref[N] matched by an insert at ours[N+k]) EXCEPT ONE, at ref[242:245] /
 * ours[242:246]:
 *
 *     rom   movs r2,#224 / lsls r2,r2,#1 / adds r3,r6,r2
 *     ours  *adds r2,r5,#0* / movs r1,#224 / lsls r1,r1,#1 / adds r3,r7,r1
 *
 * The extra `adds r2,r5,#0` is the SECOND `0xa0 << 17` being copied out of the
 * callee-saved register it was allocated, instead of rematerialised.  Naming it
 * gave it its own pseudo; it still got a hard register because nothing in the
 * block competes for one.  THIS IS THE SAME RESIDUE AS
 * src/non_matching/rom_c9000/80cd358.c case 1 and the same bound: a REG_EQUIV
 * constant is only rematerialised when global alloc declines to give it a
 * register, and there are free callee-saved registers to give.
 *
 * Measured and inert on the definition-position axis, all still gap +1:
 *   a2 assigned BETWEEN the two calls 81; both assigned locally before the first
 *   call 86; a2 inlined as the literal 80; a1 inlined 86; a THIRD name a3 at the
 *   top 79 (exactly inert -- the third name is dead).
 *
 * ===== MEASURED NEGATIVES =====
 *
 * AUTOSWEEP: both cheap axes CLOSED on the old body.  The anchor axis was CAPPED
 * at 40 of 52 in the first run and has since been re-run at --max 52: ALL 52
 * boundaries measured, all 102 or worse.  Flag axis 8/8, best 95 (-fno-gcse) but
 * see below.
 *
 * Flag axis RE-RUN on THIS body, 11 groups: (none) 79; -fno-strength-reduce,
 * -fno-rerun-loop-opt, -fno-schedule-insns, -fno-cse-follow-jumps all exactly
 * 79; -fno-strict-aliasing *75* at gap +1 and dsize 0, aligned 239 (84.5%) --
 * a real but marginal improvement, recorded as an annotation and NOT as the
 * park's figure, since this is not a landing and a per-file flag group is an
 * owner-facing decision; -fno-schedule-insns2 109; -fno-rerun-cse-after-loop 118
 * at gap +2; -ffixed-r7 280 at gap +4; -fno-gcse 287 at gap +12; -O1 289 at
 * gap +14; -fno-expensive-optimizations 159 at gap -1.
 *
 * *** THE OLD HEADER'S -fno-gcse READING IS REFUTED. ***  autosweep reported
 * "-fno-gcse 95 of 283" against the old body's 102 and flagged it; on this body
 * it is 287 at gap +12.  It was a phase artefact of the old body.
 *
 * ===== WHAT IS RIGHT, carried forward and all re-confirmed =====
 *
 * THE BIGGEST SINGLE WIN WAS A SINGLE EXIT: five `return 0;` replaced by
 * `goto out;` took it 152 -> 98 (the six-instruction lever recorded in
 * src/overlays/rom_7ced6c/ovl_30_c_c_a_a_a_a.c, applying five times over).
 *
 * THE GLOBAL MUST BE THE ARRAY IDIOM.  `extern unsigned char gState[];` with
 * `g = gState;` is what produces `mov r2,#0xe0 / lsl r2,#1 / add r3,r6,r2`; a
 * struct pointer folds the whole thing into one pool word `=gState+448`.
 *
 * FACTS, so nobody re-derives them: the area ids are the linker symbols
 * _AREA_71 / _7b / _7d / _7e / _86 (area.sym), which is why the ROM pools
 * 0x7b/0x7d/0x7e where an imm8 would fit, while the flag ids (0xef7, 0x8d1,
 * 0x240-0x243, 0xef4, 0xfd7) are plain integers.  The `.L1004` block is a
 * short-circuit `||`.  The control flow is decoded and believed correct.
 *
 * STILL OPEN BESIDES THE +1: the gState base is r7 for us and r6 in the ROM
 * (with `v`/`e` taking the other), a two-value rotation; the ROM's
 * `ldr r6, =gState` sorts at ref[9] where ours sorts at ours[4]; and the
 * repeated ordering difference where the ROM emits `mov r0, #imm` BETWEEN the
 * two `lsl`s of the other arguments, at all four __Func_8012078 sites and both
 * __Func_808edac sites.  All three are reorders and cost no length.
 *
 * MEASURED, earlier rounds: struct-pointer global 145; array idiom 152; array
 * idiom + single exit 98; moving the `g = gState` assignment later 98;
 * rewriting the `||` as explicit gotos 98.
 */
extern unsigned char gState[];
extern int _AREA_71;
extern int _AREA_7b;
extern int _AREA_7d;
extern int _AREA_7e;
extern int _AREA_86;
extern unsigned char iwram_3001ebc[];
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_808edac(int a, int b, int c);
extern void __Func_8012078(int a, int b, int c, int d);
extern void OvlFunc_946_200991c(void);
extern void OvlFunc_946_200967c(void);
extern int OvlFunc_946_20088c0(int slot);
extern void OvlFunc_946_200aed8(int a);
extern void OvlFunc_946_2009214(void);
extern void OvlFunc_946_2009494(void);

int OvlFunc_946_2008f70(void)
{
    unsigned char *g;
    short *e;
    unsigned char *p;
    unsigned char *a;
    int v;
    int t;
    int a1;
    int a2;
    int b1;
    int b2;

    a1 = 0xa0 << 17;
    a2 = 0xa0 << 17;
    b1 = 0xb8 << 17;
    b2 = 0xb8 << 17;
    g = gState;
    p = *(unsigned char **)iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x81 << 2;
    e = (short *)(g + (0xe0 << 1));
    v = *e;
    if (v == (int)(&_AREA_7b)) {
        OvlFunc_946_200991c();
        goto out;
    }
    if (v == (int)(&_AREA_7d)) {
        if (__GetFlag(0xef7) == 0) {
            __Func_8010704(0, 3, 1, 1, 0xd, 0x28);
            __Func_8010704(0, 2, 1, 1, 0xf, 0x28);
            __Func_808edac(0x65, 0xd8 << 16, 0xa2 << 18);
        }
        if (*e == v) {
            if (*(short *)(g + (0xe1 << 1)) == 5 ||
                __GetFlag(0x8d1) != 0) {
                __SetFlag(0x8d1);
                __Func_8010704(0, 1, 1, 1, 0xd, 0x1e);
                __Func_808edac(0x64, 0xd8 << 16, 0xf4 << 17);
            }
            goto out;
        }
    }
    t = *(short *)(g + (0xe0 << 1));
    if (t == (int)(&_AREA_71)) {
        OvlFunc_946_200967c();
        *(int *)(__MapActor_GetActor(8) + 0x38) = 0x81 << 16;
        OvlFunc_946_20088c0(9);
        OvlFunc_946_20088c0(0xa);
        if (__GetFlag(0x240) != 0) {
            a = __MapActor_GetActor(0xb);
            if (a != 0) {
                *(char *)(a + 0x59) = 0;
                __Actor_SetAnim(a, 4);
                __Actor_SetSpriteFlags(a, 0);
            }
            __Func_8012078(0, 0x98 << 17, b1, 0xfd);
        }
        if (__GetFlag(0x241) != 0) {
            a = __MapActor_GetActor(0xc);
            if (a != 0) {
                *(char *)(a + 0x59) = 0;
                __Actor_SetAnim(a, 4);
                __Actor_SetSpriteFlags(a, 0);
            }
            __Func_8012078(0, 0xa0 << 15, b2, 0xfd);
        }
        if (__GetFlag(0x242) != 0) {
            a = __MapActor_GetActor(0xd);
            if (a != 0) {
                *(char *)(a + 0x59) = 0;
                __Actor_SetAnim(a, 4);
                __Actor_SetSpriteFlags(a, 0);
            }
            __Func_8012078(0, 0xc0 << 15, 0xa8 << 17, 0xfd);
        }
        if (__GetFlag(0x243) != 0) {
            a = __MapActor_GetActor(0xe);
            if (a != 0) {
                *(char *)(a + 0x59) = 0;
                __Actor_SetAnim(a, 4);
                __Actor_SetSpriteFlags(a, 0);
            }
            __Func_8012078(0, 0x90 << 16, a1, 0xfd);
            __Func_8012078(0, 0xbc << 18, a2, 0xfd);
        }
        if (__GetFlag(0xfd7) == 0)
            OvlFunc_946_200aed8(8);
        goto out;
    }
    if (t == (int)(&_AREA_7e)) {
        if (__GetFlag(0xef4) == 0) {
            __Func_8010704(0, 0, 1, 1, 0x25, 0xa);
            __Func_808edac(0x64, 0x96 << 18, 0xa8 << 16);
        }
    }
    t = *(short *)(g + (0xe0 << 1));
    if (t < (int)(&_AREA_7e))
        goto out;
    if (t > (int)(&_AREA_86))
        goto out;
    OvlFunc_946_2009214();
    if (*(short *)(g + (0xe1 << 1)) == 5)
        OvlFunc_946_2009494();
out:
    return 0;
}
