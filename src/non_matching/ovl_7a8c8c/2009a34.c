/* OvlFunc_922_2009a34  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_c_a_c_c.s
 * Best screen: 18 instructions in disagreeing regions, of 50 (rom 50, ours 49).
 *
 * BLOCKER CLASS: register allocation, and one folded address computation.
 *
 * The area-id part is SOLVED -- `_AREA_3f` and `_AREA_40` were already defined
 * and the pool loads reproduce exactly. What remains is below that.
 *
 *  1. The ROM materialises the second pointer and then loads through it with a
 *     zero offset:  `add r3, r5, r1 / mov r1, #0 / ldrsh r2, [r3, r1]`.
 *     gcc folds the add into the load's register-offset form,
 *     `ldrsh r2, [r6, r2]`, which is one instruction shorter.
 *  2. Every register downstream is renamed as a consequence.
 *
 * WHAT WAS TRIED
 *   1. The offset zero as a named local (the usual form). 18 of 50.
 *   2. The offset zero inlined as `(unsigned int)0`, which is exactly the fix
 *      that took its sibling OvlFunc_921_200816c from 9 of 46 to an exact match
 *      in the same round. BYTE-IDENTICAL here.
 *
 * (2) is the useful record: that lever works when the competing value is
 * another ZERO, and does nothing when the problem is a folded ADDRESS. The two
 * look similar in the diff and are not the same defect.
 *
 * THE OFFSET DERIVATION IS RIGHT and should not be re-derived: the ROM builds
 * 0x16c with `mov r1, #0xb6 / lsl r1, #1`, uses it to index iwram_3001ebc, then
 * turns it into 0x1c0 with `add r1, #0x54` for the gState read. `k = 0xb6 << 1;
 * ... k = k + 0x54;` reproduces that reuse; two independent constants do not.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char *iwram_3001ebc;
extern int _AREA_3f;
extern int _AREA_40;
extern void OvlFunc_922_2009ad0(int a, int b);
extern int __GetFlag(int id);

void OvlFunc_922_2009a34(void)
{
    unsigned char *b;
    unsigned char *p;
    unsigned char *g;
    unsigned int k;
    int m;
    int v;
    int w;

    k = 0xb6 << 1;
    p = iwram_3001ebc + k;
    b = (unsigned char *)&gState;
    k = k + 0x54;
    m = *(short *)(p + (unsigned int)0);
    g = b + k;
    v = *(short *)(g + (unsigned int)0);
    if (v == (int)(&_AREA_3f)) {
        if (m == 0x11) {
            w = 0x20;
            w = -w;
            OvlFunc_922_2009ad0(0, w);
        } else {
            w = 0x20;
            w = -w;
            OvlFunc_922_2009ad0(w, 0);
        }
    }
    k = 0xe0 << 1;
    g = b + k;
    v = *(short *)(g + (unsigned int)0);
    if (v != (int)(&_AREA_40))
        return;
    if (m != 0x19)
        return;
    if (__GetFlag(0x309) == 0)
        return;
    OvlFunc_922_2009ad0(0, 0x20);
}
