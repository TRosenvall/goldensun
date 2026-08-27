/* OvlFunc_956_2008b30 -- NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7e0928/ovl_30_c_c_a_c.s
 * Best screen: 3 differing of 47, streams the same length.
 *
 * BLOCKER CLASS: which register holds a pooled mask, and when it is loaded.
 *
 *     rom    ldr r2, =0xfff00000 / mov r0, #0xc0 / and r3, r2 / lsl r0, #12
 *     ours   ldr r0, =0xfff00000 / and r3, r0 / mov r0, #0xc0 / lsl r0, #12
 *
 * The ROM interleaves the addend's construction between the pool load and the
 * `and`, which frees r0 for it; we finish the mask first and then reuse r0.
 * Three instructions, same length, same operands.
 *
 * WHAT WAS TRIED, all byte-identical at 3 except where noted:
 *   - a named `int` for the mask (batch 97's type lever -- the mask and the
 *     field are both `int` here, so there is no width to change)
 *   - named locals for BOTH the mask and the 0xc0 << 12 addend
 *   - `0xfff00000 & a->f10` instead of `a->f10 & 0xfff00000`
 *   - computing the whole third argument into a local in two statements
 *     (WORSE, 9 of 47 -- it moves the `and` above the SetAnim call)
 *
 * Everything else is right: the gState slot pointer held in r6 across three
 * calls, the clamp at 0xa6 << 18, and the two `mov`+`lsl` constants.
 */
struct A {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x30 - 0x14];
    int f30;
    int f34;
};

extern unsigned char gState[];
extern struct A *__MapActor_GetActor(int slot);
extern void __Actor_SetAnim(struct A *a, int n);
extern void __Actor_TravelTo(struct A *a, int x, int y, int z);
extern void __Actor_WaitMovement(struct A *a);
extern void __MapActor_Surprise(int slot, int n);
extern void __Func_8092708(int a, int b, int c);

void OvlFunc_956_2008b30(void)
{
    unsigned char *g;
    int *slot;
    struct A *a;
    int lim;

    g = gState;
    slot = (int *)(g + (0xfa << 1));
    a = __MapActor_GetActor(*slot);
    lim = 0xa6 << 18;
    if (a->f8 > lim)
        a->f8 = lim;
    a->f34 = 0x80 << 9;
    a->f30 = 0x80 << 10;
    __Actor_SetAnim(a, 5);
    __Actor_TravelTo(a, a->f8, a->fc, (a->f10 & 0xfff00000) + (0xc0 << 12));
    __Actor_WaitMovement(a);
    __MapActor_Surprise(*slot, 0x81 << 1);
    __Func_8092708(*slot, 6, 0);
}
