/* Cluster OvlFunc_901_2008a80..OvlFunc_901_2008a80 extracted from
 * goldensun/asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_c_c_a_c_c.s.
 *
 * Total .text for this TU = 72 bytes (= 0x48). Parked; elevated in batch 278.
 * TWO ARGUMENT PINS -- one fakematch row.
 *
 * IDENTICAL TWIN OF OvlFunc_898_2008ef4 (src/overlays/rom_793768/ovl_314_c_c_c_a_c_a_a_c_a_c.c)
 * -- read that file for the derivation and for why this retires the "argument precompute"
 * blocker class: the residue is a sched2 priority tie broken by INSN_LUID, not a compiler
 * difference, and two argument pins move the r0 fill earlier in the chain. Each source was
 * verified by objcmp against its OWN reference. If you edit one, edit both.
 *
 * Landing this also makes OvlFunc_901_2008a80 a real symbol, which matters for the still-parked
 * OvlFunc_901_2008c1c: that function's twin differs from OvlFunc_898_2009090 in exactly one
 * relocation -- its tail call targets this function rather than OvlFunc_898_2008ef4 -- so when
 * it is solved the pair must land as TWO files, not a shared body.
 */
extern unsigned char iwram_3001ebc[];
extern void __MapActor_SetSpeed(unsigned int, int, int);
extern void __Func_809218c(int, int, int);
extern void __Func_8091e9c(int);

void OvlFunc_901_2008a80(int a, int b, int c)
{
    char *base;

    {
        register int p0 __asm__("r0") = 0;
        register int p1 __asm__("r1") = 0x80 << 8;
        __MapActor_SetSpeed(p0, p1, 0x80 << 7);
    }
    __Func_809218c(0, a, b);
    base = *(char **)iwram_3001ebc;
    *(int *)(base + (0xe4 << 1)) = 0x10;
    __Func_8091e9c(c);
}
