/* OvlFunc_954_200842c -- NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7db0c8/ovl_30_c_c_a_a_c_c.s
 * Best screen: 9 differing of 43, streams the same length.
 *
 * BLOCKER CLASS: register allocation on a shifted field read, cascading.
 *
 * The ROM reads the coordinate into one register and shifts it into ANOTHER:
 *
 *     rom   ldr r3, [r0, #0x10] / asr r0, r3, #0x14 / cmp r0, #8 / str r0, [sp, #4]
 *     ours  ldr r3, [r0, #0x10] / asr r3, #0x14     / cmp r3, #8 / str r3, [sp, #4]
 *
 * Both are the same instruction -- thumb's immediate ASR is always the
 * three-operand `asr rd, rs, #n`, and ours simply has rd == rs. The ROM reuses
 * r0, which held the actor pointer and is dead by then, for the result;
 * REG_ALLOC_ORDER prefers r3 and that is what we get. The choice propagates to
 * the `cmp`, the `str` and the second copy of the same sequence, which is six
 * of the nine.
 *
 * The other three are argument-setup ordering: `mov r3, #1 / mov r5, #0x40`
 * against our reverse, and `mov r1, #0x18 / mov r0, #0x40` against ours.
 *
 * WHAT WAS TRIED, all six byte-identical at 9 except where noted:
 *   - naming the loaded field first (`t = a->f10; y = t >> 20;`) so that the
 *     load and the shift are separate statements -- this is the spelling that
 *     usually separates two values into two registers, and here it does nothing
 *   - `dir = (y > 8) ? -0x30 : 0x30` instead of the assign-then-override form
 *   - `y < 9` instead of `y <= 8`
 *   - the shared 0x40 as a literal at both call sites instead of a local
 *     (batch 94's non-signal -- correctly makes no difference)
 *   - withholding __Func_8010704's prototype, with and without the named field:
 *     WORSE, 13 of 43 both ways
 *
 * The prototype lever is the interesting negative here. Three of the nine
 * differences are exactly the argument-move rotation it addresses, and the ROM
 * wants r0 LATER in the final call, which is the direction that usually
 * responds to withholding the declaration. It does not: it costs four more.
 * That is consistent with the softened statement of the lever in
 * docs/elevation.md -- the direction is not predictable -- and it is the second
 * function where the rotation has some other cause.
 *
 * The gState base IS correctly a local here; the `mov r2, #0xfa / lsl r2, #1 /
 * add r3, r2` sequence matches, so the fold rule from batch 94 is satisfied and
 * is not what is wrong.
 */
struct A { unsigned char pad00[0x10]; int f10; };

extern unsigned char gState[];
extern struct A *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_954_200833c(int a, int b, int c);

void OvlFunc_954_200842c(void)
{
    unsigned char *g;
    struct A *a;
    int y;
    int dir;
    int e;

    g = gState;
    a = __MapActor_GetActor(*(int *)(g + 0x1f4));
    y = a->f10 >> 20;
    dir = -0x30;
    if (y <= 8)
        dir = 0x30;
    e = 0x40;
    __Func_8010704(0x43, 8, 3, 1, e, y);
    OvlFunc_954_200833c(0x11, 0, dir);
    __Func_8010704(0x40, 0x18, 3, 1, e, __MapActor_GetActor(0x11)->f10 >> 20);
}
