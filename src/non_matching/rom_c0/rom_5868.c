/* Func_8005868 @ 0x08005868
 *
 * Source asm: goldensun/asm/rom_c0/rom_56cc_a_a_a.s
 *
 * NOT SPLIT. The .s still holds all four of its functions and the linker script
 * is untouched.
 *
 * A flash-write-and-verify: call the writer through a function pointer held in
 * ewram_2004c04, and if it reports failure fall back to VerifyFlashSector and
 * return whether that found anything.
 *
 * Twenty-seven instructions against twenty-nine. The call form is RIGHT -- this
 * is the third distinct `_call_via_rN` shape and the first where the pointer
 * comes out of a GLOBAL rather than being the address of a named function:
 *
 *     ldr r2, =ewram_2004c04 / ldr r3, [r2] / bl _call_via_r3
 *
 * which is just `extern u16 (*ewram_2004c04)(u16, void *);` and calling it.
 * That part reproduces exactly. Both residues are elsewhere.
 *
 * RESIDUE 1 -- REGISTER BIRTH ORDER, one instruction. The ROM loads the base
 * pointer into r3 and then COPIES it:
 *
 *     rom    ldr r3, [r3] / ... / mov r6, r3 / ... / add r6, #0x40
 *     ours   ldr r5, [r3] / ... / add r5, #0x40
 *
 * The ROM's `mov` exists because r3 is wanted again for the second pool load
 * (`ldr r3, [r2]`), so it stages through r3 and moves out. gcc loads straight
 * into the callee-saved register and never needs the move. The two also
 * disagree about which of r5/r6 holds the pointer and which holds the sector --
 * the ROM gives r6 to the FIRST-born value, gcc gives it r5.
 *
 * RESIDUE 2 -- the final boolean, one instruction. The ROM computes `x != 0`
 * branchlessly:
 *
 *     rom    mov r3, r0 / neg r0, r3 / orr r0, r3 / lsr r0, #0x1f
 *     ours   cmp r0, #0x0 / beq L1 / mov r0, #0x1
 *
 * gcc folds the comparison into control flow because the other arm of the `if`
 * returns the constant 1, so both arms become jumps to a common tail.
 *
 * TRIED for residue 2, all still short:
 *   1. `return !!r;`                      -- 27
 *   2. `return (r != 0) ? 1 : 0;`         -- 27
 *   3. `return (s32)((u32)(r | -r) >> 31);` -- 28, and that is writing the
 *      idiom out by hand rather than finding the source form, so it would not
 *      be the right answer even if it reached 29
 *
 * Branch polarity was checked and is already correct: the ROM's `beq` jumps
 * AWAY to the VerifyFlashSector block and falls through to `mov r0, #1`, so
 * `return 1` is the `if` body, which is how it is written below.
 *
 * What would match is open on both counts. Residue 2 is the more tractable
 * question -- what makes gcc-2.96 materialise a boolean instead of branching
 * when the other arm is a constant.
 */
#include "gba/types.h"

extern u32 iwram_3001f1c;
extern u16 (*ewram_2004c04)(u16 sector, void *buf);
extern s32 VerifyFlashSector(u16 sector, void *buf);

s32 Func_8005868(u16 sector)
{
    u8 *buf;
    u16 (*fp)(u16, void *);
    s32 r;

    buf = (u8 *)iwram_3001f1c;
    buf += 0x40;
    fp = ewram_2004c04;
    if (fp(sector, buf) != 0)
        return 1;
    r = VerifyFlashSector(sector, buf);
    return r != 0;
}
