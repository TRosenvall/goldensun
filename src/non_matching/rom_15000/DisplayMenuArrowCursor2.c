/* DisplayMenuArrowCursor2 (DrawPartyPanel) -- NON-MATCHING.
 * NON-MATCHING: 34 encodings of 140 differ (objcmp).
 * asm/rom_15000/rom_1aeec_a_a_c_a_c_a_a.s.
 *
 * objcmp: "XX ENCODINGS differ in 34 place(s) (ref 140, ours 140)" -- SIZE EXACT.
 * tryc --align: 15 instruction(s) in disagreeing regions, of 141.
 *
 * Verify with:
 *   python3 tools/objcmp.py <this> asm/rom_15000/rom_1aeec_a_a_c_a_c_a_a.s --func DisplayMenuArrowCursor2
 *
 * Struct layout shared with DisplayMenuArrowCursor (0x34-byte Arrow records at
 * m+8, OamSprite as in the landed Func_801c0dc).  Bitfield order matches.
 *
 * RESIDUE, three pieces:
 *  1. prologue: ROM computes o = (m + i*0x34) + 0x28 and a = m + (i*0x34 + 8)
 *     separately; ours shares m + i*0x34.  Explicit casts / order swaps: inert.
 *  2. src: ROM loads L342f8 into r5 early in the i!=0 arm and copies it into r11
 *     at the arm's end.  A block-local `t` reproduces the r5 load BUT src then
 *     loses its hi-register class and is caller-saved to the stack around
 *     AllocSpriteSlot (sub sp,#4), and the `src = 0` that feeds `a->f2 = 0`
 *     through r11 disappears.  `a->f2 = (int)src`, a second temp in the else
 *     arm, dropping `src = 0`: all worse (24-46).
 *  3. o in r8 vs ROM r10, &a[i].x in r10 vs ROM r8 (priority swap).
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
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int n, void *src);

void DisplayMenuArrowCursor2(struct Menu *m, int i)
{
    struct OamSprite *o;
    struct Arrow *a;
    void *src;
    unsigned int n;

    a = &m->a[i];
    o = &a->oam;
    src = 0;
    a->f2 = 0;
    if (i != 0) {
        n = m->f394;
        src = L342f8;
        if (m->f39c != 0)
            n -= m->f39c;
        if (n > 5) {
            a->f2 = 1;
            n = 5;
        }
        m->a[1].x = m->f396 + (n - 1) * 16 + 0x11;
    } else {
        src = L33ef8;
        m->a[0].x = m->f396 - 9;
        if (m->f39c != 0)
            m->a[0].f2 = 1;
    }
    if (m->a[i].y == 0) {
        m->a[i].slot = AllocSpriteSlot();
        m->a[i].tile = UploadSpriteGFX(m->a[i].slot, 0x80, src);
        m->a[i].y = m->f398;
        m->a[i].f0 = 0;
        o->objMode = 0;
        o->mosaic = 0;
        o->bpp = 1;
        o->affineMode = 0;
        o->matrixNum = 0;
        o->size = 0;
        o->shape = 2;
        o->priority = 0;
    }
}
