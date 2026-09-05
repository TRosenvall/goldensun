/* Func_80b8fd4 -- 0x080b8fd4, 0xd8 bytes, 83 instructions + an 8-word pool.
 *
 * Sets up the battle camera for one actor: point the view block at
 * iwram_3001e80[0], point a second block at iwram_3001e80[0x20], write the
 * pitch/yaw/distance, walk the matrix stack (identity, translate, yaw, pitch),
 * transform one point, install the projection, then run Func_80c0a24 with the
 * second block's busy flag raised and lowered around it.
 *
 * FAMILY. A third member of the SetUpProjection group: src/rom_b5000/
 * rom_b7410_c_c_a_a_b.c (Func_80b7f9c) and src/rom_f4000/rom_f4008_a_a_c.c
 * (Func_80f4028). Same camera block, same `struct View`, same field offsets,
 * same `bl _call_via_r3` idiom. What this one adds is the second block
 * (iwram_3001e80[0x20]), the gPhysVec[4] store and the five-argument
 * Func_80c0a24 tail with its stack argument.
 *
 * VERDICT: exact on the FIRST screen, from ordinary C.
 *   OK Func_80b8fd4 -- 216 bytes, 91 encodings and 12 relocations identical
 * No pins. No barriers. No flags. No Makefile rule -- the default
 * `asm/%.o: src/%.c` rule and plain GCC296_CFLAGS.
 *
 * THE TWO INTERLEAVE SITES, and why nothing was needed for them.
 * This function was picked as a straight-line carrier of "the zero interleaved
 * into a shifted build" (docs/elevation.md, and its "Re-derived: the wall hid
 * 230 functions" sub-section). Both sites came out unaided:
 *
 *   site 1, the store block
 *       mov r3,#0x80 / mov r2,r8 / mov r6,#0 / lsl r3,#7
 *     -- the second block's pointer copy AND the zero land inside the 0x4000
 *     build. Produced by nothing more than the ROM's own statement order with
 *     the shift written WHOLE (`q[0] = 0x80 << 7;`).
 *
 *   site 2, the divide call
 *       mov r1,#0xc0 / ldr r3,=Func_80008ac / lsl r1,#8 / ldr r0,=0x3c90000
 *     -- here the interleaved single-instruction value is the FUNCTION POINTER,
 *     not a zero. It is bought by WHERE the pointer local is assigned, not by
 *     any pin: `divide = Func_80008ac;` immediately before the call. That is
 *     the whole lever at this site (see the measured table: hoisting it costs
 *     91 of 91).
 *
 * WHICH ARGUMENT TO PIN: none. The ROM's order at site 2 is already the one
 * gcc picks, so there is no interleave to buy with a pin and no later repeat of
 * either constant to defeat with one. Every pin spelling tried measured EXACTLY
 * ZERO and was stripped rather than shipped.
 *
 * WHAT IS ACTUALLY LOAD-BEARING (each verified by removal):
 *   - the two typed function-pointer locals, assigned ADJACENT to their calls;
 *     they are what emits `bl _call_via_r3`.
 *   - `t.z = s->v2.z;` -- the point's z is READ BACK from the field, not
 *     rewritten as the literal 0x2ee0000.
 *   - the `1` written as a PLAIN LITERAL at both `q[4]` and `q[5]`; gcc
 *     commons it into callee-saved r5 by itself, exactly as the ROM does.
 *   - `iwram_3001e80` declared `int[]` and indexed [0] / [0x20], which is what
 *     gives `ldr r3,[r3]` / `add r3,#0x80` / `ldr r3,[r3]` rather than a
 *     folded `ldr r3,[r3,#0x80]` (same idiom as
 *     src/rom_b5000/rom_b8228_c_a_c_c_c_c.c).
 *
 * MEASURED WORSE / MEASURED INERT (objcmp, ref = 216 bytes / 91 encodings):
 *
 *   probe                                             result
 *   -------------------------------------------------------------------------
 *   fn-ptr locals removed, direct calls               48 differing, 204 bytes
 *   `divide = Func_80008ac` hoisted off the call site  91 differing, 224 bytes
 *   `t.z` written as the literal 0x2ee0000            87 differing, 224 bytes
 *   the `1` given a named local                       93 differing, 228 bytes
 *   -------------------------------------------------------------------------
 *   named `int z = 0` for the eight zero stores       INERT (identical)
 *   named local for the repeated `0x80 << 7`          INERT
 *   register r0/r1 pins on the divide's arguments     INERT
 *   the same pins plus a split mov/lsl build          INERT
 *   split-build locals for all four Func_80c0a24 args INERT
 *
 * NEW (grepped by concept first -- "named zero", "do not name zeros", "the
 * same function can want a named zero and a bare one", "scaffolding that
 * measures inert"):
 *   THE PUSHED-ZERO RULE IS SUFFICIENT, NOT NECESSARY. The recorded refinement
 *   of "do not name zeros" says a zero the ROM keeps in a PUSHED register
 *   across calls IS a named local; the two sibling functions in this very
 *   family both say so in their own headers ("THE ZERO IS NAMED", because its
 *   live range crosses four calls). Here the ROM keeps 0 in r6 across five
 *   calls -- the rule's exact profile -- and the named and bare spellings
 *   produce BYTE-IDENTICAL objects. So the prologue tell tells you a named zero
 *   is SAFE, not that it is required; at -O2 gcc will do the same commoning
 *   unaided. Per "scaffolding that measures inert must not ship", the bare form
 *   is what lands, and a family template's "the zero is named" must be
 *   re-measured rather than transplanted.
 *
 * LANDING. The .s holds TEN functions, so a whole-file replacement is not
 * available; this is a three-way split of
 * asm/rom_b5000/rom_b8228_c_a_c_c_a_c_a.s (tools/split_asm.py: "carries data:
 * no", "label exports: none needed", "no labels cross between the remaining
 * pieces"):
 *     _a.s  Func_80b8f58
 *     _b.c  Func_80b8fd4        <- this file
 *     _c.s  Func_80b90ac .. Func_80b9724   (8 functions)
 * stage1.ld:1417 is the ONLY line in the tree naming the object on its full
 * path:
 *     asm/rom_b5000/rom_b8228_c_a_c_c_a_c_a.o(.text)
 * and becomes three lines in that order:
 *     asm/rom_b5000/rom_b8228_c_a_c_c_a_c_a_a.o(.text)
 *     asm/rom_b5000/rom_b8228_c_a_c_c_a_c_a_b.o(.text)
 *     asm/rom_b5000/rom_b8228_c_a_c_c_a_c_a_c.o(.text)
 * The basename must NOT stay `rom_b8228_c_a_c_c_a_c_a`, or gcc's generated .s
 * would overwrite the .s being split.
 */
#include "gba/types.h"

struct View {
    vec3_t v0;
    vec3_t v1;
    vec3_t v2;
    unsigned char pad24[0x10];
    short pitch;
    short yaw;
};

extern int iwram_3001e80[];
extern int gPhysVec[];
extern void InitMatrixStack(void);
extern void MatrixTranslatev(vec3_t *v);
extern void MatrixYaw(int angle);
extern void MatrixPitch(int angle);
void Func_80009c0(vec3_t *a, vec3_t *b);
fx32 Func_80008ac(fx32 num, fx32 denom);
void Func_8005258(fx32 focal, fx32 zMin, fx32 zMax);
void Func_80c0a24(int a, int b, int c, int d, int e);

void Func_80b8fd4(int n)
{
    struct View *s;
    int *q;
    vec3_t t;
    void (*transform)(vec3_t *, vec3_t *);
    fx32 (*divide)(fx32, fx32);
    fx32 r;

    s = (struct View *)iwram_3001e80[0];
    q = (int *)iwram_3001e80[0x20];
    s->v1.y = 0xa0 << 11;
    s->v1.x = 0;
    s->v1.z = 0;
    q[0] = 0x80 << 7;
    s->yaw = 0x80 << 7;
    s->pitch = 0xf4 << 8;
    s->v2.y = 0;
    s->v2.z = 0x2ee0000;
    s->v2.x = 0;
    InitMatrixStack();
    MatrixTranslatev(&s->v1);
    MatrixYaw(s->yaw);
    MatrixPitch(s->pitch);
    t.x = 0;
    t.y = 0;
    t.z = s->v2.z;
    transform = Func_80009c0;
    transform(&t, &s->v0);
    divide = Func_80008ac;
    r = divide(0x3c90000, 0xc0 << 8);
    Func_8005258(0, r, 0x7920000);
    gPhysVec[4] = n + 0x78;
    q[4] = 1;
    Func_80c0a24(0xf0 << 15, (0x76 - n) << 16, 0, 0x80 << 4, 0x80 << 10);
    q[5] = 1;
    q[4] = 0;
}
