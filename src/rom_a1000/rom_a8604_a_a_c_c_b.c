/* Cluster Func_80a8cc0..Func_80a8cc0 extracted from goldensun/asm/rom_a1000/rom_a8604_a_a_c.s.
 *
 * Total .text for this TU = 116 bytes (= 0x74).
 * Preserves the original ROM layout when slotted between
 * asm/rom_a1000/rom_a8604_a_a_c_b.o and asm/rom_a1000/rom_a8604_a_a_c_c_c.o in
 * goldensun/stage1.ld.
 *
 * Was parked at 4 of 50. THE FIX IS ONE WORD: the callee is declared `void`, not
 * `int`. No pins, no flags.
 *
 * The park's four differing instructions were an r0 rotation at the second call,
 * and docs/elevation.md's table of argument orders gcc-2.96 can emit names it
 * directly: for an argument needing a precompute, a VOID callee emits `r2, r0, r1`
 * and an INT callee emits `r2, r1, r0`. The ROM has
 * `mov r2, r8 / add r5, r3 / mov r0, r10 / mov r1, r5` -- the void row.
 *
 * The park tried "the no-prototype lever", which is a DIFFERENT change: removing a
 * declaration gives the callee an implicit `int`, which is the wrong row. The
 * callee's return type was never varied. Third function this batch to turn on a
 * callee return type, after OvlFunc_898_2008e0c and Func_80218dc.
 *
 * The park also recorded "SCHED2 is not a candidate ... this ordering is the
 * scheduler's own choice". It is not the scheduler's: it is
 * precompute_register_parameters reacting to the return type.
 */
extern void _Func_8019000(int a, int b, int c, int d, int e);

void Func_80a8cc0(int a, int b, int c, int d)
{
    int base;

    base = 0xf281 + d * 2;
    _Func_8019000(a, (0x80 << 3) | base, b, c, 0);
    _Func_8019000(a, 0xf280 + d * 2, b + 1, c, 0);
    _Func_8019000(a, base, b + 2, c, 0);
}
