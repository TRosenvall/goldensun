/* Func_8029274 -- asm/rom_15000/rom_23178_a_c.s
 *
 * BLOCKER: post-reload scheduling + one allocator preference. 6 of 47,
 * LENGTH EXACT. Came down from 12 by four separate documented levers, so this
 * file is mostly a record of which ones bit and which backfired.
 *
 * WHAT IT IS. Value to hex string: mask a nibble, add 0x30 or 0x37, write it
 * into an 8-byte stack buffer least-significant digit first, then copy the
 * buffer back out in reverse. The digit count is clamped to 5.
 *
 * THE FOUR THAT WORKED, isolated one at a time from a 12-differing baseline:
 *
 *   name the 0xf mask in a local assigned BEFORE the buffer pointer   12 -> 10
 *       (fixes `mov r6,#0xf / mov r4,sp` birth order)
 *   invert the digit test to `if (d <= 9)` so the ROM's FALLTHROUGH is
 *       the 0x30 arm                                                  12 -> 10
 *   compare the copy-back pointer signed, `(int)p >= (int)buf`         8 -> 7
 *       (ROM uses `bge`; a plain pointer compare is unsigned, `bcs`)
 *   make the digit UNSIGNED so the nibble test is `bhi` not `bgt`      7 -> 6
 *
 * Combined: 6 of 47, length exact.
 *
 * THE TWO THAT BACKFIRED, and they are the useful part:
 *
 *   rewriting the copy-back loop in int arithmetic (`q = i + lim`,
 *   `*(char *)q`) instead of pointers                    12 -> 26, ONE SHORT
 *
 *       This looked like the right move because it produces the signed
 *       compare the ROM has. It does -- but converting the loop BODY to
 *       integer arithmetic costs far more than the compare gains. Casting
 *       ONLY the comparison, and leaving the body as pointer dereferences,
 *       gets the same `bge` for free. Change one thing at a time: bundling
 *       these four edits at once produced 25 differing and hid the fact that
 *       three of them were correct.
 *
 *   a SEPARATE pointer variable for the copy-back loop     6 -> 21, two short
 *
 *       Motivated by the ROM using different registers for the two loops
 *       (r4 then r1), which looked like two variables. It is not: giving gcc
 *       a second pointer changes how the buffer address itself is kept and
 *       wrecks the prologue. Declaring it before or after `p` is identical.
 *
 * WHAT REMAINS, two clusters:
 *
 *   1. rom `strb r3,[r4] / add r2,#1`   ours `add r2,#1 / strb r3,[r4]`
 *      Pure scheduling. Moving `i++` after `p++` in the source changes
 *      nothing -- measured, byte-identical.
 *
 *   2. rom keeps the copy-back pointer in r1 (the register that held the
 *      digit count); gcc puts it in r4, which is free under -fcall-used-r4.
 *      Both are dead-correct; the ROM reuses a register gcc has no reason to
 *      prefer. This is the same allocator-preference class as
 *      src/non_matching/rom_f4000/80f4100.c.
 *
 * Nothing in the remaining six is reachable by naming or ordering, which the
 * two backfires above bound fairly tightly.
 */
void Func_8029274(unsigned int val, unsigned int n, char *out)
{
    char buf[8];
    char *p;
    int i;
    unsigned int d;
    int mask;

    if (n > 5)
        n = 5;
    i = 0;
    if (n != 0) {
        mask = 0xf;
        p = buf;
        do {
            d = val & mask;
            if (d <= 9)
                d += 0x30;
            else
                d += 0x37;
            *p = d;
            i++;
            val >>= 4;
            p++;
        } while (i != n);
    }
    i = n - 1;
    if (i >= 0) {
        p = buf + i;
        do {
            *out = *p;
            p--;
            out++;
        } while ((int)p >= (int)buf);
    }
}
