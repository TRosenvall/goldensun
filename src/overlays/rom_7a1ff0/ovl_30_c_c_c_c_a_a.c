/* OvlFunc_914_2008abc  --  0x02008abc, cut from
 * goldensun/asm/overlays/rom_7a1ff0/ovl_30_c_c_c_c_a.s.
 *
 * The palette fade driver: walk palette entries 0..0xdf, skipping two ranges,
 * and pass each colour through the per-entry scaler (elevated in batch 79). One
 * of FOUR members, one per overlay, differing only in which four functions they
 * call.
 *
 * FOUND BY SHAPE. `tools/find_shape.py --clusters` groups every remaining
 * function by its instruction stream with constants, labels and call targets
 * wildcarded, and this came back as a four-member cluster with nothing solved
 * in it. Solving one and filling the captured call targets into the template
 * gave the other three, all clean on the first screen. find_twins.py cannot see
 * this family -- the members are not byte-identical, they call different
 * functions.
 *
 * THE LOOP COUNTER NEEDS A NAMED INTERMEDIATE. Written `i += 0x10000;` and
 * tested on `i`, gcc increments in place and the function comes out ONE
 * INSTRUCTION SHORT of the ROM:
 *
 *     rom    add r3, r6, r2 / ... / mov r6, r3 / cmp r3, r2 / bls
 *     ours   add r6, r2     / ... / cmp r6, r3 / bls
 *
 * Assigning through `n` and testing `n` gives the ROM's three-operand add and
 * the copy back. Testing `i` after assigning through `n` is NOT enough -- the
 * intermediate has to be what the comparison reads.
 *
 * Both range tests are nested `if`s rather than `&&`, and the second is written
 * `(unsigned short)(u - 0xc1) > 7` so the comparison stays in the high half,
 * which is what the ROM's `lsl #16 / cmp against 0x70000` is.
 */
extern void OvlFunc_914_2008b8c(void);
extern void OvlFunc_914_2008bcc(void);
extern void OvlFunc_914_2008bac(void);
extern unsigned short OvlFunc_914_2008b24(unsigned short c, int s);
extern void __Func_8091200(int a, int b);

void OvlFunc_914_2008abc(int scale)
{
    unsigned int i;
    unsigned int u;
    unsigned short *p;
    unsigned int n;

    OvlFunc_914_2008b8c();
    i = 0;
    do {
        u = i >> 16;
        if ((unsigned int)(i - 0x110000) > (0xc0 << 11)) {
            if ((unsigned short)(u - 0xc1) > 7) {
                p = (unsigned short *)((0xa0 << 19) + u * 2);
                *p = OvlFunc_914_2008b24(*p, scale);
            }
        }
        n = i + (0x80 << 9);
        i = n;
    } while (n <= (0xdf << 16));
    OvlFunc_914_2008bcc();
    OvlFunc_914_2008bac();
    __Func_8091200(0x80 << 9, 0);
}
