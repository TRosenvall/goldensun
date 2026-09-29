/* Func_800d340 / UpdateEntitiesXZ (0x0800d340) -- NON-MATCHING, 290 of 379 encodings differ.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_9000/800d340.c asm/rom_9000/rom_ca6c_c_c.s \
 *       --func Func_800d340
 *
 * SIZE AND COUNT BOTH MATCH EXACTLY: 788 bytes, 379 encodings, and the
 * RELOCATION SEQUENCE is right through the first five veneers.  So 290 IS a
 * distance, and it is ONE REGISTER-ALLOCATION DECISION wide.  aligncmp
 * (position-tolerant, masks nothing) reads 201 of 379 aligned-equal = 53.0%;
 * that number is low only because the pool offsets moved -- the ROM's literal
 * pool lands mid-function at 0x234, ours at 0x2fc, so every `ldr rX, [pc, #imm]`
 * and every long branch reads as a difference.
 *
 * THE ONE DIFFERENCE, STATED PRECISELY.  The ROM SPILLS `y` and keeps
 * `vx` in a register; we do the reverse.
 *
 *     ROM   r5=a  r11=x  r9=z  r10=vx  r8=fxmul-ptr   sp+0=f sp+4=arrived sp+8=y sp+0xc=i
 *     ours  r5=a  r9=x   r10=z r11=y   r8=fxmul-ptr   sp+0=ptr-spill sp+4=f sp+8=arrived sp+0xc=i
 *
 * The ROM's has-target block holds x, z, dx, dz, vx, q and the fx32_multiply
 * pointer across calls -- with r4 call-used (`-fcall-used-r4`) that is r6, r7,
 * r8, r9, r10, r11 plus r4, i.e. EVERY allocatable register -- so `y` had
 * nowhere to go.  Ours fits `y` in r11 because our `vx` reuses `dx`'s register
 * instead of taking a global one, which leaves r11 free.  Everything else in
 * the residue is downstream of that single choice:
 *
 *   1. `y` on the stack makes the ROM load `f` into r0 in the post-integration
 *      block (r4 is busy with `ldr r4, [sp, #8]`), which leaves r4 free, which
 *      is what lets `case 0x10` emit `mov r4, r11` and CROSS-JUMP into the
 *      shared `sub/sub/eor/cmp/bge` tail at .Ld596.  With `f` in r4 we emit
 *      `mov r0, r9` instead and the tail is duplicated.
 *   2. With x/z/y occupying r9/r10/r11, the no-target block's fx32_multiply
 *      pointer gets r4 -- call-clobbered -- and gets a str/ldr pair around each
 *      of the two real `bl _call_via_rN` calls.  The ROM's `mov r8, r3` is one
 *      instruction and no spill.
 *
 * MEASURED LADDER (ref 788 bytes / 379 encodings):
 *   first draft, no `f[1] != 0` guard, pointer bumps as body statements
 *                                              792 / 381, 278 differ
 *   + pointer bumps moved into the for-increment (the ROM's `continue` target
 *     is BEFORE all three bumps, so they are third-clause, not body)
 *   + `if (f[1] != 0)` around the switch          800 / 385, 287 differ
 *   + `t = *(int *)(a + 0x38);` named in case 0x10 only
 *                                                788 / 379, 290 differ   <- this file
 *
 * WHAT IS ESTABLISHED, and each item was read off the ROM rather than guessed:
 *   - THE CONTINUE TARGET FIXES THE LOOP SHAPE.  `.Ld62a` sits above `sub r3,#1`,
 *     `add r4,#0x70` AND `add r5,#0x70`, so the two pointer bumps are in the
 *     for-statement's third clause.  Written as the last two statements of the
 *     body instead, the `*(int *)a == 0` continue skips them -- a BUG, not just
 *     a mismatch, and it read 4 instructions shorter, which is how it was found.
 *   - TWO INDUCTION POINTERS, NOT ONE.  `f = a + 0x55` is a named local: the ROM
 *     reads f[0], f[1], f[3], f[5] and f[0xc] off one register.  It has to be a
 *     source local because Thumb `ldrb` reaches only +31, so `a[0x55]` alone
 *     would cost an `add` per access.
 *   - `if (f[1] != 0)` IS A SOURCE-LEVEL GUARD, not switch lowering.  gcc-2.96's
 *     three-case decision tree for {0x10, 0x11, 0x12} is exactly the ROM's
 *     `cmp #0x11 / beq / cmp #0x11 / bgt / cmp #0x10 / beq / b` and emits NO
 *     leading `cmp #0`.  Adding the guard costs exactly the ROM's two extra
 *     instructions.
 *   - `arrived = 0` LIVES INSIDE the `f[0xc] == 0` branch.  On the f[0xc] != 0
 *     path the ROM never initialises sp+4 and still tests it below, so the
 *     original reads an uninitialised value there.  Reproduced as-is.
 *   - THE ZERO STORES ARE CSE OF `arrived`.  `*(int *)(a + 0x24) = 0` comes out
 *     as `ldr r4, [sp, #4] / str r4, ...` because cse knows that pseudo holds 0.
 *     Written as a literal `0`, which is what produces it -- naming `arrived`
 *     explicitly is not needed.
 *   - `(vx | vz) != 0` uses `orr` (one branch); the facing test at .Ld602 uses
 *     `vx != 0 || vz != 0` (two `cmp`/`bne`).  The ROM spells the same idea two
 *     different ways in one function and both spellings are load-bearing.
 *   - Func_8000888 is math.h's `fx32_multiply` (the `mov r12, pc / bx rN` macro
 *     form); Func_8000948 and Func_80008ac are PLAIN FUNCTION POINTERS
 *     (`ldr rN, =X / bl _call_via_rN`).  Same split as the landed neighbour
 *     src/rom_9000/rom_ca6c_c_a.c (Actor_TravelTo, 0x800d14c), which is the
 *     function immediately below this one and the source of every convention
 *     here: `unsigned char *a` with `*(int *)(a + off)` casts, `/ 0x10000` for
 *     the rounding divide, `0x80 << 24` for the no-target sentinel, and a named
 *     `axis = a + 0x56`.
 *   - `t = vy < 0 ? -vy : vy` after `vy = -fx32_multiply(vy, *(int *)(a + 0x44))`
 *     gives the ROM's `mov r1, r3` reuse of the un-negated product; cse does it.
 *
 * INERT, MEASURED (all still 800/385 unless noted):
 *   - all six permutations of the three position loads;
 *   - declaring `y` first, last, or between the others;
 *   - naming the fx32_multiply products, the floor word, or the max-speed word;
 *   - `y += vy` vs `y = y + vy`; `y = vy + y`;
 *   - a dead `y = 0;` / `x = 0; z = 0;` before the loop (dce removes it);
 *   - swapping either square-sum's operand order (283-288 differ, same 385);
 *   - naming the sum before `fp = Func_8000948` -- the recorded lever from
 *     rom_ca6c_c_a.c -- 292/294/296 differ at 379, i.e. WORSE here;
 *   - reading the position triple through `p = (int *)(a + 8)`: 796 / 383.
 *   - naming `t` in ALL THREE switch arms: 784 / 377, TWO INSTRUCTIONS SHORT --
 *     gcc cross-jumps more than the ROM, so the ROM names it in at most two.
 *     0x10+0x12 named is also 788/379/290 (identical output to this file);
 *     0x11 or 0x12 alone is 804/387.
 *   - assigning dz before dx: 792/381 but it SPILLS x and grows the frame to
 *     0x14 -- a lower count that is further away, the warning in
 *     docs/elevation.md about counts not ranking candidates.
 *
 * NEXT: this is the REG_ALLOC_ORDER class in HANDOFF.md.  The testable step is
 * the one recorded there -- rebuild gcc-2.96 with REG_ALLOC_ORDER starting at 4
 * and re-screen.  Short of that, the lever to look for is anything that makes
 * `vx` a global allocno while leaving `y` without a register; every
 * pressure-raising local tried above either changed the instruction count or
 * was absorbed.
 */
#include "math.h"

extern unsigned char *iwram_3001e64;
extern int Func_8000948(int v);
extern int Func_80008ac(int a, int b);
extern int atan2(int dz, int dx);

void Func_800d340(void)
{
    unsigned char *a;
    unsigned char *f;
    unsigned char *axis;
    int i;
    int x;
    int y;
    int z;
    int dx;
    int dz;
    int d;
    int q;
    int vx;
    int vz;
    int vy;
    int s;
    int k;
    int t;
    int arrived;
    int ang;
    int (*fp)(int);
    int (*div)(int, int);

    a = iwram_3001e64;
    f = a + 0x55;
    for (i = 13; i >= 0; i--, a += 0x70, f += 0x70) {
        if (*(int *)a == 0)
            continue;
        y = *(int *)(a + 0xc);
        x = *(int *)(a + 8);
        z = *(int *)(a + 0x10);
        if (f[0xc] == 0) {
            arrived = 0;
            if (*(int *)(a + 0x38) != 0x80 << 24) {
                dx = (*(int *)(a + 0x38) - x) / 0x10000;
                dz = (*(int *)(a + 0x40) - z) / 0x10000;
                fp = Func_8000948;
                d = fp(dx * dx + dz * dz) << 16;
                if (d <= 0xffffff) {
                    dx = *(int *)(a + 0x38) - x;
                    dz = *(int *)(a + 0x40) - z;
                    fp = Func_8000948;
                    d = fp(fx32_multiply(dx, dx) + fx32_multiply(dz, dz)) << 8;
                }
                if (d == 0) {
                    x = *(int *)(a + 0x38);
                    z = *(int *)(a + 0x40);
                } else {
                    div = Func_80008ac;
                    q = div(d, *(int *)(a + 0x34));
                    vx = *(int *)(a + 0x24) + fx32_multiply(dx, q);
                    *(int *)(a + 0x24) = vx;
                    vz = *(int *)(a + 0x2c) + fx32_multiply(dz, q);
                    *(int *)(a + 0x2c) = vz;
                    fp = Func_8000948;
                    s = fp(fx32_multiply(vx, vx) + fx32_multiply(vz, vz)) << 8;
                    if (s > *(int *)(a + 0x30)) {
                        div = Func_80008ac;
                        k = div(s, *(int *)(a + 0x30));
                        *(int *)(a + 0x24) = fx32_multiply(vx, k);
                        *(int *)(a + 0x2c) = fx32_multiply(vz, k);
                    }
                }
            } else {
                vx = *(int *)(a + 0x24);
                vz = *(int *)(a + 0x2c);
                if ((vx | vz) != 0) {
                    fp = Func_8000948;
                    s = fp(fx32_multiply(vx, vx) + fx32_multiply(vz, vz)) << 8;
                    if (s != 0) {
                        t = s - *(int *)(a + 0x34);
                        if (t < 0) {
                            *(int *)(a + 0x24) = 0;
                            *(int *)(a + 0x2c) = 0;
                        } else {
                            div = Func_80008ac;
                            k = div(s, t);
                            *(int *)(a + 0x24) = fx32_multiply(vx, k);
                            *(int *)(a + 0x2c) = fx32_multiply(vz, k);
                        }
                    } else {
                        *(int *)(a + 0x24) = 0;
                        *(int *)(a + 0x2c) = 0;
                    }
                }
            }
            if ((f[0] & 2) != 0) {
                if (y > *(int *)(a + 0x14)) {
                    vy = *(int *)(a + 0x28) - *(int *)(a + 0x48);
                    *(int *)(a + 0x28) = vy;
                } else {
                    vy = *(int *)(a + 0x28);
                    if (vy < 0) {
                        y = *(int *)(a + 0x14);
                        vy = -fx32_multiply(vy, *(int *)(a + 0x44));
                        *(int *)(a + 0x28) = vy;
                        t = vy < 0 ? -vy : vy;
                        if (t <= *(int *)(a + 0x48)) {
                            *(int *)(a + 0x28) = 0;
                            vy = 0;
                        }
                    }
                }
            } else {
                vy = *(int *)(a + 0x28);
            }
        } else {
            vy = *(int *)(a + 0x28);
        }
        y = y + vy;
        x = x + *(int *)(a + 0x24);
        z = z + *(int *)(a + 0x2c);
        axis = a + 0x56;
        if (f[1] != 0)
        switch (f[1]) {
        case 0x10:
            t = *(int *)(a + 0x38);
            if (x == t || ((*(int *)(a + 8) - t) ^ (x - t)) < 0)
                arrived = 1;
            break;
        case 0x11:
            if (y == *(int *)(a + 0x3c)
             || ((*(int *)(a + 0xc) - *(int *)(a + 0x3c)) ^ (y - *(int *)(a + 0x3c))) < 0)
                arrived = 1;
            break;
        case 0x12:
            if (z == *(int *)(a + 0x40)
             || ((*(int *)(a + 0x10) - *(int *)(a + 0x40)) ^ (z - *(int *)(a + 0x40))) < 0)
                arrived = 1;
            break;
        }
        if (arrived != 0) {
            if (f[3] != 0) {
                *(int *)(a + 0x24) = 0;
                *(int *)(a + 0x2c) = 0;
                x = *(int *)(a + 0x38);
                z = *(int *)(a + 0x40);
                if (f[0] == 0) {
                    y = *(int *)(a + 0x3c);
                    *(int *)(a + 0x28) = 0;
                }
            }
            *(int *)(a + 0x38) = 0x80 << 24;
            *(int *)(a + 0x3c) = 0x80 << 24;
            *(int *)(a + 0x40) = 0x80 << 24;
            *axis = 0;
        }
        *(int *)(a + 8) = x;
        *(int *)(a + 0xc) = y;
        *(int *)(a + 0x10) = z;
        if ((f[5] & 1) != 0) {
            vx = *(int *)(a + 0x24);
            vz = *(int *)(a + 0x2c);
            if (vx != 0 || vz != 0) {
                ang = (short)(atan2(vz, vx) - *(unsigned short *)(a + 6));
                if (ang > 0x1000)
                    ang = 0x1000;
                if (ang < -0x1000)
                    ang = -0x1000;
                *(unsigned short *)(a + 6) = *(unsigned short *)(a + 6) + ang;
            }
        }
    }
}
