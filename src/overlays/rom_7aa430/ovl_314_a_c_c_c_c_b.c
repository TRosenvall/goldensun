/* OvlFunc_923_2008cc0  --  0x02008cc0, cut from goldensun/asm/overlays/rom_7aa430/ovl_314_a_c_c_c_c.s.
 *
 * Preserves the original ROM layout when slotted after
 * asm/overlays/rom_7aa430/ovl_314_a_c_c_c_c.o in goldensun/overlays/rom_7aa430/overlay.ld.
 * Spawns one particle burst: fills a 0x28-byte descriptor on the stack, then
 * hands it to OvlFunc_common0_10c with the emitter's position nudged -- eight
 * sixteenths of a tile minus the low nibble of the frame counter in x, a fixed
 * 0x1a0000 in y -- and a randomised downward velocity.
 *
 * One of TWO byte-identical copies -- OvlFunc_923_2008cc0 and OvlFunc_924_2008cd0.
 * Found with tools/find_twins.py.
 *
 * MATCHED ON THE FIRST SCREEN, 62 lines against 62 with nothing differing, and
 * the two readings worth recording are both about what NOT to invent.
 *
 * THE MULTIPLY CHAIN IS gcc'S, NOT THE SOURCE'S:
 *
 *     lsl r4, r0, #1 / add r4, r0        u * 3
 *     lsl r3, r4, #4 / add r4, r3        ... * 17
 *     lsl r3, r4, #8 / add r4, r3        ... * 257     = u * 0x3333
 *
 * Six instructions with no multiply in sight, and it is one `u * 0x3333` in the
 * source. Written as the shift-and-add chain by hand it would not have matched;
 * written as the product it falls straight out. 0x3333 is a quarter of the
 * 0xcccc that goes into the descriptor twice, which is the tell that both are
 * the same magic number.
 *
 * THE DESCRIPTOR IS ONE AGGREGATE, NOT FOUR LOCALS. `add r6, sp, #0x10` once,
 * every field written through r6, and `str r6, [sp, #0xc]` passes its address
 * as the eighth argument. Four separate locals would not share a base register
 * and the frame would come out a different size; 0x38 is 0x10 of outgoing
 * argument space plus the 0x28 the descriptor occupies, and only its first four
 * words are ever written.
 *
 * `iwram_3001e40` is read twice -- once for the parity test that picks 7 or 5,
 * once for the low nibble of the nudge -- and the ROM keeps its ADDRESS in r5
 * across the call to __Random rather than the value, which is what two separate
 * reads of a global give.
 */

struct Emit {
    int f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[0x18];
};

struct Src {
    unsigned char pad00[8];
    int x;
    int y;
    int z;
};

extern unsigned int iwram_3001e40;
extern unsigned int __Random(void);
extern void OvlFunc_common0_10c(int a, int b, int c, int d,
                                int e, int f, int g, struct Emit *h);

int OvlFunc_923_2008cc0(struct Src *p)
{
    struct Emit s;
    unsigned int u;
    int k;

    s.f4 = 7;
    if ((iwram_3001e40 & 1) == 0)
        s.f4 = 5;
    s.f8 = 0xcccc;
    s.fc = 0xcccc;
    s.f0 = 0;
    u = (__Random() << 3) >> 16;
    k = u * 0x3333;
    OvlFunc_common0_10c(p->x + ((8 - (iwram_3001e40 & 0xf)) << 16),
                        p->y + (0xd0 << 13),
                        p->z,
                        0,
                        -k,
                        0,
                        0xb0 << 12,
                        &s);
    return 0;
}
