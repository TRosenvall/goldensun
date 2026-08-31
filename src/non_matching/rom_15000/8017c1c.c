/* Func_8017c1c  @ 0x08017c1c  [rom_15000]
 *
 * Source asm: goldensun/asm/rom_15000/rom_178b0_c.s
 *
 * BLOCKER: a pooled zero and a three-operand address form. 47 lines against
 * 47, SEVEN differing, from 47 on the first transcription.
 *
 * TWO LEVERS DID THE WORK, and both are reusable:
 *
 *   1. HOISTING THE ZERO ABOVE THE GUARD FIXED THE WHOLE PROLOGUE. Written as
 *      `z = 0;` inside the `if`, gcc needs r0 for it, so the incoming first
 *      argument gets saved to a callee-saved register and every parameter
 *      shifts: 47 differing with the first at line 1. Assigning `z = 0;`
 *      before the `if` frees r0, the ROM's parameter registers fall out, and
 *      the first difference moves to line 12. 47 -> 36.
 *   2. NAMING THE BYTE OFFSET gives the register-offset store. The ROM has
 *      `strh r2, [r4, r3]`; written inline as
 *      `*(unsigned short *)(base + (0xeb << 4) + (*q << 1)) = z` gcc
 *      materialises the address with an `add` first. Assigning
 *      `off = (0xeb << 4) + (*q << 1);` and storing to `base + off` gives the
 *      ROM's form. 36 -> 7.
 *
 * Note (2) is the OPPOSITE of the batch-156 rule that a local holding only an
 * address should be deleted. The distinction is what the local holds: an
 * ADDRESS gcc can rematerialise buys nothing and constrains the schedule, but
 * an INDEX participates in the addressing mode and naming it is what selects
 * the register-offset form. Ask which you have.
 *
 * WHAT IS LEFT:
 *   - 1 line: the ROM pools the stored zero (`ldr r2, =0x0`) where we emit
 *     `mov r2, #0x0`. The documented rule is that a zero stored through a u16
 *     needs an `int` local; it HAS one, hoisted, and gcc still builds it.
 *     Tried: `unsigned int` instead of `int` (no change).
 *   - 6 lines: the tail's address arithmetic. The ROM shifts in place and
 *     reuses (`lsl r1, #1` / `add r2, r1, r3` / `add r1, r4, r1`); we emit a
 *     three-operand shift and a different add split, which also puts the `bl`
 *     on the wrong side of the pool branch. Writing `n = n * 2;` as its own
 *     statement before the call does not move it.
 *
 * The pool shape itself is NOT the blocker -- lengths already agree at 47.
 */
struct S {
    unsigned char pad00[0xc];
    unsigned short fc;
    unsigned short fe;
};

extern char *iwram_3001e8c;
extern void Func_801de5c(void *a, void *b, void *c);

void Func_8017c1c(void *p, struct S *s, int x, int y)
{
    char *base;
    unsigned short *q;
    int z;
    int off;
    int n;

    base = iwram_3001e8c;
    z = 0;
    if (p == 0) {
        q = (unsigned short *)(base + 0x12b2);
        p = base + (0xeb << 4);
        off = (0xeb << 4) + (*q << 1);
        *(unsigned short *)(base + off) = z;
        *q = (*q + 1) & 0x1ff;
    }
    n = ((s->fe + y + 1) << 5) + (s->fc + x) + 1;
    if ((unsigned int)n < 0x280) {
        n = n * 2;
        Func_801de5c(p, base + n, (void *)(0x6002000 + n));
    }
}
