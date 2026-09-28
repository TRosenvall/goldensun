/* Actor_TravelTo -- 0x0800d14c, the only function in asm/rom_9000/rom_ca6c_c_a.s,
 * which carries no data.  WHOLE-FILE conversion: 440 bytes, 212 encodings and 7
 * relocations identical.
 *
 * THE SECOND LEVER IS A ONE-VALUE LEVER, AND ITS BOUNDARY IS MEASURED.  `mag` named
 * before the call is 5 -> 4 of 212 (the lever the landed sibling
 * src/rom_9000/rom_d924_c_c_b.c records, and what makes the pool load pick r3).  Then
 * naming the THIRD product separately -- `p3 = dz * dz;` -- is 4 -> 0: the third
 * product's pseudo otherwise has a live range disjoint from the second's, reload gives
 * both r3, and the anti-dependence pins `add r0,r3` before `mul r3,r6`.  Naming it makes
 * it live across the first add, it takes r2, and the scheduler emits the ROM's
 * `mul r2,r6 / add r0,r3 / add r0,r2`.
 *
 * BUT NAMING ALL THREE PRODUCTS IS 67 OF 212 AT 432 BYTES -- four instructions short.
 * So this is not "name the products"; it is one specific product, and the boundary is
 * part of the finding.  Inert or worse: the function pointer hoisted to the top (117),
 * calling the symbol with no pointer local (179), and all six permutations of the three
 * squares (7-9).
 *
 * Reading notes (batch 292 brief F):
 *  - epilogue `pop {r0} / bx r0` => VOID (the `pop {r1}` tell is absent).
 *  - `Func_80008ac(r0, r1)` returns r1/r0: the first site is
 *    div(accel, mul(speed,speed)) and the physics only works as speed^2/accel,
 *    so the LANDED prototype in src/rom_f4000/rom_f4008_a_a_c.c naming it
 *    (num, denom) has the two backwards.  Declared (int,int) here.
 *  - Func_8000888 is reached through math.h's fx32_multiply (unpinned "r" form)
 *    because each region has 3 sites sharing ONE register -- the 2+-site rule in
 *    docs/elevation.md "The `.call_via` helper, corrected".
 *  - Func_8000948 and Func_80008ac are plain function-pointer calls
 *    (`ldr r3,=X / bl _call_via_r3`).
 *  - `mov r3,#0x56 / add r3,r7 / mov r12,r3` is a named pointer local; the
 *    0x58/0x55 reads are inline (`mov r3,r7 / add r3,#0x58`).
 *  - d is REUSED for the div result (merge lever: the result can be the input).
 *
 * EXACT: 440 bytes, 212 encodings and 7 relocations identical (objcmp --func).
 * NO SHIMS: no PIN macros, no "+r" barriers, no volatile, no DMA3_SET, no .equ.
 * math.h's fx32_multiply is the tree's existing inline (52+ landed files use it).
 *
 * LOAD-BEARING CONSTRUCTS, each measured as a single drop from the next-best:
 *  1. `mag` named before the call (the recorded lever in the landed sibling
 *     src/rom_9000/rom_d924_c_c_b.c: "the sum of squares completes before the
 *     function pointer is materialised").  5 -> 4 of 212, and it is what makes
 *     the pool load `ldr r3,=Func_8000948` pick r3 instead of r2 (the
 *     relocation `_call_via_r2` -> `_call_via_r3`).
 *  2. `p3 = dz * dz;` named SEPARATELY while mag stays named.  4 -> 0.
 *     WHY: the third product's pseudo otherwise has a live range DISJOINT from
 *     the second's, so reload gives both r3; the anti-dependence then pins
 *     `add r0,r3` BEFORE `mul r3,r6`.  Naming it makes it live across the first
 *     add, it gets r2, and the scheduler emits the ROM's
 *     `mul r2,r6 / add r0,r3 / add r0,r2`.  `mag += p3;` as a separate
 *     statement is equally exact (both measured OK).
 *     NOTE the boundary: naming ALL THREE products is 67 of 212 at 432 bytes
 *     (4 instructions SHORT) -- this is a one-value lever, not "name the
 *     products".
 *  3. Both `/ 0x10000` divisions and the abs pairs came out first try.
 *     The SECOND abs pair is written in-place on dx (`if (dx < 0) dx = -dx;`)
 *     and the FIRST as ternaries into new locals, because the ROM negs into a
 *     fresh register in the first pair and into r1 itself in the second.
 *
 * INERT / WORSE, measured:
 *   - `fp = Func_8000948;` at the top of the function: 117 of 212, 436 bytes.
 *   - calling the symbol directly without a pointer local: 179 of 212, 432.
 *   - all six permutations of the three squares (7, 8, 9, 9, 9, 9 of 212).
 *   - `dx*dx + (dy*dy + dz*dz)`: 9.  Explicit parens around the left pair: 4
 *     (identical to no parens, as expected).
 */
#include "math.h"

extern int Func_8000948(int v);
extern int Func_80008ac(int a, int b);
extern int FastIntSqrtFP1616_RAM(int v);

void Actor_TravelTo(unsigned char *r0, int x, int y, int z)
{
    unsigned char *a;
    unsigned char *axis;
    int dx;
    int dy;
    int dz;
    int d;
    int mag;
    int p3;
    int q;
    int adj;
    int ax;
    int ay;
    int az;
    int (*fp)(int);
    int (*div)(int, int);

    a = r0;
    dx = (x - *(int *)(a + 8)) / 0x10000;
    dy = (y - *(int *)(a + 0xc)) / 0x10000;
    dz = (z - *(int *)(a + 0x10)) / 0x10000;
    p3 = dz * dz;
    mag = dx * dx + dy * dy + p3;
    fp = Func_8000948;
    d = fp(mag) << 16;
    if (d < 0x80 << 13) {
        int m1;
        int m2;
        int m3;
        dx = x - *(int *)(a + 8);
        dy = y - *(int *)(a + 0xc);
        dz = z - *(int *)(a + 0x10);
        m1 = fx32_multiply(dx, dx);
        m2 = fx32_multiply(dy, dy);
        m3 = fx32_multiply(dz, dz);
        d = FastIntSqrtFP1616_RAM(m1 + m2 + m3);
    }
    if (d < 0x80 << 9) {
        *(int *)(a + 8) = x;
        *(int *)(a + 0xc) = y;
        *(int *)(a + 0x10) = z;
        *(int *)(a + 0x38) = 0x80 << 24;
        *(int *)(a + 0x3c) = 0x80 << 24;
        *(int *)(a + 0x40) = 0x80 << 24;
    } else {
        if (*(a + 0x58) == 0) {
            int s = *(int *)(a + 0x30);
            q = fx32_multiply(s, s);
            div = Func_80008ac;
            q = div(*(int *)(a + 0x34), q);
            if (d > q)
                adj = d - q / 2;
            else
                adj = d / 2;
            div = Func_80008ac;
            d = div(d, adj);
            x = *(int *)(a + 8) + fx32_multiply(x - *(int *)(a + 8), d);
            y = *(int *)(a + 0xc) + fx32_multiply(y - *(int *)(a + 0xc), d);
            z = *(int *)(a + 0x10) + fx32_multiply(z - *(int *)(a + 0x10), d);
        }
        *(int *)(a + 0x38) = x;
        *(int *)(a + 0x3c) = y;
        *(int *)(a + 0x40) = z;
        dx = x - *(int *)(a + 8);
        dy = y - *(int *)(a + 0xc);
        dz = z - *(int *)(a + 0x10);
        axis = a + 0x56;
        *axis = 0x10;
        ax = dx < 0 ? -dx : dx;
        az = dz < 0 ? -dz : dz;
        if (ax < az) {
            *axis = 0x12;
            dx = dz;
        }
        if (*(a + 0x55) == 0) {
            if (dx < 0)
                dx = -dx;
            ay = dy < 0 ? -dy : dy;
            if (dx < ay)
                *axis = 0x11;
        }
    }
}
