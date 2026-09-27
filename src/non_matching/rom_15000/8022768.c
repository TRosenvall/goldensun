/* Func_8022768 -- asm/rom_15000/rom_21dfc_a_c_c_c_a.s   (PARK, fresh target)
 *
 * Clip (x, y, w, h) to the 30x20 BG map at iwram_3001e8c, then for each row set
 * or clear tile-attribute bit 12 (palette bank bit) from flag&1 and mark the row
 * group dirty in the byte at +0xea3 (bit 1 << (y>>2) ... i.e. 2 << (y >> 2)).
 * Note the y clip compares against 29 but clamps to 20 - y (0x14), as the ROM does.
 * Called with that signature by Func_802281c (EXACT, same .s).
 *
 * 77 encodings of 87 differ (objcmp: "ENCODINGS differ in 77 place(s) (ref 87,
 * ours 87)") -- same length, but the count is inflated by a register
 * permutation that runs through the whole body.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_15000/8022768.c asm/rom_15000/rom_21dfc_a_c_c_c_a.s --func Func_8022768
 *
 * BLOCKER: register allocation of the incoming arguments.  The ROM keeps y in
 * its incoming r1, moves h to r0 and w to r7 at once, and puts pal in r12
 * (with r5 as the reload scratch for r9/r12/r8); the mask 0xffffefff lives in r9,
 * the constant 2 in r10, base+0xea3 in r14.  Ours keeps x in r0 and
 * gives w/h r5/r4.  The ROM's shape is otherwise reachable:
 *   * the outer loop is a counted "for (j = 0; j < h; j++)" addressed with
 *     y + j -- loop reversal then makes h a down-counter in r0 and y + j an
 *     incrementing giv, which is exactly the ROM's "sub r0,#1 ... add r1,#1";
 *   * "if (w > 0)" guarding it gives the ROM's test order (w first, then h);
 *   * the shift must be unsigned: the ROM uses "lsr r3, r1, #2".
 * NOT reached: the ROM's inner entry test is "mov r2,r7 / cmp r2,#0 / beq"
 * (reversed counter tested != 0), ours "cmp r5,#0 / ble".
 *
 * INERT / WORSE (all measured): a do/while outer loop with h-- / y++ (78);
 * "for (i = w; i != 0; i--)" inner (89 lines, w lands in r12); unsigned i
 * (kills reversal); "for (i = 0; i != w; i++)" (69 but no reversal); p[i]
 * indexing (83 lines); short vs unsigned short map; mask spelled 0xefff or
 * "&= ~0x1000; |= pal"; pal named/unnamed/reusing the flag parameter; a raw
 * byte-offset "off" instead of &bg->map[y][x].
 */
struct Bg {
    short map[32][32];
    unsigned char pad[0xea3 - 0x800];
    unsigned char dirty;
};

extern struct Bg *iwram_3001e8c;

void Func_8022768(int x, int y, int w, int h, int flag)
{
    struct Bg *bg = iwram_3001e8c;
    int pal = (flag & 1) << 12;
    short *p;
    int i, j;

    if (x < 0) {
        w += x;
        x = 0;
    }
    if (x + w > 29)
        w = 30 - x;
    if (y < 0) {
        h += y;
        y = 0;
    }
    if (y + h > 29)
        h = 20 - y;
    if (w > 0) {
        for (j = 0; j < h; j++) {
            p = &bg->map[y + j][x];
            for (i = 0; i < w; i++) {
                *p = (*p & ~0x1000) | pal;
                p++;
            }
            bg->dirty |= 2 << ((unsigned int)(y + j) >> 2);
        }
    }
}
