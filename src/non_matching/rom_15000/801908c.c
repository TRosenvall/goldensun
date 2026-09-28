/* Func_801908c -- 0x0801908c, asm/rom_15000/rom_18cac_c.s.
 *
 * ONE function, NO data section (datacheck.py exit 0, no EXPORTS line), so
 * landing this is a plain WHOLE-FILE conversion: no split, no export, no
 * linker-script change.
 *
 * NOT MATCHING: 22 encodings of 151 differ (objcmp), and this IS a true
 * distance -- ref 320 bytes / 151 instructions, ours 320 / 151, and BOTH
 * relocations identical in type, symbol and offset
 * (R_ARM_THM_CALL Func_8003d28 at 0xb4, R_ARM_ABS32 Data_366f8 at 0xf4).
 * tryc --align agrees at 23 of 152.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/801908c.c \
 *     asm/rom_15000/rom_18cac_c.s --whole
 *
 * ======================= WHAT IS ESTABLISHED =======================
 * The prototype and every struct here are FIXED BY A NEIGHBOUR, not guessed:
 * src/non_matching/rom_15000/80191cc.c calls Func_801908c(e) with struct Ent *,
 * and its struct Ent / struct Spr / struct Req are reused verbatim.  Every
 * bitfield mask in the ROM (0xc1 on byte 7 = pri:5 at bit 9, 0xfc on byte 5 =
 * c0:2, 0x1ff / 0xfffffe00 on the halfword at +6 = x:9) matches that park's
 * table 4 exactly, which is independent confirmation of the layout.
 *
 * THREE LEVERS LANDED, 30 -> 26 -> 22:
 *
 * 1. INITIALISER ORDER: `int g = 0x100;` BEFORE `struct Spr *o = &e->spr;`
 *    30 -> 26 and moves objcmp's first difference from index 5 to index 13.
 *    The ROM interleaves the two initialisations as
 *    `mov r7,#0x80 / mov r5,r6 / sub sp,#8 / lsl r7,#1 / add r5,#0x10`;
 *    with `o` first, ours emits o's two insns before g's two.  This is
 *    declaration order mattering on an EXACT TIE, which is the narrow case
 *    docs/elevation.md allows.
 *
 * 2. A BLOCK-LOCAL `{ u32 a = e->fc; ... }` PER SWITCH ARM, not one function-
 *    scope local shared by arms 9 and 10.  26 with per-arm scope, 30 with the
 *    shared local.  One pseudo spanning two disjoint arms changes local-alloc's
 *    quantity order for the whole switch.  This is the brief's scope lever
 *    (`{ int t = 2; }` per block) on a loaded value rather than a constant.
 *
 * 3. `u32 c = a; ... a = a + 1; e->fc = a;` -- the increment must be on a
 *    local that is MODIFIED, with a separate copy carrying the index.  26 -> 22.
 *    The three-read spelling `if (e->fc <= 7) { g = tbl[...e->fc...]; e->fc += 1; }`
 *    also reaches 151 instructions but puts regmove's copy on the wrong side.
 *
 * ============================ THE BLOCKER ============================
 * cse2 REWRITES THE GUARD to use the load pseudo instead of the copy, in arms
 * 11 and 12.  The ROM is
 *      ldrh r3,[r6,#0xc] / mov r0,r3 / cmp r0,#7 / bhi / add r3,#1 / strh r3
 *      ... lsl r3,r0,#2
 * -- the copy is compared AND indexed, and the load pseudo is incremented in
 * place (2-operand `add`).  Ours gets the copy and the roles right and compares
 * the ORIGINAL: `ldrh r2 / mov r3,r2 / cmp r2,#7 / ... add r2,#1 / lsl r3,#2`.
 * `c` and `a` hold the same value at the compare, so cse substitutes the load
 * pseudo there; the substitution is not blocked by declaration order (y1),
 * by comparing the member instead (y2), or by deriving `a` from `c` (y3) --
 * all three measure exactly 22.
 *
 * PROOF THAT THIS IS THE WHOLE RESIDUE, and it is a SHIM, so it is NOT used:
 *   __asm__ ("" : "+r" (c));  after `u32 c = a;` in each arm measures
 *   12 of 151 at exact size, count and relocations.  That is the brief's
 *   "+r" barrier working on COPY DIRECTION -- its real class -- and it
 *   confirms the 10 arm-11/12 differences are one cse substitution and
 *   nothing else.  Left out of the candidate: a shim must be booked in
 *   fakematch.txt and this function does not need to land that way.
 *
 * THE OTHER 8 of the 22 are a pure r2<->r3 ROLE SWAP in arms 9 and 10:
 * the ROM gives the loaded value r2 and the increment/mask temp r3, ours the
 * reverse.  Same instructions, same order, same count.  With the barrier in
 * place these 8 are the ONLY remainder besides a two-instruction sched2
 * rotation of `ldr r2,=Data_366f8` past `strh r3,[r6,#0xc]`.
 *
 * MEASURED INERT (all exactly 26 on the same base, so none of these is the
 * lever): `0x1f & v` instead of `v & 0x1f` -- the AND is a two-address
 * commutative op and BOTH orders give the same register roles, so the mul
 * lever's "read the ROM's mov and put the other operand on the right"
 * procedure does NOT carry over to `and` here; naming the masked index in a
 * local (30); `*(Data_366f8 + i)` instead of `Data_366f8[i]` (26);
 * `int` instead of `u32` for the arm locals (26); a `u16 *t = Data_366f8;`
 * local per arm (28, WORSE).
 * MEASURED WORSE: -fno-regmove (52) -- which is the positive evidence that
 * regmove is the pass placing the copy, and that it is placing nearly all of
 * it correctly; -fno-rerun-cse-after-loop (34); -fno-cse-follow-jumps and
 * -fno-gcse are both INERT at 26.
 * MEASURED SHORT (149 instructions, 2 under, so not distances): every
 * spelling that lets cse delete the copy outright -- a plain
 * `u32 a = e->fc; if (a <= 7) { e->fc = a + 1; g = tbl[...]; }` is 18 aligned
 * but two instructions short, and 18 there is NOT comparable to 22 here.
 *
 * NO SHIMS in the code below: no `register ... __asm__`, no `__asm__ ("")`,
 * no .equ, no per-file flags, no fakematch row.
 */
#include "gba/types.h"

struct Spr {
    u32 f0;
    u8  f4;
    u8  c0:2, c2:2, c4:1, c5:1, c6:2;
    u16 x:9, pri:5, mode:2;
    u16 tile:10, t10:6;
};

struct Ent {
    struct Ent *next;
    u8  pad4;
    u8  state;
    u16 f6;
    u8  f8;
    u8  f9;
    u16 fa;
    u16 fc;
    u8  fe;
    u8  ff;
    struct Spr spr;
};

struct Req {
    u16 w;
    u16 h;
    u16 ang;
    u16 pad;
};

extern u16 Data_366f8[];

extern int Func_8003d28(struct Req *r);

void Func_801908c(struct Ent *e)
{
    int g = 0x100;
    struct Spr *o = &e->spr;
    u32 v;
    struct Req s;

    switch (e->state) {
    case 9:
        {
            u32 a = e->fc;
            e->fc = a + 1;
            g = Data_366f8[a & 0x1f];
        }
        break;
    case 10:
        {
            u32 b = e->fc;
            e->fc = b + 1;
            g = Data_366f8[b & 0x1f] >> 1;
        }
        break;
    case 11:
        {
            u32 a = e->fc;
            u32 c = a;
            if (c <= 7) {
                a = a + 1;
                e->fc = a;
                g = Data_366f8[0x10 + c * 2];
            }
        }
        break;
    case 12:
        {
            u32 b = e->fc;
            u32 d = b;
            if (d <= 7) {
                b = b + 1;
                e->fc = b;
                g = Data_366f8[0x10 + d * 2] >> 1;
            }
        }
        break;
    }

    if (g == 0x100) {
        o->pri = 0;
        o->c0 = 0;
        o->x = e->f6;
        o->f4 = *(u16 *)&e->f8;
    } else {
        s.w = g;
        s.h = g;
        s.ang = 0;
        o->pri = Func_8003d28(&s);
        if (g > 0x100) {
            o->c0 = 3;
            o->x = e->f6 - 8;
            o->f4 = e->f8 - 8;
        } else {
            o->c0 = 1;
            o->x = e->f6;
            o->f4 = *(u16 *)&e->f8;
        }
    }
}
