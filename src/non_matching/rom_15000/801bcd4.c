/* Func_801bcd4 (0x0801bcd4) -- NON-MATCHING.
 * Blocker class: CROSS-JUMPING WE CANNOT SUPPRESS, over a register rotation.
 *
 * 91 lines against the ROM's 90 -- one LONG -- with 54 differing.
 *
 * A sprite-slot loader: allocate a slot if the caller passed -1, give up if the
 * allocator is exhausted, then dispatch on a kind code through a nine-entry
 * jump table and call one of five loaders, four of which take the slot by value
 * and two by address.
 *
 * WHAT CAME OUT RIGHT WITHOUT A LEVER: the nine-entry jump table from a plain
 * switch on `kind` with cases 1,2,4,6,7,8,9 and 3/5 falling to default; the
 * `sub r0, r7, #1 / cmp r0, #8 / bhi` bias-and-bound; the three-word stack
 * frame with the slot at sp+8, the out-parameter at sp+4 and the fifth argument
 * at sp+0; and the reload of the slot from sp+8 after every call, which is
 * gcc's own consequence of taking its address.
 *
 * THE ONE REAL FIX, and it is worth keeping: COMPARE THE COPY, NOT THE
 * PARAMETER. The ROM's guard is `cmp r4, r3` where r4 is `s` and r3 is -1, and
 * it saves `mov r6, r4` before branching so the early return has the old value.
 * Written `if (slot == -1)` gcc compares the parameter, never makes the copy,
 * and returns from the parameter's own register: 83 differing. Written
 * `s = slot; if (s == -1) { ...; return slot; }` -- compare the copy, return the
 * parameter -- it drops to 54. Two names for one value are only distinct to gcc
 * when each is READ in a different place.
 *
 * THE BLOCKER. gcc cross-jumps the two `LoadInventoryIcon` arms and the ROM
 * does not:
 *
 *     rom    mov r2, r4 / mov r0, r5 / mov r1, #0x3a / bl LoadInventoryIcon
 *            mov r2, r4 / mov r0, r5 / mov r1, #0x2a / bl LoadInventoryIcon
 *
 *     ours   mov r0, r6 / mov r1, #0x3a / b L11
 *            mov r0, r6 / mov r1, #0x2a / L11: mov r2, r4 / bl LoadInventoryIcon
 *
 * The cause is the ARGUMENT ORDER. The ROM sets r2, then r0, then r1, so the
 * only common suffix the two arms share is the `bl` itself and jump.c leaves
 * them alone. We set r0, r1, r2, which makes `mov r2, r4 / bl` a two-instruction
 * common suffix -- long enough to merge. So the cross-jump is a SYMPTOM of the
 * argument order, not an independent problem, and suppressing it means
 * reproducing the ROM's r2-first evaluation.
 *
 * That is the same knob as the recorded return-type lever, and here it does not
 * turn: declaring all five loaders `int`, and declaring only LoadInventoryIcon
 * `int`, are both byte-identical to the `void` version at 54 differing. The
 * lever moves r0 relative to the OTHER argument registers when the callee
 * returns a value; it does not let r2 be chosen first.
 *
 * MEASURED (rom 90 lines):
 *   `if (slot == -1)`, all callees void            91 lines, 83 differing
 *   `if (s == -1)`, return slot                    91, 54  <- kept
 *   as above, all five callees declared int        91, 54  (byte-identical)
 *   as above, only LoadInventoryIcon int           91, 54  (byte-identical)
 *
 * Under the 54 there is also a plain r5/r6 rotation -- the ROM keeps the
 * second argument in r5 and we keep it in r6 -- which is the recorded
 * allocation-order class and would still be there if the cross-jump were fixed.
 *
 * NEXT: the question worth one screen is whether the third argument can be made
 * to evaluate first by giving it a side effect or an address-taken form that
 * gcc must sequence early. Everything spelling-level has been tried.
 */
extern int AllocSpriteSlot(void);
extern void LoadOldUIIcon(int a, int b, int *p, int *q, int e);
extern void LoadInventoryIcon(int a, int b, int c);
extern void LoadMoveIcon(int a, int b, int *p, int *q, int e);
extern void LoadStatusIcon(int a, int b, int c);
extern void LoadUIBanner(int a, int b, int c);

int Func_801bcd4(int kind, int b, int slot, int d)
{
	int s;
	int t;

	s = slot;
	if (s == -1) {
		s = AllocSpriteSlot();
		if (s == 0x60)
			return slot;
	}
	switch (kind) {
	case 1:
	case 6:
		LoadOldUIIcon(b, d, &s, &t, 1);
		break;
	case 2:
		LoadInventoryIcon(b, 0x3a, s);
		break;
	case 7:
		LoadInventoryIcon(b, 0x2a, s);
		break;
	case 4:
		LoadMoveIcon(b, d, &s, &t, 1);
		break;
	case 8:
		LoadStatusIcon(b, 0, s);
		break;
	case 9:
		LoadUIBanner(b, 0, s);
		break;
	default:
		return s;
	}
	return s;
}
