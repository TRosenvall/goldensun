/* Cluster OvlFunc_952_200bf84..OvlFunc_952_200bf84 extracted from goldensun/asm/overlays/rom_7d768c/ovl_30_c_a_c_a.s.
 *
 * Slotted between ovl_30_c_a_c_a_a.o and the rest of the overlay.
 *
 * BUILT WITH -fno-rerun-cse-after-loop; see CSE_CFLAGS in the Makefile and the
 * standing item in HANDOFF.md. The flag id is read in a guard and written in
 * the body -- the recognition rule from batch 50 -- and gcc`s second CSE pass
 * hoists it into a callee-saved register across the call, where the ROM simply
 * loads it twice.
 *
 * THE __ActorMessage CALL IS DUPLICATED IN BOTH ARMS rather than joined after
 * the `if`. The ROM emits it twice, once per arm, and writing it once below
 * the branch would be the tidier reading and the wrong one.
 */
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int b);

void OvlFunc_952_200bf84(void)
{
    if (!__GetFlag(0x96d)) {
        __SetFlag(0x96d);
        __MessageID(0x2239);
        __ActorMessage(9, 0);
    } else {
        __MessageID(0x223a);
        __ActorMessage(9, 0);
    }
}
