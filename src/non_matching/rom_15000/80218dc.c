/* Func_80218dc -- 0x080218dc, asm/rom_15000/rom_20198_c_c_c_a_a_c_c_c.s
 * Twin of Func_80a8cc0 -- 0x080a8cc0 (see ../rom_a1000/80a8cc0.c).
 *
 * 50 of 50 lines, FOUR differing.  Candidate: scratch/L218dc.c.
 *
 * DECODED IN FULL -- three calls to the same five-argument callee, sharing a
 * base computed once:
 *
 *     base = 0xf315 + d * 2;
 *     Func_8019000(a, (0x80 << 3) | base, b,     c, 0);
 *     Func_8019000(a, 0xf314 + d * 2,     b + 1, c, 0);
 *     return Func_8019000(a, base, b + 2, c, 0);
 *
 * The twins differ in the two pooled constants (0xf315/0xf314 against
 * 0xf281/0xf280), in the callee's spelling, AND IN RETURN TYPE -- which the
 * epilogue rule reads off directly: this one ends `pop {r1} / bx r1` and so
 * returns the last call's value, while Func_80a8cc0 ends `pop {r0} / bx r0`
 * and is void.  Written that way both land at the same 4 of 50, which is the
 * confirmation that the reading was right.
 *
 * BLOCKER: argument-setup order at the SECOND call only -- a rotation of four
 * independent movs.
 *      rom   mov r0, r10 / mov r1, r5 / mov r3, r9 / add r2, #1
 *      ours  mov r1, r5  / mov r3, r9 / add r2, #1 / mov r0, r10
 * The first and third calls match exactly; only the middle one, whose r1 is
 * computed in place and whose r2 needs an increment, has gcc filling r0 last.
 *
 * TRIED, all 4: the no-prototype lever; CSE, GCSE, STRENGTH.  SCHED2 is not a
 * candidate -- disabling it moves the first difference to line 7 and costs 25
 * lines, which confirms the ROM was built with scheduling ON and that this
 * ordering is the scheduler's own choice.
 */
