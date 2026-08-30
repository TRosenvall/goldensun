/*
 * OvlFunc_909_200809c -- asm/overlays/rom_79c738/ovl_30_c_c_a_a_a_a.s
 *
 * BLOCKER: gcc chains four byte stores that the ROM recomputes. 39 lines
 * against 41 -- TWO SHORT.
 *
 * The four stores are at +0xa6, +0xbe, +0xd6 and +0xee, which are all 0x18
 * apart. The ROM recomputes the address from the base for the FIRST THREE and
 * chains only the fourth (`add r2, #0x18`). gcc sees the arithmetic
 * progression and strength-reduces all four into one walking register, which
 * is two instructions cheaper.
 *
 * TRIED AND REJECTED:
 *
 *   * Writing the first three as base-relative subscripts and the fourth
 *     through an explicitly advanced pointer (`q = p + 0xd6; *q = 3;
 *     q += 0x18; *q = 1;`), to reproduce exactly that split. NO CHANGE --
 *     byte-identical. gcc re-derives the progression regardless of how the
 *     source spells it.
 *
 * Note the fourth store cannot be `q[0x18]`: a Thumb `strb` immediate offset
 * reaches 31, so `q[0x18] = 1` would be ONE instruction and the ROM has two.
 * The advance is real, it just is not enough on its own.
 *
 * SETTLED: the area comparison is against a POOLED 0x21, which an eight-bit
 * mov could build, so it is `(int)&_AREA_21` -- and here the field genuinely is
 * the area halfword at gState+0x1C0, so area.sym is the right space. The
 * gState offset also needs the local-pointer form or gcc folds it into
 * `ldr =gState+448`.
 */
extern unsigned char gState[];
extern int _AREA_21;
extern unsigned char L29b4[] __asm__(".L29b4");
extern unsigned char L299c[] __asm__(".L299c");
extern void __Func_808b868(unsigned char *p);
extern int __GetFlag(int id);

unsigned char *OvlFunc_909_200809c(void)
{
    unsigned char *gp;
    unsigned char *p;

    gp = gState;
    if (*(short *)(gp + (0xe0 << 1)) == (int)&_AREA_21) {
        p = L29b4;
        __Func_808b868(p);
        if (__GetFlag(0x84e)) {
            p[0xa6] = 2;
            p[0xbe] = 0;
            p[0xd6] = 3;
            p[0xee] = 1;
        }
        return p;
    } else {
        return L299c;
    }
}
