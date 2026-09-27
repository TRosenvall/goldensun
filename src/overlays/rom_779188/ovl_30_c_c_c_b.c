/* OvlFunc_879_2008238  --  0x02008238, split out of
 * asm/overlays/rom_779188/ovl_30_c_c_c.s; OvlFunc_879_20082e8 stays in _c_a.s
 * and the data/bss in _c_c.s (.L68c and .L6a0, both .lcomm, were exported for
 * the split). Matched from scratch.
 *
 * A `volatile int *` store pointer gives `stmia rN!, {rX}` inside the loop:
 * through a plain pointer cse2 folds the post-increments into fixed offsets
 * plus one add. The loop-invariant constant in r8 needed a named local
 * assigned just before the loop, and its expression a named sub-term `y`.
 */
struct SpriteSlot {
    unsigned short size;
    unsigned short vramOffset;
};

struct Oam {
    int a;
    int b;
    int c;
};

extern unsigned char L650[] __asm__(".L650");
extern unsigned char L68c[] __asm__(".L68c");
extern struct Oam L6a0[] __asm__(".L6a0");
extern struct SpriteSlot gSpriteSlots[];
extern int iwram_3001e40;
extern void __Func_8003dec(struct Oam *p, int n);

void OvlFunc_879_2008238(void)
{
    struct Oam *q;
    volatile int *p;
    int tile;
    int i;
    int n;
    int x;
    int y;

    q = L6a0;
    p = (volatile int *)q;
    tile = gSpriteSlots[*(short *)L650].vramOffset >> 5;
    i = 0;
    x = 0x88;
    for (; i <= 0x11; i++) {
        y = 0xe8 - (0x12 - i) * 8;
        *p++ = 0;
        *p++ = (y << 16) | x | 0x8400;
        *p++ = 0xf000 | tile;
        n = *(short *)L68c / 2 - i;
        if (n < 0)
            n = 0;
        if (n <= 2 && (iwram_3001e40 & 1))
            n = 0;
        if (n)
            __Func_8003dec(q++, 0xff);
        tile += 2;
    }
    (*(short *)L68c)++;
}
