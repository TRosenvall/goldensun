/* Func_80da24c  --  0x080da24c
 *
 * Cut out of goldensun/asm/rom_c9000/rom_d9ab8_c_c_c_c.s.
 *
 * The .s header said the body was not traced. It is now.
 *
 * Builds the list of target slots an area-of-effect battle animation should
 * play over. The span is `n = s->f10 * 2 + 1`, an odd width centred on the
 * caster. The first loop finds where the caster (s->fc) sits in the halfword
 * slot table at +0x24, defaulting to index 0 if it is not there at all; the
 * second walks the whole span, recentres each position with
 * `idx + i - s->f10`, and writes through the ones that land inside
 * [0, s->f14) -- so targets that would fall off either end of the party are
 * dropped rather than clamped. The return value is how many survived.
 *
 * BOTH LOOP BOUNDS ARE `!=`, NOT `<`. The ROM guards each loop with
 * `cmp r5, #0 / beq` and closes them with `cmp r4, r5 / beq` and
 * `cmp r4, r5 / bne` -- equality throughout. A `<` bound compiles the guard as
 * `ble` and the close as a signed compare, which is a different instruction at
 * every one of those four sites. This is the same tell as BuildDraw2DFuncs and
 * is worth treating as a first-pass read of any counted loop: look at the
 * guard's condition code before writing the `for`.
 *
 * `s->f10` IS A DIRECT FIELD READ, not a named local, even though it is used
 * both to compute the span before the loop and inside the loop body. The ROM
 * reloads it at the top of every iteration after the first -- `ldr r1, [r0,
 * #0x10]` at the back-edge target, with the pre-loop load serving the peeled
 * first iteration. That is the standard rotated-loop shape for a field read
 * inside a loop, and naming it would hoist the load out.
 *
 * The subtraction is three-address (`sub r2, r3, r1`), so the recentred value
 * goes to its OWN destination rather than accumulating -- `v = idx + i - f10;`
 * as a statement, not a compound assignment onto the sum.
 *
 * The default index is invisible in the ROM. `mov r12, r6` sets it from the
 * already-zero count, and the match-at-zero and exhausted-without-match paths
 * both fall through without writing it, because `idx = i` at i == 0 stores what
 * is already there. Do not read the missing store as a missing case.
 *
 * Matched on the first candidate, no probing.
 */

struct S {
    unsigned char pad00[0xc];
    int fc;
    int f10;
    int f14;
    unsigned char pad18[0x24 - 0x18];
    short tbl[1];
};

int Func_80da24c(struct S *s, short *out)
{
    int n;
    int i;
    int idx;
    int count;
    int v;

    count = 0;
    idx = 0;
    n = s->f10 * 2 + 1;
    for (i = 0; i != n; i++) {
        if (s->fc == s->tbl[i]) {
            idx = i;
            break;
        }
    }
    for (i = 0; i != n; i++) {
        v = idx + i - s->f10;
        if (v >= 0 && v < s->f14)
            out[count++] = v;
    }
    return count;
}
