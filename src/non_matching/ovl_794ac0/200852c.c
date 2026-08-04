/* OvlFunc_899_200852c  [ovl_794ac0]
 *
 * Source asm: goldensun/asm/overlays/rom_794ac0/ovl_30_a_c_a_c_a.s
 *
 * NOT SPLIT, and it would not need one -- the .s holds only this function and
 * no data. It is left in place because it does not match.
 *
 * A one-shot conversation: check two save flags, say one of three lines, and
 * on the first pass set a flag and run a short beat.
 *
 * Blocker: CONSTANT-CSE, 38 instructions against 36 -- and note it is LONGER,
 * not shorter, which is unusual for this class and is the whole point.
 *
 * The flag id 0x851 is used twice on the SAME path, once to read and once to
 * write:
 *
 *     rom    ldr r0, =0x851 / bl __GetFlag ... ldr r0, =0x851 / bl __SetFlag
 *     ours   ldr r5, =0x851 / mov r0, r5 / bl __GetFlag ... mov r0, r5 / bl __SetFlag
 *
 * gcc hoists the constant into a callee-saved register and reuses it across the
 * branch. That saves one pool load and costs a `push {r5}` and a `pop {r5}`, so
 * the CSE gcc performs to save an instruction ends up costing two.
 *
 * This is the same defect as the rest of the constant-CSE class -- gcc refusing
 * to rebuild a value the ROM rebuilds -- but it is the clearest specimen so far
 * of WHY it is a defect rather than a preference: on a Thumb target with four
 * low scratch registers, caching a pooled constant across a call is a losing
 * trade, and Camelot's compiler did not make it.
 *
 * Contrast src/overlays/rom_7a8c8c/ovl_30_c_a_c_c_a_c_c_c_a.c, elevated in the
 * same batch. That function also repeats two flag ids, but on MUTUALLY
 * EXCLUSIVE arms, so gcc never has both live at once and no CSE arises. The
 * distinction to look for before attempting one of these is whether the
 * repeated constant sits on one path or on two.
 *
 * TRIED:
 *   1. everything implicit (the form below) -- 38
 *   2. prototypes on __GetFlag and __SetFlag, separately and together -- 38,
 *      byte-identical; this is not an argument-order problem
 *
 * Nothing in the tree defeats constant-CSE yet. See also
 * src/non_matching/ovl_7f2f14/20087d8.c and
 * src/non_matching/rom_b5000/rom_c00d8.c.
 */
void OvlFunc_899_200852c(void)
{
    __CutsceneStart();
    if (__GetFlag(0x856) != 0) {
        if (__GetFlag(0x851) == 0) {
            __MessageID(0x1276);
            OvlFunc_899_2008354(0x10);
            __CutsceneWait(0xa);
            OvlFunc_899_200c63c(0x10, 3, 0x14);
            __SetFlag(0x851);
        } else {
            __MessageID(0x1278);
        }
    } else {
        __MessageID(0x1250);
    }
    OvlFunc_899_2008354(0x10);
    __CutsceneEnd();
}
