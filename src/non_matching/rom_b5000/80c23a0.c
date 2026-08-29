/* Func_80c23a0  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_b5000/rom_c1a34_a_a_c_c_a_a.s
 * Best screen: 4 instructions in disagreeing regions, of 16 (streams same length).
 *
 * BLOCKER CLASS: register allocation, r2 and r3 exchanged and nothing else.
 *
 *      rom   ldr r3, =tbl / lsl r2, r0, #3 / add r2, r3 / ldrb r0, [r2, #3]
 *      ours  ldr r2, =tbl / lsl r3, r0, #3 / add r3, r2 / ldrb r0, [r3, #3]
 *
 * The STRUCTURE is already right -- the table is loaded first, the index is
 * shifted second, and the index register is the destination of the add in both.
 * Only the two names are swapped.
 *
 * WHAT WAS TRIED
 *  1. A named index local added to the table cast to an integer, so the shifted
 *     value is unambiguously the accumulator.  Byte-identical, still 4 of 16.
 *  2. Inverting the arms so the table path is the branch target rather than the
 *     fall-through (the lever that landed OvlFunc_919_200826c in this same
 *     round).  MUCH WORSE, 12 of 16 -- gcc restructures the whole function.
 *
 * Attempt (2) is worth recording as a limit on that lever: inverting arms fixes
 * a branch SENSE when the two arms are otherwise symmetric. Here one arm is a
 * table walk and the other a single load, and swapping them changes which one
 * gcc lays out first.
 */
extern unsigned char Lc7420[] __asm__(".Lc7420");

int Func_80c23a0(int i)
{
    unsigned char *p;
    unsigned int v;

    if ((unsigned int)i > 0xab)
        return *(unsigned short *)Lc7420;
    p = Lc7420 + (i << 3);
    v = p[3];
    return (v << 27) >> 28;
}
