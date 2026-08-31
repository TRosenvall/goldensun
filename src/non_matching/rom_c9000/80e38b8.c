/* Func_80e38b8 -- asm/rom_c9000/rom_e28f4_c_c_a.s
 *
 * BLOCKER: an unreachable register COPY of the base pointer. 42 of 43,
 * one line short -- and the one missing line is `mov r4, r0`.
 *
 * A three-axis integrator: position += velocity on each axis, then each
 * velocity scaled by s/64 (the signed `+0x3f / asr #6` idiom), with the second
 * axis's velocity incremented by an argument first and stored TWICE -- once
 * raw, once scaled. Both stores are in the C below because both are in the ROM.
 *
 * The ROM copies the object pointer out of r0 into r4 immediately
 * (`mov r4, r0`) and addresses all six fields through the copy, which frees r0
 * to hold the first loaded velocity. Ours addresses everything through r0 and
 * never makes the copy, so every subsequent line is offset.
 *
 * MEASURED:
 *   fields addressed through the parameter        42 lines, 42 differ
 *   a named local `p = o` used throughout         42 lines, 42 differ
 *                                                 (BYTE-IDENTICAL)
 *
 * THIS IS THE DOCUMENTED BOUNDARY, second instance. docs/elevation.md records
 * from Func_80a8b10 that a copy of an UNCHANGING value is unreachable: any
 * local initialised from the parameter is provably the same rtx, gcc coalesces
 * it, and no `mov` is emitted. There the copy was a loop limit (`mov r12, r5`);
 * here it is a base pointer. Same mechanism, same result.
 *
 * Worth recording as a second data point because the base-pointer form LOOKS
 * more promising than the limit form -- naming a base pointer is a lever that
 * works elsewhere in this file (it fixed the buffer base on Func_8020b64).
 * The difference is that on Func_8020b64 the ROM kept a base AND a moving
 * cursor, two values that diverge. Here the copy never diverges from the
 * parameter, so there is nothing to name.
 *
 * Rule of thumb this supports: name a pointer only when the ROM shows the two
 * copies taking DIFFERENT values later. If the copy is only ever the same
 * address, it is an allocator artifact.
 */
void Func_80e38b8(int *o, int s, int d)
{
    int a;
    int b;
    int c;

    a = o[3];
    o[0] = o[0] + a;
    b = o[4];
    o[1] = o[1] + b;
    c = o[5];
    o[2] = o[2] + c;
    b = b + d;
    o[4] = b;
    o[3] = (s * a) / 64;
    o[4] = (b * s) / 64;
    o[5] = (s * c) / 64;
}
