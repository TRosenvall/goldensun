/* Cluster Func_80e38b8 extracted from goldensun/asm/rom_c9000/rom_e28f4_c_c_a.s by
 * `tools/split_s.py asm/rom_c9000/rom_e28f4_c_c_a.s Func_80e38b8`.
 *
 * That one split yields three pieces -- _a keeps BaseAnim_RapidSlash and Anim_Gaia, _b
 * is this function, _c is Func_80e3908 -- and BOTH _b and _c land, so one split
 * invocation takes two functions off the park count. Text only, no data, no new export.
 * No pins, no flags.
 *
 * A three-axis integrator: position += velocity on each axis, then each velocity scaled
 * by s/64 (the signed `+0x3f / asr #6` idiom), with the second axis's velocity taking an
 * impulse first and stored twice -- once raw, once scaled. Both stores are here because
 * both are in the ROM.
 *
 * ================ TWO LEVERS, AND THE PARK'S VERDICT WAS WRONG ================
 *
 * The park called the missing `mov r4, r0` "THE DOCUMENTED BOUNDARY, second instance"
 * and filed it as unreachable: a copy of an UNCHANGING value is provably the same rtx,
 * gcc coalesces it, nothing in C asks for it. That reading was argued from Func_80a8b10
 * and it is refuted here. Two source levers produce the copy, and both are required.
 *
 * ONE: NO NAMED LOCALS. Read every field back through the parameter and let cse supply
 * the reuse. The park's named `a`, `b`, `c` are what defeat it -- with the first
 * velocity in a local, the scheduler hoists its multiply up beside its load, which
 * shortens the value's live range, leaves r0 free across the whole body, and lets
 * local-alloc take the parameter's copy-suggested register (`qty_phys_copy_sugg`,
 * local-alloc.c:1684-1691, consumed at :2013) so the prologue copy is deleted. Reading
 * the field back instead keeps the loaded value live to the multiply and r0 occupied,
 * so the pointer is pushed to r4 and the copy survives. The copy is not asked for
 * directly; it falls out of the pressure.
 *
 * TWO: EVERY MULTIPLY OPERAND ORDER WRITTEN AS THE REVERSE OF THE ROM'S. Thumb's
 * two-address `mul Rd, Rm` makes the FIRST operand the destination, and for a
 * commutative MULT gcc puts the DYING operand there -- so the source must name the
 * operands the other way round from the order the assembly shows. The park had the
 * right rule written down and the wrong conclusion drawn from it: it recorded "Thumb's
 * two-operand `mul` puts the result in the FIRST operand's register, and the ROM's
 * `mul r3, r0` / `mul r1, r5` say which", then wrote the orders the ROM shows rather
 * than their reverse, and every one of the three came out swapped.
 *
 * NEITHER HALF IS CLOSE ALONE: no-locals alone is still one instruction short, the
 * operand swap alone leaves most of the stream differing. Together it is exact.
 *
 * THE RULE OF THUMB THE PARK PROPOSED SHOULD BE RETIRED, not refined. It read: "name a
 * pointer only when the ROM shows the two copies taking DIFFERENT values later; if the
 * copy is only ever the same address, it is an allocator artifact." The copy here is
 * only ever the same address and it is still source-reachable. What a base-pointer copy
 * actually reports is REGISTER PRESSURE ACROSS THE BODY, so the question to ask is which
 * values the ROM keeps live, not whether the pointer diverges.
 */
void Func_80e38b8(int *o, int s, int d)
{
    o[0] = o[0] + o[3];
    o[1] = o[1] + o[4];
    o[2] = o[2] + o[5];
    o[4] = o[4] + d;
    o[3] = (o[3] * s) / 64;
    o[4] = (s * o[4]) / 64;
    o[5] = (o[5] * s) / 64;
}
