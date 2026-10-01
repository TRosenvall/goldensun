/* Func_801908c -- 0x0801908c, asm/rom_15000/rom_18cac_c.s.   (PARK)
 *
 * ONE function, NO data section (datacheck.py exit 0, no EXPORTS line), so
 * landing this is a plain WHOLE-FILE conversion: no split, no export, no
 * linker-script change.
 *
 * NOT MATCHING: 22 encodings of 151 differ (objcmp), and this IS a true
 * distance -- ref 320 bytes / 151 instructions, ours 320 / 151, and BOTH
 * relocations identical in type, symbol and offset
 * (R_ARM_THM_CALL Func_8003d28 at 0xb4, R_ARM_ABS32 Data_366f8 at 0xf4).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/801908c.c \
 *     asm/rom_15000/rom_18cac_c.s --whole
 *
 * The prototype and every struct here are FIXED BY A NEIGHBOUR, not guessed:
 * src/non_matching/rom_15000/80191cc.c calls Func_801908c(e) with struct Ent *,
 * and its struct Ent / struct Spr / struct Req are reused verbatim.  Every
 * bitfield mask in the ROM (0xc1 on byte 7 = pri:5 at bit 9, 0xfc on byte 5 =
 * c0:2, 0x1ff / 0xfffffe00 on the halfword at +6 = x:9) matches that park's
 * table 4 exactly.
 *
 * THREE LEVERS LANDED, 30 -> 26 -> 22 (all still in the body, all re-measured):
 *  1. `int g = 0x100;` BEFORE `struct Spr *o = &e->spr;`  (30 -> 26)
 *  2. A BLOCK-LOCAL `{ u32 a = e->fc; ... }` PER SWITCH ARM, not one
 *     function-scope local shared by arms 9 and 10.  (26 with, 30 without)
 *  3. `u32 c = a; ... a = a + 1; e->fc = a;` -- the increment on a local that
 *     is MODIFIED, with a separate copy carrying the index.  (26 -> 22)
 *
 * ======================= THE BLOCKER, RE-ATTRIBUTED =======================
 *
 * The old blocker line said "cse2 REWRITES THE GUARD".  That line is
 * SELF-REFUTING against this file's own measurement further down, which reads
 * "-fno-regmove (52) -- positive evidence that REGMOVE is the pass placing the
 * copy".  Both cannot be the blocker, and `-fno-cse-follow-jumps` and
 * `-fno-gcse` are recorded INERT at 26 while `-fno-rerun-cse-after-loop` only
 * moves it to 34 (worse, not closed).  Read as "the copy's PLACEMENT is
 * regmove's, and which operand the compare takes was fixed earlier".
 *
 * THE REFERENCE, read off asm/rom_15000/rom_18cac_c.s rather than paraphrased.
 * Arms 9 and 10 (.L190b6 / .L190c8) use a THREE-operand increment and keep the
 * load live for the mask:
 *     ldrh r2,[r6,#0xc] / add r3,r2,#1 / strh r3,[r6,#0xc]
 *     mov r3,#0x1f / ldr r1,=Data_366f8 / and r3,r2 / lsl r3,#1 / ldrh r7,[r1,r3]
 * Arms 11 and 12 (.L190da / .L190f0) make a COPY, compare the COPY, and use a
 * TWO-operand in-place increment on the load:
 *     ldrh r3,[r6,#0xc] / mov r0,r3 / cmp r0,#7 / bhi
 *     add r3,#1 / ldr r2,=Data_366f8 / strh r3,[r6,#0xc]
 *     lsl r3,r0,#2 / add r3,#0x20 / ldrh r7,[r2,r3]
 * (arm 11 copies into r0, arm 12 into r1 -- distinct pseudos, which is the
 * independent confirmation of lever 2's per-arm scope.)
 *
 * The 22 are: 10 in arms 11/12 (ours compares the LOAD, `cmp r2,#7`, where the
 * ROM compares the COPY, `cmp r0,#7`) + 8 a pure r2<->r3 ROLE SWAP in arms 9/10
 * (same instructions, same order, same count) + 2 a sched2 rotation of
 * `ldr r2,=Data_366f8` past `strh r3,[r6,#0xc]`.
 * A `__asm__ ("" : "+r" (c))` after `u32 c = a;` in each arm measures 12 of 151
 * at exact size, count and relocations -- so the 10 ARE one substitution and
 * nothing else.  THAT IS A MEASUREMENT DEVICE AND IT IS NOT IN THE CODE BELOW.
 *
 * *** BATCH-316b: THE CLUSTER'S SHARED IDIOM REACHES THIS PARK, AND IS INERT. ***
 * `ldrh r3 / mov r0,r3 / cmp r0,#7` is the same shape as
 * DisplayMenuArrowCursor's `ldrh r2 / mov r3,r2 / cmp r3,#0` and 8021cb8's
 * `ldrb r2 / mov r3,r2 / cmp r3,#0xff`: THE ROM COMPARES A REGISTER COPY, NOT
 * THE LOADED VALUE.  On DisplayMenuArrowCursor that idiom is worth 16 -> 6,
 * spelled as TWO LOCALS OF DIFFERENT WIDTH -- the wide one carries the
 * arithmetic, the narrow one the compare, so the two have different RTL modes
 * and cse cannot substitute.  TRANSFERRED HERE AND MEASURED: it does nothing.
 *
 * 14-variant cross, one container, crossing type(arms 9/10 local) x type(the
 * arms 11/12 copy local) x which local the guard compares:
 *     a:u32  c:u32 / u16 / int / u8      22 / 22 / 24 / 23
 *     a:u16  c:u32 / u16 / int / u8      22 / 22 / 24 / 23
 *     a:int  c:u32 / u16 / int / u8      22 / 22 / 24 / 23
 *     control, guard on the LOAD, c:u32  22  (= this body)
 *     control, guard on the LOAD, c:u16  131, dsize +8, RELOCDIFF
 * NOTHING BEATS 22.  The reason the narrow/wide trick fails here (and it is the
 * transferable part) is that it needs the SURVIVING use to be WIDER than the
 * compare.  In DisplayMenuArrowCursor the wide local fed an `o->x + d` add; here
 * both ends are the same width, so the modes line up and cse substitutes.
 *
 * MEASURED INERT (all exactly 26 on the lever-2 base, so none is the lever):
 * `0x1f & v` instead of `v & 0x1f` -- the AND is a two-address commutative op
 * and BOTH orders give the same register roles; naming the masked index in a
 * local (30); `*(Data_366f8 + i)` (26); `int` instead of `u32` for the arm
 * locals (26); a `u16 *t = Data_366f8;` local per arm (28, WORSE).
 * MEASURED WORSE: -fno-regmove (52); -fno-rerun-cse-after-loop (34).
 * MEASURED INERT: -fno-cse-follow-jumps, -fno-gcse (both 26).
 * MEASURED SHORT (149 instructions, 2 under, so NOT comparable): every spelling
 * that lets cse delete the copy outright, e.g. a plain
 * `u32 a = e->fc; if (a <= 7) { e->fc = a + 1; g = tbl[...]; }`.
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
