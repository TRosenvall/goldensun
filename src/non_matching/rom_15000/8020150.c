/* Func_8020150 (0x08020150) -- NON-MATCHING.
 * Blocker class: gcc takes a second high register where the ROM spills to the
 * outgoing-argument slot.
 *
 * 37 lines against 37, 32 differing, and the 32 are one rotation. Both keep the
 * loop's constant 0x10 in r8 -- that part came out unprompted -- but they
 * differ on the loop counter:
 *
 *              a      p      i      0x10   n
 *     rom      r7     r5     r6     r8     r4, SPILLED to [sp, #4]
 *     ours     r8     r6     r7     r10    r5
 *
 * `-fcall-used-r4` makes r4 caller-saved, so the ROM's `n` cannot survive the
 * call in a register and is stored to the frame -- and the slot it uses is the
 * SIXTH ARGUMENT's own outgoing slot, which the call is about to write anyway.
 * gcc declines the spill and takes r10 instead, paying a save, a restore and a
 * `mov` per use.
 *
 * MEASURED (rom 37 lines, both at exact length):
 *   `(signed char)*p` on an `unsigned char *`      37, 32 -- gcc emits a single
 *                          `ldrsb r0, [r6, r0]` where the ROM has
 *                          `ldrb / lsl #24 / asr #24`
 *   `(*p << 24) >> 24` written as explicit shifts  37, 32 (inert -- gcc
 *                          recognises the idiom and still emits `ldrsb`)
 *
 * WHAT THIS FUNCTION ESTABLISHES ANYWAY: the r8 is not the problem. The
 * constant 0x10 has to survive the call with r4-r7 already committed, so gcc
 * puts it in r8 exactly as the ROM does, with no lever at all. That is the
 * result this round turned on -- see the docs entry.
 *
 * NEXT: nothing source-level for the spill-versus-r10 choice.
 */
extern void Func_801e9d4(int c, int b, void *a, int n, int m, int n2);

void Func_8020150(void *a, unsigned char *b)
{
    unsigned char *p;
    int n;
    int i;
    int m;

    if (a == 0)
        return;
    m = 0x10;
    p = b + 0x28;
    n = 0;
    i = 3;
    do {
        Func_801e9d4((signed char)*p, 2, a, n, m, n);
        i--;
        p++;
        n += 0x18;
    } while (i >= 0);
}
