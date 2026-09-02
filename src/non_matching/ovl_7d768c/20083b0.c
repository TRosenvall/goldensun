/* OvlFunc_952_20083b0 (0x020083b0) -- NON-MATCHING.
 * Blocker class: SCHEDULING, and the tie-break is now characterised exactly.
 *
 * TWO instructions of 88, one adjacent swap, and the literal pool is exact.
 *
 *     rom    mov r1,#0xe0 / mov r2,#0 / mov r0,r5   / lsl r1,#8 / bl F
 *     ours   mov r1,#0xe0 / mov r2,#0 / lsl r1,#8   / mov r0,r5 / bl F
 *
 * THE SAME SOURCE IDIOM TWENTY INSTRUCTIONS EARLIER COMES OUT RIGHT. An
 * identically-shaped three-argument call with a mov+lsl constant emits the
 * ROM's `r1 / r2 / r0 / lsl`. So the constant's spelling is not the variable --
 * the CONTEXT is, and the difference is measurable.
 *
 * THE MECHANISM, from sched2's ready lists rather than inference. The
 * pre-scheduling order at both sites is `mov r1,#C / lsl r1 / mov r0,r5 /
 * mov r2,#K`: the big constant is force_reg'd in
 * precompute_register_parameters because Thumb cannot load 0xe000 in one
 * instruction, so the `lsl` ALWAYS has a lower LUID than `mov r0,r5`. That
 * rules out every reorder-the-source lever before it is tried.
 *
 * The ready list then breaks ties on INSN_PRIORITY, then on number of
 * dependents, then on lower LUID:
 *
 *     the earlier site   lsl has 3 dependents, `mov r0,r5` has 4  -> r0 wins
 *     this site          lsl has 4, `mov r0,r5` has 4  -> tie -> LUID -> lsl
 *
 * The `lsl` picks up its fourth dependent solely because an INTERVENING CALL
 * sits between this `bl` and the next explicit write of r1. At the earlier site
 * the next r1 write arrives before any call, so the count is three and r0 wins.
 *
 * Confirmed by construction, not just by reading: replacing the intervening
 * one-argument call with any TWO-register-argument call reproduces the ROM's
 * order exactly; a one-argument call gives ours; a zero-argument call gives a
 * third order. Moving the later statement earlier does not help -- it is the
 * intervening CALL, not statement distance.
 *
 * > A mov+lsl argument constant sinks to the last slot before the `bl` only
 * > when the next call's own register-argument fill begins before any
 * > intervening call. This is also the shape a source permuter cannot reach,
 * > because no permutation of the source changes the RTL emission order here.
 *
 * MEASURED, all at rom 88 / ours 88 (aligned):
 *   the spelling below                                   2
 *   the callee's prototype dropped, or declared `int`    2 each
 *   `0x70 << 9` or `0xe0 * 0x100` for the constant       2 each
 *   the argument through a local at the call site        2
 *   that local hoisted to the top of the body            11
 *   both three-argument constants hoisted                19
 *   __ActorMessage's prototype dropped or `int`          4 each
 *   __MapActor_SetBehavior declared `int`                6
 * NINETEEN FLAGS probed, every one leaving the same four-instruction window:
 *   -fno-rerun-cse-after-loop, -fno-gcse, -fno-strict-aliasing, -ffixed-r7,
 *   -fno-strength-reduce, -fno-schedule-insns, -fno-cse-follow-jumps,
 *   -fno-expensive-optimizations, -fno-caller-saves, -fno-thread-jumps,
 *   -fno-sched-interblock, -fno-sched-spec, -fno-delayed-branch,
 *   -fno-function-cse, -fno-force-mem, -fno-peephole, -fno-defer-pop,
 *   -fno-cse-skip-blocks, -O1.
 * Plus source probes: literal stores instead of the shared value; a volatile
 * actor pointer; unsigned and struct-typed variants; an extra unused parameter;
 * `register`; K&R and varargs declarations of five different callees;
 * __attribute__((const)) and ((pure)); an enum constant; a pointer-typed third
 * parameter; and the tail statements reordered.
 */
extern unsigned char gScript_952__0200c570[];

extern int *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __MapActor_SetIdle(int slot);
extern void __CutsceneWait(int n);
extern void __Func_80925cc(int slot, int a);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Func_8092adc(int slot, int a, int b);

void OvlFunc_952_20083b0(int slot)
{
	int *e;
	unsigned char *s;
	int v;

	e = __MapActor_GetActor(slot);
	__CutsceneStart();
	s = gScript_952__0200c570;
	__MapActor_SetBehavior(slot, s);
	__MessageID(0x2009);
	__ActorMessage(slot, 0);
	__MapActor_SetIdle(slot);
	v = 0x80 << 9;
	e[7] = v;
	e[6] = v;
	__CutsceneWait(0x1e);
	__Func_80925cc(slot, 2);
	__CutsceneWait(0x1e);
	__Func_80925cc(slot, 2);
	__CutsceneWait(0x3c);
	__ActorMessage(slot, 0);
	__CutsceneWait(0x14);
	__MapActor_Emote(slot, 0x81 << 1, 0x3c);
	__Func_80925cc(slot, 2);
	__CutsceneWait(0x1e);
	__Func_80925cc(slot, 2);
	__CutsceneWait(0x1e);
	__Func_80925cc(slot, 2);
	__CutsceneWait(0x1e);
	__MapActor_SetBehavior(slot, s);
	__ActorMessage(slot, 0);
	__Func_8092adc(slot, 0xe0 << 8, 0);
	__CutsceneWait(0xa);
	e[7] = v;
	e[6] = v;
	__MapActor_SetBehavior(slot, s);
	__CutsceneEnd();
}
