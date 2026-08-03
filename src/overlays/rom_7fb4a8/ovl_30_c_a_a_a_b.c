/* Cluster OvlFunc_971_2009050..OvlFunc_971_2009050 extracted from goldensun/asm/overlays/rom_7fb4a8/ovl_30_c_a_a_a.s.
 *
 * Split out of that .s; the _a and _c parts stay as assembly and keep their
 * slots in goldensun/overlays/rom_7fb4a8/overlay.ld.
 *
 * Stops the current scene, switches the sound mode, and queues a map change.
 *
 * TWO DETAILS, both previously recorded and both needed here:
 *
 * 1. The RETURN TYPE IS NOT void even though nothing is returned. gcc-2.96
 *    pops the return address into r1 rather than r0 for a non-void function,
 *    and the ROM's epilogue is `pop {r1} / bx r1`. Declared void it is
 *    `pop {r0} / bx r0` and the last two instructions differ.
 *
 * 2. The map id is a SYMBOL. `ldr r0, =1` where `mov r0, #1` would do is the
 *    pool tell -- gcc never pools what it can mov. _ID_1 in unknown_id.sym.
 *    This function was named in docs/elevation.md as an example of that tell
 *    long before the tell was actionable.
 */
extern void __Func_8006358(void);
extern void __SetSoundFXMode(int mode);
extern void __SetDestMap(int map, int entrance);
extern int _ID_1;

int OvlFunc_971_2009050(void)
{
    __Func_8006358();
    __SetSoundFXMode(2);
    __SetDestMap((int)(&_ID_1), 1);
}
