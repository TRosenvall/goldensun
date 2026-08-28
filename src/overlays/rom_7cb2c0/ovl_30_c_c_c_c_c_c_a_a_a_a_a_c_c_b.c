/* Cluster OvlFunc_945_200b66c..OvlFunc_945_200b66c extracted from goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c.s.
 *
 * Total .text for this TU = 328 bytes (= 0x0148).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_a.o and
 * asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_c.o in
 * goldensun/overlays/rom_7cb2c0/overlay.ld.  It was the 7th of FIFTEEN
 * functions in that .s, which is the largest cluster split so far.
 *
 * Area-entry dispatch: a chain of flag tests, each arm placing actors and
 * returning, with a two-flag tail condition.
 *
 * x AND y ARE NAMED BEFORE THE `if`, not inside it -- do not fold them back.
 * The single guarded __MapActor_SetPos is the whole difference otherwise: the
 * ROM slots `mov r0, #8` between the second shift and the first, and gcc emits
 * it after both.  Naming the two split builds in the dominating block places it.
 * Inline, the function is 2 of 127 lines; named, exact.
 *
 * Everything else -- seven early-return arms, the 0x8a << 4 flag id, and the
 * four-argument OvlFunc_945_200c890 calls whose r1 and r3 are both split builds
 * -- reproduced on the first screen with no help.
 */
extern int __GetFlag(int id);
extern void __WaitFrames(int n);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void OvlFunc_945_200b7b4(void);
extern void OvlFunc_945_200b8ac(void);
extern void OvlFunc_945_200c254(int n);
extern void OvlFunc_945_200c890(int a, int b, int c, int d);
extern void OvlFunc_945_200c8e8(int a, int b, int c);

void OvlFunc_945_200b66c(void)
{
    int x, y;

    __WaitFrames(1);
    OvlFunc_945_200b7b4();
    if (__GetFlag(0x93e)) {
        OvlFunc_945_200c8e8(4, 4, 0);
        OvlFunc_945_200c890(8, 0xce << 1, 0xde, 0xc0 << 6);
        OvlFunc_945_200c890(9, 0xe5 << 1, 0xa1, 0x80 << 8);
        return;
    }
    x = 0xec << 17;
    y = 0x98 << 16;
    if (__GetFlag(0x8a << 4)) {
        __MapActor_SetPos(8, x, y);
        __MapActor_SetAnim(9, 5);
        OvlFunc_945_200c8e8(4, 4, 0);
        return;
    }
    if (__GetFlag(0x92b)) {
        OvlFunc_945_200c8e8(0x10, 0, 0);
        OvlFunc_945_200c8e8(4, 4, 0);
        OvlFunc_945_200c254(3);
        return;
    }
    if (__GetFlag(0x92a)) {
        OvlFunc_945_200c8e8(0x10, 0, 0);
        OvlFunc_945_200c8e8(4, 3, 0);
        OvlFunc_945_200c254(2);
        return;
    }
    if (__GetFlag(0x929)) {
        OvlFunc_945_200c8e8(0x10, 0, 0);
        OvlFunc_945_200c8e8(4, 2, 0);
        OvlFunc_945_200c254(1);
        return;
    }
    if (__GetFlag(0x928)) {
        OvlFunc_945_200c8e8(0x10, 0, 0);
        __MapActor_SetPos(0xa, 0, 0);
        OvlFunc_945_200c254(0);
        return;
    }
    __MapActor_SetAnim(9, 5);
    if (__GetFlag(0x925)) {
        if (__GetFlag(0x926) == 0)
            OvlFunc_945_200b8ac();
    }
}
