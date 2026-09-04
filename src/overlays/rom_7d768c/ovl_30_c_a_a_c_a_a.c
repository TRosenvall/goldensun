// fakematch
/* OvlFunc_952_20083b0  --  0x020083b0
 *
 * From goldensun/asm/overlays/rom_7d768c/ovl_30_c_a_a_c_a_a.s, which held this
 * function alone, so no split was needed.
 *
 * PARKED AT 2 OF 88 with the most carefully argued diagnosis in the park
 * directory, and the argument is correct in every particular:
 *
 *     rom    mov r1,#0xe0 / mov r2,#0 / mov r0,r5 / lsl r1,#8 / bl F
 *     ours   mov r1,#0xe0 / mov r2,#0 / lsl r1,#8 / mov r0,r5 / bl F
 *
 * The park traced it into sched2 rather than inferring it. Thumb cannot load
 * 0xe000 in one instruction, so the constant is force_reg'd in
 * precompute_register_parameters, which means the `lsl` ALWAYS has a lower LUID
 * than `mov r0, r5`. The ready list breaks ties on INSN_PRIORITY, then on
 * number of dependents, then on lower LUID -- and it showed that an
 * identically-shaped call twenty instructions earlier comes out RIGHT because
 * there the counts are 3 against 4, while here they tie at 4 and the LUID
 * decides. It even identified why the `lsl` picks up its fourth dependent: an
 * intervening call sits between this `bl` and the next explicit write of r1.
 *
 * From that it concluded the shape "rules out every reorder-the-source lever
 * before it is tried". That conclusion is true and it is not the whole story.
 * Every lever it had in view reorders the SOURCE, and the LUID order is fixed
 * before source order can matter. A pin does not reorder the source: it names
 * the hard register, so the argument is never force_reg'd into a pseudo and the
 * precompute path that creates the LUID ordering is not entered at all.
 *
 *     q1 = 0xe0;  q2 = 0;  q0 = slot;  q1 <<= 8;
 *     __Func_8092adc(q0, q1, q2);
 *
 * That matches. THE ANALYSIS WAS RIGHT ABOUT THE MECHANISM AND THE MECHANISM
 * WAS NOT THE ONLY ROUTE -- the seventh park in five batches to be exactly
 * correct about the pass it names and wrong that the site is unreachable. The
 * pattern is now specific enough to state as a check: when a park's argument
 * ends in "no ORDERING of the source reaches this", ask whether a pin avoids
 * the pass rather than arguing with it.
 *
 * The park's own body was kept and screens at exactly the 2 of 88 recorded, so
 * only the one call site below is changed from what it had.
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
	{
		register int q0 __asm__("r0");
		register int q1 __asm__("r1");
		register int q2 __asm__("r2");
		q1 = 0xe0;
		q2 = 0;
		q0 = slot;
		q1 <<= 8;
		__Func_8092adc(q0, q1, q2);
	}
	__CutsceneWait(0xa);
	e[7] = v;
	e[6] = v;
	__MapActor_SetBehavior(slot, s);
	__CutsceneEnd();
}
