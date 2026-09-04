// fakematch
/* OvlFunc_943_2009a98  --  0x02009a98
 *
 * Cut out of goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_a_c.s.
 *
 * Resets a cutscene stage: blank the camera target, park eight actors at the
 * origin, then place two of them at fixed spots with fixed facings and hand off
 * to OvlFunc_943_2009c14.
 *
 * THIS WAS PARKED AT 28 OF 75 AND THE PARK IS NOW WITHDRAWN -- the second one
 * to fall to the interleaved `-1` form, after OvlFunc_881_200b2f0. Three levers
 * took it from 28 to zero, and all three were already in the notebook by the
 * time it was revisited:
 *
 *   1. INTERLEAVE each `-1` assignment with its own negation rather than
 *      grouping the assignments and the negations                 28 -> 25
 *   2. the two halfword stores need `int` locals: 0x3000 pools as
 *      `=0x3000` and 0xb000 as `=0xffffb000`, negative at short
 *      width -- ordinary blocker 1b                               25 -> 2
 *   3. the pinned r2 constant must be ASSIGNED after the shift
 *      rather than initialised at its declaration                  2 -> 0
 *
 * The park had claimed the residue was "a second hoist plus an ordering" and
 * that the second pin might be "actively wrong rather than insufficient". It
 * was neither: the pin was right and incomplete, and the halfword stores were
 * never diagnosed at all because the `-1` triple was masking them. WHEN A PARK
 * IS WRITTEN WITH ONE BLOCKER STILL UNRESOLVED, ITS ACCOUNT OF THE REST IS
 * UNRELIABLE -- the unresolved instruction shifts every later line and the
 * itemised regions describe an alignment, not a diagnosis.
 *
 * The three pin blocks and the interleaved triple are all load-bearing; each
 * was measured on the original park or here.
 */

extern void __CutsceneStart(void);
extern void __WaitFrames(int n);
extern void __MapActor_SetPos(int slot, int x, int z);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_800fe9c(void);
extern void OvlFunc_943_2009c14(int a, int b);

void OvlFunc_943_2009a98(void)
{
    unsigned char *a;
    int v;

    __CutsceneStart();
    {
        register int p0 __asm__("r0");
        register int p1 __asm__("r1");
        register int p2 __asm__("r2");
        register int p3 __asm__("r3");
        p0 = 1;
        p0 = -p0;
        p1 = 1;
        p1 = -p1;
        p2 = 1;
        p2 = -p2;
        p3 = 0;
        __Func_80933f8(p0, p1, p2, p3);
    }
    __WaitFrames(1);
    __MapActor_SetPos(0x14, 0, 0);
    __MapActor_SetPos(0x16, 0, 0);
    __MapActor_SetPos(0x18, 0, 0);
    __MapActor_SetPos(0x19, 0, 0);
    __MapActor_SetPos(0x1a, 0, 0);
    __MapActor_SetPos(0x1b, 0, 0);
    __MapActor_SetPos(0, 0, 0);
    __MapActor_SetPos(0x17, 0, 0);
    a = __MapActor_GetActor(0x17);
    v = 0xc0 << 6;
    *(short *)(a + 6) = v;
    {
        register int q1 __asm__("r1") = 0xe8;
        register int q2 __asm__("r2");
        register int q0 __asm__("r0") = 0x15;
        q1 <<= 16;
        q2 = 0x28a0000;
        __MapActor_SetPos(q0, q1, q2);
    }
    a = __MapActor_GetActor(0x15);
    v = 0xb0 << 8;
    *(short *)(a + 6) = v;
    __Func_80933f8(0xe8 << 16, -1, 0x9f << 18, 0);
    __Func_800fe9c();
    __WaitFrames(1);
    OvlFunc_943_2009c14(0x17, 0x15);
}
