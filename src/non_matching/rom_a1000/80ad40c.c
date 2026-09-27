/* Func_80ad40c -- DrawDjinnActors -- NON-MATCHING, 73 encodings of 121 differ
 * (objcmp: ref 121 / ours 121, same size, same 0x24 frame).
 *
 * Verify with:
 *   python3 tools/objcmp.py /tmp/claude-0/-home-user-goldensun/ad08b1ee-c1a0-56c8-b3c2-c0ff6481844c/scratchpad/b287/V/Func_80ad40c.park.c \
 *     asm/rom_a1000/rom_ad274_c_a_a_c_a.s --func Func_80ad40c
 *
 * FRESH TARGET (batch 287).  The .s holds only this function.  Sibling of the landed
 * Func_80ad35c (rom_ad274_c_a_a_b.c); same Sprite/Blk structs plus int scl[4] at 0x244.
 *
 * THE SHAPE IS RIGHT, the residue is ONE allocation decision cascading.  The ROM keeps
 * yy (0x1e20000 - ys<<16) in r10 and spills the ys offset to [sp+4]; ours puts yy in
 * r4 by CALLER-SAVE (saved around __divsi3 at [sp]) and gives the ys offset r11.
 * Measured in .17.lreg/.18.greg: yy is pref LO_REGS, r5-r7 are taken, so global's
 * first find_reg fails and the caller-save retry succeeds on r4 because
 * CALLER_SAVE_PROFITABLE(refs 6, calls 1) = 4*1 < 6.  For the ROM's r10, either yy had
 * <= 4 refs / >= 2 calls crossed, or r4 was held by a higher-priority conflicting
 * pseudo when yy was allocated (then the GENERAL retry lands on r10 exactly, by
 * REG_ALLOC_ORDER 8,10,9,11).
 *
 * LEVER THAT GOT 76 -> 73: `sc = scale` ONCE, OUTSIDE the loop, stores as
 * `scale[0] = v; sc[1] = v;` and `_UpdateSprite(p, pos, sc, mode)`.  sc is then the
 * hoisted callee-saved pointer (the ROM's r11), the frame becomes the ROM's 0x24, and
 * `b->scl[i] = scale[0]` reloads from [sp+12] exactly as the ROM does (the int* store
 * through sc kills cse's memory of scale[0]).  The ROM's `mov r4, r11` in each arm is
 * then a RELOAD register inherited into the call argument -- it should appear by
 * itself once yy leaves r4.
 *
 * INERT (76): add operand order in pos[2]; -(ys<<16) + K; unsigned yy; pos[2] built
 * from pos[1]; scale[1] = -s vs = scale[0]; a static inline SetScale(p, v) helper;
 * &scale[0] / (int *)&scale spellings.  WORSE: yy after p->b2 (102); sc assigned in
 * each arm (99-112); sc assigned before `s =` inside the loop (106).
 */
struct Sprite {
    unsigned char pad00[9];
    unsigned char b0 : 2,
                  b2 : 2,
                  b4 : 4;
};

struct Blk {
    unsigned char pad000[0x114];
    struct Sprite *actors[8];
    short xs[8];
    short ys[8];
    unsigned char pad154[0x224 - 0x154];
    struct Sprite *actors2[4];
    short xs2[4];
    short ys2[4];
    int scl[4];
};

extern struct Blk *iwram_3001f2c;
extern void _UpdateSprite(struct Sprite *s, int *pos, int *scale, int mode);
extern void _Func_80219c8(int addr);

void Func_80ad40c(void)
{
    struct Blk *b;
    struct Sprite *p;
    int scale[2];
    int pos[4];
    int i;
    int yy;
    int s;
    int mode;
    int *sc;

    b = iwram_3001f2c;
    sc = scale;
    _Func_80219c8(0x6002500);
    for (i = 0; i < 4; i++) {
        p = b->actors2[i];
        if (p != 0) {
            yy = 0x1e20000 - (b->ys[i] << 16);
            p->b2 = 0;
            s = b->scl[i];
            if (s < 0) {
                scale[0] = -s;
                sc[1] = -s;
            } else {
                scale[0] = s + (0x10000 - s) / 3;
                sc[1] = scale[0];
                b->scl[i] = scale[0];
            }
            pos[0] = b->xs2[i] << 16;
            pos[1] = yy;
            pos[2] = (b->ys2[i] << 16) + yy;
            pos[3] = 0;
            mode = 0x8000;
            if (b->ys2[i] >= 0)
                mode = 0x4000;
            _UpdateSprite(p, pos, sc, mode);
        }
    }
}
