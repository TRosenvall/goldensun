/* Cluster OvlFunc_948_2009bc4..OvlFunc_948_2009bc4 extracted from goldensun/asm/overlays/rom_7d30e0/ovl_30_c_c_a_a_c_c_a_c_c.s.
 *
 * Slotted between ovl_30_c_c_a_a_c_c_a_c_c_a.o and the rest of the overlay.
 *
 * Both arms of the `if` pass the SAME stack-arg pair (0x2d, 0x2b) and the ROM
 * builds it FRESH IN EACH ARM, so the two locals are assigned inside each arm
 * rather than hoisted above the branch. That is the batch-52 rule -- count the
 * materialisations in the reference before deciding where the assignment goes.
 */
extern void __StopTask(void *fn);
extern void OvlFunc_948_2009e94(void);
extern void __MapActor_SetPos(int slot, int a, int b);
extern int __GetFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_948_2009ec0(void);
extern void __SetFlag(int id);

void OvlFunc_948_2009bc4(void)
{
    int m;
    int n;

    __StopTask((void *)OvlFunc_948_2009e94);
    __MapActor_SetPos(0xe, 0, 0);
    if (__GetFlag(0x207)) {
        m = 0x2d;
        n = 0x2b;
        __Func_8010704(0x3a, 0x24, 1, 1, m, n);
    } else {
        m = 0x2d;
        n = 0x2b;
        __Func_8010704(0x2e, 0x2b, 1, 1, m, n);
    }
    OvlFunc_948_2009ec0();
    __SetFlag(0x206);
}
