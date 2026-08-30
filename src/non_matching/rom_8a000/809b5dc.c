/*
 * Func_809b5dc  (AdvanceEffectCounters)  --  asm/rom_8a000/rom_9ad70_c_c_a.s
 *
 * BLOCKER: const-remat (a pooled 1) plus an address-computation interleave.
 * 47 lines against 48. ONE instruction short.
 *
 * THE ENTIRE TAIL IS ALREADY BYTE-IDENTICAL -- verified by dumping our own -S
 * output, not by trusting the screen. From the gState compare through both
 * __modsi3 arms, both Func_809b450 calls, the join, and the +6 fixup, every
 * instruction matches including the `b .L5` that makes the first arm one
 * longer than the second. Whatever is left is confined to the first eight
 * instructions.
 *
 * SETTLED, and each was worth a screen:
 *
 *   * `v = (*b)++` on a `short *`. The ROM does ldrh / add / strh and then
 *     lsl #16 / asr #16 on the ORIGINAL value. That separate sign-extension is
 *     the tell for a post-increment: a plain `n = *b; *b = n + 1;` reuses one
 *     ldrsh and never emits the shift pair.
 *
 *   * The gState offset must be built at RUNTIME. Writing
 *     `*(short *)(gState + (0xed << 1))` lets gcc fold the whole thing into one
 *     pooled `ldr r3, =gState+0x1da`; assigning gState to a local `unsigned
 *     char *g` first forces the ROM's `mov r2,#0xed / lsl r2,#1 / add r3,r2`.
 *     That single change was worth THREE instructions -- 44 lines to 47 -- and
 *     is the same lever that closed Func_808b25c.
 *
 * WHAT IS LEFT, both measured:
 *
 *   1. The ROM compares the gState halfword against a POOLED 1 (`ldr r3, =1 /
 *      cmp r2, r3`); we emit `cmp r3, #1`. Reading into a named `short m` and
 *      comparing `m == 1` does NOT pool it -- tried, no change. This looks like
 *      the halfword exception in const.sym, but the exception as written covers
 *      a constant that MEETS a halfword in an arithmetic expression, and a
 *      compare against an ldrsh result is apparently not that. Either the
 *      exception needs widening or this is a genuine _CONST_1 symbol; deciding
 *      that needs more measurements than one function can justify.
 *
 *   2. The +0x64 load and the +0x66 address are interleaved. The ROM finishes
 *      `add r3,#0x64 / mov r1,#0 / ldrsh r6` before touching +0x66; we compute
 *      both addresses first. Dropping the `a` pointer variable so the read is a
 *      bare `*(short *)(e + 0x64)` does NOT change it -- tried, byte-identical
 *      output. gcc pairs the two address computations on its own.
 *
 * Item 1 is one instruction and is the whole length difference. Solve it and
 * item 2 may well fall out with it, since the register numbering downstream is
 * what the interleave perturbs.
 */
extern unsigned char gState[];
extern void Func_809b450(unsigned char *e);

void Func_809b5dc(unsigned char *e)
{
    unsigned char *g;
    short *b;
    short m;
    int n;
    int v;

    n = *(short *)(e + 0x64);
    b = (short *)(e + 0x66);
    v = (*b)++;
    g = gState;
    m = *(short *)(g + (0xed << 1));
    if (m == 1) {
        if (v % 7 == 0)
            Func_809b450(e);
    } else {
        if (v % 5 == 0)
            Func_809b450(e);
    }
    if (n == 1)
        *(short *)(e + 6) += 0xc0 << 4;
}
