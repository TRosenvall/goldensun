/* Cluster OvlFunc_964_20093e0..OvlFunc_964_20093e0 extracted from goldensun/asm/overlays/rom_7ed0a0/ovl_30_a_c_a_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7ed0a0/ovl_30_a_c_a_c_a.o and the rest of the overlay in
 * goldensun/overlays/rom_7ed0a0/overlay.ld.
 *
 * Two map edits inside a cutscene. The stack-arg-pair lever in its SHARED
 * form: 0x19 is the [sp] value for BOTH calls, so it is named once and used
 * twice and gcc parks it in r5 exactly as the ROM does. The other stack value
 * differs between the calls and stays a literal, because the ROM rebuilds it
 * each time (`mov r3, #0x31` then `mov r3, #0x33`).
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_964_20080c4(void);
extern void OvlFunc_964_200a480(void);

void OvlFunc_964_20093e0(void)
{
    int s = 0x19;

    __CutsceneStart();
    __Func_8010704(0x59, 0x31, 3, 2, s, 0x31);
    __Func_8010704(0x59, 0x33, 8, 5, s, 0x33);
    OvlFunc_964_20080c4();
    OvlFunc_964_200a480();
    __CutsceneEnd();
}
