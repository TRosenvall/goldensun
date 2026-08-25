/* Cluster Func_80b2884..Func_80b2884 extracted from goldensun/asm/rom_b0000/rom_b0070_c_c_a_a.s.
 *
 * Total .text for this TU = 52 bytes (= 0x34).
 * Slotted between the _a and _c pieces in goldensun/stage1.ld.
 *
 * Shifts a message id by a per-language offset chosen from a signed byte at
 * iwram_3001f2c+0x3aa.
 *
 * UNPARKED BY IDENTIFYING A NAMESPACE THROUGH THE CALLER. The ROM spends four
 * instructions to add ten --
 *
 *      ldr r3, =0xd2e / ldr r2, =0xd24 / sub r3, r2 / add r0, r3
 *
 * -- and a compiler does not leave `0xd2e - 0xd24` unfolded, so both operands
 * were symbols. Written as literals gcc folds it to `add r0, #0xa` and the
 * function comes out eight instructions short.
 *
 * The namespace came from following the result, not from the values. The
 * already-elevated caller src/rom_b0000/rom_b0070_c_c_a_b.c passes it to
 * _Func_8017658's first argument, and elevated code elsewhere passes
 * `(int)&_MSG_14` in that position; `_MSG_d21` was already defined, one of the
 * same run. Five ids were added to message.sym on that evidence.
 *
 * THE TEMPORARY HAD TO GO. `d = X - Y; base += d;` puts the two pooled loads in
 * the opposite registers from the ROM (12 of 30). Writing `base += X - Y;`
 * directly matches exactly. A named temporary is usually harmless and here it
 * decided the allocation.
 */
extern unsigned char *iwram_3001f2c;
extern int _MSG_d24;
extern int _MSG_d2e;
extern int _MSG_d38;
extern int _MSG_d42;

int Func_80b2884(int base)
{
    unsigned char *p;
    unsigned int o;
    int k;
    int d;

    o = 0x3aa;
    p = iwram_3001f2c + o;
    k = *(signed char *)(p + (unsigned int)0);
    if (k == 1) {
        base += (int)(&_MSG_d2e) - (int)(&_MSG_d24);
    }
    if (k == 2) {
        base += (int)(&_MSG_d38) - (int)(&_MSG_d24);
    }
    if (k == 3) {
        base += (int)(&_MSG_d42) - (int)(&_MSG_d24);
    }
    return base;
}
