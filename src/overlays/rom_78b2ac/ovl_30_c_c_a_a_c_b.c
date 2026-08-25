/* Cluster OvlFunc_890_20081ec..OvlFunc_890_20081ec extracted from goldensun/asm/overlays/rom_78b2ac/ovl_30_c_c_a_a_c.s.
 *
 * Slotted between ovl_30_c_c_a_a_c_a.o and the rest of the overlay.
 *
 * BUILT WITH -fno-rerun-cse-after-loop; see CSE_CFLAGS in the Makefile. A flag
 * id appears on BOTH SIDES OF A CALL -- read in the guard, written in the body
 * -- and at plain -O2 gcc`s second CSE pass hoists it into a callee-saved
 * register, spending a push and a pop to save one pool load. The ROM loads it
 * twice.
 *
 * Here the id is 0x202: `__GetFlag(0x202)` guards the body and `__SetFlag(0x202)`
 * is inside it. 22 instructions against 20 without the flag, exact with it.
 */
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);

void OvlFunc_890_20081ec(void)
{
    if (!__GetFlag(0x202)) {
        __Func_8091200(0x202db1, 1);
        __Func_8091254(0x14);
        __SetFlag(0x202);
        __ClearFlag(0x80 << 2);
        __ClearFlag(0x201);
    }
}
