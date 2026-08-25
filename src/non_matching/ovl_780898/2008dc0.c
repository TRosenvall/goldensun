/* OvlFunc_883_2008dc0  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_a_c_c_a_c_c.s
 * Best screen: 2 instructions in disagreeing regions, of 16 (streams same length).
 *
 * BLOCKER CLASS: argument-setup ordering.
 *
 * THIS FILE STANDS FOR A GROUP OF SEVEN.  The same four-call shape, differing
 * only in constants, appears as:
 *
 *      OvlFunc_883_2008dc0   ovl_30_c_c_c_a_a_a_a_c_c_a_c_c.s
 *      OvlFunc_883_2008e54   ovl_30_c_c_c_a_a_a_c_c_c_a_a.s
 *      OvlFunc_883_2008e84   ovl_30_c_c_c_a_a_a_c_c_c_a_a.s
 *      OvlFunc_883_2008f5c   ovl_30_c_c_c_a_a_a_c_c_c_a_a.s
 *      OvlFunc_883_2008f8c   ovl_30_c_c_c_a_a_a_c_c_c_a_a.s
 *      OvlFunc_884_200881c   rom_784360/ovl_30_c_a_a_a_c_c_c_c_c_a.s
 *      OvlFunc_884_20088ac   rom_784360/ovl_30_c_a_a_a_c_c_c_c_c_c_c.s
 *
 * Six of them have the identical instruction shape
 * `mov r1, #K / mov r0, #0 / lsl r1, #1 / ldr r2, =P`; this one has the
 * operands transposed (`mov r2, #0xd2 / mov r0, #0 / ldr r1, =0x101 /
 * lsl r2, #1`) and is otherwise the same.  Two were screened and both land on
 * exactly 2 of 16 with the same single cause.  ONE FIX WOULD TAKE ALL SEVEN.
 *
 * THE CAUSE
 *
 *      rom   mov r2, #0xd2 / mov r0, #0x0  / ldr r1, =0x101 / lsl r2, #0x1
 *      ours  mov r2, #0xd2 / ldr r1, =0x101 / lsl r2, #0x1  / mov r0, #0x0
 *
 * The ROM materialises the FIRST argument second, between the base of the
 * shifted argument and the pool load. gcc sinks it to last, because a
 * dependency-free `mov` of a small constant has nothing to hold it in place.
 *
 * WHAT WAS TRIED -- NOTHING MOVES IT
 *
 *  Source spellings, all byte-identical to each other:
 *    1. The literal `0` passed directly.
 *    2. A local assigned `z = 0` at the very top of the function, before the
 *       first call, then passed.
 *    3. The shift as its own statement (`k <<= 1`) rather than in the argument.
 *    4. The shift folded into the initialiser (`k = 0xe0 << 8`), tested on the
 *       sibling OvlFunc_909_2009958.
 *
 *  Compiler flags, all byte-identical to the default:
 *    -fno-schedule-insns, -fno-peephole, -fno-caller-saves, -fomit-frame-pointer
 *
 *  `--no-sched2` is WORSE, 4 of 16, so the second scheduler is wanted here and
 *  is not the thing placing this instruction.
 *
 * Since no scheduler flag reaches it, the placement is decided during argument
 * expansion, before scheduling runs at all -- which is why source order cannot
 * influence it either.
 *
 * THIS IS NOW THE DOMINANT SMALL-FUNCTION BLOCKER.  Counting only what has been
 * screened, it holds these seven plus OvlFunc_930_2008870 (2 of 24),
 * OvlFunc_930_20088a8 (5 of 24) and OvlFunc_909_2009958 (6 of 18) -- ten
 * functions, every one of them within six instructions.  It belongs with the
 * -fno-rerun-cse-after-loop count in HANDOFF.md as evidence for a compiler
 * difference rather than a source problem.
 */
extern unsigned char L7544[] __asm__(".L7544");
extern void __PlaySound(int id);
extern void __Func_8010560(void *p, int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_8091e9c(int a);

void OvlFunc_883_2008dc0(void)
{
    int k;

    __PlaySound(0xbc);
    __Func_8010560(L7544, 0x2d, 0xb);
    k = 0xd2;
    __Func_809218c(0, 0x101, k << 1);
    __Func_8091e9c(0xb);
}
