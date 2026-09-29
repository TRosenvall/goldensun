/* Func_800b388 / SubmitSpriteProjected (0x0800b388) -- NON-MATCHING, 185 of 363 encodings differ.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_9000/800b388.c asm/rom_9000/rom_b074_a_c.s \
 *       --func Func_800b388
 *
 * SIZE, COUNT AND THE WHOLE RELOCATION SEQUENCE MATCH: 764 bytes, 363 encodings,
 * eleven relocations in the ROM's order (PhysMove, UpdateSpriteAnim, Func_8003d28,
 * iwram_3001e68, Func_8000888, Func_8003dec x2, PhysMove, Func_8003dec x2,
 * Func_8003f78), and the LITERAL POOL lands at the ROM's own offset -- the two
 * ABS32 words are at 0x1a8/0x1ac in both.  aligncmp reads 254 of 363
 * aligned-equal = 70.0%.  So 185 is a real distance and it is ONE stack slot wide.
 *
 * THE ONE DIFFERENCE: `sub sp, #0x38` against our `sub sp, #0x34`.  The ROM
 * SPILLS `flip` (sp+0xc, one `str` after UpdateSpriteAnim and two `ldr`s) and
 * keeps `sy` in r14; we keep `flip` in r14 and give `sy` r4.  Both assignments
 * are legal -- there is no call between UpdateSpriteAnim and either flip test,
 * and `sy` dies before Func_8003d28 -- and both come out at exactly 363
 * instructions, so the ROM's choice costs it a `str`/two `ldr`s and buys back
 * the three-instruction r14 clamp (`mov r1,#0xfc / lsl r1,#9 / mov r14,r1`).
 *
 *     ROM   r4=q  r5=d,s,sx  r6=fxmul-ptr then -oy  r7=obj  r8=ox  r9=p21
 *           r10=&v  r11=hw  r14=sy   flip at sp+0xc
 *     ours  r4=q then sy  r5=s then fxmul-ptr  r6=sx  r7=obj  r8=ox
 *           r9=p21 then -oy  r10=&v  r11=hw  r14=flip
 *
 * With nine callee-usable registers and ten values live across the
 * fx32_multiply sites, exactly one has to go to memory.  The ROM packs `d`,
 * `s` and `sx` into one register (they are a chain) and so has r5 free for
 * nothing else; we pack `q`/`sy` and `s`/`-oy` instead and r14 falls to flip.
 * The missing slot also shuffles the aggregate offsets: ROM blk@0x18, v@0x20,
 * g@0x2c; ours blk@0x14, g@0x1c, v@0x28.  Aggregate order is NOT declaration
 * order here -- swapping `vec3_t v; vec3_t g;` is byte-identical.
 *
 * THIS FILE'S SOURCE OF TRUTH is the park for its own file-mate,
 * src/non_matching/rom_9000/800b168.c (UpdateSprite, 0x800b168, the function
 * immediately above this one in asm/rom_9000/rom_b074_a_c.s).  Everything
 * structural here is that park's finding, re-used and re-confirmed:
 *   - THE TWO OAM ENTRIES ARE ONE STRUCT TYPE at obj+0 and obj+0xc, with the
 *     entry fields as BITFIELDS -- `unsigned char mode : 2` at +5 gives
 *     `mov r3,#4 / neg r3,r3`; `unsigned short x : 9` at +6 gives
 *     `ldrh / and ~0x1ff / orr / strh`; `unsigned short mtx : 5` gives
 *     `ldrb [r0,#7]` with `mov r3,#0x3f / neg r3,r3` and `lsl #1`.
 *   - `e->mtx = mtx;` ALONE produces the ROM's `and #0x1f`.  Writing
 *     `e->mtx = mtx & 0x1f;` makes gcc drop its own mask: 362 encodings at the
 *     same 764 bytes (170 differ), i.e. a LOWER count that is the wrong shape.
 *   - `(unsigned short)(sx >> 8)` is the ROM's `lsl #8 / lsr #16`.
 *   - `signed char` for the +0x22/+0x23 offsets (`s8` is plain `char`, which is
 *     UNSIGNED here).  +0x22 comes out `ldrsb r0,[r3,r0]` and +0x23
 *     `ldrb / lsl #24 / asr #24` from the SAME spelling, because gcc reuses the
 *     +0x22 address register with `add r3, #1`.
 *   - `*p21 >> 1` MUST APPEAR TWICE, once for `hh` and once inside the oy
 *     product; that is a real second reference, not a CSE miss.
 *
 * WHAT IS NEW HERE, read off this function rather than inherited:
 *   - THE AFFINE REQUEST BLOCK IS ONLY 8 BYTES.  `union AffineReq { int w[2];
 *     unsigned short h[4]; };` with no padding member -- the 0x38 frame has no
 *     room for the 32-byte form 800b168.c needed, and the flip's read-back
 *     through `blk.h[0]` still requires the union.
 *   - `hx2` IS UNSIGNED.  The companion entry's y uses `lsr r2, r1, #1`, and a
 *     signed `int hx2` gives `asr`.  ONE variable serves both axes: 8 (0x10 in
 *     affine mode 3) for x and `hx2 >> 1` for y, where 800b168.c has separate
 *     hx2/hy2 locals.
 *   - THE ARGUMENT LIST IS FIVE WIDE: `(obj, vec3_t *pos, int *scale, u16 style,
 *     int prio)`.  The frame is 0x38 plus eight pushed words = 0x58, and
 *     `ldr r3, [sp, #0x58]` is the fifth argument, read four times.
 *   - `pos` is a vec3_t POINTER handed to PhysMove (rom_c0/rom_49a8_b.c:374,
 *     `s32 PhysMove(vec3_t *src, vec3_t *dst)`), not the four-word walk
 *     800b168.c has.  It is NOT advanced; `scale` IS (`ldmia r3!, {r1}` =
 *     `*scale++`), and the advanced value is written back to its stack slot.
 *   - THE CULL TEST IS ON THE PROJECTED POINT, not on depth: `v.z == 0 ||
 *     v.x < -0x20 || v.x > (0x88 << 1) || v.y < -0x20 || v.y > 0xd0`, each its
 *     own `b` to the shared cull tail, which is why the ROM has five
 *     `bXX .L / b .Lb658` pairs.  `-0x20` is built once with `mov/neg` and
 *     reused for both axes, so it must be spelled `-0x20` and not pooled.
 *   - VOID.  The epilogue's `pop {r0} / bx r0` sets no return value and the
 *     cull tail falls into it.
 *   - `if (prio == 0) p = ((0x80 << 2) - v.z) / 2 + 0x80; if (p <= 0) p = 1;`
 *     -- the `/ 2` is the ROM's `lsr #31 / add / asr #1` signed halve.
 *
 * MEASURED, ALL INERT (764/363/189 differ unless noted):
 *   - swapping the `vec3_t v; vec3_t g;` declarations; declaring `g` inside the
 *     companion block; adding an unused spare local.
 *   - merging `d` into `s` (768/365), merging `s` into `sx` (193 differ),
 *     declaring `sy` before `sx`.
 *   - `oy` negated at the store instead of at the product: 760/361 -- two
 *     instructions SHORT, so the ROM names the NEGATED product.
 *   - `mtx &= 0x1f;` before the companion store: 193 differ.
 *   - `int style` with an inner `(u16)` cast: 760/361.
 *   - the mode-0 arm as `mtx = 0; if (flip) {...}` instead of `else if`: 760/361.
 *   - declaration reordering that puts flip/hh/hx2/mode last: 185 differ, which
 *     is this file and the best measured; the original v1 ordering reads 189.
 *
 * NEXT: force `flip` to memory without changing the instruction count.  Every
 * pressure-raising local tried either shortened the function or was absorbed.
 * This is the same REG_ALLOC_ORDER class as HANDOFF.md records; the decisive
 * experiment is rebuilding gcc-2.96 with REG_ALLOC_ORDER starting at 4.
 *
 * SPLIT SHAPE: asm/rom_9000/rom_b074_a_c.s holds UpdateSprite (0x800b168, still
 * parked at src/non_matching/rom_9000/800b168.c) and Func_800b388, and
 * tools/datacheck.py reports NO data in the file.  Landing this one alone is a
 * two-way split, UpdateSprite's .s first and this .c second, and the export
 * already exists -- src/rom_9000/exports.s:8 carries
 * `.export_func Func_800b388`.  NO SHIMS: tools/shimcount.py reports none; the
 * only inline asm reached is math.h's own fx32_multiply.
 */
#include "gba/types.h"
#include "math.h"

union AffineReq {
    int w[2];
    unsigned short h[4];
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

extern unsigned char *iwram_3001e68;
extern int PhysMove(vec3_t *src, vec3_t *dst);
extern int UpdateSpriteAnim(unsigned char *obj, int style);
extern int Func_8003d28(union AffineReq *req);
extern void Func_8003dec(struct Spr *e, int prio);
extern void Func_8003f78(int slot);

void Func_800b388(unsigned char *r0, vec3_t *pos, int *scale, u16 style, int prio)
{
    unsigned char *obj;
    unsigned char *p21;
    struct Spr *e;
    union AffineReq blk;
    vec3_t v;
    vec3_t g;
    int d;
    int s;
    int q;
    int sx;
    int sy;
    int ox;
    int oy;
    int p;
    int mtx;
    int hw;
    int flip;
    int hh;
    unsigned int hx2;
    int mode;

    obj = r0;
    mode = 1;
    if (*(short *)(iwram_3001e68 + 4) != 0)
        goto cull;
    d = PhysMove(pos, &v);
    if (v.z == 0 || v.x < -0x20 || v.x > 0x88 << 1 || v.y < -0x20 || v.y > 0xd0)
        goto cull;
    if ((*(obj + 0x1d) & 2) != 0)
        s = *(int *)(obj + 0x18);
    else
        s = fx32_multiply(d, *(int *)(obj + 0x18));
    hw = *(obj + 0x20) >> 1;
    p21 = obj + 0x21;
    hh = *p21 >> 1;
    hx2 = 8;
    flip = UpdateSpriteAnim(obj, style);
    q = (s + (0x80 << 3)) & 0xfffff800;
    sx = fx32_multiply(q, *scale++);
    sy = fx32_multiply(q, *scale);
    if (sx > 0x1f7ff)
        sx = 0xfc << 9;
    if (sy > 0x1f7ff)
        sy = 0xfc << 9;
    ox = fx32_multiply((signed char)*(obj + 0x22), sx);
    oy = -fx32_multiply((*p21 >> 1) - (signed char)*(obj + 0x23), sy);
    if (sx > 0x80 << 9 || sy > 0x80 << 9) {
        hw <<= 1;
        hh <<= 1;
        mode = 3;
        hx2 = 0x10;
    } else if (sx == 0x80 << 9 && *(unsigned short *)(obj + 0x1e) == 0 && sy == sx) {
        mode = 0;
    }
    if (mode != 0) {
        blk.w[1] = (blk.w[1] & 0xffff0000) | *(unsigned short *)(obj + 0x1e);
        blk.w[0] = (blk.w[0] & 0xffff0000) | (unsigned short)(sx >> 8);
        if (flip != 0) {
            blk.w[0] = (blk.w[0] & 0xffff0000) | (unsigned short)-blk.h[0];
            ox = -ox;
        }
        blk.w[0] = (blk.w[0] & 0xffff) | ((unsigned short)(sy >> 8) << 16);
        mtx = Func_8003d28(&blk);
    } else if (flip != 0) {
        mtx = 8;
        ox = -ox;
    } else {
        mtx = 0;
    }
    e = (struct Spr *)obj;
    e->x = v.x - hw + ox;
    e->y = v.y - hh + oy;
    e->mode = mode;
    e->mtx = mtx;
    if (prio == 0) {
        p = ((0x80 << 2) - v.z) / 2 + 0x80;
        if (p <= 0)
            p = 1;
        Func_8003dec(e, p);
    } else {
        Func_8003dec(e, prio);
    }
    if ((*(obj + 0x26) & 1) != 0) {
        g.x = pos->x;
        g.y = 0;
        g.z = pos->z;
        PhysMove(&g, &v);
        e = (struct Spr *)(obj + 0xc);
        e->x = v.x - hx2;
        e->y = v.y - (hx2 >> 1) + 2;
        e->mode = mode;
        e->mtx = mtx;
        if (prio == 0)
            Func_8003dec(e, 0);
        else
            Func_8003dec(e, prio);
    }
    return;
cull:
    if ((*(obj + 0x1d) & 1) == 0) {
        Func_8003f78(*(obj + 0x1c));
        *(obj + 0x25) = 1;
    }
}
