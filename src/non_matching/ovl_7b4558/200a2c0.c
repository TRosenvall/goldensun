/* OvlFunc_927_200a2c0 (0x0200a2c0) -- NON-MATCHING, 61 differing of 193 in clean C.
 * Blocker class: STRAIGHT-LINE REPEATED-CONSTANT CSE, in one basic block across ~20 calls.
 * UNREACHABLE-WITH-EVIDENCE in clean C. Never attempted before batch 279.
 *
 * asm/overlays/rom_7b4558/ovl_30_c_c_c_a_a_c_c_c.s (2 functions, with the parked
 * OvlFunc_927_200a1b0, so landing would need a split).
 *
 * THE BACK HALF IS EXACT ON THE FIRST SCREEN, and that is the new information here. The
 * 17-iteration cos/sin loop with its `int v[3]` frame array and the `v[0] + v[0]/2` rounding
 * divide, the eight-argument OvlFunc_927_2008ae8 call with four stack slots, both `ldrsh` from a
 * re-called MapActor_GetActor, the gState[0x22b] write, the __Func_8091f90 area call and the whole
 * epilogue all reproduced with no work.
 *
 * EVERYTHING DIFFERING IS CONSTANT BUILDS, NOTHING ELSE: 0x88<<16 (2 sites), -1 (7 uses),
 * 0xa0<<11 (6), 0x80<<9 (3), 0xe666 (3). gcc commons each into a callee-saved pseudo and widens
 * the push list by one register (`push {r5,r6,r7}` against the ROM's `{r6,r7}`); the ROM rebuilds
 * every one at its site.
 *
 * SO THE THREE-NAMED-LOCALS LEVER HAS NO BLOCK TO DOMINATE FROM. Its recorded precondition is that
 * the assignments sit in a block a branch DOMINATES, and THE WHOLE PRE-LOOP REGION HERE IS ONE
 * BASIC BLOCK ACROSS ABOUT TWENTY `bl`s -- calls do not end a basic block. That is the cleanest
 * statement of the precondition's failure mode found so far: it is not that the lever is weak, it
 * is that there is no branch.
 *
 * NO FLAG REACHES IT. -fno-rerun-cse-after-loop, -fno-gcse, -fno-cse-follow-jumps,
 * -fno-strict-aliasing, -fno-expensive-optimizations, -fno-schedule-insns, -fno-strength-reduce
 * and two combinations all leave 61; -fno-force-mem 65; -fno-schedule-insns2 89. The sibling
 * src/overlays/rom_7b4558/ovl_30_c_c_c_a_a_c_c_a.c already read this to the pass -- the FIRST
 * cse_main, ungated by any -f flag -- and that holds here.
 *
 * SCAFFOLDED BEST IS 5 DIFFERING AND STILL NOT EXACT, which is why this is parked rather than
 * landed: it takes FIFTEEN `__asm__ ("" : "+r" (x))` launders across six of the eight
 * constant-bearing call sites (ladder 61 -> 37 -> 25 -> 23 -> 18 -> 5). Fifteen launder rows that
 * still leave four real instruction placements wrong is not a landing.
 *
 * TWO THINGS FROM THE SCAFFOLDED WORK WORTH KEEPING ANYWAY:
 *   * A THIRD LAUNDER MODE: laundering the BASE and doing the shift at the call site
 *     (`g = 0xa0; asm; f(g << 11, ...)`) beats laundering the finished value, on two of the three
 *     sites that pass 0xa0<<11 twice. Not in the sibling's write-up.
 *   * "ANCHOR EVERY ARGUMENT" IS NOT UNIVERSAL. An exhaustive sweep over order x mask x mode found
 *     the MINIMUM masks are 010, 1100 and 100 on the respective sites -- full anchoring is strictly
 *     worse on four of them. Leaving a whole site bare is worse everywhere.
 *
 * THE IRREDUCIBLE 4 are both sched2 argument-fill-order ties with NO MEMORY OPERAND, so the
 * char-pointer aliasing lever that closed OvlFunc_927_2008ae8 this batch has nothing to bite on:
 *   (a) __Func_80933f8 wants `movs r3,#1` scheduled BEFORE the three completions and gcc puts it
 *       last -- all 24 orders x 16 masks x 2 modes measured, floor 5.
 *   (b) OvlFunc_927_2008d90 wants `adds r1,r5,#0` AFTER `lsls r3,#12` -- all 16 masks x orders x
 *       pinned and unpinned measured, plus hard-register pins on all four arguments of both sites,
 *       plus struct-typed short reads instead of casts: floor 5 in every case.
 *
 * Note the scaffolded form also carries one extra _AREA_46 relocation where the ROM's .s spells the
 * literal 0x46 -- the known AREA-symbol object-level shape, with _AREA_46 already in area.sym.
 *
 * NEXT: nothing in clean C, and the scaffolded route is not worth its rows. If the
 * straight-line repeated-constant class is ever broken it will be at cse_main, and this function is
 * a good specimen because everything EXCEPT the constant builds is already exact.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_46;

struct Actor {
    /* 0x00 */ int pad0[2];
    /* 0x08 */ int x, y, z;
    /* 0x14 */ int pad14;
    /* 0x18 */ int a, b;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Actor_SetSpriteFlags(void *actor, int flags);
extern void __Func_8092950(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
extern void __Func_809259c(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8091f90(int id, int b);
extern void __Func_8091eb0(int map, int entrance);
extern void __PlaySound(int id);
extern int __cos(int a);
extern int __sin(int a);
extern void OvlFunc_927_2008e18(int a);
extern void OvlFunc_927_2008d90(int a, int b, int c, int d);
extern void OvlFunc_927_2008ae8(int a, int b, int c, int d, int e, int f, int g, int h);

void OvlFunc_927_200a2c0(void)
{
    struct Actor *a;
    unsigned char *gs;
    unsigned int i;
    int v[3];
    int ang;
    int x, y;

    a = __MapActor_GetActor(0x12);
    __CutsceneStart();
    __Func_8092950(0x12, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x12), 0);
    __MapActor_SetPos(0x12, 0x88 << 16, 0xb4 << 17);
    __Func_80933d4(0x80 << 8, 0x80 << 5);
    __Func_80933f8(0x88 << 16, -1, 0xc4 << 17, 1);
    __Func_8093530();
    __CutsceneWait(0x3c);
    __Func_8012330(0xa0 << 11, 0xa0 << 11, 0x80 << 9);
    OvlFunc_927_2008e18(0x12);
    __Func_809259c(0, 2);
    __Func_8012330(-1, -1, 0xe666);
    __Func_8012350();
    __Func_8092adc(0, 0xc0 << 8, 0x14);
    __CutsceneWait(0x28);
    __Func_8012330(0xa0 << 11, 0xa0 << 11, 0x80 << 9);
    OvlFunc_927_2008e18(0x12);
    __Func_8012330(-1, -1, 0xe666);
    __Func_8012350();
    __CutsceneWait(0x28);
    OvlFunc_927_2008e18(0x12);
    a->a = 0x13333;
    a->b = 0x13333;
    __Func_8092950(0x12, 5);
    OvlFunc_927_2008d90(0x12, 0x88, 0xc4 << 1, 0xf0 << 12);
    __CutsceneWait(0xf);
    __Func_8012330(0xa0 << 11, 0xa0 << 11, 0x80 << 9);
    for (i = 0; i <= 0x10; i++) {
        ang = i << 12;
        v[0] = __cos(ang);
        v[1] = 0;
        v[2] = __sin(ang);
        v[0] = v[0] + v[0] / 2;
        OvlFunc_927_2008ae8(a->x, 0, a->z, v[0], v[1], v[2], 1, 0);
    }
    __CutsceneWait(0x1e);
    __Func_8012330(-1, -1, 0xe666);
    __Func_8012350();
    __PlaySound(0x94);
    __Func_80925cc(0x12, 2);
    __CutsceneWait(0x14);
    x = *(short *)((char *)__MapActor_GetActor(0) + 0xa);
    y = *(short *)((char *)__MapActor_GetActor(0) + 0x12);
    OvlFunc_927_2008d90(0x12, x, y - 0x10, 0x80 << 12);
    __CutsceneWait(0xa);
    gs = (unsigned char *)&gState;
    gs[0x22b] = 3;
    __Func_8091f90((int) (&_AREA_46), 0xf);
    __Func_8091eb0(0x35, 1);
    __CutsceneEnd();
}
