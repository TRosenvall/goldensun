/* MapActor_WaitAnim -- 0x08091c44.  EXACT.
 * ref: asm/rom_8a000/rom_91584_c_c_a_a_a_a_a.s
 *
 * objcmp:
 *   OK whole file -- 56 bytes, 26 encodings and 2 relocations identical
 *   OK MapActor_WaitAnim -- 56 bytes, 26 encodings and 2 relocations identical
 * 26 encodings / 56 bytes (24 halfword insns + two 4-byte `bl`).
 * PINS: 0.  No shims, no volatile, no .equ, no per-file flags, no device.
 *
 * SPLIT SHAPE: none needed.
 *   `python3 tools/split_s.py --dry-run asm/rom_8a000/rom_91584_c_c_a_a_a_a_a.s \
 *        MapActor_WaitAnim`
 *     -> "holds only MapActor_WaitAnim and no data; convert it directly, no split needed"
 *   `python3 tools/datacheck.py asm/rom_8a000/rom_91584_c_c_a_a_a_a_a.s` prints NOTHING.
 *   grep -c thumb_func_start = 1.  So the .c takes the stem unchanged.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_8a000/rom_91584_c_c_a_a_a_a_a.c \
 *     asm/rom_8a000/rom_91584_c_c_a_a_a_a_a.s --whole
 *
 * ================ WHAT THIS FUNCTION DOES ================
 *
 * Waits up to 0x5a frames for a field actor to leave the given animation,
 * giving up early if the slot is empty or the actor is not draw kind 1
 * (DRAW_KIND_SINGLE, one sprite at +0x50).  The animation byte is at +0x24 of
 * the sprite.  Semantics merged from both retired parks; they agreed on this.
 *
 * ================ THE RESIDUE, AND WHAT CLOSED IT ================
 *
 * Both parks got the control flow right and then stalled one instruction short
 * on the preheader, where the ROM stages the sprite pointer through a scratch:
 *
 *     rom    ldr r3, [r0, #0x50] / mov r6, r3 / mov r5, #0 / add r6, #0x24
 *     parks  ldr r5, [r0, #0x50] / mov r6, #0 / add r5, #0x24
 *
 * Both parks diagnosed this as an unsolved "load-then-move" class shared with
 * src/non_matching/rom_b5000/rom_c00d8.c and src/non_matching/rom_c0/rom_5868.c,
 * and recorded "separate variables defeat gcc's reuse of a value it COMPUTED,
 * not a copy it can coalesce -- not worth a third attempt".  THAT DIAGNOSIS IS
 * REFUTED, and the refutation is in the machine description, not in a dump.
 *
 * `*thumb_addsi3` (arm.md:496) carries `[(set_attr "length" "2")]` -- ONE
 * halfword, always -- and its only CONST_INT alternatives are 0 and 1, whose
 * operand-1 constraints are `%0` and `0`.  So a `plus` of a register and a
 * CONST_INT that does not fit `imm3` HAS NO ALTERNATIVE WITH DISTINCT rd/rn:
 * gcc cannot emit `p = s + 0x24` as one insn when p and s are different
 * registers, and RELOAD SPLITS IT into a `mov` plus an in-place `add`.  Two
 * separate insns post-reload is exactly what sched2 is then free to schedule
 * around -- which is how `mov r5, #0` gets BETWEEN the ROM's `mov r6, r3` and
 * its `add r6, #0x24`.  Non-adjacency was the clue: no single insn's output
 * template can have another insn inserted into it, so the two halves were
 * never one insn, so there was never a copy for gcc to coalesce.
 *
 * The parks wrote the pointer in TWO statements (`p = a->sprite;` then
 * `p += 0x24;`).  That is an in-place add on a pseudo gcc can set directly from
 * the load, so it allocates one register and emits three insns.  Writing it as
 * ONE EXPRESSION (`p = (u8 *)a->sprite + 0x24;`) leaves the load result its own
 * short-lived pseudo -- local-alloc gives it r3, REG_ALLOC_ORDER's first choice
 * -- while `p`, live across the call, becomes a global allocno in r6.  Reload
 * then splits, and the fourth insn appears.
 *
 * >>> A LEVER, STATED GENERALLY: on thumb, `q = p + K` for K outside imm3 is
 * >>> TWO post-reload insns whenever q and p get different registers, and ONE
 * >>> in-place insn when they share.  Spelling it as one expression rather than
 * >>> assign-then-increment is what decides which.  Any near-miss where the ROM
 * >>> has `mov rA, rB / <unrelated insn> / add rA, #K` is this shape.
 * >>> Candidates: rom_b5000/rom_c00d8.c and rom_c0/rom_5868.c, which both
 * >>> parks named as the same unsolved residue.
 *
 * ================ THE 2x2 THAT PROVED IT ================
 *
 * Neither edit lands alone; the crossing is the whole result.
 *
 *   pointer spelling      `i = 0` position   objcmp
 *   two statements        before the pointer   18 of 26   (= park rom_91c44.c)
 *   two statements        after  the pointer   18 of 26
 *   ONE expression        after  the pointer    2 of 26
 *   ONE expression        BEFORE the pointer    0  <-- this file
 *
 * The one-expression form is the prerequisite; `i = 0` first is what completes
 * it.  `i = 0` first alone is worth NOTHING (18 either way), which is exactly
 * the "inert for want of a prerequisite" shape.
 *
 * ================ MEASURED AND LOAD-BEARING ================
 *
 * 1. THE GOTO CONTROL FLOW.  `i = 0; goto test; inc: i++; test: if (i > 0x59)`
 *    reproduces the ROM's `b` over the increment.  Every structured spelling
 *    gets gcc's rotated loop (test at the bottom) because `i = 0` provably
 *    satisfies the entry test, so gcc drops it.  Inherited from park
 *    rom_91c44.c, re-measured here: the same body with `while (i <= 0x59)`
 *    reads 14 of 26 even WITH the one-expression pointer.  Park 8091c44.c's
 *    three further spellings (`for`, `do`/`while` with a leading guard, an
 *    explicit `goto` to a label before the test) are recorded there as all
 *    equal; not re-run.
 * 2. `anim == *p`, NOT `*p == anim`.  Operand order survives to the `cmp`:
 *    `*p == anim` reads 1 of 26, differing only at index 21 (`cmp r7, r3` vs
 *    `cmp r3, r7`).  A one-instruction residue hiding in a comparison's operand
 *    order is worth remembering.
 * 3. `p = (u8 *)a->sprite + 0x24;` as one expression, and AFTER `i = 0;`.
 *    See the 2x2 above.
 *
 * ================ MEASURED AND EXACTLY INERT (all still 0) ================
 *
 * - `s32 anim` instead of `u32 anim`.
 * - A NAMED POINTER for the +0x54 kind test (`k = (u8 *)a; k += 0x54; *k`)
 *   against the inline `*((u8 *)a + 0x54)`.  **This REFUTES park 8091c44.c's
 *   "WHAT IS RIGHT" claim** that the named pointer "is what produces the ROM's
 *   `mov r3, r0 / add r3, #0x54 / ldrb r3, [r3]`".  It is not: thumb `ldrb`
 *   takes imm5, 0x54 does not fit, so reload materialises the address either
 *   way.  Same mechanism as the lever above, in its inert direction.
 * - `i >= 0x5a` instead of `i > 0x59` -- combine's `simplify_comparison`
 *   collapses them, as docs/elevation.md's operator-sweep note says.
 * - DECLARATION ORDER of `p` and `i` (both permutations).
 * - An intermediate named `s` (`s = (u8 *)a->sprite; p = s + 0x24;`).  Exact.
 *   So the lever is the ONE-EXPRESSION add, not the absence of a second name --
 *   which is the half the parks never crossed.
 *
 * This file ships the two-local form because it is the shorter true statement.
 *
 * ================ PARK HYGIENE ================
 *
 * This function had TWO parks, both claiming "18 of 26".  Both held a real
 * function definition (neither was the zero-byte-TU failure mode), but the
 * figures were not comparable:
 *   src/non_matching/rom_8a000/rom_91c44.c   ref 26 / ours 26, size equal -- 1 insn short
 *   src/non_matching/rom_8a000/8091c44.c     ref 26 / OURS 24, size 56 vs 52 -- 2 insns short
 * The shared "18" is positional MISALIGNMENT in both cases, not a distance; the
 * honest numbers were 1 and 2.  rom_91c44.c's own prose already said "28 of 29"
 * and so contradicted its own header line.  Both are retired by this landing.
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
    i = 0;
    p = (u8 *)a->sprite + 0x24;
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
