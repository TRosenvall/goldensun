/* Func_80c1014 (0x080c1014) -- NON-MATCHING.
 * Blocker class: REGISTER-ROLE SWAP plus one uncreatable copy.
 *
 * 31 lines against the ROM's 33. The bodies agree instruction for instruction;
 * the two missing lines and every register difference come from one thing the
 * ROM does and gcc will not:
 *
 *     rom    mov r5, sp / mov r7, r0 ... (guard) ... mov r8, r5
 *            then `mov r2, r8 / ldrsh r0, [r6, r2]` each iteration
 *     ours   mov r7, sp / mov r8, r0 ... no copy ...
 *            then `ldrsh r0, [r6, r7]` each iteration
 *
 * The ROM materialises the buffer address in r5, copies it into r8 INSIDE the
 * guard, and reloads it into a scratch register every iteration. We keep it in
 * one callee-saved register and index straight off that, which is a shorter and
 * obviously better allocation -- and two instructions the ROM has.
 *
 * MEASURED, all 31 lines and 25 differing, byte-identical to each other:
 *   one `base` local passed to the call and used in the loop
 *   a SECOND local `p = base;` assigned inside the guard -- the shape the
 *     ROM's `mov r8, r5` reads as; gcc coalesces it straight back
 *   the array passed directly to the call and `p = buf;` inside the guard,
 *     so the two materialisations have no common variable at all
 *
 * The third is the interesting negative: even with no shared name between the
 * call argument and the loop base, gcc still produces ONE address and never the
 * ROM's copy. This is the same wall as src/non_matching/ovl_77dd1c/2009498.c
 * from the previous round -- the C has no way to demand a particular
 * callee-saved register, or a redundant copy into one.
 *
 * WHAT IS RIGHT: the offset-first operand order on the load. The ROM's
 * `ldrsh r0, [r6, r2]` puts the OFFSET register first, which
 * `*(short *)(off + (int)p)` produces and `p[i]` does not -- the rule recorded
 * for register-offset loads, applied per access.
 *
 * NEXT: nothing source-level.
 */
extern int Func_80b6c08(int a, short *buf);
extern void Func_80c0f98(int v, int b);

void Func_80c1014(int arg0)
{
    short buf[14];
    short *base;
    int count;
    int n;
    int off;
    int v;

    base = buf;
    count = Func_80b6c08(3, base);
    if (count > 0) {
        off = 0;
        n = count;
        do {
            v = *(short *)(off + (int)base);
            if (v != arg0)
                Func_80c0f98(v, 1);
            n--;
            off += 2;
        } while (n != 0);
    }
}
