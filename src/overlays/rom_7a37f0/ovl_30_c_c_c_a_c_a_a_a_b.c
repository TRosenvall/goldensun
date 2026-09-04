// fakematch
/* OvlFunc_916_20088b0  --  0x020088b0
 *
 * Cut out of goldensun/asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a_a_a.s.
 *
 * A short cutscene: two actors walk toward each other, a chime plays, two
 * dialogue lines are queued, and a slot is cleared on the way out.
 *
 * ONE LEVER, and it is the one that keeps recurring: A POOL CONSTANT USED
 * TWICE IS REMATERIALISED. `0x3333` is the third argument of both
 * __MapActor_SetSpeed calls. gcc loads it once into r5 and feeds both from
 * there --
 *
 *     ldr r5, =0x3333 ... mov r2, r5 ... mov r2, r5
 *
 * -- which also costs an instruction the ROM does not spend, so the function
 * came out 76 lines against 75. The ROM issues `ldr r2, =0x3333` at each call.
 * Pinning r2 at both sites forces the reload, because r2 is call-clobbered and
 * the value cannot survive the `bl`.
 *
 * This is a POOL LOAD rather than a `mov`, which is worth noting: the
 * rematerialisation lever is not specific to constants small enough to
 * materialise inline. The mechanism is the register class, not the width of
 * the value.
 *
 * TORN DOWN. Removing the first pin block gives 67 differing and the function
 * one instruction long again; removing the second gives 3. Both earn their
 * place, so both stay.
 *
 * THE `.L12c4` SLOT is a `.lcomm` four-byte cell that is already `.global` in
 * asm/overlays/rom_7a37f0/ovl_30_c_c_c_c_c_c_c.s, so no export step was
 * needed. C cannot spell a name beginning with a dot, and the tree's idiom for
 * that -- used by src/overlays/rom_7a37f0/ovl_30_c_c_c_a_a_a_b.c, which reads
 * the SAME slot -- is an asm-renamed extern:
 *
 *     extern int L12c4 __asm__(".L12c4");
 *     p = *(short **)&L12c4;
 *
 * The halfword store of zero through that pointer takes its zero from the pool
 * (`ldr r3, .L970` against a `.word 0`) with no help; that is blocker 1b
 * behaving as documented, and writing a plain `0` is correct here.
 *
 * CHOSEN WITH tools/crossed.py, which cleared this function and rejected three
 * of the five candidates ranked beside it.
 */

extern int L12c4 __asm__(".L12c4");

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_916_20088b0(void)
{
    short *p;
    int e;

    __CutsceneStart();
    __MapActor_SetAnim(0, 8);
    __CutsceneWait(6);
    __PlaySound(0xef);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x80; q2 = 0x3333; q0 = 8; q1 <<= 8;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    __MapActor_SetAnim(8, 2);
    __MapActor_TravelTo(8, 0x68, 0xb0);
    __CutsceneWait(6);
    __MapActor_SetAnim(0, 2);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q0 = 0; q1 = 0x4ccc; q2 = 0x3333;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    __Func_809228c(0, 8, 0);
    __CutsceneWait(0x18);
    __MapActor_SetAnim(0, 1);
    __MapActor_WaitMovement(8);
    __MapActor_SetAnim(8, 1);
    __PlaySound(0x90 << 1);
    __PlaySound(0xd5);
    e = 9;
    __Func_8010704(5, 9, 1, 4, 4, e);
    __Func_8010704(0, 0, 1, 4, 6, e);
    p = *(short **)&L12c4;
    *p = 0;
    __CutsceneEnd();
}
