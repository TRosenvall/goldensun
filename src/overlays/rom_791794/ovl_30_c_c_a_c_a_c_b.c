/* OvlFunc_897_200a8dc  --  0x0200a8dc
 *
 * The second function of goldensun/asm/overlays/rom_791794/ovl_30_c_c_a_c_a_c.s;
 * OvlFunc_897_200a84c stays as assembly in ovl_30_c_c_a_c_a_c_a.s.
 *
 * Repaints a doorway one of two ways depending on the flag it is handed, then
 * refreshes the map. The two arms copy the same two rectangle shapes from
 * different source tiles.
 *
 * THE SHARED 2 IS ASSIGNED INSIDE EACH ARM. The ROM has `mov r5, #2` twice,
 * once at the head of each branch, and both arms then use r5 for two stack
 * arguments. Assigning it once before the `if` would hoist it above the test;
 * the ROM's duplication is what a local assigned in both blocks produces.
 *
 * The second call in each arm passes a literal 1 in the fifth slot and the
 * local in the sixth, matching `mov r3, #1 / str r3, [sp] / ... /
 * str r5, [sp, #4]`. One named, one not, exactly as the assembly has it.
 *
 * Matched on the first screen.
 */
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);

void OvlFunc_897_200a8dc(int which)
{
    int t;

    if (which) {
        t = 2;
        __CopyMapTiles(9, 0x2d, 0x41, 5, t, t);
        __CopyMapTiles(0xb, 0x2e, 0x43, 6, 1, t);
    } else {
        t = 2;
        __CopyMapTiles(0x59, 2, 0x41, 5, t, t);
        __CopyMapTiles(0x66, 0x20, 0x43, 6, 1, t);
    }
    __Func_800fe9c();
}
