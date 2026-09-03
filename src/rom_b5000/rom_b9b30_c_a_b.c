/* Func_80ba918  --  0x080ba918
 *
 * Cut out of goldensun/asm/rom_b5000/rom_b9b30_c_a.s.
 *
 * Walks every sprite the enumerator hands back and, for each, sets the first
 * part's low byte to n and every later part's to 0, forcing 0xff into each
 * part's +0x16 flags byte on the way.
 *
 * `x |= LITERAL` ON A BYTE FIELD IS DELETED WHEN THE LITERAL IS ALL-ONES; A
 * REGISTER MASK IS NOT. MEASURED: `q->f16 |= 0xff` in the OUTER block compiled
 * to `mov r3,#255 / strb r3,[r2,#22]` -- combine folded `x | 0xff` to `0xff`
 * and dropped the ldrb and the orr entirely, coming out six instructions SHORT
 * of the ROM. The SAME source text inside the inner loop survived as
 * ldrb / orr / strb, because loop.c had already hoisted the literal into a
 * pseudo before combine ran, so combine only ever saw `(ior reg reg)`.
 *
 * So a ROM read-modify-write with an all-ones mask OUTSIDE a loop proves the
 * mask was a NAMED VARIABLE in the source; inside a loop it proves nothing.
 * That also explains the r10: a named `int mask` is live across the `bl`, so
 * global-alloc gives it a callee-saved high register and every use costs a
 * `mov rLow, r10`.
 *
 * REFINEMENT TO THE orr-DESTINATION LEVER. The documented cures for a constant
 * landing in the destination -- `*p = K | *p`, `*p = *p | K`, a narrow local,
 * an int local -- were all tried and ALL FOUR SCORED IDENTICALLY at 13 of 50.
 * When the second operand is a REGISTER-HELD MASK rather than a literal, the
 * fix is to name the LOADED VALUE instead of the constant:
 * `t = q->f16; q->f16 = t | mask;` took 13 to 6 in one step, with `int t` and
 * `unsigned char t` interchangeable. The existing entry covers only naming the
 * constant; this is the mirror case, and it is the one that applies whenever
 * the ROM's mask arrives in a register.
 *
 * A RELOAD SCRATCH REGISTER IS A STATEMENT-ORDER TELL. The last six differences
 * were `mov r1, r10 / orr r3, r1` against the ROM's `mov r4, r10 / orr r3, r4`,
 * plus the matching prologue scratch. READ from .18.greg: "Spilling for insn
 * 48. Using reg 1 for reload 0" -- reload takes the LOWEST free register, so
 * the ROM choosing r4 means r1 was ALREADY OCCUPIED at allocation time. The
 * only thing that could occupy it is the pointer p, which sched2 later moved
 * below the orr. Hoisting `p = &s->parts[1];` above the read-modify-write in
 * the source took 6 to 0.
 *
 * GENERALISATION: when the only residue is a scratch-register rotation, do NOT
 * reach for allocation-order arithmetic. Ask which source statement the ROM
 * must have evaluated EARLIER to make the low register busy, and hoist it --
 * sched2 will put it back where the ROM shows it. This is the LICM "promote AND
 * sweep the position" rule one level down. (Only the ordering constraint is
 * real: p merely has to be assigned before the OR, and three placements
 * satisfying that all match.)
 *
 * `pop {r1} / bx r1` NAMES A RETURN VALUE, BUT `return s;` IS NOT THE SPELLING.
 * gcc const-propagates s == 0 off the loop-exit edge and emits an extra
 * `mov r0, #0`. What works is an `int` return type with NO return statement at
 * all: r0 stays live at the epilogue and nothing is materialised. That is the
 * second face of that entry.
 *
 * THE ROM'S `sub` SITTING AFTER THE HOISTED CONSTANTS IS THE TELL FOR AN UPWARD
 * `for`, NOT FOR A HAND-WRITTEN do/while. An explicit `c--` statement lands in
 * the preheader BEFORE the LICM hoists, while a plain `for (j = 1; j < c; j++)`
 * lets check_dbra_loop synthesise the countdown AFTER them, which is the ROM's
 * order.
 */

struct Q {
    unsigned char pad00[5];
    unsigned char f5;
    unsigned char pad06[0x16 - 6];
    unsigned char f16;
};

struct Spr {
    unsigned char pad00[0x27];
    unsigned char count;
    struct Q *parts[1];
};

extern struct Spr *Func_80b7f70(void *a, int i);

int Func_80ba918(void *a, int n)
{
    struct Spr *s;
    struct Q *q;
    struct Q **p;
    int i;
    int c;
    int t;
    int j;
    int mask;

    i = 0;
    mask = 0xff;
    while ((s = Func_80b7f70(a, i)) != 0) {
        q = s->parts[0];
        p = &s->parts[1];
        t = q->f16;
        q->f16 = t | mask;
        c = s->count;
        q->f5 = n;
        for (j = 1; j < c; j++) {
            q = *p++;
            q->f5 = 0;
            q->f16 |= 0xff;
        }
        i++;
    }
}
