/* Cluster Func_80e3908 extracted from goldensun/asm/rom_c9000/rom_e28f4_c_c_a.s by the
 * SAME split that produced the _b piece: `tools/split_s.py` on Func_80e38b8 leaves this
 * function alone in _c. Text only, no data, no new export. No pins, no flags.
 *
 * NOT ONE OF THIS BRIEF'S ASSIGNED TARGETS. It is the two-axis version of the
 * three-axis integrator in the _b piece beside it, it was parked on the identical
 * blocker, and the two levers that land that one land this one unchanged. It is here
 * because it costs nothing extra: same piece, same split, same reading.
 *
 * One integration step: the velocities at +0xc and +0x10 are added into the positions
 * at +0 and +4, the second takes an impulse, and both are then scaled by the argument
 * and divided by 64 with the signed `+0x3f / asr #6` idiom.
 *
 * ================ WHY IT LANDS ================
 *
 * Read the header of the _b piece for the mechanism in full. In short: NO NAMED LOCALS,
 * so the field reads stay live to their multiplies and register pressure pushes the
 * record pointer out of r0 into r4, leaving the prologue copy in place; and EVERY
 * MULTIPLY OPERAND ORDER WRITTEN AS THE REVERSE OF THE ROM'S, because Thumb's
 * two-address `mul Rd, Rm` makes the first operand the destination and gcc puts the
 * dying operand there for a commutative MULT.
 *
 * This park's own notes were a near miss on both counts. It recorded the two-address
 * `mul` rule correctly and then wrote the orders the assembly shows instead of their
 * reverse, and it tried the pointer copy as a NAMED LOCAL (`p = q;`) -- the one form
 * that is genuinely unreachable, because a local initialised from the parameter is the
 * same rtx -- rather than removing the other locals, which is what frees the register.
 * Its "nothing in C asks for a copy whose only purpose is to free a register" is true
 * and beside the point: nothing asks for the copy, the pressure produces it.
 */
void Func_80e3908(int *p, int a, int b)
{
    p[0] = p[0] + p[3];
    p[1] = p[1] + p[4];
    p[4] = p[4] + b;
    p[3] = (p[3] * a) / 64;
    p[4] = (a * p[4]) / 64;
}
