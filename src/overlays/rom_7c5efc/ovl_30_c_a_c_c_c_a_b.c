/* Cluster OvlFunc_941_200833c..OvlFunc_941_200833c extracted from goldensun/asm/overlays/rom_7c5efc/ovl_30_c_a_c_c_c_a.s.
 *
 * Slotted between ovl_30_c_a_c_c_c_a_a.o and the rest of the overlay.
 *
 * BUILT WITH -fno-rerun-cse-after-loop; see CSE_CFLAGS in the Makefile. A flag
 * id appears on BOTH SIDES OF A CALL -- read in the guard, written in the body
 * -- and at plain -O2 gcc`s second CSE pass hoists it into a callee-saved
 * register, spending a push and a pop to save one pool load. The ROM loads it
 * twice.
 *
 * Here the id is 0x201, guarding the body and set at the end of it. 25
 * instructions against 23 without the flag, exact with it.
 *
 * TWO GUARDS, each an early return: the ROM has two separate `cmp`/`bne` to the
 * same label, which is what a pair of sequential ifs produces.
 */
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_801776c(int a, int b);
extern void __PlaySound(int id);
extern void OvlFunc_941_2008210(void);

void OvlFunc_941_200833c(void)
{
    if (__GetFlag(0x201))
        return;
    if (__GetFlag(0x80 << 2))
        return;
    __Func_801776c(0x1528, 1);
    __PlaySound(0x9d);
    OvlFunc_941_2008210();
    __SetFlag(0x201);
    __ClearFlag(0x202);
}
