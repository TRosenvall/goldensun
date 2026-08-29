/* OvlFunc_965_200a660  --  asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c_c_c.s
 *
 * BLOCKER CLASS: register birth order -- two callee-saved registers exchanged,
 * and every use of them with it.
 * Status: 30 lines against 30, 19 differing, all of them r5/r6 and r0/r1.
 *
 * WHAT IT DOES
 * Builds a three-word argument block on the stack -- two coordinates offset by
 * a table entry split into its high and low halves, and a value from
 * __Func_8011f54 -- and passes it to OvlFunc_965_200806c.
 *
 * THE DIFFERENCE. The ROM keeps the actor in r5 and the stack block in r6; we
 * do the opposite. REG_ALLOC_ORDER reaches r5 before r6, so the ROM created the
 * actor's pseudo first and ours creates the block's first. The block's pseudo
 * is created when gcc lowers the local array's address, which happens ahead of
 * any statement -- so no reordering of the body moves it.
 *
 * A NOTE ON SCREENING THIS ONE. tryc.py picks per-file flags from the Makefile,
 * and the wildcard rule `src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c` gives
 * this path -O1. The tool prints its own warning that a wildcard rule may
 * belong to a NEIGHBOURING translation unit, and it is right to: at -O1 the diff
 * is 22 lines, at -O2 it is 19. The numbers above are the -O2 ones. Anyone
 * retrying this must pass --cflags "-O2" or they are measuring the wrong
 * compiler.
 *
 * The body reading is believed right: every instruction is present, and the
 * table entry split -- `hi = 0xffff0000 & v` added to one coordinate, `v << 16`
 * added to the other -- reproduces the ROM's `and r3, r0` / `lsl r0, #16` pair
 * exactly, including which operand is the destination.
 */

struct A {
    unsigned char pad00[6];
    unsigned short f6;
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[0xe];
    unsigned char f22;
};

extern int L2fd4[] __asm__(".L2fd4");
extern int __Func_8011f54(int a);
extern void OvlFunc_965_200806c(int *p, struct A *a);

void OvlFunc_965_200a660(struct A *a)
{
    int t[3];
    int v;
    int hi;

    v = L2fd4[a->f6 >> 12];
    hi = 0xffff0000 & v;
    t[0] = a->f8 + hi;
    t[2] = a->f10 + (v << 16);
    t[1] = __Func_8011f54(a->f22);
    OvlFunc_965_200806c(t, a);
}
