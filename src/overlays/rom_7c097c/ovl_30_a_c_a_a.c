/* OvlFunc_936_2008040  --  0x02008040
 *
 * The whole of goldensun/asm/overlays/rom_7c097c/ovl_30_a_c_a_a.s, which held
 * this function and no data.
 *
 * An idle behaviour for a wandering villager, run once per frame: while the
 * countdown at +0x66 is running, tick it down; when it reaches zero, roll for a
 * new pose -- stand, sit, or turn by a random angle -- and roll a new countdown.
 *
 * THE SWITCH VALUE IS UNSIGNED, and that is the largest single decision. The
 * ROM dispatches with `bcc` and `bhi`:
 *
 *      cmp r0, #1 / beq <case 1>
 *      cmp r0, #1 / bcc <case 0>
 *      cmp r0, #4 / bhi <default>
 *      cmp r0, #3 / bcc <default>
 *      b <cases 3 and 4>
 *
 * An `int` switch value gives the signed forms (`bgt`) and 46 differing of 55;
 * making it `unsigned` takes that to 11. The four case labels are also what
 * produce the decision tree rather than an equality chain -- batch 91's rule,
 * which needs three or more.
 *
 * THE DECREMENT IS WRITTEN TWICE SO THE RETURN IS SHARED. Both exits return 1,
 * and the ROM puts `mov r0, #1` at the tail with the decrement block reached
 * from two places. Written with an early `return 1` and one decrement after the
 * `if`, gcc hoists the constant above the test instead (11 differing).
 * Duplicating the two-line decrement into both branches lets gcc cross-jump
 * them and leaves the constant at the tail.
 *
 * That is batch 96's return-block lever in the case where there is only ONE
 * return value: with nothing to choose between, what decides the layout is
 * whether the source gives gcc two identical tails to merge.
 *
 * The three shift pairs are 16-bit extracts at an offset -- `<< 3 >> 16` is
 * bits 13..28, `<< 15 >> 16` is bits 1..16, `* 5 << 4 >> 16` is the countdown
 * scaled to roughly 0..80. Batch 93's shape.
 */
struct A { unsigned char pad00[6]; unsigned short f6; };

extern unsigned int __Random(void);
extern void __Actor_SetAnim(struct A *a, int n);

int OvlFunc_936_2008040(struct A *a)
{
    short *p;
    unsigned int v;
    int t;

    p = (short *)((char *)a + 0x66);
    if (*p == 0) {
        v = __Random() << 3 >> 16;
        switch (v) {
        case 0:
            __Actor_SetAnim(a, 3);
            break;
        case 1:
            __Actor_SetAnim(a, 4);
            break;
        case 3:
        case 4:
            a->f6 = a->f6 + (__Random() << 15 >> 16);
            break;
        }
        t = __Random() * 5 << 4 >> 16;
        *(unsigned short *)p = t;
        if (t != 0) {
            t = *(unsigned short *)p;
            *(unsigned short *)p = t - 1;
        }
        return 1;
    }
    t = *(unsigned short *)p;
    *(unsigned short *)p = t - 1;
    return 1;
}
