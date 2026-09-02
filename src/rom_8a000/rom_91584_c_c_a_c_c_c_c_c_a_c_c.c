/* Func_8092504 -- 0x08092504, the whole of
 * goldensun/asm/rom_8a000/rom_91584_c_c_a_c_c_c_c_c_a_c_c.s, so no split was
 * needed and the linker script is unchanged.
 *
 * Polls a byte on a field actor for up to 0x5a frames and returns as soon as it
 * differs from the value read at entry.
 *
 * THE PARK HAD THE FIRST LEVER AND STATED IT AS AN INFERENCE, correctly. The
 * ROM allocates a four-byte frame for ONE value, writes the entry byte into it,
 * and re-reads it on EVERY iteration rather than keeping it in a callee-saved
 * register -- and r5, r6 and r7 are all pushed, so it is not short of them. A
 * memory-resident local re-read at each use is what `volatile` produces, and
 * declaring it so took the body from 29 lines with 33 differing to exactly 34
 * with 7. Two further levers close the rest, and both are new.
 *
 * ASSIGNMENT ORDER OF AN ADDRESS-HOLDING POINTER FIXES REGISTER BIRTH ORDER.
 * The residue was the recorded r6/r7 swap: the ROM polls through r7 and holds
 * the actor's animation pointer in r6, and every spelling produced the reverse.
 * Introducing a named pointer TO THE LOCAL (`sample = &initial;`) and assigning
 * it BEFORE the polled pointer creates that pseudo earlier in RTL expansion and
 * flips the whole allocation. Declaration order does nothing -- three orders
 * were byte-identical, which the park had already found. It is the assignment
 * statement that moves it, the same distinction batch 179 drew for a sentinel.
 * The direction is counter-intuitive: the EARLIER-born pseudo takes the HIGHER
 * callee-saved register.
 *
 * `volatile` MUST BE ON THE POINTEE AS WELL AS THE POINTER. With
 * `int initial; volatile int *sample;` the loop load is right but the one-time
 * store degrades to `str r3, [sp, #0]` -- gcc keeps the known `sample == sp+0`
 * equivalence for the single non-loop reference. Qualifying the object itself
 * suppresses that substitution and both accesses go through the register. No
 * flag reached this: -fno-gcse, -fno-strict-aliasing and
 * -fno-rerun-cse-after-loop all leave the one line.
 *
 * MEASURED (rom 34 lines):
 *   the park's plain `volatile int` local              34 lines, 7 differing
 *   a named pointer to it, assigned AFTER the polled pointer
 *                                                      34, 14
 *   `int` local, `volatile int *` pointer, assigned first
 *                                                      34, 1
 *   both qualified, pointer assigned first             34, MATCH
 *   the same on actor.h's types                        34, MATCH (byte-identical)
 *   `int init[1];` non-volatile array                  29, 33
 */
#include "actor.h"

extern Actor *GetFieldActor(s32 slot);
extern void WaitFrames(s32 n);

void Func_8092504(s32 slot)
{
	Actor *actor;
	u8 *anim;
	volatile s32 initial;
	volatile s32 *sample;
	s32 frames;

	actor = GetFieldActor(slot);
	if (actor == NULL)
		return;
	if (actor->drawKind != DRAW_KIND_SINGLE)
		return;

	sample = &initial;
	anim = (u8 *)actor->sprite + 0x24;
	*sample = *anim;
	for (frames = 0; frames <= 0x59; frames++) {
		WaitFrames(1);
		if (*sample != *anim)
			break;
	}
}
