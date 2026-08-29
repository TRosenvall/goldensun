/* Cluster Func_808e118..Func_808e118 extracted from goldensun/asm/rom_8a000/rom_8d9a4_a_c_a_a_a.s.
 *
 * Slotted between rom_8d9a4_a_c_a_a_a_a.o and the rest of stage1.ld.
 *
 * Clears a halfword and, if a second halfword is non-zero, hands off.
 *
 * TWO THINGS HAD TO BE NAMED, AND NEITHER IS OBVIOUS FROM THE ROM.
 *
 * The stored zero is a NAMED int. Written as the literal `0`, gcc-2.96 puts
 * the constant in a LITERAL POOL, loads it with `ldrh`, and has to plant the
 * pool mid-function with a `b` jumping over it -- 20 instructions against 18,
 * with the ROM's plain `mov r2, #0` nowhere in sight. This is the inverted
 * narrow_constant tell: where gcc pools what the ROM builds with a `mov`, the
 * source had a variable.
 *
 * The destination is a NAMED pointer. With the address left inside the store
 * expression, gcc materialises the zero BEFORE computing the address; the ROM
 * does `add r1, r3, r2` and only then `mov r2, #0`. Naming `q` fixes the order.
 *
 * Each fix alone leaves the other defect: named zero only is 6 of 18, literal
 * only is 7 of 18 at the wrong length.
 */
extern unsigned char *iwram_3001ebc;
extern void Func_808e5d8(int a);

void Func_808e118(void)
{
    unsigned char *p;
    short *q;
    int z;

    p = iwram_3001ebc;
    q = (short *)(p + 0xcb6);
    z = 0;
    *q = z;
    if (*(short *)(p + 0xcb8) != 0)
        Func_808e5d8(0x2090);
}
