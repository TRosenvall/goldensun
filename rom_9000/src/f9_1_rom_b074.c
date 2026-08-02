/* Func_b074 -- SetSpritePairPosition
 *
 * Positions an object's two OAM entries (main at +0x00, companion/shadow at
 * +0x0C) from world coordinates and a scale pair.  See f9_1_rom_b074.s for
 * the ROM's disassembly and its annotation.
 *
 * STATUS: NOT MATCHING, AND NOT REACHABLE WITH agbcc.
 *
 * The ROM allocates lr (r14) as a general-purpose register -- `mov r14, r3`
 * caches obj+0x21 there for the length of the function.  It can do that because
 * this is a leaf: nothing calls out, so lr is dead after the prologue saves it.
 *
 * agbcc never does this.  Across all 161 functions this project builds from C,
 * it emits `mov lr, ...` exactly zero times.  The ROM does it in 117 functions.
 * That is a capability the original compiler has and agbcc does not, so no C
 * input reaches this shape -- the same kind of wall as the register-offset
 * addressing mode, and the reason the permuter plateaus here.
 *
 * What IS fixed below: `h` is unsigned, so the halving is `lsr` as in the ROM
 * rather than `asr`.  That was a real defect in the earlier draft.
 *
 * What remains, and is not worth chasing until the compiler question settles:
 *   - four high registers saved (r8-r11); agbcc saves three, because it has one
 *     fewer long-lived value to place once lr is off the table,
 *   - the halfword clear-mask is 0xFFFFFE00 in the ROM.  agbcc folds it to
 *     0xFE00 because `ldrh` tells it the value is 16-bit.  Writing `~0x1ff` in
 *     the C does not change this -- verified.
 *
 * See docs/matching.md, "A fourth obstacle".
  */
typedef unsigned char u8;
typedef signed char s8;

void Func_b074(void *obj, int a1, int a2, int a3, int a4, int *scale)
{
    int aa2;
    int aa3;
    u8 *hp;
    int halfW, halfH;
    int mode;
    int offA, offB;
    int sx, sy;
    int t, d, corr;
    unsigned int h;
    int x1, y1, x2, y2;
    u8 *o2;

    halfW = *(u8 *)((u8 *)obj + 0x20) >> 1;
    hp    = (u8 *)obj + 0x21;
    halfH = *hp >> 1;

    offB = 8;
    offA = 4;

    aa2 = a2;
    aa3 = a3;
    mode = 1;
    sx = *scale++;
    sy = *scale;

    if (sx > 0x10000 || sy > 0x10000) {
        mode = 3;
        offB = 16;
        offA = 8;
        halfW <<= 1;
        halfH <<= 1;
    }

    t = a1 >> 16;
    x1 = t - halfW;

    d = aa3 - aa2;
    h = *hp;
    corr = (s8)*(u8 *)((u8 *)obj + 0x23);
    h = ((h >> 1) - corr) * sy;
    d = (d >> 16) - halfH;
    y1 = d - ((h + 0xffff) >> 16);

    *(u8 *)((u8 *)obj + 5) = (*(u8 *)((u8 *)obj + 5) & ~3) | mode;
    *(unsigned short *)((u8 *)obj + 6) =
        (*(unsigned short *)((u8 *)obj + 6) & 0xfe00) | (x1 & 0x1ff);
    *(u8 *)((u8 *)obj + 4) = y1;

    x2 = t - offB;
    y2 = ((aa3 - a4) >> 16) - offA;

    o2 = (u8 *)obj + 0xc;
    o2[5] = (o2[5] & ~3) | mode;
    *(unsigned short *)(o2 + 6) =
        (*(unsigned short *)(o2 + 6) & 0xfe00) | (x2 & 0x1ff);
    o2[4] = y2;
}
