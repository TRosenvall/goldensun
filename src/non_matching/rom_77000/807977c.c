/*
 * Func_807977c  --  asm/rom_77000/rom_79460_c_c_a_a_a_c.s
 *
 * BLOCKER: register-role swap (callee-saved birth order).
 *
 * The C below screens at 42 lines against 42, 23 differing, and every
 * difference is one of two register-role swaps -- the instruction MULTISET is
 * already correct. The whole disagreement is in the prologue:
 *
 *      rom  mov r3,#0xf / add r3,r7 / mov r10,r3 / mov r3,#1 / ... / mov r8,r3
 *      ours mov r3,#0xf / mov r1,#1 / add r3,r7 / ...       / mov r10,r1 / mov r8,r3
 *
 * The ROM puts the LOOP END POINTER in r10 and the constant 1 in r8; we put 1
 * in r10 and the end pointer in r8. `dest` lands in r5 for the ROM and r6 for
 * us. Both are consequences of the same thing: which of the two loop-invariant
 * values gcc hoists FIRST. The ROM computes the end pointer first, so it takes
 * the lower-numbered slot.
 *
 * TRIED, both screened:
 *
 *   do { ... p++; } while (p <= L84a8c + 0xf);   -- 23 differing (best)
 *   for (p = L84a8c; p <= L84a8c + 0xf; p++)     -- 40 differing, WORSE
 *
 * The `for` spelling was the obvious lever: it puts the bound textually before
 * the body, which is the order the ROM hoists in. It made things worse, and
 * that is the finding worth keeping -- textual order of the two expressions is
 * NOT what picks the hoist order here. gcc hoists out of the rotated loop body
 * in its own order and the `for` only changed the loop shape.
 *
 * NOT YET TRIED: naming the end pointer as an explicit local before the loop
 * (`end = L84a8c + 0xf;`), which is the one spelling that makes the pointer a
 * real value with a birth point rather than a hoisted subexpression. That is
 * the next thing to attempt.
 *
 * The spill is genuine and reproduces: `n` is stored to [sp] across the call
 * and reloaded, in both the ROM and ours, so the register pressure is right.
 */
extern unsigned char L84a8c[] __asm__(".L84a8c");
extern int *Func_8077330(int n);

int Func_807977c(unsigned char *dest)
{
    unsigned char *p;
    int n;
    int b;

    p = L84a8c;
    n = 0;
    do {
        b = *p;
        if (*Func_8077330(0) & (1 << b)) {
            *dest = b;
            n++;
            dest++;
        }
        p++;
    } while (p <= L84a8c + 0xf);
    *dest = 0x20;
    return n;
}
