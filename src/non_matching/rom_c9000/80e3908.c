/* Func_80e3908  --  0x080e3908, asm/rom_c9000/rom_e28f4_c_c_a.s
 *
 * BLOCKER CLASS: an elided register copy -- gcc is one instruction AHEAD.
 * Status: 30 lines against the ROM's 31, every instruction otherwise in order.
 *
 * WHAT IT DOES
 * One integration step: the two velocity words at +0xc and +0x10 are added into
 * the positions at +0 and +4, the vertical velocity takes an impulse, and both
 * velocities are then scaled by the argument and divided by 64.
 *
 * THE DIFFERENCE
 *      rom    mov r4, r0 / ldr r3, [r4] / ldr r0, [r4, #0xc] ...
 *      ours              / ldr r3, [r0] / ldr r1, [r0, #0xc] ...
 *
 * The ROM copies the record pointer out of r0 into r4 and works from r4,
 * because it wants r0 for the velocity. gcc keeps the pointer in r0 and puts
 * the velocity in r1, which needs no copy. Every subsequent register is renamed
 * as a consequence, but there is one fewer instruction.
 *
 * WHAT WAS TRIED
 *   - the pointer copied into a named local (`p = q;`) -- gcc coalesces it away
 *   - the first velocity read hoisted above the position update, to make gcc
 *     want r0 for it -- byte-identical
 * Both are the same idea and neither reaches it: nothing in C asks for a copy
 * whose only purpose is to free a register.
 *
 * SETTLED AND WORTH KEEPING: `a * vx` and `vy * a` have the operands that way
 * round because Thumb's two-operand `mul` puts the result in the FIRST operand's
 * register, and the ROM's `mul r3, r0` / `mul r1, r5` say which. And `/ 64` on
 * a signed int is what produces `cmp #0 / bge / add #0x3f / asr #6`; an
 * arithmetic shift would be one instruction and is not what the ROM has.
 */

void Func_80e3908(int *p, int a, int b)
{
    int vx;
    int vy;

    vx = p[3];
    p[0] += vx;
    vy = p[4];
    p[1] += vy;
    vy += b;
    p[4] = vy;
    p[3] = a * vx / 64;
    p[4] = vy * a / 64;
}
