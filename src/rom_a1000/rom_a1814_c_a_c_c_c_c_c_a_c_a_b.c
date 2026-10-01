/* Func_80a3d6c (CountInventory) @ 0x080a3d6c  --  LANDS, BYTE-IDENTICAL
 *
 * FIGURE: 0.  Measured, not inherited:
 *     OK Func_80a3d6c -- 48 bytes, 22 encodings and 1 relocations identical
 *     OK whole file   -- 48 bytes, 22 encodings and 1 relocations identical
 * Baseline for the park body in src/non_matching/rom_a1000/rom_a3d6c.c was
 * 6 of 22 (instruction counts equal, relocations ok).
 *
 * Verify with:
 *     docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/rom_a1000/rom_a1814_c_a_c_c_c_c_c_a_c_a_b.c \
 *       asm/rom_a1000/rom_a1814_c_a_c_c_c_c_c_a_c_a_b.s --whole
 *
 * THE PARK'S RECIPE WAS STALE, which is why its figures cannot be trusted.
 * It cites asm/rom_a1000/rom_a1814_c_a_c_c_c_c_c_a.s -- a path that no longer
 * exists.  The live file is asm/rom_a1000/rom_a1814_c_a_c_c_c_c_c_a_c_a.s
 * (two further splits have landed in it since).  Repoint the park header, and
 * repoint the sibling park src/non_matching/rom_a1000/80a3ddc.c, which cites the
 * same dead path.
 *
 * SPLIT.  The live .s holds THREE functions.
 *   python3 tools/datacheck.py asm/rom_a1000/rom_a1814_c_a_c_c_c_c_c_a_c_a.s -> CLEAN
 *   python3 tools/split_s.py asm/rom_a1000/rom_a1814_c_a_c_c_c_c_c_a_c_a.s Func_80a3d6c --dry-run
 *     would write ..._c_a_b.s  (1 function, 37 lines)  [Func_80a3d6c]
 *     would write ..._c_a_c.s  (2 functions, 89 lines) [Func_80a3d9c, Func_80a3ddc]
 *     would REMOVE ..._c_a.s ; would rewrite stage1.ld
 * INSTALL AT: src/rom_a1000/rom_a1814_c_a_c_c_c_c_c_a_c_a_b.c, deleting
 *             asm/rom_a1000/rom_a1814_c_a_c_c_c_c_c_a_c_a_b.s after the split.
 * AFTER LANDING, repoint the recipes of 80a3d9c.c and 80a3ddc.c to
 *             asm/rom_a1000/rom_a1814_c_a_c_c_c_c_c_a_c_a_c.s.
 *
 * PIN COUNT: 0.  No fakematch.txt row needed.
 *
 * THE MECHANISM: ONE LEVER -- the counter's INITIALISER POSITION.
 * `s32 count = 0;` initialised at its declaration and declared FIRST, ahead of
 * `slot`.  Nothing else changes.
 *
 * It buys two things at once, which is why the park's one-at-a-time screens
 * could not see it:
 *
 *  1. THE BIRTH ORDER.  The ROM emits `mov r5, #0` BEFORE `add r0, #0xd8`.
 *     All of mov-#0, add-#0xd8 and mov-#0xe are independent and have priority 1
 *     in the pre-loop block (the pool load `ldr =0x1ff` has cost 2 and takes
 *     index 2 in both streams), so sched2 ranks the other three on INSN_LUID
 *     alone.  Declaring the counter first puts its initialiser ahead of the
 *     pointer adjustment in the stream.  That is the whole of the ordering half.
 *
 *  2. THE r4/r5 SWAP, which is the same edit.  Taken off .17.lreg rather than
 *     off the park: with the counter declared last, the mask and the counter
 *     compete for r3-first REG_ALLOC_ORDER and the counter wins.  Moving the
 *     counter's birth ahead of the pointer adjustment lengthens its live range
 *     and shortens the window the mask competes over, and allocno_compare's
 *     floor_log2(R)*R/L flips: the mask is allocated first and takes r4, the
 *     counter falls to r5 -- the ROM's assignment.
 *
 * REFUTED, and this is the finding worth carrying.  The park says:
 *   "Hoisting it into a named local declared before the counter does not do it
 *    -- that variant loses an instruction somewhere else instead (19 vs 20)."
 *   "Both orders -- mask assigned before count, and count before mask -- give
 *    20 lines against the ROM's 22 and 19 differing ... So naming the mask at
 *    all is what costs the instructions, independently of where it is named.
 *    The lever reaches two independent values that are ALREADY both in
 *    registers; it does not reach a constant that gcc would otherwise fold."
 *
 * Both sentences are about the MASK.  Every screen in that park moved the mask;
 * none moved the COUNTER, and the counter is the half that matters.  The
 * conclusion drawn from the mask screens -- "naming it changes how many
 * instructions exist rather than which register each gets" -- is true of the
 * mask and false as a general boundary, and it was explicitly offered as a
 * boundary "worth carrying to the other register-birth-order parks."  It should
 * not be carried.  The mask must stay an inline literal (gcc hoists it to a
 * pseudo on its own, giving `ldr r4,=0x1ff / mov r3,r4 / and r3,r2`); only the
 * counter moves.
 *
 * Measured inert/worse, all with the mask left inline:
 *   count=0 placed between _GetUnit and the +0xd8, via a named record pointer  5
 *   same, as body assignments rather than declaration initialisers            5
 *   same, with the for rewritten as do/while                                  5
 *   same, with the for rewritten as while                                     5
 *   park body (slot declared first)                                           6
 *   counter declared and initialised first                                    0
 */
#include "gba/types.h"

extern u8 *_GetUnit(s32 unitId);

/* Counts the non-empty slots among the fifteen inventory halfwords at +0xD8.
 *
 * The slot format, worth recording: bits 0..8 the item id, bit 9 locked
 * (equipped or a key item), bits 11..15 the quantity less one.  That is why
 * consuming one unit subtracts 0x800.
 */
s32 Func_80a3d6c(s32 unitId)
{
    s32 count = 0;
    u16 *slot = (u16 *)(_GetUnit(unitId) + 0xd8);
    s32 i;

    for (i = 14; i >= 0; i--) {
        if ((*slot++ & 0x1ff) != 0)
            count++;
    }
    return count;
}
