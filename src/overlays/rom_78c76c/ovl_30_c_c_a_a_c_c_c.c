/* OvlFunc_891_2008c8c  --  0x02008c8c
 * OvlFunc_891_2008eb0  --  0x02008eb0
 * [asm/overlays/rom_78c76c/ovl_30_c_c_a_a_c_c_c.s -- BOTH functions of the
 *  file. Its earlier sibling OvlFunc_891_2008614 is
 *  src/overlays/rom_78c76c/ovl_30_c_c_a_a_c_c_b.c]
 *
 * Byte-exact, both, under tools/objcmp.py against the trimmed reference and
 * against asm/overlays/rom_78c76c/ovl_30_c_c_a_a_c_c_c.s itself:
 *
 *   OK OvlFunc_891_2008c8c -- 548 bytes, 242 encodings and 29 relocations identical
 *   OK OvlFunc_891_2008eb0 -- 308 bytes, 145 encodings and 6 relocations identical
 *
 * and the WHOLE translation unit, assembled the way the Makefile assembles it
 * and compared against asm/overlays/rom_78c76c/ovl_30_c_c_a_a_c_c_c.s as one
 * object, is 856 bytes, 387 encodings and 35 relocations identical. (objcmp's
 * --func trims the REFERENCE only, so the two-function file has to be checked
 * that way; the per-function lines above come from single-function extracts of
 * this exact source.)
 *
 * No flag group. Both build under plain GCC296_CFLAGS through the generic
 * `asm/%.o: src/%.c` rule; no Makefile edit is needed and none should be added.
 *
 * ---------------------------------------------------------------- 2008c8c
 *
 * FOUR ARGUMENT PINS, AND ALL FOUR ARE ORDERING JOBS. Plain C is 242 encodings
 * against 242 with 8 differing -- four call sites, two transposed `mov`s each.
 * Dropping any one pin costs exactly 2 and the cost is additive across the set
 * (2/4/6/8 over all sixteen subsets), with SIZE and RELOCATIONS silent at every
 * point. That is the recorded partition: a pin whose only job is ordering never
 * moves a `bl`, so no relocation offset moves; a pin covering a real CSE loss
 * would move all of them.
 *
 * The fill is UNIFORM WHOLE-VALUE ASCENDING at all four sites -- q0, q1, q2 in
 * that order, `0x80 << 7` written whole -- even though two of the four ROM
 * sites EMIT r1, r2, r0. Read the source order off what the pins have to
 * establish, not off the emission order; the scheduler puts the pool load and
 * the `lsl` back where the ROM has them.
 *
 * `saved = *g` GOES FIRST IN BOTH ARMS, though the ROM emits it first in one
 * arm and interleaved into the field copies in the other. Written to follow
 * each arm's emission order it is still exact; written uniformly the OTHER way
 * (after the three copies, which is arm 2's emission order) it is 11 differing.
 * The two arms are one idiom with one source order and the scheduler does the
 * rest.
 *
 * THE `goto`-LOOP LEVER IS HARMFUL HERE, and the ROM says so before you try it.
 * The immediate neighbour src/non_matching/ovl_78c76c/2008098.c is the same
 * overlay, the same `__CopyMapTiles` twice-per-iteration loop, the same
 * count-up exit -- and it NEEDS the goto rewrite. This one must not have it:
 * 202 of 242 differing and 16 bytes short. The discriminator is the one
 * docs/elevation.md already records under "Selecting for the `goto`-loop lever":
 * look in the loop BODY for a rebuilt invariant. 2008098 rebuilds its stack
 * pair inside the body; here the ROM hoists it -- `mov r2, #4 / mov r8, r2 /
 * mov r5, #2` sits in each loop's preheader -- so LICM is wanted and `goto`,
 * which disables it, throws the hoist away.
 *
 * THREE PREHEADERS MEANS THREE LIVE RANGES, NOT ONE PAIR OF LOCALS. The ROM
 * re-materialises 4 and 2 at each of the three loop heads. Plain literals in
 * the body give exactly that, because LICM hoists per loop. Hoisting them by
 * hand into one pair of locals ahead of all three loops -- the neighbour park's
 * own lever -- fuses the three ranges into one and costs 125 of 242, 12 bytes
 * short.
 *
 * `i != K`, NOT `i < K`. Both loop families exit on `cmp / bne`, the recorded
 * up-count tell; `<` lets gcc reverse the dead-counter loops and costs 18 (the
 * three tile loops) and 17 (the four nudge loops).
 *
 * `t` IS INDEXED DIRECTLY. `mov r5, r7` at each of the four nudge-loop heads is
 * gcc's own loop-invariant copy of the array base, not a pointer in the source:
 * adding `q = t;` before each loop costs 89 of 242.
 *
 * MEASURED AND INERT, so not shipped: a shared local for the `4` that the two
 * tail `__CopyMapTiles` calls pass as argument 5 and then argument 6 (the ROM
 * keeps it in r5 across both). Byte-identical to the literals.
 *
 * ---------------------------------------------------------------- 2008eb0
 *
 * A 9-BIT BITFIELD, NOT A HAND-WRITTEN MERGE. Hand-masking
 * `*(unsigned short *)(p + 6) = (*(unsigned short *)(p + 6) & ~0x1ff) | (v & 0x1ff)`
 * is 140 of 145 differing and 8 bytes long. Declaring the field
 * `unsigned short x : 9` and writing `p->x = v` is exact but for one statement
 * order. Read the mask WIDTH off the ROM exactly as docs/elevation.md says:
 * `ldr r1, =0xfffffe00` is a 32-bit mask, so the original was a bitfield.
 *
 * NEW (grepped first against "1b", "Halfword constant pooling", "store_bit_field"
 * and "The mask's WIDTH tells you which spelling the original used"): the
 * recorded width rule is stated for a BYTE container and it holds at HALFWORD
 * container width too, where the tell is DOUBLE and the second half is blocker
 * 1b. Hand-masking gives both
 *     mov r2, #0xfe / lsl r2, #8      (~0x1ff narrowed to 16 bits)
 *     ldrh r1, <pool>                 (0x1ff pooled AS A HALFWORD)
 * against the ROM's `ldr =0xfffffe00` and `ldr =0x1ff`. The bitfield fixes both
 * in one move, because store_bit_field builds the clear-mask AND the value-mask
 * at int width, so neither is ever a HImode const_int and the arm.md
 * constraint-ordering fact that IS blocker 1b never gets to apply. This is an
 * extension of the recorded rule, not a new mechanism: 1b's escape is still
 * "make the value SImode", and the bitfield is a second way of doing that which
 * the 1b section does not list.
 *
 * `/ 0x10000`, NOT THE EXPANSION. `cmp / bge / add 0xffff / asr #16` is what
 * gcc-2.96 emits for a signed divide by 65536. Writing that expansion by hand
 * costs 120 of 145 and 4 extra bytes.
 *
 * `i = 0;` IS ITS OWN STATEMENT IN THE THIRD LOOP, and it must come BEFORE
 * `b += 8;`. The ROM's third preheader reads `mov r2, #8 / mov r7, #0 /
 * add r8, r2`. As a `for`-init the counter is emitted at the END of the
 * preheader and the pair transposes: 2 differing. Both orders of `b += 8`
 * against the whole `v` computation were swept; moving it above the `v`
 * computation is 19. The recorded `Func_80798e0` lever, at one slot.
 *
 * `unsigned int i` -- the loops exit on `bls`, not `ble`; a signed counter is
 * 13 differing. The `do`/`while (i <= 8)` shape and the declaration are the
 * ones the file-family neighbour src/overlays/rom_78c76c/ovl_30_c_c_a_c_c_a_a_c_b.c
 * already uses over the same `.L2a50` table, which is what named the struct.
 *
 * ALSO MEASURED AND EXACT, so free: the range test as
 * `(unsigned)(b + 0x10) <= 0xaf` instead of the readable pair of signed bounds,
 * and `v = (... ) | -0x20;` folded instead of split over two statements. The
 * readable spellings are kept.
 *
 * ------------------------------------------------------- measured worse
 *
 * 2008c8c, all against ref 242 encodings / 548 bytes:
 *
 *   drop 1 of the 4 pins (any)                 2 differing   size, relocs silent
 *   drop 2 / 3 / 4 pins                        4 / 6 / 8     size, relocs silent
 *   `saved = *g` after the field copies,
 *     uniformly in both arms                  11 differing
 *   `i < K` on the three tile loops           18 differing
 *   `i < 0x1e` on the four nudge loops        17 differing
 *   `q = t;` pointer alias per nudge loop     89 of 242, 544 bytes
 *   one hoisted pair of stack-arg locals
 *     ahead of all three tile loops          125 of 242, 536 bytes
 *   `goto` loops for the three tile loops    202 of 242, 532 bytes
 *
 *   INERT (exact, not shipped): a shared local for the tail `4`; following each
 *   arm's own emission order for `saved = *g` instead of one uniform order.
 *
 * 2008eb0, all against ref 145 encodings / 308 bytes:
 *
 *   `b += 8;` before `i = 0;` (or a `for`-init)  2 differing
 *   signed loop counter                         13 differing
 *   `b += 8;` above the `v` computation          19 differing
 *   hand-written `/ 0x10000` expansion          120 of 145, 312 bytes
 *   hand-written mask/merge instead of the
 *     bitfield                                 140 of 145, 316 bytes, relocs differ
 *
 *   INERT (exact, not shipped): `(unsigned)(b + 0x10) <= 0xaf` for the range
 *   test; the `| -0x20` folded into the `v` expression; `for (i = 0; i <= 8;
 *   i++)` for the first two loops.
 *
 * ---------------------------------------------------------------- landing
 *
 * WHOLE-FILE, NO LINKER EDIT. The .s holds these two functions and nothing
 * else, so the .c replaces it outright at
 * src/overlays/rom_78c76c/ovl_30_c_c_a_a_c_c_c.c and the generic
 * `asm/%.o: src/%.c` rule rebuilds asm/overlays/rom_78c76c/ovl_30_c_c_a_a_c_c_c.o
 * from it. overlays/rom_78c76c/overlay.ld names that .o exactly once,
 *
 *     asm/overlays/rom_78c76c/ovl_30_c_c_a_a_c_c_c.o(.text)
 *
 * inside `.text`, and nowhere else -- its `.data` and `.bss` blocks name only
 * ovl_30_c_c_c_c_c_c.o. Nothing to remap.
 */
extern unsigned char *iwram_3001e70;
extern int L2974 __asm__(".L2974");

/* One 0xc-byte entry of the sprite table the file-family neighbour
 * src/overlays/rom_78c76c/ovl_30_c_c_a_c_c_a_a_c_b.c fills three ints at a
 * time. Bits 0..8 of the halfword at +6 are a signed X, written as a bitfield
 * because the ROM's clear-mask is 32 bits wide. */
struct Ent {
    int f0;
    unsigned char f4;
    unsigned char f5;
    unsigned short x : 9;
    unsigned short f6hi : 7;
    int f8;
};

extern struct Ent L2a50[] __asm__(".L2a50");

extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneWait(int n);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8003dec(struct Ent *p, int n);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_891_2008c8c(void)
{
    int **g;
    int *saved;
    unsigned char *p;
    int t[3];
    int i;
    int dir;

    g = (int **)iwram_3001e70;
    p = __MapActor_GetActor(0);
    if (*(int *)(p + 0x10) < (0xb3 << 16)) {
        { PIN3; q0 = 0; q1 = 0x23f; q2 = 0x84;
          __Func_80921c4(q0, q1, q2); }
        { PIN3; q0 = 0; q1 = 0x80 << 7; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0x1e);
        saved = *g;
        t[0] = *(int *)(p + 8);
        t[1] = *(int *)(p + 0xc);
        t[2] = *(int *)(p + 0x10);
        *g = t;
        for (i = 0; i != 0x1e; i++) {
            t[2] += 0x80 << 9;
            __CutsceneWait(1);
        }
        __CutsceneWait(0x28);
        dir = 1;
    } else {
        { PIN3; q0 = 0; q1 = 0x241; q2 = 0xde;
          __Func_80921c4(q0, q1, q2); }
        { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0x1e);
        saved = *g;
        t[0] = *(int *)(p + 8);
        t[1] = *(int *)(p + 0xc);
        t[2] = *(int *)(p + 0x10);
        *g = t;
        for (i = 0; i != 0x1e; i++) {
            t[2] += 0xffff0000;
            __CutsceneWait(1);
        }
        __CutsceneWait(0x28);
        dir = 2;
    }
    for (i = 0; i != 6; i++) {
        __CopyMapTiles(2, 0x1c, 0x22, 0xa, 4, 2);
        __CutsceneWait(8);
        __CopyMapTiles(2, 0x1e, 0x22, 0xa, 4, 2);
        __CutsceneWait(8);
    }
    for (i = 0; i != 0xa; i++) {
        __CopyMapTiles(2, 0x1c, 0x22, 0xa, 4, 2);
        __CutsceneWait(4);
        __CopyMapTiles(2, 0x1e, 0x22, 0xa, 4, 2);
        __CutsceneWait(4);
    }
    for (i = 0; i != 0xc; i++) {
        __CopyMapTiles(2, 0x1c, 0x22, 0xa, 4, 2);
        __CutsceneWait(2);
        __CopyMapTiles(2, 0x1e, 0x22, 0xa, 4, 2);
        __CutsceneWait(2);
    }
    __CopyMapTiles(2, 0x1c, 0x22, 0xa, 4, 2);
    __CopyMapTiles(8, 0x37, 0x20, 0x28, 8, 4);
    __CutsceneWait(0x3c);
    if (dir == 1) {
        for (i = 0; i != 0x1e; i++) {
            t[2] += 0xffff0000;
            __CutsceneWait(1);
        }
    } else if (dir == 2) {
        for (i = 0; i != 0x1e; i++) {
            t[2] += 0x80 << 9;
            __CutsceneWait(1);
        }
    }
    *g = saved;
}

void OvlFunc_891_2008eb0(void)
{
    int *cam;
    struct Ent *p;
    int a, b, v;
    unsigned int i;

    cam = (int *)(iwram_3001e70 + 0xe4);
    p = L2a50;
    a = cam[0] / 0x10000;
    b = 0x50 - cam[1] / 0x10000;
    if (b >= -0x10 && b <= 0x9f) {
        v = (L2974 >> 10) - a;
        v |= -0x20;
        i = 0;
        do {
            p->x = v;
            p->f4 = b;
            __Func_8003dec(p, 0);
            i++;
            v += 0x20;
            p++;
        } while (i <= 8);
        v = (L2974 >> 9) - a;
        v |= -0x20;
        i = 0;
        do {
            p->x = v;
            p->f4 = b;
            __Func_8003dec(p, 0);
            i++;
            v += 0x20;
            p++;
        } while (i <= 8);
        v = (L2974 >> 8) - a;
        v |= -0x20;
        i = 0;
        b += 8;
        do {
            p->x = v;
            p->f4 = b;
            __Func_8003dec(p, 0);
            i++;
            v += 0x20;
            p++;
        } while (i <= 8);
    }
    L2974 += 0x80;
}
