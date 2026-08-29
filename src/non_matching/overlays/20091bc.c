/* OvlFunc_971_20091bc -- 0x020091bc, asm/overlays/rom_7fb4a8/ovl_30_c_c.s
 * and its twin OvlFunc_971_2009228 -- 0x02009228, same .s
 *
 * The two differ in exactly two constants (message ids 0x292a/0x292b against
 * 0x292c/0x292d), so one solution elevates both.
 *
 * 43 of 43 lines, TWO differing.  Candidate at scratch/L91bc.c.
 *
 * SOLVED, and the first half generalises:
 *
 *   THE EPILOGUE REGISTER TELLS YOU THE RETURN TYPE.  The ROM ends
 *   `pop {r5} / pop {r1} / bx r1`, and gcc gave `pop {r0} / bx r0`.  gcc pops
 *   the return address into r0 when r0 is dead -- i.e. when the function
 *   returns void -- and into another register when r0 carries a return value.
 *   Declaring the function `int` and writing `return __CloseUIBox(h, 1);`
 *   moved the difference from the epilogue to one argument pair.
 *
 *   Both `while (__Func_8017364() == 0) __WaitFrames(1);` loops reproduce from
 *   the plain while form -- the ROM's `b test / body / test: / beq body` is
 *   gcc's rotation, not a goto loop.
 *
 * BLOCKER: argument order at the FIRST of the two __CloseUIBox calls.
 *      rom   mov r0, r5 / mov r1, #0x1
 *      ours  mov r1, #0x1 / mov r0, r5
 * The second call, which is the returned one, matches.  Neither argument is a
 * split build, so the interleave lever has nothing to work with.
 *
 * TRIED: the no-prototype lever (no change -- the callee appears twice and
 * gcc's unprototyped order matches the second site, not the first); naming the
 * `1` as a local in the dominating block; forcing a copy of `h` before the
 * call.  All 2.
 */
