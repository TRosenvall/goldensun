/* OvlFunc_911_200a910  --  0x0200a910
 *
 * Cut out of goldensun/asm/overlays/rom_79e5c0/ovl_30_c_c.s.
 *
 * Two independent map fixups on arrival, each repainting tiles once its flag is
 * set. Matched on the first screen.
 *
 * FOUR STACK-ARGUMENT SITES, FOUR PAIRS OF LOCALS. Naming both values of a
 * stack-argument pair adjacent to the call is the documented lever; what this
 * function adds -- with OvlFunc_936_20098a4, elevated the same round -- is that
 * each SITE needs its own pair. Reusing one pair across sites puts the values
 * in the wrong registers at every one of them.
 *
 * The pairs are assigned in the order the ROM stores them: `[sp]` first, then
 * `[sp, #4]`, which is the reverse of the argument order and is what the ROM's
 * `mov r3 / mov r2 / str r3, [sp] / str r2, [sp, #4]` says.
 */
extern int __GetFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);
extern void __WaitFrames(int n);

void OvlFunc_911_200a910(void)
{
    int s0, s1, s2, s3, s4, s5;

    if (__GetFlag(0x845) == 0) {
        __MapActor_SetPos(8, 0, 0);
        s0 = 9;
        s1 = 0x12;
        __Func_8010704(9, 0x11, 5, 1, s0, s1);
        __Func_800fe9c();
        __WaitFrames(1);
    } else {
        __MapActor_SetPos(9, 0, 0);
    }
    if (__GetFlag(0x847)) {
        s2 = 5;
        s3 = 7;
        __CopyMapTiles(0x5b, 0x13, 0x48, 9, s2, s3);
        s4 = 8;
        s5 = 0xb;
        __Func_8010704(0x17, 0xb, 5, 7, s4, s5);
        __Func_800fe9c();
        __WaitFrames(1);
    }
}
