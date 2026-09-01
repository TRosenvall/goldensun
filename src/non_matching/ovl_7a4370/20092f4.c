/* OvlFunc_917_20092f4 (0x020092f4) -- NON-MATCHING.
 * Blocker class: scheduling -- two argument pairs transposed, four lines.
 *
 * 230 lines against the ROM's 230, FOUR differing, and 226 of the 230 include
 * two gcc-generated jump tables and every one of the cross-jumped tails that
 * make this function's layout look hand-woven.
 *
 *     rom    mov r1, #0x8 / mov r0, #0x8 / bl __MapActor_SetAnim
 *     ours   mov r0, #0x8 / mov r1, #0x8 / bl __MapActor_SetAnim
 *
 * Both sites are `__MapActor_SetAnim(8, 8)` inside case 10 of the first switch.
 * THE SAME CALL IN case 6 MATCHES -- the ROM orders r0 first there. So the ROM
 * itself emits both orders for the identical call, in different arms of the
 * same switch, and no source property distinguishes them. Case 6's is the last
 * call before its `break`; case 10's are each followed by a `__WaitFrames(6)`
 * that wants r0.
 *
 * MEASURED (rom 230 lines, all at exact length):
 *   baseline                                    230, 4  <- best
 *   __MapActor_SetAnim declared to return int   230, 4
 *   __MapActor_SetAnim's prototype withheld     230, 4
 *   `n8 = 8;` in the dominating entry block
 *     (the batch-176 basic-block lever)         230, 4
 *   -fno-gcse / -fno-strength-reduce /
 *     -fno-strict-aliasing /
 *     -fno-rerun-cse-after-loop                 230, 4 (all inert)
 *   -fno-schedule-insns                         230, 4
 *   -fno-peephole, -fno-defer-pop               230, 4
 *   -fno-schedule-insns2                        230, 50 (the recorded
 *                                     "destroying the evidence" signature --
 *                                     the second scheduler is producing 226 of
 *                                     the 230 correct lines and turning it off
 *                                     loses twelve times more than it gains)
 *
 * WHAT IS RIGHT, and is why this is worth reading:
 *
 *   TWO JUMP TABLES FROM TWO PLAIN `switch` STATEMENTS. The ROM's
 *   `ldr r3, [r3, r2] / mov pc, r3` with an inline `.word` table comes out of
 *   `switch (b)` over contiguous cases with no help. Do not hand-write the
 *   dispatch.
 *
 *   THE CROSS-JUMPED TAILS ARE FREE, AND THE CASE ORDER IS WHAT PRODUCES THEM.
 *   The ROM's arms branch into the middle of OTHER arms -- case 0 of the first
 *   switch ends `mov r0, #8 / b .L1488` where .L1488 is inside case 0 of the
 *   SECOND switch. That is gcc merging identical tails, and it happens by
 *   itself PROVIDED the cases are written in the ROM's layout order. Two arms
 *   also fall through (case 10 into cases 7 and 11; the second switch's case 4
 *   into case 1), which only works if those cases are adjacent in the source in
 *   that order. **Order the cases by their label addresses in the ROM, not
 *   numerically.**
 *
 * NEXT: nothing source-level in eight probes.
 */
extern void __MapActor_SetAnim(int slot, int anim);
extern void __WaitFrames(int n);

void OvlFunc_917_20092f4(int a, unsigned int b)
{
    if (a == 0xa) {
        switch (b) {
        case 0:
            __MapActor_SetAnim(8, 1);
            __WaitFrames(6);
            __MapActor_SetAnim(8, 3);
            break;
        case 1:
            __MapActor_SetAnim(8, 1);
            break;
        case 2:
            __MapActor_SetAnim(8, 1);
            __WaitFrames(6);
            __MapActor_SetAnim(8, 5);
            break;
        case 3:
            __MapActor_SetAnim(8, 1);
            __WaitFrames(6);
            __MapActor_SetAnim(8, 4);
            break;
        case 4:
            __MapActor_SetAnim(8, 1);
            __WaitFrames(6);
            __MapActor_SetAnim(8, 3);
            __WaitFrames(6);
            __MapActor_SetAnim(8, 1);
            __WaitFrames(6);
            __MapActor_SetAnim(8, 3);
            __WaitFrames(6);
            __MapActor_SetAnim(8, 1);
            break;
        case 5:
            __MapActor_SetAnim(8, 1);
            __WaitFrames(6);
            __MapActor_SetAnim(8, 2);
            break;
        case 6:
            __MapActor_SetAnim(8, 6);
            __WaitFrames(6);
            __MapActor_SetAnim(8, 8);
            break;
        case 8:
            __MapActor_SetAnim(8, 6);
            __WaitFrames(6);
            __MapActor_SetAnim(8, 9);
            break;
        case 9:
            __MapActor_SetAnim(8, 6);
            __WaitFrames(6);
            __MapActor_SetAnim(8, 0xa);
            break;
        case 10:
            __MapActor_SetAnim(8, 6);
            __WaitFrames(6);
            __MapActor_SetAnim(8, 8);
            __WaitFrames(6);
            __MapActor_SetAnim(8, 6);
            __WaitFrames(6);
            __MapActor_SetAnim(8, 8);
            __WaitFrames(6);
        case 7:
        case 11:
            __MapActor_SetAnim(8, 6);
            break;
        }
    } else {
        switch (b) {
        case 0:
            __MapActor_SetAnim(9, 1);
            __WaitFrames(6);
            __MapActor_SetAnim(9, 3);
            break;
        case 2:
            __MapActor_SetAnim(9, 1);
            __WaitFrames(6);
            __MapActor_SetAnim(9, 5);
            break;
        case 3:
            __MapActor_SetAnim(9, 1);
            __WaitFrames(6);
            __MapActor_SetAnim(9, 4);
            break;
        case 4:
            __MapActor_SetAnim(9, 1);
            __WaitFrames(6);
            __MapActor_SetAnim(9, 3);
            __WaitFrames(6);
            __MapActor_SetAnim(9, 1);
            __WaitFrames(6);
            __MapActor_SetAnim(9, 3);
            __WaitFrames(6);
        case 1:
            __MapActor_SetAnim(9, 1);
            break;
        case 5:
            __MapActor_SetAnim(9, 1);
            __WaitFrames(6);
            __MapActor_SetAnim(9, 2);
            break;
        }
    }
    __WaitFrames(0xc);
}
