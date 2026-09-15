/* InitAnimContext -- 0x080b9d34, split out of asm/rom_b5000/rom_b9b30_a.s.
 *
 * Builds an animation context from a descriptor: copies two bit-fields out of
 * the descriptor's flag word, then walks its unit-id list and keeps the ids
 * whose unit is alive (or all of them, if a flag bit says so), falling back to
 * the first id if none qualified.
 *
 * FOUR LEVERS, 66 differing to exact, and three of them are recorded ones:
 *
 *   1. `s8` IS UNSIGNED IN THIS TREE. include/gba/types.h has
 *      `typedef char s8;` and Camelot's code is built with __CHAR_UNSIGNED__,
 *      so `s8` gives `ldrb` where the ROM has `ldrsb`. The list length needs
 *      `signed char` spelled out. Worth 35 differing and two instructions of
 *      length -- and it is a trap in the OPPOSITE direction to the usual one,
 *      because the type is NAMED s8.
 *   2. THE OUTPUT IS INDEXED BY THE COUNT, NOT A WALKING POINTER. The ROM's
 *      `strh r3, [r7] / add r7, #2` advances exactly when the count does, so
 *      `c->f24[n] = *p;` is the source and strength_reduce makes the pointer.
 *      Writing the walking pointer instead (`*q++ = *p;`) is 31 differing:
 *      it becomes a variable competing for the register the context pointer
 *      wants, and the two swap. Same shape as the batch-265 result -- an
 *      induction variable's final form is evidence about the PASS.
 *   3. THE INPUT POINTER IS COMPUTED INSIDE THE LOOP. The ROM's
 *      `add r5, r6, #2` sits AFTER the entry test, interleaved with the output
 *      pointer's setup. Hoisting it above the loop (`p = &s->f2;` before the
 *      `for`) leaves it above the `bge` and costs 5; writing
 *      `p = &s->f2 + i;` inside the body lets strength_reduce sink and reduce
 *      it to the ROM's form.
 *   4. THE FLAG WORD IS UNSIGNED. `(v & 0x3000) >> 12` on a signed `int` emits
 *      `asr`; the ROM has `lsr`. One encoding, and the only thing the
 *      side-by-side instruction diff showed once the formatting differences
 *      (hex against decimal, two-operand against three) were read past.
 *
 * THE LAST ONE WAS THE RETURN TYPE, again: the ROM ends `pop {r1} / bx r1`, and
 * a void function pops into r0 because r0 is dead. Declaring a return type it
 * never returns reserves r0. Fourth function this session to end on that.
 *
 * NOTE tools/tryc.py reports this function with its pool-placement warning --
 * the reference keeps its literal pool inside the function body. objcmp
 * verifies the pool word and `make compare` settles the placement.
 *
 * tools/datacheck.py confirms the source .s carries no data section, so this is
 * an ordinary split with no rehoming needed.
 *
 * EXACT: 144 bytes, 70 encodings, 1 relocation, measured three times.
 */
#include "gba/types.h"

struct Src {
    u8 f0;
    signed char f1;
    u8 f2;
    u8 pad03[0x55];
    int f58;
};

struct Ctx {
    int f0;
    int pad04;
    int f8;
    int fc;
    int f10;
    int f14;
    int f18;
    int f1c;
    int pad20;
    u16 f24[1];
};

extern u8 *_GetUnit(s32 id);
int InitAnimContext(struct Src *s, struct Ctx *c)
{
    int i;
    int n;
    u8 *p;
    u8 *u;
    u32 v;

    n = 0;
    c->f1c = n;
    v = s->f58;
    c->f0 = v & 0xfff;
    c->f18 = (v & (0xc0 << 6)) >> 12;
    c->f8 = s->f0;
    for (i = 0; i < s->f1; i++) {
        p = &s->f2 + i;
        u = _GetUnit(*p);
        if (*(s16 *)(u + 0x38) != 0 || (s->f58 & (0x80 << 9)) != 0) {
            c->f24[n] = *p;
            n++;
        }
    }
    if (n == 0) {
        c->f24[0] = s->f2;
        n = 1;
    }
    c->fc = s->f2;
    c->f14 = n;
    c->f10 = 1;
}
