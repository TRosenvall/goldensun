/* Func_80b0a20  --  0x080b0a20, asm/rom_b0000/rom_b0070_a_a_c_c_a_a.s
 *
 * BLOCKER CLASS: the width of a pooled ZERO, and the setup order that follows
 * from it.
 * Status: 28 lines against the ROM's 29 in the best spelling below.
 *
 * WHAT IT DOES
 * Writes an x and a y into an actor record and the sprite it points at: three
 * halfword copies each, a 9-bit field at bits 0-8 of the sprite halfword at
 * +0x16, and two flag bytes.
 *
 * FOUR THINGS ARE SETTLED AND SHOULD BE KEPT:
 *
 *   1. THE 0xffff IS A NAMED int, NOT A u16 CAST. `xs = (unsigned short)x`
 *      gives `lsl #16 / lsr #16`; the ROM masks with a register holding 0xffff,
 *      and reuses that register for y. Only a named `int m = 0xffff` produces
 *      that.
 *   2. THE 9-BIT FIELD IS HAND-WRITTEN, NOT A BITFIELD. Declared
 *      `unsigned short bf : 9`, gcc narrows the 0x1ff to a HALFWORD pool entry
 *      (`ldrh`); the ROM loads it as a word. Written as
 *      `t = b->f16; t = 0xfffffe00 & t; t |= x;` with int locals, both masks
 *      come out word-width and correct. That is the reverse of the batch-71
 *      bitfield rule and it is the mask WIDTH that decides, exactly as that
 *      rule says -- read the width, do not assume the construct.
 *   3. The value is the destination of both ANDs: `x &= m; x &= 0x1ff;`.
 *   4. `a->p` is re-read after the field write, because the write may alias it.
 *      That happens on its own and needs no help.
 *
 * WHAT IS LEFT is one instruction and its knock-on. The ROM loads the zero it
 * stores to +0xc as a WORD, `ldr r4, =0x0`, before the first byte store; gcc
 * pools it as a HALFWORD, `ldrh`, and emits it later.
 *
 * THE SYMBOL READING WAS TESTED AND IS NOT THE ANSWER. `ldr rN, =0x0` where
 * `mov rN, #0` would do looks like the pool tell, and a symbol address cannot
 * be narrowed, so a word-width pooled zero is suggestive. Substituting
 * `(int)(&_AREA_00)` DOES produce the ROM's `ldr r3, =0x0` -- the mechanism is
 * real -- but the function then comes out at 26 lines, three SHORT, because gcc
 * re-optimises the byte store around it. The same substitution on
 * Func_809b0dc, the other function with this symptom, goes from 1 differing
 * line to 4.
 *
 * So the width tell is real and the substitution is not a fix. Recorded that
 * way rather than as a naming lead, because guessing a namespace on top of a
 * spelling that makes the function WORSE would be two mistakes.
 *
 * REVISITED, and the missing instruction is identified.  It is not the zero at
 * all: the ROM's last instructions are
 *
 *      b .Lb0a64 / .pool_aligned / .Lb0a64: / pop {r5,r6} / pop {r0} / bx r0
 *
 * -- a real branch over an in-function LITERAL POOL placed before the epilogue.
 * That branch is the one line we are short.  `.pool_aligned` expands to
 * `.align 2, 0` + `.pool` (include/macros.inc), so this is pool placement, not
 * control flow, and no source spelling reaches it.
 *
 * MEASURED: across every elevated translation unit in the tree, ZERO generated
 * `.s` files contain a mid-function pool.  old_agbcc emits the pool at
 * `.func_end` and never early, so this pattern has never been reproduced here.
 *
 * THE LEAD, and it is a translation-unit one.  Three of the FOUR functions in
 * asm/rom_b0000/rom_b0070_a_a_c_c_a_a.s carry a mid-function `.pool_aligned`,
 * so early pool dumping looks like a property of how that TU was compiled rather
 * than of this function.  A single-function `.c` can therefore probably never
 * match it.  The test would be to elevate the cluster -- all four functions in
 * one `.c`, which the "Cluster X..Y" headers elsewhere in the tree show is a
 * supported shape -- and see whether the pool lands early on its own.
 *
 * That is a real experiment rather than another spelling, and it is the right
 * next step for this file.  It is also four functions' worth of work, so it
 * wants a round of its own rather than being tacked onto one.
 */

struct B {
    unsigned char pad00[6];
    unsigned short f6;
    unsigned short f8;
    unsigned char pad0a[0xa];
    unsigned char f14;
    unsigned char pad15;
    unsigned short f16;
};

struct A {
    struct B *p;
    unsigned short f4;
    unsigned short f6;
    unsigned short f8;
    unsigned short fa;
    unsigned char fc;
    unsigned char fd;
};

void Func_80b0a20(struct A *a, int x, int y)
{
    struct B *b;
    int m;
    int t;
    int z;

    b = a->p;
    m = 0xffff;
    a->fd = 1;
    b->f6 = x;
    a->f8 = x;
    a->f4 = x;
    x &= m;
    x &= 0x1ff;
    z = 0;
    a->fc = z;
    t = b->f16;
    t = 0xfffffe00 & t;
    t |= x;
    b->f16 = t;
    a->fa = y;
    a->f6 = y;
    a->p->f8 = y;
    y &= m;
    a->p->f14 = y;
}
