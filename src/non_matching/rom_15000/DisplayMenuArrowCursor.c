/* DisplayMenuArrowCursor (EmitPartySprites) -- NON-MATCHING.
 * NON-MATCHING: 16 encodings of 133 differ (objcmp).
 * asm/rom_15000/rom_1aeec_a_a_a_a.s.
 *
 * objcmp: "XX ENCODINGS differ in 16 place(s) (ref 133, ours 133)" -- SIZE EXACT.
 * tryc --align: 11 instruction(s) in disagreeing regions, of 130.
 *
 * Verify with:
 *   python3 tools/objcmp.py <this> asm/rom_15000/rom_1aeec_a_a_a_a.s --func DisplayMenuArrowCursor
 *
 * RESIDUE:
 *  1. (8 of the 11) each arm loads a[1].f0 / a[0].f0 TWICE (ldrh ... cmp ... ldrh);
 *     the ROM loads once and compares a COPY (`ldrh r2 / mov r3,r2 / cmp r3`).
 *     Traced in .00.rtl: the compare load is `(set (reg:HI) (mem:HI))` and the add
 *     load is `(zero_extend:SI (mem:HI))`, so cse cannot merge them.  The ROM's
 *     pattern is the one our own tail `if (a[i].f0) a[i].f0--;` produces (two
 *     HImode loads merged into a copy).  Tried: an int/uint/short/u16 local for
 *     the compare, for the add, for both, assigned inside the if; field declared
 *     `short`; OamSprite with u16 bitfield containers -- 11 at best, most 15-34.
 *  2. (3) x load order / AND operand (`mov r3,r12; and r3,r1`) and the i copy
 *     (`mov r1,r8` vs ours r2).  A local for x: inert or worse.
 */
struct OamSprite {
    unsigned char pad[4];
    unsigned int y:8;
    unsigned int affineMode:2;
    unsigned int objMode:2;
    unsigned int mosaic:1;
    unsigned int bpp:1;
    unsigned int shape:2;
    unsigned int x:9;
    unsigned int matrixNum:5;
    unsigned int size:2;
    unsigned int tileNum:10;
    unsigned int priority:2;
    unsigned int paletteNum:4;
};

struct Arrow {
    unsigned short f0;
    unsigned short f2;
    unsigned short slot;
    unsigned short tile;
    unsigned short x;
    short y;
    unsigned char pad0c[0x14];
    struct OamSprite oam;
    unsigned char pad2c[8];
};

struct Menu {
    unsigned char pad00[8];
    struct Arrow a[2];
    unsigned char pad70[0x394 - 0x70];
    unsigned short f394;
    unsigned short f396;
    unsigned short f398;
    unsigned short pad39a;
    unsigned short f39c;
};

extern unsigned char L342f8[] __asm__(".L342f8");
extern unsigned char L33ef8[] __asm__(".L33ef8");
extern unsigned int iwram_3001800;
extern int UploadSpriteGFX(int slot, int n, void *src);
extern int _GetFlag(int id);
extern void Func_8003dec(void *p, int n);

void DisplayMenuArrowCursor(struct Menu *m, int i)
{
    struct OamSprite *o;
    unsigned char *src;
    int f;

    f = (iwram_3001800 >> 2) & 7;
    if (m->a[i].f2 == 0)
        return;
    o = &m->a[i].oam;
    o->x = m->a[i].x;
    o->y = m->a[i].y;
    if (i != 0) {
        src = L342f8;
        if (m->a[1].f0 != 0)
            o->x = o->x + m->a[1].f0;
    } else {
        src = L33ef8;
        if (m->a[0].f0 != 0)
            o->x = o->x - m->a[0].f0;
    }
    o->tileNum = UploadSpriteGFX(m->a[i].slot, 0x80, src + f * 0x80);
    if (_GetFlag(0x103)) {
        if (*(unsigned short *)((unsigned char *)m + 0x2e2) == 1)
            o->objMode = 1;
        else
            o->objMode = 0;
    }
    Func_8003dec(o, 0xee);
    if (m->a[i].f0 != 0)
        m->a[i].f0--;
}
