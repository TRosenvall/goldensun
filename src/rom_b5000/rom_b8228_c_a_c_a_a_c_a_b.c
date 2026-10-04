/* Func_80b8530 (0x080b8530) -- GetCombatantDrawSlot.  MATCHING.
 *
 * MATCHING: 0 of 30 encodings, 68 bytes, 4 relocations.  Pin-free,
 * device-free, no per-file flags.
 *
 * Verify with (BEFORE the split, against the installed multi-function .s):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     scratch_elev/b323/A/p1_candidate.c \
 *     asm/rom_b5000/rom_b8228_c_a_c_a_a_c_a.s --func Func_80b8530
 *
 * Verify with (AFTER the split, at the INSTALLED path):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_b5000/rom_b8228_c_a_c_a_a_c_a_b.c \
 *     asm/rom_b5000/rom_b8228_c_a_c_a_a_c_a_b.s --whole
 *
 * SPLIT SHAPE.  asm/rom_b5000/rom_b8228_c_a_c_a_a_c_a.s holds THREE functions
 * -- Func_80b84c0, Func_80b8530, Func_80b8574 -- so the target needs the
 * three-way split.  `tools/split_s.py ... Func_80b8530 --dry-run`:
 *
 *     would write asm/rom_b5000/rom_b8228_c_a_c_a_a_c_a_a.s  (1 function, 52 lines)
 *     would write asm/rom_b5000/rom_b8228_c_a_c_a_a_c_a_b.s  (1 function, 37 lines)
 *     would write asm/rom_b5000/rom_b8228_c_a_c_a_a_c_a_c.s  (1 function, 193 lines)
 *     would REMOVE asm/rom_b5000/rom_b8228_c_a_c_a_a_c_a.s
 *     would rewrite stage1.ld            (the single line 1603)
 *
 * so the target lands at src/rom_b5000/rom_b8228_c_a_c_a_a_c_a_b.c.
 *
 * `tools/datacheck.py asm/rom_b5000/rom_b8228_c_a_c_a_a_c_a.s` prints NOTHING
 * and exits 0: no .rodata/.data/.bss, no data labels, so no text/data split and
 * no `exports` are needed.  `Func_80b8530` is already declared in
 * src/rom_b5000/exports.s (`.export_func Func_80b8530`, line 20), which is
 * where its callers in rom_c9000 reach it.
 *
 * `objcmp.py --whole` against the post-split single-function cut is green too
 * -- 68 bytes, 30 encodings, 4 relocations -- so the section tail is right.
 * That check matters here precisely because the function becomes the ONLY one
 * in its object, which is where --func cannot see zero-vs-nop tail fill.
 *
 * PINS: 0.  No inline asm, no `register ... __asm__`, no shim.
 *
 * WHAT THE RESIDUE WAS.  The park (src/non_matching/rom_b5000/rom_b8530.c,
 * 10 of 30) read the first differing index and called it "Blocker class 2,
 * REGISTER BIRTH ORDER, down to ONE instruction: rom `lsr r3, r0, #8`, ours
 * `lsr r0, r0, #8` -- gcc reuses r0, the register the call returned in; the ROM
 * moves it to r3, which is what REG_ALLOC_ORDER hands a NEW pseudo."
 *
 * It is not birth order and it is not one instruction.  It is THE NUMBER OF
 * EXIT POINTS.  The ROM has ONE, and one result register:
 *
 *     lsr  r3, r0, #8        <- value 1 into r3
 *     cmp  r3, #0 / bne  .Lout
 *     ...second lookup...
 *     mov  r3, #0xc0 / lsl r3, #13     <- value 2 into r3
 *     cmp  r0, #0 / bne  .Lout
 *     mov  r3, #0xc0 / lsl r3, #14     <- value 3 into r3
 *   .Lout:
 *     mov  r0, r3            <- the single return
 *
 * Every one of the three results is written to the SAME register and the
 * function ends `mov r0, r3 / pop {r5} / pop {r1} / bx r1`.  The park spelled
 * three separate `return` statements, so each value went straight into r0,
 * there was no common tail, and the r3-vs-r0 difference showed up at index 9
 * and again across indices 19-25.  The `lsr r3` was never an allocation fact:
 * it is the result being a VARIABLE.
 *
 * MEASURED (ref 30 encodings; `tools/sweep_variants.py`, which imports objcmp):
 *   the installed park, three `return`s                   10, first at 9
 *   one result var, `r = 0x180000;` BEFORE the 2nd call   13, first at 0 + RELOCDIFF
 *   one result var, `r = 0x300000;` BEFORE the 2nd call   14, first at 0 + RELOCDIFF
 *   one result var, call result named, then the constants  0  <- exact
 *   one result var, ternary                                0  <- exact  (shipped)
 *   one result var, nested if/else                         0  <- exact
 *
 * TWO THINGS WORTH KEEPING from that table:
 *
 *   1. A SOURCE EDIT THAT MAKES A VALUE LIVE ACROSS A CALL CHANGES INSTRUCTION
 *      ZERO.  Putting the constant store before the second call makes the
 *      result pseudo live across it, so it takes a callee-saved register, the
 *      prologue push set changes, and the figure jumps to first=0 with the
 *      relocations reordered.  This is the batch-322 pin bound ("a register pin
 *      is NOT a free diagnostic: it changes the prologue push set") arriving
 *      from a pure source edit rather than a pin.
 *
 *   2. THE NAMED TEMPORARY, THE TERNARY AND THE if/else ARE ONE EQUIVALENCE
 *      CLASS HERE.  All three are byte-identical.  jump2 folds the if/else
 *      form's unconditional `b` into the common tail because both arms are
 *      constant stores to the same pseudo and the tail is the epilogue.  The
 *      ternary is shipped: fewest statements, and no temporary the ROM gives no
 *      evidence for.
 *
 * ONE PARK-HYGIENE RESULT, since the brief flagged the source-stem filename as
 * the shape in which double parks hide: `grep -rln 'Func_80b8530'` over the
 * tree finds exactly ONE definition, this park.  The other hits are rom_c9000
 * call sites, tools/name_proposals.tsv, docs/name-proposals.md, and an `extern`
 * plus one call in src/rom_b5000/rom_b8228_c_a_c_a_a_b.c.  THERE IS NO SECOND
 * PARK and the 10 was correct.  The park does carry one stale line: its
 * `Source asm:` names `rom_b8228_c_a_c_a_a_c.s` while its recipe names
 * `..._c_a.s`; the recipe is right, the `_c_a` file is the one holding
 * `.thumb_func_start Func_80b8530`.
 */
#include "gba/types.h"

struct Unit {
    u8 pad_00[0x128];
    u8 classIdx;
};

extern struct Unit *_GetUnit(s32 unitId);
extern s32 GetEnemyHeight(s32 classIdx);
extern s32 Func_80c23c0(s32 classIdx);

/* Resolves a combatant's class to a draw slot, defaulting when neither lookup
 * reports one.  The unit record is resolved TWICE, once per lookup, rather
 * than cached; that repetition is in the ROM.
 */
s32 Func_80b8530(s32 unitId)
{
    s32 slot;

    slot = (u8)GetEnemyHeight(_GetUnit(unitId)->classIdx) << 16;
    if (slot == 0)
        slot = Func_80c23c0(_GetUnit(unitId)->classIdx) != 0 ? 0x180000 : 0x300000;
    return slot;
}
