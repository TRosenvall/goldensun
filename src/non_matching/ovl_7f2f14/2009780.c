/* OvlFunc_968_2009780 -- 0x02009780,
 * asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c_a_a_c.s
 *
 * 49 of 49 lines, TWO differing.  Candidate at scratch/N9780_best.c.
 *
 * SCREEN IT WITH --cflags "-O2".  This file's name falls inside the
 * `ovl_30_c_a_c_a_c_c%` O1 wildcard belonging to a neighbouring stem, and at
 * -O1 it is 51 lines and 30 differing.  tryc.py prints the WILDCARD hint;
 * believe it.  An elevation would need an explicit Makefile rule pinning this
 * object to the default flags, as ovl_30_c_c_a_c_a_c_b did in batch 147.
 *
 * SOLVED, and it is the batch-150 rule paying off twice: BOTH halfword stores
 * need a TYPED FIELD.  Written through a cast, `*(short *)(p + 0xcba) = 0`
 * pools the literal -- `ldr r3, =0x0` where the ROM has `mov r3, #0` -- and the
 * same at +0xcb6 with the 1.  Declaring the object a struct with `short` fields
 * at those two offsets gives the mov form for both, in a scratch register, at
 * no cost.  27 differing -> 2, and the line count goes exact.
 *
 * BLOCKER: argument-construction interleave, and NO LEVER IS AVAILABLE.
 *      rom   mov r1, #0xe0 / mov r2, #0x0 / mov r0, #0xa / lsl r1, #0x8
 *      ours  mov r1, #0xe0 / mov r2, #0x0 / lsl r1, #0x8 / mov r0, #0xa
 * `mov r0` wants to sit INSIDE the constant's two-instruction build.  The lever
 * for that shape is naming the constant in a block that dominates the call so
 * gcc rematerialises it there -- and this function's prologue is `push {lr}`
 * ALONE.  It holds nothing in a callee-saved register, so any named local buys
 * a push/pop pair the ROM does not have: naming `0xe0 << 8` before
 * __CutsceneStart costs two lines and 34 differing, and naming it one call
 * earlier costs exactly the same.
 *
 * This is the same wall as src/non_matching/ovl_7b2078/2008658.c, and for the
 * same stated reason: the interleave is not rematerialisation of a spilled
 * pseudo here, gcc emits it unprompted in the ROM, and on a function with no
 * register pressure there is no handle on it.  The prototype lever is the wrong
 * direction -- it moves `mov r0` to the END, and this one needs it EARLIER.
 */
