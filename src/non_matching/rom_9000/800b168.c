/* UpdateSprite / SubmitSpritePairWithLabel (0x0800b168) -- NON-MATCHING:
 * 245 encodings of 260 differ (objcmp).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_9000/UpdateSprite.c asm/rom_9000/rom_b074_a_c.s \
 *       --func UpdateSprite
 *
 * 245 IS NOT A DISTANCE: size 544 against 520, instructions 260 against 249.
 * Register-blind aligned (scratch_elev/b291/D/dis5.sh, a hand-rolled shim) it is
 * 192 of the ROM's 260 matching, up from 162 on the first candidate.  ELEVEN
 * INSTRUCTIONS AND ONE STACK SLOT SHORT, and four of the eleven are localised.
 *
 * NOTE ON THE .s ITSELF: its header comment says "the live build compiles
 * f9_2_rom_b168.c".  THAT IS STALE -- stage1.ld:110 links
 * asm/rom_9000/rom_b074_a_c.o, and neither the Makefile nor any .ld mentions the
 * tracked legacy tree at rom_9000/src/.  I did not read that tree.
 *
 * WHAT IS RIGHT -- this function is almost entirely bitfield plumbing and the
 * layout is now pinned:
 *   - THE TWO OAM ENTRIES ARE ONE STRUCT TYPE AT obj+0 AND obj+0xc, WITH THE
 *     ENTRY AT +4 OF IT.  The ROM reaches the companion as `obj + 0xc` and then
 *     uses offsets 4..7 off THAT, which is only consistent with a nested object of
 *     the same type -- `Func_8003dec(obj, prio)` and `Func_8003dec(child, kind)`.
 *     Writing the entry as a struct based at obj+4 gives offsets 0..3 and misses.
 *   - THE ENTRY FIELDS ARE BITFIELDS, AND THAT IS WHAT PRODUCES THE ROM'S MASKS.
 *     `unsigned char mode : 2` at +5 gives `movs r3,#4 / negs r3,r3` for ~3 --
 *     store_bit_field builds the mask in SImode -- where an `unsigned char`
 *     lvalue masked by hand gives the narrowed `movs r3,#252`.  `unsigned short
 *     x : 9` at +6 gives `ldrh/and ~0x1ff/orr/strh` and `unsigned short mtx : 5`
 *     gives `ldrb [r0,#7]` because bits 9-13 of the halfword are bits 1-5 of the
 *     high byte.  This is the GBA OAM attr0/attr1 layout.
 *   - THE AFFINE REQUEST BLOCK IS NOT BITFIELDS.  Declared as `unsigned int :16`
 *     fields gcc narrows every store to `strh`; the ROM's `ldr/and/orr/str`
 *     insert pattern comes from EXPLICIT MASKING ON INT WORDS, reading the
 *     UNINITIALISED local (the high halves are never written and the frame's last
 *     0x20 bytes are the block).  The flip's negation reads back through a
 *     halfword lvalue (`ldrh r3,[r0]`), so the block wants a union of `int w[2]`
 *     and `unsigned short h[4]`.
 *   - `(unsigned short)(sx >> 8)` is the ROM's two-instruction `lsl #8 / lsr #16`;
 *     `(sx >> 8) & 0xffff` is three (`asr #8 / and`).
 *   - THE DEPTH TEST IS SIGNED.  `pz > 0xff9c0000` makes the constant `unsigned`
 *     in C and gcc emits `bls`; the ROM's `bgt` needs `pz <= -0x640000`, and the
 *     `prio = 1` arm must be the `then` so gcc lays the blocks out in the ROM's
 *     order.
 *   - THE ROUNDED SCALES ARE SHIFTS, NOT DIVISIONS.  `(sx * (signed char)obj[0x22]
 *     + 0xffff) >> 16` is the ROM's unconditional `add 0xffff / asr #16`;
 *     `/ 0x10000` would add the `cmp/bge` rounding branch.
 *   - `signed char` for the +0x22/+0x23 offsets: `s8` in include/gba/types.h is
 *     plain `char`, which is UNSIGNED here.
 *   - `px >> 16` MUST APPEAR TWICE, once inside the companion branch and once
 *     after it.  Computing it once into a local before the branch is 10 fewer
 *     register-blind matches: the ROM's second `asr r5, r2, #16` at .Lb2d8 is a
 *     real second reference, and it is what earns `px` a high register instead of
 *     a stack slot.
 *   - The four-word position block is walked with `*p++`; only words 0, 1, 2 and 3
 *     are used and the ROM's priority test reads word 1 while its shift reads
 *     word 2, which the .s header comment conflates into "z".  Reproduced as-is.
 *
 * WHERE THE ELEVEN ARE:
 *   - FOUR ARE ONE FOLD gcc DOES AND THE ROM DID NOT.  The ROM computes
 *     `((w0 & 0xffff0000) | lo) & 0xffff | hi16` and keeps the dead
 *     `& 0xffff0000`; gcc proves it dead and emits `lo | hi16` with no read at
 *     all.  Two statements, four statements, and NAMED LOCALS FOR THE TWO MASKS
 *     (the recorded `r = 0x1f; r &= c;` lever) are all inert -- combine folds it
 *     through the register too.
 *   - The rest is the file's usual register question: the ROM spills the two
 *     half-extents (sp+0x18, sp+0x1c) and keeps the x coordinate in r10, ours does
 *     the reverse, and that is the missing ninth stack slot (frame 0x44 vs 0x40).
 *
 * NEXT: the dead `& 0xffff0000`.  It is four instructions and it is the only
 * difference here that is not register allocation, so it is the one worth a
 * session -- try making the first insert's result live (a second reader) rather
 * than trying to stop combine.
 */
#include "gba/types.h"

union AffineReq {
    int w[2];
    unsigned short h[4];
    int pad[8];
};

struct Spr {
    int f0;
    unsigned char y;
    unsigned char mode : 2;
    unsigned char f5 : 6;
    unsigned short x : 9;
    unsigned short mtx : 5;
    unsigned short f6 : 2;
};

extern int UpdateSpriteAnim(unsigned char *obj, int style);
extern int Func_8003d28(union AffineReq *req);
extern void Func_8003dec(struct Spr *e, int prio);

int UpdateSprite(unsigned char *r0, int *pos, int *scale, u16 style)
{
    unsigned char *obj;
    unsigned char *p21;
    struct Spr *e;
    union AffineReq blk;
    int sx;
    int sy;
    int px;
    int pz;
    int py;
    int pg;
    int flip;
    int rot;
    int mode;
    int mtx;
    int hw;
    int hh;
    int hx2;
    int hy2;
    int prio;
    int kind;
    int X;
    int Y;

    obj = r0;
    hw = *(obj + 0x20) >> 1;
    p21 = obj + 0x21;
    hh = *p21 >> 1;
    hx2 = 8;
    hy2 = 4;
    sx = *scale++;
    px = *pos++;
    sy = *scale;
    pz = *pos++;
    py = *pos++;
    pg = *pos;
    flip = UpdateSpriteAnim(obj, style);
    if (flip == 0 && sx == 0x10000 && sy == sx && *(unsigned short *)(obj + 0x1e) == 0) {
        mode = 0;
        mtx = 0;
    } else {
        rot = *(unsigned short *)(obj + 0x1e);
        mode = 1;
        blk.w[1] = (blk.w[1] & 0xffff0000) | rot;
        blk.w[0] = (blk.w[0] & 0xffff0000) | (unsigned short)(sx >> 8);
        blk.w[0] = (blk.w[0] & 0xffff) | ((unsigned short)(sy >> 8) << 16);
        if (flip != 0)
            blk.w[0] = (blk.w[0] & 0xffff0000) | (unsigned short)-blk.h[0];
        mtx = Func_8003d28(&blk);
    }
    if (sx > 0x10000 || sy > 0x10000) {
        hw <<= 1;
        hh <<= 1;
        mode = 3;
        hx2 = 0x10;
        hy2 = 8;
    }
    if (pz <= -0x640000) {
        prio = 1;
        kind = 0;
    } else {
        prio = (py >> 17) + 0xa;
        kind = 2;
    }
    Y = ((py - pg) >> 16) - hy2;
    if ((*(obj + 0x26) & 1) != 0 && Y <= 0x9f) {
        e = (struct Spr *)(obj + 0xc);
        e->mode = mode;
        mtx &= 0x1f;
        e->mtx = mtx;
        e->x = (px >> 16) - hx2;
        e->y = Y;
        Func_8003dec(e, kind);
    }
    X = ((px >> 16) - hw) + ((sx * (signed char)*(obj + 0x22) + 0xffff) >> 16);
    Y = ((py - pz) >> 16) - hh;
    Y = Y - ((sy * ((*p21 >> 1) - (signed char)*(obj + 0x23)) + 0xffff) >> 16);
    if (X <= 0xef && Y <= 0x9f) {
        e = (struct Spr *)obj;
        e->x = X;
        e->y = Y;
        e->mode = mode;
        mtx &= 0x1f;
        e->mtx = mtx;
        Func_8003dec(e, prio);
    }
    return 0;
}
