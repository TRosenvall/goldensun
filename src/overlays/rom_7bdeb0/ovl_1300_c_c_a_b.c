/* OvlFunc_934_2009390 -- a scripted door sequence: walk the actor up, swap its
 * animation and priority, redraw four tile rects and set the completion flag.
 *
 * Preserves the ROM layout when slotted before
 * asm/overlays/rom_7bdeb0/ovl_1300_c_c_a_c.o in
 * goldensun/overlays/rom_7bdeb0/overlay.ld.
 *
 * THREE LEVERS, and the third is a refinement worth keeping.
 *
 *   DELETE THE ADDRESS-ONLY LOCAL. Holding the actor pointer in a local forced
 *   `mov r3, r0 / add r3, #0x55`; the ROM's destructive `add r0, #0x55` says
 *   the pointer dies at the store. Inlining the call into the store's lvalue
 *   removed three instructions and fixed a register rotation further down. The
 *   sibling in ovl_1300_c_c_b.c KEEPS its pointer local, so this is the one
 *   place the family idiom has to be broken.
 *
 *   THE CALLEE'S RETURN TYPE, on one callee out of twenty-three. All four calls
 *   to the tile-rect routine had `mov r0, #imm` a slot early; declaring it
 *   `int` makes gcc reserve r0 and emit it last. The other twenty-two calls
 *   were already right as `void`.
 *
 *   A CONSTANT'S LIVE RANGE STARTS WHERE IT IS ASSIGNED, and that can be inside
 *   an expression. `two = 2;` as its own statement put `mov r3,#2 / mov r8,r3`
 *   BEFORE the actor lookup; the ROM has them after. Embedding the assignment
 *   in the store -- `__MapActor_GetActor(s.b)[0x23] = (two = 2);` -- starts the
 *   pseudo after the call. That was the last five instructions.
 *
 * The six-word by-value struct reproduces its `ldmia`/`stmia` pair with no
 * help, and the epilogue's `pop {r0}` confirmed `void` before anything was
 * written.
 *
 * MEASURED (rom 118 lines):
 *   sibling style, pointer local, all callees void      121 lines, 23 aligned
 *   pointer local deleted, the rect routine `int`       118, 5
 *   the same with `two = 2` folded into the store       118, MATCH
 */
struct S { int a, b, c, d, e, f; };

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern int OvlFunc_934_2008758(struct S *s);
extern void OvlFunc_934_20088ec(struct S s);
extern int OvlFunc_934_2008528(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_934_2008cd0(unsigned char *p);

void OvlFunc_934_2009390(void)
{
	struct S s;
	int zero, four, two;

	__CutsceneStart();
	if (OvlFunc_934_2008758(&s)) {
		OvlFunc_934_20088ec(s);
		if ((s.c >> 20) == 0x11) {
			__MapActor_SetAnim(s.b, 3);
			zero = 0;
			__MapActor_GetActor(s.b)[0x55] = zero;
			*(int *)(__MapActor_GetActor(s.b) + 0x44) = zero;
			__Func_809228c(s.b, -0xc, 0);
			__MapActor_WaitMovement(s.b);
			__MapActor_SetAnim(s.b, 3);
			__Func_8092b08(0xa, 3);
			__MapActor_GetActor(s.b)[0x55] = 3;
			__Func_809228c(s.b, -6, 0);
			OvlFunc_934_2008cd0(__MapActor_GetActor(s.b));
			__MapActor_SetAnim(s.b, 8);
			__MapActor_GetActor(s.b)[0x23] = (two = 2);
			four = 4;
			OvlFunc_934_2008528(0, s.c >> 20, (s.e >> 20) - 2, 1, four, zero);
			OvlFunc_934_2008528(2, s.c >> 20, (s.e >> 20) - 2, 1, four, zero);
			OvlFunc_934_2008528(2, 0x10, 0x12, 1, two, zero);
			OvlFunc_934_2008528(0, 0x10, 0x10, 1, four, zero);
			__SetFlag(0x203);
			__PlaySound(0xf0);
		}
	}
	__CutsceneEnd();
}
