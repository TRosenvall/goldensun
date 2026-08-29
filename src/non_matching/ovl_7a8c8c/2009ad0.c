/* OvlFunc_922_2009ad0  --  0x02009ad0, asm/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_c_c_c_a.s
 * (a second byte-identical copy exists; solving one solves both)
 *
 * BLOCKER CLASS: argument precompute (calls.c:805).
 * Status: 30 lines against the ROM's 30, THREE transposed, everything else
 * exact -- including the seven other calls in the sequence.
 *
 * WHAT IT DOES
 * A scripted hop: start the cutscene, set the party leader's speed, walk it to
 * the caller's coordinates, jump, play two animations around a movement wait,
 * end the cutscene.
 *
 * THE WHOLE DIFFERENCE
 *      rom   mov r1,#0xa0 / mov r2,#0xa0 / mov r0,#0x0 / lsl r1,#0xa / lsl r2,#0x9
 *      ours  mov r1,#0xa0 / mov r2,#0xa0 / lsl r1,#0xa / lsl r2,#0x9 / mov r0,#0x0
 *
 * __MapActor_SetSpeed(0, 0xa0 << 10, 0xa0 << 9) has TWO expensive arguments and
 * a cheap one that is not last. precompute_register_parameters copies both
 * shifted values into pseudos before any hard register is loaded, and
 * load_register_parameters then fills r0 forward -- so the cheap `mov r0, #0`
 * lands after both shifts. The ROM interleaves it between them.
 *
 * This is the documented argument-precompute class and it is the ONLY thing
 * wrong here; the six calls with cheap-only arguments all match exactly, which
 * is itself confirmation of the rule's scope.
 *
 * The identical call in OvlFunc_911_200a608 fails the same way, and its park
 * records the flag probes (`-fno-gcse`, literals instead of shifts) that came
 * back negative for both.
 */

extern void __CutsceneStart(void);
extern void __MapActor_SetSpeed(int a, int b, int c);
extern void __Func_809228c(int a, int b, int c);
extern void __MapActor_Jump(int a, int b, int c);
extern void __MapActor_SetAnim(int a, int b);
extern void __MapActor_WaitMovement(int a);
extern void __CutsceneEnd(void);

void OvlFunc_922_2009ad0(int a, int b)
{
    __CutsceneStart();
    __MapActor_SetSpeed(0, 0xa0 << 10, 0xa0 << 9);
    __Func_809228c(0, a, b);
    __MapActor_Jump(0, 4, 0);
    __MapActor_SetAnim(0, 7);
    __MapActor_WaitMovement(0);
    __MapActor_SetAnim(0, 6);
    __CutsceneEnd();
}
