/* MapActor_WaitAnim (0x08091c44) -- NON-MATCHING.
 * Blocker class: LOOP GUARD HOISTED -- ours is three lines SHORT.
 *
 * 26 lines against the ROM's 29, 19 differing.
 *
 * Waits up to 0x5a frames for a field actor to leave the given animation,
 * giving up early if the slot is empty or is not a plain actor.
 *
 * WHAT IS RIGHT: the null guard, the kind test through a named pointer at +0x54
 * (which is what produces the ROM's `mov r3, r0 / add r3, #0x54 / ldrb r3, [r3]`
 * rather than an immediate-offset load), the sprite pointer taken from +0x50
 * and advanced to +0x24 in two steps, and the frame counter's 0x59 bound.
 *
 * WHAT IS WRONG is the loop's entry. The ROM tests the counter FIRST, reaching
 * the test with a `b` over the increment:
 *
 *     b .L91c66
 *   .L91c64: add r5, #1
 *   .L91c66: cmp r5, #0x59 / bgt done / ... / beq .L91c64
 *
 * Written as a `while` with the wait inside, gcc rotates the loop so the guard
 * sits at the bottom and the branch-over disappears, which is three
 * instructions cheaper. The rotation is the compiler's own and no spelling of
 * the loop reached it in this round: a `for`, a `do`/`while` with a leading
 * guard, and an explicit `goto` to a label before the test all produced the
 * same 26 lines.
 *
 * This is the mirror of the branch-over-pool correction from batch 177 -- there
 * a `b` to the next label was ours and not the ROM's; here it is the ROM's and
 * not ours. Worth reading together.
 *
 * NEXT: the loop-rotation question is whether an early-exit spelling that makes
 * the counter live BEFORE the loop body forces gcc to emit the guard first.
 * Only three spellings were tried; this park is recorded at the depth reached.
 */
extern unsigned char *GetFieldActor(int slot);
extern void WaitFrames(int n);

void MapActor_WaitAnim(int slot, int anim)
{
	unsigned char *a;
	unsigned char *k;
	unsigned char *p;
	int i;

	a = GetFieldActor(slot);
	if (a == 0)
		return;
	k = a;
	k += 0x54;
	if (*k != 1)
		return;
	p = *(unsigned char **)(a + 0x50);
	i = 0;
	p += 0x24;
	while (i <= 0x59) {
		WaitFrames(1);
		if (*p != anim)
			return;
		i++;
	}
}
