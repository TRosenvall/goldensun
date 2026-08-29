/* OvlFunc_956_20081c8  [ovl_7e0928]
 *
 * Source asm: goldensun/asm/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_a.s
 *
 * NOT SPLIT. The .s still holds all seven of its functions and the overlay
 * linker script is untouched.
 *
 * Waits ten frames, then spins until two overlay globals reach 3 and 1, giving
 * up after 0x78 frames.
 *
 * Blocker: THE PRE-HEADER LOAD MERGE. 25 of 26, short by exactly one
 * instruction. See src/non_matching/preheader_load_merge.c for the class.
 *
 * This is the overlay member of the family, and it confirms the class is not
 * specific to the main ROM or to any one symbol kind -- here both loaded values
 * are `.L` data labels in the overlay, bound with gcc asm-labels, and the shape
 * of the residue is identical to the two main-ROM members.
 *
 * PROGRESS AND A DIAGNOSIS, batch 43. Found by tools/rank_parks.py --flags.
 *
 * AT -O1 THIS GOES TO 2, and all three members of this family land on the SAME
 * two instructions -- Func_80064b8 (25), Func_8012350 (27) and
 * OvlFunc_956_20081c8 (26). At -O2 the diff is the pre-header load merge this
 * file was named for; -O1 removes that and leaves one thing:
 *
 *     rom    mov r3, #0x96 / add r6, #0x1 / lsl r3, #0x1 / cmp r6, r3
 *     ours   add r6, #0x1  / mov r3, #0x96 / lsl r3, #0x1 / cmp r6, r3
 *
 * The loop limit is a TWO-INSTRUCTION CONSTANT and the ROM splits its mov/lsl
 * pair around the loop counter's increment. That is the arg-interleave shape,
 * on a comparison operand rather than a call argument.
 *
 * IT IS UNREACHABLE, AND THE MECHANISM SAYS SO IN ADVANCE. The basic-block
 * lever retires that shape, and it requires REG_N_REFS == 2 -- and REG_N_REFS
 * is incremented by `bb->loop_depth + 1`, so a set-once-used-once pseudo INSIDE
 * A LOOP counts 4, not 2. See docs/elevation.md.
 *
 * Confirmed rather than assumed, all at -O1:
 *
 *     the limit as a named local assigned in the loop body   2 of 27  (this file)
 *     the limit written inline in the comparison             3 of 27
 *     the limit assigned after the increment                 2 of 27
 *     the limit assigned BEFORE the loop, i.e. the lever     7 of 27  (worse)
 *
 * So the three are one blocker, not three, and the blocker is a known-closed
 * one rather than an open question. A per-file -O1 rule would take them from
 * three-or-more out to two, which is not worth a build-system change on its
 * own; it is recorded here so that whoever finds a way through the loop case
 * knows these three come with it.
 *
 * IMPROVED, batch 56: 6 instructions in disagreeing regions down to 4. The
 * counter increment goes AFTER the __WaitFrames call, not before it. The ROM
 * has `mov r0, #1 / add r5, #1 / bl __WaitFrames` -- the increment scheduled
 * into the argument-setup slot -- and writing `i++;` first puts the add ahead
 * of the `mov r0`. Neither source order produces the ROM's interleave, but
 * this one leaves only the pre-header load and the label shift that follows
 * from it.
 *
 * Nothing here changes the class: still short by exactly one instruction, still
 * that instruction.
 */
extern int L5480 __asm__(".L5480");
extern int L5484 __asm__(".L5484");
extern void __WaitFrames(int n);

void OvlFunc_956_20081c8(void)
{
    int i;
    int v;

    __WaitFrames(0xa);
    v = L5480;
    i = 0;
    goto check;
loop:
    i++;
    __WaitFrames(1);
    if (i > 0x77)
        return;
    v = L5480;
check:
    if (v != 3)
        goto loop;
    if (L5484 != 1)
        goto loop;
}
