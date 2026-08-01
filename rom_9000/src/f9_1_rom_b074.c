/* Func_b074 -- SetSpritePairPosition
 *
 * Positions an object's two OAM entries (main at +0x00, companion/shadow at
 * +0x0C) from world coordinates and a scale pair.  See f9_1_rom_b074.s for
 * the ROM's disassembly and its annotation.
 *
 * STATUS: NOT MATCHING -- 23 of ~61 instructions aligned.  f9_1_rom_b074.s is
 * present, so the .s rule wins and THIS FILE IS NOT BUILT.  Delete the .s to
 * switch over, and expect the checksum to fail until it matches.
 *
 *     tools/asmdiff.py Func_b074 rom_9000/src/f9_1_rom_b074.c \
 *         --rom-offset 0xb074 --rom-size 244
 *
 * Known remaining gap: the ROM uses FOUR high registers (ip=obj, fp=a3,
 * sl=a2, r9=mode, plus r8=a1>>16 assigned late), and critically it never saves
 * a1 -- a1 stays in r1 while the scale pointer is loaded into r2.  agbcc loads
 * the scale pointer into r1 instead, which forces a1 out to a high register and
 * changes the whole prologue.  Getting agbcc to pick r2 there is the next thing
 * to try; pinning a2/a3 to r10/r11 (done below) and computing a1>>16 early each
 * helped a little, combining them did not.
 *
 * This is materially harder than Func_b684: 61 instructions, four high
 * registers, two stack locals and an embedded literal pool whose placement must
 * also match.  Worth revisiting with decomp-permuter (see docs/matching.md).
 */
typedef unsigned char u8;
typedef signed char s8;

void Func_b074(void *obj, int a1, int a2, int a3, int a4, int *scale)
{
    register int aa2 asm("r10");
    register int aa3 asm("r11");
    u8 *hp;
    int halfW, halfH;
    int mode;
    int offA, offB;
    int sx, sy;
    int t, d, h, corr;
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
