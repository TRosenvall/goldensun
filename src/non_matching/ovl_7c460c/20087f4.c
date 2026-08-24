/* OvlFunc_939_20087f4  [ovl_7c460c]  --  0x020087f4
 *
 * Source asm: goldensun/asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a.s
 *
 * Picks one of three message ids from two save bits and has actor 0x12 say it.
 * Twenty-eight instructions in the ROM; the closest attempt is twenty-seven.
 *
 * Blocker: SPECULATIVE LITERAL HOIST. The inner choice is a two-way pick
 * between two pooled constants feeding one call, and the ROM keeps it as a
 * real branch with a join:
 *
 *     cmp r0, #0 / bne .L1 / ldr r0, =0x1be2 / b .L2
 *     .L1: ldr r0, =0x1ba5
 *     .L2: bl __MessageID
 *
 * gcc loads the fall-through constant BEFORE the compare and then conditionally
 * overwrites it, which also inverts the branch and costs the join:
 *
 *     ldr r3, =0x1be2 / cmp r0, #0 / beq .L1 / ldr r3, =0x1ba5
 *     .L1: mov r0, r3 / bl __MessageID
 *
 * The extra `mov r0, r3` is what makes it 27 against 28 -- gcc picked a
 * scratch register for the merge and then had to move it into place.
 *
 * TRIED:
 *   1. `__MessageID(cond ? 0x1be2 : 0x1ba5)` in the argument            27
 *   2. a named `int id`, if/else assigning it, then one call            27
 *   3. the same written with gotos and an explicit join label           27
 *   4. __MessageID called in BOTH arms, which gets the ROM's branch     29
 *      structure exactly right -- cmp/bne, fall-through constant first,
 *      the join in the right place -- and then does not merge the two
 *      `bl __MessageID` into one, so it is one instruction LONG
 *   5. -fno-thread-jumps, -fno-schedule-insns, -fno-gcse,
 *      -fno-expensive-optimizations, -fno-schedule-insns2 on 1-4        no change
 *
 * ATTEMPT 4 IS THE INTERESTING ONE and is where a next attempt should start.
 * Everything about the shape is right except that gcc-2.96 does not cross-jump
 * the identical one-instruction tail. If the two arms can be made to share a
 * LONGER tail -- or if a cross-jumping flag turns out to exist in this
 * compiler -- the function falls out. The outer arms are not the problem: the
 * ROM duplicates `mov r0,#0x12 / mov r1,#0 / bl __ActorMessage` in both of
 * them, so the source duplicates them too and gcc reproduces that happily.
 *
 * The body below is attempt 2, the shortest of the three that get the outer
 * structure right.
 */

extern int __GetFlag(int flag);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int b);

void OvlFunc_939_20087f4(void)
{
    int id;

    if (__GetFlag(0x941) == 0) {
        if (__GetFlag(0x85a) == 0)
            id = 0x1be2;
        else
            id = 0x1ba5;
        __MessageID(id);
        __ActorMessage(0x12, 0);
    } else {
        __MessageID(0x250c);
        __ActorMessage(0x12, 0);
    }
}
