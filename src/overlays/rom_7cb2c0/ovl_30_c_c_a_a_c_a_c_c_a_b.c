/* OvlFunc_945_2009144  --  0x02009144
 *
 * Cut out of goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_a_a_c_a_c_c_a.s.
 *
 * Scans map-actor slots 8..0x41 and returns the first actor standing inside a
 * +/-12 box around the queried point, or NULL. Its one caller,
 * OvlFunc_945_2009280 next door, reads the result as a plain "is this tile
 * blocked" flag.
 *
 * BOTH FIELD READS ARE NAMED LOCALS, not direct field reads. The ROM issues
 * `mov r7,#0xa / ldrsh r2 / mov r7,#0x12 / ldrsh r4` back to back, before any
 * `cmp`. Written inline inside the `&&` chain, gcc sinks the second load past
 * the x tests, where the short circuit puts it -- 22 positions differ. This is
 * a third face of "one load kept across a call is a named local": there is no
 * call here at all, and the discriminator is instead EAGER VERSUS LAZY ISSUE
 * ACROSS A SHORT-CIRCUIT CHAIN. A load issued before its own guard has been
 * evaluated is a named local.
 *
 * THE FOUR BOUNDS ARE LOCALS, AND THEY ARE WRITTEN BEFORE `p`. Left inline
 * they are LICM-hoisted, and hoists land at the END of the preheader -- after
 * `add r3, #0x34`, four slots too early. Promoting them to locals is necessary
 * but NOT sufficient: with the locals written after `p` the residue is
 * identical. Once the expression is a source statement the preheader keeps
 * source order through sched2, because the five computations are mutually
 * independent and rank_for_schedule falls through to the original insn number.
 * The lever has two halves and both must be applied: promote to a local, AND
 * sweep its position.
 *
 * `bls` on the trip count makes `i` unsigned; signed gives `ble`.
 *
 * Not a source tell, do not chase: `neg`+`add` for `x - 12` rather than `sub`
 * is forced by Thumb-1, whose three-address `sub rd, rn, #imm` only encodes an
 * imm3, so a 12 with the source operand still live has to go via a register.
 */

struct A {
    unsigned char pad00[0xa];
    short fa;
    unsigned char pad0c_[0];
    int fc;
    unsigned char pad10[2];
    short f12;
};

extern char *iwram_3001ebc;

struct A *OvlFunc_945_2009144(int x, int y)
{
    char *blk;
    struct A **p;
    struct A *a;
    unsigned int i;
    int ax;
    int ay;
    int x0;
    int x1;
    int y0;
    int y1;

    blk = iwram_3001ebc;
    i = 8;
    x0 = x - 12;
    x1 = x + 12;
    y0 = y - 12;
    y1 = y + 12;
    p = (struct A **)(blk + 0x34);
    do {
        a = *p;
        p++;
        ax = a->fa;
        ay = a->f12;
        if (x0 < ax && x1 > ax && y0 < ay && y1 > ay)
            return a;
        i++;
    } while (i <= 0x41);
    return 0;
}
