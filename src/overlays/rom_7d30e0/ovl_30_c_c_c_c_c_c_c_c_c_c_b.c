/* Cluster OvlFunc_948_200a188..OvlFunc_948_200a188 extracted from goldensun/asm/overlays/rom_7d30e0/ovl_30_c_c_c_c_c_c_c_c_c_c.s.
 *
 * Total .text for this TU = 264 bytes (= 0x0108).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7d30e0/ovl_30_c_c_c_c_c_c_c_c_c_c_a.o and asm/overlays/rom_7d30e0/ovl_30_c_c_c_c_c_c_c_c_c_c_c.o
 * in goldensun/overlays/rom_7d30e0/overlay.ld.  The trailing .data follows the
 * last function and travels with _c.
 *
 * Restores four actor positions and replays five completion flags on area entry.
 *
 * x AND y ARE NAMED IN THE BLOCK DOMINATING EACH GUARDED CALL -- do not fold
 * them back into the argument list.  All four __MapActor_SetPos calls have the
 * same interleave: the ROM slots `mov r0, <slot>` between the two constant movs
 * and their two shifts, and gcc emits it after both shifts instead.  Naming the
 * two SPLIT BUILDS -- and leaving the slot a bare literal -- places it.  Inline
 * literals give 12 differing; naming x and y INSIDE each if-body instead of
 * before it gives the same 12, which is the dominance precondition rather than
 * a coincidence.  Both were screened.
 *
 * Worth noting against docs/elevation.md: the guard here is a CALL
 * (__GetFlag), not a memory load as in OvlFunc_932_200a9dc, so the named values
 * cross a call and could have cost callee-saved registers.  They do not -- gcc
 * rematerialises them in each arm and the function still pushes only lr.
 */
extern int __GetFlag(int id);
extern void __WaitFrames(int n);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void OvlFunc_948_200a0c4(int a, int b);
extern void OvlFunc_948_2008f40(int n);
extern void OvlFunc_948_2008fdc(int n);

void OvlFunc_948_200a188(void)
{
    int x, y;

    __WaitFrames(1);
    OvlFunc_948_200a0c4(0xc, 0xf3);
    OvlFunc_948_200a0c4(0xb, 0xf4);
    OvlFunc_948_200a0c4(0xa, 0xf4);
    OvlFunc_948_200a0c4(9, 0xf4);
    OvlFunc_948_200a0c4(8, 0xf4);
    x = 0xe8 << 16;
    y = 0xda << 18;
    if (__GetFlag(0xee7) == 0)
        __MapActor_SetPos(8, x, y);
    x = 0x94 << 17;
    y = 0xce << 18;
    if (__GetFlag(0xee8) == 0)
        __MapActor_SetPos(9, x, y);
    x = 0xa4 << 17;
    y = 0xbe << 18;
    if (__GetFlag(0xee9) == 0)
        __MapActor_SetPos(0xa, x, y);
    x = 0xb4 << 17;
    y = 0xda << 18;
    if (__GetFlag(0xeea) == 0)
        __MapActor_SetPos(0xb, x, y);
    if (__GetFlag(0x9c << 4))
        OvlFunc_948_2008f40(0);
    if (__GetFlag(0x9c1))
        OvlFunc_948_2008f40(1);
    if (__GetFlag(0x9c2))
        OvlFunc_948_2008f40(2);
    if (__GetFlag(0x9c3))
        OvlFunc_948_2008f40(3);
    if (__GetFlag(0x9c4))
        OvlFunc_948_2008fdc(0);
}
