// fakematch
/* OvlFunc_936_2008504  --  0x02008504
 *
 * Cut out of goldensun/asm/overlays/rom_7c097c/ovl_30_c_c_c_a_a_c_a_a_c.s.
 *
 * PARKED AT 2 OF 54 under "the `neg` interleave", a blocker the park treated as
 * a FAMILY rather than a single case -- it records this residue as
 * byte-for-byte the same as OvlFunc_939_2008c74 with a different callee.
 *
 *     rom   mov r2, #8 / mov r1, #0 / neg r2, r2 / mov r0, #0
 *     ours  mov r2, #8 / neg r2, r2 / mov r1, #0 / mov r0, #0
 *
 * Pinning r1 and r2 and assigning them in the ROM's order matches:
 *
 *     q2 = 8;  q1 = 0;  q2 = -q2;   __Func_809228c(0, q1, q2);
 *
 * THE DISCRIMINATOR PREDICTED THIS ONE TOO. The interleaved argument here is a
 * ZERO, which is what defeated src/non_matching/ovl_7ebdfc/2008120.c -- but
 * there BOTH interleaved arguments were zeros with nothing to order them
 * against, while here the single interleaved zero sits against a mov/neg pair
 * on a DISTINCT value. That is the same sub-case as
 * src/overlays/rom_7ec968/ovl_30_c_c_a_a_c_b.c and it yields the same way. The
 * rule is not "the value must be non-zero"; it is that the interleaved
 * argument needs an operation nearby whose order the source can set.
 *
 * TORN DOWN: pinning r0 as well matches and is byte-identical, so the r0 pin is
 * not kept -- the literal 0 is passed directly.
 *
 * THE PARK'S CODE WAS NOT IN THE PARK. The file carried its measurements and
 * pointed at `scratch/q8504.c` for the candidate itself, which happened to
 * still exist. A park that keeps its numbers and not its source is a park that
 * cannot be resumed once a scratch directory is cleared; the body is now in
 * this file where it belongs.
 *
 * KEPT FROM THE PARK: the two stack arguments of both __CopyMapTiles calls are
 * the same value here, so ONE shared local is right and the stack-argument-PAIR
 * lever is deliberately not used.
 */

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __PlaySound(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __WaitFrames(int n);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int a);
extern void __Func_809228c(int a, int b, int c);
extern void __CutsceneWait(int n);
extern void __Func_8091e9c(int n);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);

void OvlFunc_936_2008504(void)
{
    int t;

    __CutsceneStart();
    __PlaySound(0xbc);
    t = 2;
    __CopyMapTiles(0x24, 0x17, 0x2b, 0xc, t, t);
    __WaitFrames(5);
    __CopyMapTiles(0x27, 0x17, 0x2b, 0xc, t, t);
    __WaitFrames(5);
    __MapActor_SetSpeed(0, 0x80 << 8, 0x80 << 7);
    __MapActor_GetActor(0)[0x55] = 0;
    __MapActor_SetAnim(0, 2);
    {
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q2 = 8;
        q1 = 0;
        q2 = -q2;
        __Func_809228c(0, q1, q2);
    }
    __CutsceneWait(0xa);
    __Func_8091e9c(2);
    __MapTransitionOut();
    __WaitMapTransition();
    __CutsceneEnd();
}
