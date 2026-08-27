/* OvlFunc_955_200862c -- NOT MATCHING. 8 differing of 49, one cause.
 * ref: asm/overlays/rom_7ddb88/ovl_30_c_c_c_a_c_a.s
 *
 * BLOCKER: the destination register of the shift, at three sites.
 *     rom   ldr r3, [r0, #8] / asr r2, r3, #0x14 / str r2, [sp]
 *     ours  ldr r3, [r0, #8] / asr r3, #0x14     / str r3, [sp]
 * gcc coalesces the shift result with the loaded value because the load is dead
 * immediately after -- the documented limit of the named-intermediate lever
 * ("the lever needs the two values to be simultaneously live, which a shift's
 * input and output are not").  At the last call the same cause shows up as a
 * clean r2/r3 transposition across the two stack stores.
 *
 * Everything else is exact, including the shared 0xb in r5 across three calls
 * and the argument rotation (4,1) in the final call.
 *
 * MEASURED, all 8 of 49: the shift inline in the call argument; a named `int`
 * per site; the actor in a named local; the address in a named `int *`;
 * a subscript `((int *)actor)[2]`; -fno-regmove, -fno-rerun-cse-after-loop,
 * -fno-gcse, -fno-schedule-insns, -fno-expensive-optimizations,
 * -fno-strict-aliasing.  `t = ...; v = t; v >>= 20;` is 9.
 * -fno-schedule-insns2 and -O1 are 26; -fcall-saved-r4 is 14.
 */
extern void __Func_8010704(int, int, int, int, int, int);
extern unsigned char *__MapActor_GetActor(int);

void OvlFunc_955_200862c(void)
{
    int s;
    int v1, v2, v3;

    s = 0xb;
    __Func_8010704(0x64, 0xb, 0xc, 4, 0xe, s);
    v1 = *(int *)(__MapActor_GetActor(0xf) + 8) >> 20;
    __Func_8010704(0xd, 0x1c, 1, 4, v1, s);
    v2 = *(int *)(__MapActor_GetActor(0x10) + 8) >> 20;
    __Func_8010704(0xd, 0x1c, 1, 4, v2, s);
    v3 = *(int *)(__MapActor_GetActor(0x11) + 0x10) >> 20;
    __Func_8010704(0xd, 0x1c, 4, 1, 0x12, v3);
}
