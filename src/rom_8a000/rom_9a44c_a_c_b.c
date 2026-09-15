// fakematch
/* Func_809a7f4 -- 0x0809a7f4, split out of asm/rom_8a000/rom_9a44c_a_c.s.
 *
 * Advances a spiralling actor: steps x and z by cos/sin of its angle, bumps the
 * angle, and on the 0x79th tick hands it a new update function with a random
 * heading.
 *
 * THE `.call_via` IS NOT A COMPILER VENEER. The ROM's indirect call is the
 * project's own macro -- `.align 2, 0 / mov r12, pc / bx r5` -- and NOT gcc's
 * `bl _call_via_rN`, which is what an ordinary C function pointer emits and
 * what the first candidate produced. It comes from include/math.h's
 * `fx32_multiply`, an inline wrapper with that sequence written as inline asm.
 * Both spellings exist in this tree and they are different code: grep for
 * `bl _call_via` against `mov r12, pc` before deciding which one a ROM has.
 * The tell is the RELOCATIONS -- the veneer form carries an R_ARM_THM_CALL to
 * `_call_via_rN` and the macro form carries none.
 *
 * TWO LEVERS, 70 differing to 5:
 *
 *   1. BOTH PRODUCTS ARE COMPUTED BEFORE EITHER STORE. Written as two
 *      statements -- `a->f8 = b->f8 + fx32_multiply(...); a->f10 = ...;` -- gcc
 *      stores the first sum immediately after the first call and never needs a
 *      third callee-saved register. The ROM keeps the first product in r8
 *      across the sin call and does both loads and both stores afterwards, so
 *      the source names both products first. Worth 62 -> 5, and it is also what
 *      puts the third register in the prologue's push.
 *   2. THE SCALE MUST BE A NAMED LOCAL. `0x80 << 12` written as a literal at
 *      both call sites measures 29 differing: gcc rebuilds it with `mov/lsl`
 *      each time rather than holding it. Naming it is the batch-266 lever, and
 *      here the value is expensive enough (two instructions) that the name is
 *      sufficient -- unlike Func_80cd52c's -1, which needed arithmetic.
 *
 * fakematch: ONE register pin, and it is genuinely either. `x` pinned to r8 and
 * `m` pinned to r10 both measure EXACT; pinning the third value (`b` to r9) is
 * WORSE, 160 bytes against 156. Three local quantities -- .17.lreg places them
 * at r9/r8/r10 against the ROM's r8/r10/r9 -- rotate by one, and removing any
 * ONE of the two from the contest settles the other two. That either-works
 * signature is the recorded marker for a contest decided by removal rather than
 * by priority ordering.
 *
 * Declaration order is INERT: three permutations (x first, x before m, ang
 * before b) all measure 5 unchanged, which is the same result declaration order
 * has given on every register contest this session.
 *
 * EXACT: 156 bytes, 72 encodings, 5 relocations, measured three times.
 */
#include "math.h"

struct B {
    u8 pad00[8];
    int f8;
    u8 pad0c[4];
    int f10;
};

struct A {
    u8 pad00[6];
    u16 f6;
    int f8;
    u8 pad0c[4];
    int f10;
    u8 pad14[0x14];
    int f28;
    u8 pad2c[0x1c];
    int f48;
    u8 pad4c[0x18];
    u16 f64;
    u16 f66;
    struct B *f68;
    void *f6c;
};

extern void Func_809a738(void);
extern unsigned Random(void);
void Func_809a7f4(struct A *a)
{
    struct B *b;
    int ang;
    int m;
    register int x __asm__("r8");
    int y;

    b = a->f68;
    ang = a->f6;
    m = 0x80 << 12;
    x = fx32_multiply(m, cos(ang));
    y = fx32_multiply(m, sin(ang));
    a->f8 = b->f8 + x;
    a->f10 = b->f10 + y;
    a->f6 = a->f6 + (0x80 << 4);
    a->f64 = a->f64 + 1;
    if (a->f64 == 0x79) {
        a->f6c = Func_809a738;
        a->f64 = 0;
        a->f66 = 0;
        a->f48 = 0x1999;
        a->f28 = 0xc0 << 10;
        a->f6 = Random();
    }
}
