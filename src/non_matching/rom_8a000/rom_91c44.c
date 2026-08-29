/* MapActor_WaitAnim @ 0x08091c44
 *
 * Source asm: goldensun/asm/rom_8a000/rom_91584_c_c_a_a_a_a.s
 *
 * NOT SPLIT. The .s still holds both of its functions and the linker script is
 * untouched.
 *
 * Spins for up to 0x5a frames while a sprite keeps the requested animation
 * byte, giving up early the moment it changes. Twenty-eight instructions
 * against twenty-nine.
 *
 * THE USEFUL RESULT IS THE LOOP SHAPE, and it generalises.
 *
 * The ROM enters its loop by jumping to the TEST, with the increment sitting in
 * its own block above it:
 *
 *     mov r5, #0 / b .L1 / .L2: add r5, #1 / .L1: cmp r5, #0x59 / bgt exit
 *
 * Every structured spelling gives gcc's ROTATED loop instead -- body first,
 * test at the bottom -- because `i = 0` provably satisfies `i <= 0x59` on
 * entry, so gcc drops the entry test. `for`, `while`, and -O1 all produce the
 * rotated form and all screen at 26 against 29.
 *
 * WRITING THE CONTROL FLOW OUT WITH GOTOS REPRODUCES IT:
 *
 *     i = 0;
 *     goto test;
 * inc:
 *     i++;
 * test:
 *     if (i > 0x59) return;
 *     ...
 *     if (cond) goto inc;
 *
 * That is legal C, it is what the ROM's control flow actually is, and it moves
 * this function from 26 to 28 of 29. Worth trying on any near-miss where the
 * ROM tests at the top of a counted loop and gcc tests at the bottom.
 *
 * RESIDUE, one instruction. The ROM loads the sprite pointer into a scratch and
 * COPIES it, and puts the counter in r5 with the pointer in r6; gcc loads
 * straight into r5 and puts the counter in r6:
 *
 *     rom    ldr r3, [r0, #0x50] / mov r6, r3 / mov r5, #0 / add r6, #0x24
 *     ours   ldr r5, [r0, #0x50] / mov r6, #0 / add r5, #0x24
 *
 * TRIED for the residue:
 *   1. two pointer variables, `s = a->sprite; p = s; p += 0x24;`, in the ROM's
 *      exact statement order -- gcc coalesces the copy away, no change
 *   2. the same with the `while` form -- 26
 *   3. -O1 on both forms -- 26
 *
 * (1) is the same negative already recorded in src/non_matching/rom_b5000/
 * rom_c00d8.c: separate variables defeat gcc's reuse of a value it COMPUTED,
 * not a copy it can coalesce. Two functions now say so; it is not worth a third
 * attempt without a new idea.
 *
 * This is the same load-then-move residue as src/non_matching/rom_c0/rom_5868.c,
 * where the ROM also stages a pointer through a scratch register before moving
 * it to a callee-saved one. Whatever produces that is common to both and is
 * unsolved.
 */
#include "gba/types.h"
#include "actor.h"

extern Actor *GetFieldActor(s32 slot);
extern void WaitFrames(s32 n);

void MapActor_WaitAnim(s32 slot, u32 anim)
{
    Actor *a;
    u8 *p;
    s32 i;

    a = GetFieldActor(slot);
    if (a == 0)
        return;
    if (*((u8 *)a + 0x54) != 1)
        return;
    p = (u8 *)a->sprite;
    i = 0;
    p += 0x24;
    goto test;
inc:
    i++;
test:
    if (i > 0x59)
        return;
    WaitFrames(1);
    if (anim == *p)
        goto inc;
}
