/* OvlFunc_917_20092f4  --  0x020092f4
 *
 * MATCHING, 0 of 207 encodings.  Whole-file conversion of
 * asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_c_a.s, which holds this function
 * and nothing else (datacheck.py prints nothing -- no data section, so NO
 * EXPORT and no split are required).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_c_a.c \
 *     asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_c_a.s --whole
 *
 * NO PIN, NO SHIM, NO FLAG, NO DEVICE.  Parked at 4 of 207 since batch 319.
 *
 * A cutscene actor script: `a` selects which of the two actors is being driven
 * (slot 8 or slot 9) and `b` is the beat index, dispatched through two plain
 * `switch` statements.  Both jump tables, and every cross-jumped tail in the
 * ROM's layout, come out of ordinary C with no help.
 *
 * ============ WHAT THE PARK HAD WRONG, AND WHAT CLOSED IT ============
 *
 * The park's residue description was WRONG IN ITS LOCATIONS.  It read:
 *
 *     "Both sites are `__MapActor_SetAnim(8, 8)` inside case 10 of the first
 *      switch.  THE SAME CALL IN case 6 MATCHES ... and no source property
 *      distinguishes them."
 *
 * The two sites are in DIFFERENT switches and are DIFFERENT calls:
 *
 *     site A  the LAST `__MapActor_SetAnim(8, 8)` of case 10, first switch
 *     site B  the LAST `__MapActor_SetAnim(9, 3)` of case 4, second switch
 *
 * and the first `(8, 8)` of case 10 MATCHES.  "No source property
 * distinguishes them" was the conclusion drawn from the wrong pair; the real
 * pair has a property, and it is positional.
 *
 * THE MECHANISM.  .19.flow2 emits EVERY argument pair as `r0 = slot` then
 * `r1 = anim`, in that order, at all 33 call sites.  sched2 then TRANSPOSES
 * the pair at every call site in a basic block EXCEPT THE LAST ONE IN THAT
 * BLOCK.  That asymmetry is in the ROM too: in all seven `break`-terminated
 * arms, the last pair is `r0` first in the ROM exactly as in ours.
 *
 * So the question is never "which register order did the author write" -- there
 * is no such thing -- it is WHERE THE BASIC BLOCK ENDED when sched2 ran.  And
 * the two differing arms are precisely the two the park believed had to be
 * written as FALL-THROUGHS:
 *
 *     case 10 falling into cases 7 / 11,   second switch's case 4 into case 1.
 *
 * A fall-through written in C ends the block at the `case` label, so the
 * trailing `__WaitFrames(6)` was the end of the block and the SetAnim before
 * it was "last" -- not transposed.  THE ROM'S BLOCK IS LONGER THAN THAT.
 *
 * ** TAIL CROSS-JUMPING RUNS IN jump2, AFTER sched2.  SO A HAND-WRITTEN
 *    FALL-THROUGH IS NOT EQUIVALENT TO A DUPLICATED TAIL PLUS `break`. **
 *
 * Writing each of those two arms with its final call SPELLED OUT and a `break`
 * -- `...; __WaitFrames(6); __MapActor_SetAnim(8, 6); break;` ahead of
 * `case 7: case 11: __MapActor_SetAnim(8, 6); break;` -- gives sched2 one
 * longer block, so the `(8, 8)` pair is no longer last and is transposed, and
 * the duplicated tail is then merged back into a fall-through by jump2.  The
 * emitted code is byte-identical to the ROM, label placement and all 77
 * relocations included.
 *
 * MEASURED (ref 207, ours 207 throughout -- figures are distances):
 *     baseline, both arms written as fall-throughs        4
 *     case 10's tail spelled out, second switch's not     2
 *     second switch's spelled out, case 10's not          2
 *     BOTH spelled out                                    0
 * The two edits are independent and each pays its own half; neither is a
 * prerequisite for the other.
 *
 * WHAT THE PARK HAD RIGHT, and it is still load-bearing:
 *   * Two jump tables out of two plain `switch` statements over contiguous
 *     cases.  Do not hand-write the dispatch.
 *   * ORDER THE CASES BY THEIR LABEL ADDRESSES IN THE ROM, not numerically.
 *     The cross-jumped tails are free and the case order is what produces
 *     them.  The order below is the ROM's.
 *   * -fno-schedule-insns2 is 50 differing: sched2 is producing the layout,
 *     not spoiling it.  That signature was read correctly.
 *
 * RETIRE the park's "NEXT: nothing source-level in eight probes" line.  All
 * eight probes were spellings of the two CALLS; the lever was in the two
 * `case` arms' TERMINATION.
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
            __MapActor_SetAnim(8, 6);
            break;
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
            __MapActor_SetAnim(9, 1);
            break;
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
