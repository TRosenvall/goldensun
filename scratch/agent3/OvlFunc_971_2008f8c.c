/* OvlFunc_971_2008f8c -- PARKED at 2 differing of 75.  Everything else is exact.
 * ref: asm/overlays/rom_7fb4a8/ovl_30_c_a_a_a_a.s   first diff at position 5.
 *
 * The ONLY residue is where gcc puts one pool load:
 *     rom   mov r6,r0 / mov r0,#0 / ldr r5,=0x294e / bl OvlFunc_971_2008f30
 *     ours  mov r6,r0 / mov r0,#0 / bl ...  ... ldr r5,=0x294e (16 slots later,
 *                                            in the __GetFlag argument block)
 * The whole prologue-to-first-`cmp` run is ONE basic block, so this is
 * post-reload scheduling (-fschedule-insns2) sinking the load to its use.
 * Measured: -fno-schedule-insns2 moves it to position 4 -- one slot too early,
 * BEFORE `mov r0,#0` -- and adds a second sunk load, 4 differing.  Four source
 * positions for `msg = 0x294e;` (before the first call, between the two calls,
 * before __CutsceneStart, immediately before the `if`) are all identical: the
 * scheduler owns the placement, not the statement order.
 * Also measured, all unchanged at 2: -fno-schedule-insns,
 * -fno-rerun-cse-after-loop, -fno-strict-aliasing, `register int msg`.
 * -fno-gcse is much worse (78 lines).
 *
 * THREE THINGS THAT WERE LOAD-BEARING and took it from 63 of 75 to 2:
 *  1. `msg` is REUSED as the __GetFlag result inside the 0x304 arm.  With a
 *     separate `v`, gcc leaves 0x294e live in r5 through the four sub-arms and
 *     DERIVES one of them from it (`add r5,#25` for 0x2967 / `add r5,#0x1e`
 *     for 0x296c) instead of the ROM's fresh pool load, and cross-jumps the
 *     arms: 71 lines against 75.  The ROM's `mov r5, r0` proves the reuse.
 *  2. `f1 = 0xbc << 2;` named in the ENTRY block with both uses inside the
 *     conditional -- the basic-block lever.  With the literal at both sites gcc
 *     CSEs the 0x2f0 into one register (`mov r5,#0xbc / lsl r5,#2 / mov r0,r5 /
 *     add r5,r7,r5`) where the ROM builds it twice.  ONE local is enough; a
 *     second local for the second site measures the same.
 *  3. `int` return with `return __CutsceneEnd();` -- the ROM's `pop {r1} / bx r1`.
 * A goto-per-arm spelling is NOT needed; plain if/else gives the ROM's blocks.
 */
extern unsigned char gState[];
extern int  OvlFunc_971_2008f30(int a);
extern void __CutsceneStart(void);
extern int  __CutsceneEnd(void);
extern void __Func_809280c(int a, int b, int c);
extern int  __GetFlag(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);

int OvlFunc_971_2008f8c(int a)
{
    unsigned char *gs;
    int msg;
    int x;
    int y;
    int f1;

    f1 = 0xbc << 2;
    msg = 0x294e;
    x = OvlFunc_971_2008f30(0);
    y = OvlFunc_971_2008f30(a);
    __CutsceneStart();
    gs = gState;
    __Func_809280c(a, *(int *)(gs + (0xfa << 1)), 0);
    if (__GetFlag(0xc1 << 2) != 0) {
        __GetFlag(f1);
        msg = __GetFlag(a + f1);
        if (__GetFlag(0x305) != 0) {
            if (msg != 0)
                msg = 0x2967;
            else
                msg = 0x296c;
            goto done;
        }
        if (msg != 0)
            msg = 0x2971;
        else
            msg = 0x2976;
        goto done;
    }
    if (x == 0) goto arm6;
    if (y != 0) goto done;
    msg = 0x2953;
    goto done;
arm6:
    msg = 0x2958;
done:
    __MessageID(msg + a - 1);
    __ActorMessage(a, 0);
    return __CutsceneEnd();
}
