/* Func_80217a4 -- 0x080217a4  (asm/rom_15000/rom_20198_c_c_c_a_a_c_c_a.s)
 *
 * BLOCKER: register roles across three small blocks, plus one missing branch.
 * 19 of 68, one line short. The hard part of this function -- a bitfield pack
 * into a stack pair -- reproduces EXACTLY.
 *
 * PROGRESS, each step isolated:
 *
 *   57  naive, with the packing written as explicit int masks
 *   37  real BITFIELDS for the stack pair, and `unsigned` for the global
 *   22  the two byte/halfword masks named as `int` locals
 *   19  the 0x1f mask and -0x3f named in the ENTRY BLOCK   <- best
 *
 * WHAT THE BITFIELD STEP BOUGHT, because it is the reusable part. The ROM does
 *
 *     ldr r3, [sp] / ldr r4, =0xffff0000 / lsl r1,#16 / ldr r2, =0xffff
 *     lsr r1,#16 / and r3,r4 / orr r3,r1 / and r3,r2 / lsl r1,#16 / orr r3,r1
 *
 * which is visibly redundant -- the second `and` discards what the first `orr`
 * just merged. Written as explicit int masks gcc SIMPLIFIES it and the whole
 * region diverges. Written as two assignments to adjacent 16-bit bitfields it
 * is instruction-for-instruction exact, redundancy included, because each
 * assignment is its own read-modify-write and gcc does not look across them.
 * The tell is exactly that redundancy: work the ROM does that a reader would
 * delete means separate bitfield assignments, not one expression.
 *
 * It also reads an UNINITIALISED stack slot before the first store, which is
 * what a bitfield write to a fresh local looks like and not a decompilation
 * error.
 *
 * THE MASK-NARROWING RULE PAID TWICE. `o[0x17] & -0x3f` written with the
 * literal compiles to `mov r3, #0xc1` -- gcc narrows the mask to the byte
 * width -- where the ROM builds -0x3f with `mov`+`neg`. Naming it as an `int`
 * local keeps it 32-bit and gives the ROM's pair. Same for the 0xfffffe00 mask
 * on the halfword at 0x16, which narrowed to `mov r1,#0xfe / lsl r1,#8`.
 *
 * WHAT REMAINS is register assignment in three places and one branch:
 *
 *   ldr r1, [r1, r3]  against  ldr r1, [r3, r1]   -- which operand is the base
 *   the o[0x17] block: same instructions, r2 and r3 exchanged
 *   the o[0x16] block: same instructions, r1 and r2 exchanged
 *   the ROM ends the guarded block with `b` to a shared epilogue; ours falls
 *   through, which is the one-line shortfall
 *
 * Measured and inert or worse: writing the guard as `if (o != 0) { ... }`
 * instead of an early return (22, unchanged); putting the `res & 0x1f` term
 * first in the store expression (25, worse); naming the 0xfff0 and 0x1ff
 * constants in the entry block as well (19, unchanged); assigning m2 before the
 * halfword read (27, worse).
 *
 * So the naming lever is exhausted here -- it moved 57 to 19 and the rest is
 * the register-role swap, which nothing in the inventory reaches.
 */
typedef struct {
    unsigned int a : 16;
    unsigned int b : 16;
    unsigned int c : 16;
    unsigned int d : 16;
} P;

extern unsigned int iwram_3001800;
extern int L37230[] __asm__(".L37230");
extern int Func_8003d28(P *p);

void Func_80217a4(unsigned char *o)
{
    P p;
    int *t;
    int idx;
    int v;
    int res;
    int m1;
    int m2;
    int h;
    int k;

    k = 0x1f;
    m1 = -0x3f;
    t = L37230;
    idx = (iwram_3001800 >> 1) & 7;
    v = t[idx] / 256;
    if (o == 0)
        return;
    p.a = v;
    p.b = v;
    p.c = 0;
    res = Func_8003d28(&p);
    o[0x17] = (o[0x17] & m1) | ((res & k) << 1);
    o[0x15] |= 3;
    h = (*(unsigned short *)(o + 6) + 0xfff0) & 0x1ff;
    m2 = 0xfffffe00;
    *(unsigned short *)(o + 0x16) = (*(unsigned short *)(o + 0x16) & m2) | h;
    o[0x14] = o[8] + 0xf0;
    o[0xf] = 0xfc;
}
