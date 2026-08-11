/* Cluster OvlFunc_968_2009f28..OvlFunc_968_2009f28 extracted from goldensun/asm/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_c_a.o and asm/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_c_c.o in
 * goldensun/overlays/rom_7f2f14/overlay.ld.
 *
 * THE STACK-ARG-PAIR LEVER. This is the function that produced it, so the
 * account is here and the other members point at it.
 *
 * The blocker: where a call takes two stack arguments, the ROM materialises
 * BOTH into separate registers before storing either, while gcc reuses one
 * register and interleaves each build with its own store:
 *
 *     rom    mov r3, #0x2d / mov r2, #0x2b / str r3, [sp] / str r2, [sp, #4]
 *     ours   mov r3, #0x2d / str r3, [sp]  / mov r3, #0x2b / str r3, [sp, #4]
 *
 * THE FIX IS TO NAME BOTH VALUES AS LOCALS, ASSIGNED IMMEDIATELY BEFORE THE
 * CALL, IN THE ROM'S ORDER. Three steps got here and each one matters:
 *
 *   literals at the call site                        3 positions differ
 *   the SHARED value named (0x2b is also argument 2) 2 positions differ
 *   both values named, first-stored one assigned first   MATCH
 *
 * The middle step is the non-obvious one. 0x2b appears twice in this call --
 * once as the second argument and once as the sixth -- and naming it is what
 * stops gcc rebuilding it, which frees the register that then holds the other
 * stack value.
 *
 * WHY AN EARLIER ATTEMPT AT THIS FAILED. src/non_matching/ovl_7b9cb4/20084cc.c
 * was parked on this class after trying named locals and getting THIRTEEN
 * differing positions -- worse than the literals. The difference is WHERE the
 * assignments go: that attempt put them at the top of the function, before an
 * intervening call, and gcc hoisted both materialisations above it. Making
 * values live EARLIER is not the same as making them live SIMULTANEOUSLY. With
 * the assignments moved next to the call, that function matches too and is now
 * elevated.
 *
 * So the lever is specifically: adjacent to the call, both named, in ROM order.
 */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_968_2009f28(void)
{
    int m;
    int n;

    __CutsceneStart();
    if (OvlFunc_968_2008cc8() == 0) {
        m = 0x2d;
        n = 0x2b;
        __Func_8010704(0x6d, n, 7, 5, m, n);
        OvlFunc_968_2008374();
    }
    __CutsceneEnd();
    OvlFunc_968_2009d48();
}
