/* UploadIcon -- asm/rom_15000/rom_1aeec_a_a_c_c.s
 *
 * NON-MATCHING, 3 differing encodings of 64  (MEASURED, batch 346).
 *   THE FIGURE IS NOW A REAL DISTANCE.  Instructions 47/47, SIZE 160/160
 *   bytes, 64/64 encodings, 16 relocations identical.  The length gap the
 *   park carried (ref 47 ours 45, 160 against 156) IS CLOSED, device-free,
 *   pin-free and with NO per-file flag.  Aligned residue (tools/aligncmp.py)
 *   17 differing in 7 hunks -> 3 in 3 hunks; aligned-equal 47 -> 61 of 64.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801bc34.c \
 *     asm/rom_15000/rom_1aeec_a_a_c_c.s --func UploadIcon
 *
 * PIECE: 5 functions in the .s, all parked, worst member 251 -- so a landing
 * here needs tools/split_s.py, NOT a whole conversion.  No split is recorded
 * in the manifest because this is a park, not a landing (install_batch.py
 * refuses a split on a non-zero figure and is right).
 * tools/datacheck.py on the .s prints nothing; piece.py reports no
 * non-function symbols, so when it does land the split is code/code.
 *
 * === WHAT IS LEFT: THREE ENCODINGS, AND THEY ARE ONE REGISTER. ===
 *
 *     rom    mov r3, #1 / neg r3, r3 / str r3, [sp, #8]
 *     ours   mov r2, #1 / neg r2, r2 / str r2, [sp, #8]
 *
 * That is the whole residue.  Every other instruction, the nine-entry jump
 * table, the pool word and all 16 relocations are identical.
 *
 * === HOW THE LENGTH GAP CLOSED, AND WHY THE PARK'S VERDICT WAS WRONG ===
 *
 * The park said:
 *   "the ROM copies the AllocSpriteSlot result into a register before using
 *    it, and we use it in place ... Nothing in the source distinguishes
 *    'store it' from 'keep a copy and store that' when the copy has no other
 *    use."
 * The observation was exactly right (two sites, -1 each):
 *     rom   bl AllocSpriteSlot / mov r2, r0 / str r2, [sp,#8] / cmp r2, #0x60
 *     ours  bl AllocSpriteSlot /              str r0, [sp,#8] / cmp r0, #0x60
 * The verdict was wrong.  Something in the source does distinguish them, and
 * it is NOT at the call site.
 *
 * MECHANISM, with citations.  With the park's body the copy pseudo is
 * block-local, so LOCAL alloc owns it -- `.18.greg` says
 * `;; 2 regs to allocate: 51 33`, i.e. it is not an allocno at all.
 * `local-alloc.c:1696-1711` records the hard register of a hard->pseudo copy
 * in `qty_phys_copy_sugg`; `local-alloc.c:1340-1348` then calls
 * `find_free_reg (..., just_try_suggested=1)` and `local-alloc.c:2010-2016`
 * restricts the search to that set.  r0 is free over [birth, death)
 * (`local-alloc.c:1972-1973`), so the pseudo gets r0, the copy becomes
 * `mov r0, r0`, and reload deletes it -- `.12.life` carries
 * `(insn 49 (set (reg:SI 42) (reg:SI 0 r0)))`, it survives to `.17.lreg`,
 * and `.18.greg` shows `(note 49 ... NOTE_INSN_DELETED)` with dispositions
 * `42 in 0  45 in 0`.
 *
 * THE FIX IS THE BRIEF'S ALLOCATION ROUTE 5 -- lengthen the range so it
 * overlaps a hard register and it prunes its own preference.  ONE shared
 * local for both arms, DEFINED BEFORE THE SWITCH so its range covers the
 * prologue where r0 holds `id`:
 *
 *     t = -1;
 *     v8 = t;
 *     switch (id) { ... case 2: t = AllocSpriteSlot(); v8 = t; ... }
 *
 * Now `t` is a global allocno whose range reaches block 0, and `.18.greg`
 * reads `;; 36 conflicts: 32 33 36 0 13` with NO preferences line at all --
 * r0 entered `used`, so `global.c:1065-1097` and `:1100-1134` drop it from
 * BOTH `hard_reg_copy_preferences` and `hard_reg_preferences` (it is set in
 * both at `global.c:1631-1646`, which is why pruning only the copy set would
 * not be enough).  `t` then takes **r2** -- not `REG_ALLOC_ORDER`'s first free
 * r3 (`arm.h:989-994`), because `prune_preferences` (`global.c:897-909`)
 * penalises r3: the conflicting allocno 32 carries `;; 32 preferences: 3`.
 * The two `mov r2, r0` copies survive and the counts equalise.
 *
 * ORDER IS LOAD-BEARING: `t = -1; v8 = t;` reads 3 and `v8 = -1; t = -1;`
 * reads 42 with 45 instructions -- the lever is gone.
 *
 * === THE REMAINING THREE, AND WHAT THEY NEED ===
 *
 * Because the thing that keeps `t` live across block 0 IS the -1, `t` is also
 * the pseudo that materialises it, so the -1 lands in r2.  The ROM has the -1
 * in r3 (a separate, block-local pseudo) and the slot in r2.  So the ROM's
 * slot pseudo is live across block 0 while NOT being the -1, and no spelling
 * found here produces that.  That is the open question, and it is now a
 * one-register question on an equal-length stream.
 *
 * INSTRUMENT (not shipped, and the reason the above is certain): the park's
 * own body with one shared local PINNED to r2,
 * `register int t __asm__("r2");`, is BYTE-IDENTICAL -- 160 bytes, 64
 * encodings and 16 relocations identical, no flag.  So the entire residue of
 * the original 42 was one register-allocation choice on one pseudo.  Pin
 * policy: a pinned landing is not a landing, so the pinned figure 0 is
 * recorded here and the shipped body is device-free at 3.
 *
 * === MEASURED, batch 346 (objcmp ndiff of 64; instruction count in brackets) ===
 *   this body                                      3   [47]  <- current
 *   `int m; m = -1; t = m; v8 = m;`                 3   [47]
 *   t declared first / middle, extra `int m`        3   [47]
 *   `t = -1; v8 = -1;`                              3   [47]
 *   park body (v8 = -1, no shared t)               42   [45]
 *   `v8 = -1; t = -1;`  (order reversed)           42   [45]
 *   `t = 1; v8 = -t;`                              42   [45]
 *   `t = 0; v8 = ~t;`                              42   [45]
 *   `m = -1; v8 = m; t = m;`                       42   [45]
 *   shared `t` with no pre-switch definition       42   [45]  (park's own
 *     negative, CONFIRMED byte-identical to the park body)
 *   two distinct temps `t`,`u`                     42   [45]
 *   `if ((v8 = AllocSpriteSlot()) == 0x60)`        42   [45]
 *   `== 0x60u`; explicit `default:`; decl order    42   [45]
 *   inverted `if (v8 != 0x60) {...} return -1;`    42   [45]
 *   source case order 2,9,1/6,4 / 1/6,4,2,9        52 / 44
 *   `t = -1; v8 = t; v4 = 0;`                      60   [49]
 *
 * FLAG AXIS (autosweep, on the PARK body -- re-cross against THIS body):
 *   -O1 23 (aligned 17 -> 22, WORSE), -fno-gcse 42, -fno-schedule-insns2 52,
 *   -ffixed-r7 42, -fno-strict-aliasing 42, -fno-strength-reduce 42,
 *   -fno-rerun-loop-opt 42, and -fno-rerun-cse-after-loop **6 of 64 with
 *   equal length, aligned 17 -> 6**.  That flag reaches equal length a
 *   DIFFERENT way -- it stops cse2 folding the compare into the stored
 *   pseudo, so the RTL keeps `(set (reg 43) (mem sfp-4))`, pseudo 43 has no
 *   copy suggestion and takes REG_ALLOC_ORDER's r3, and post-reload CSE
 *   (`reload1.c reload_cse_regs`) rewrites the load as `mov r3, r0`.  It
 *   leaves `str r0` where the ROM has `str r2`, so it cannot reach 0 and is
 *   superseded by this body.  On THIS body the flag is inert (3 either way).
 * ANCHOR AXIS (autosweep): 15 of 15 boundaries inert, device-free.  CLOSED.
 *
 * The original prose park is preserved below.
 */
extern int AllocSpriteSlot(void);
extern void LoadOldUIIcon(int a, int b, int *c, int *d, int e);
extern void LoadInventoryIcon(int a, int b);
extern void LoadUIBanner(int a, int b);
extern void LoadMoveIcon(int a, int b, int *c, int *d, int e);

int UploadIcon(int id, int a)
{
    int v4;
    int v8;
    int t;

    t = -1;
    v8 = t;
    switch (id) {
    case 1:
    case 6:
        LoadOldUIIcon(a, 0, &v8, &v4, 0);
        break;
    case 2:
        t = AllocSpriteSlot();
        v8 = t;
        if (t == 0x60)
            return -1;
        LoadInventoryIcon(a, 0x1a);
        break;
    case 9:
        t = AllocSpriteSlot();
        v8 = t;
        if (t == 0x60)
            return -1;
        LoadUIBanner(a, 0);
        break;
    case 4:
        LoadMoveIcon(a, 1, &v8, &v4, 0);
        break;
    }
    return v8;
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* UploadIcon -- 0x0801bc34, asm/rom_15000/rom_1aeec_a_a_c_c.s
 *
 * 69 lines against the ROM's 71.  Candidate at scratch/Nicon_best.c.
 * The 38 differing is a two-line shift cascading; the real residue is TWO
 * INSTRUCTIONS, one at each of two sites.
 *
 * SOLVED, and this is the part worth keeping: THE JUMP TABLE REPRODUCES FROM A
 * PLAIN SWITCH.  The ROM dispatches with `sub r0, #1 / cmp r0, #8 / bhi / lsl
 * r3, r0, #2 / ldr r3, [r3, r2] / mov pc, r3` over a nine-entry table, five of
 * whose slots point at the default.  Written as `switch (id)` with cases 1, 2,
 * 4, 6 and 9 -- gcc does the biasing itself -- the table, its entry order and
 * the whole dispatch come out exact, and so do the first 32 lines.  Four
 * elevated functions in the tree already contain a jump table, so the shape was
 * known reachable; this confirms the ordinary spelling is what reaches it.
 *
 * BLOCKER: the ROM copies the AllocSpriteSlot result into a register before
 * using it, and we use it in place.
 *      rom   bl AllocSpriteSlot / mov r2, r0 / str r2, [sp, #8] / cmp r2, #0x60
 *      ours  bl AllocSpriteSlot /              str r0, [sp, #8] / cmp r0, #0x60
 * Ours is one instruction SHORTER at each of the two sites.  `v8` has its
 * address taken -- the other two arms pass `&v8` -- so it lives in the frame,
 * and gcc stores the return value and compares it straight out of r0.
 *   [BATCH 346: the observation is right, the verdict is refuted -- see above.]
 *
 * TRIED: a separate `t = AllocSpriteSlot(); v8 = t; if (t == 0x60)` at both
 * sites, which is the obvious way to ask for the register copy.  Byte-identical
 * to the original -- gcc knows t and v8 hold the same value and folds the copy
 * away.  Nothing in the source distinguishes "store it" from "keep a copy and
 * store that" when the copy has no other use.
 *   [BATCH 346: re-measured, CONFIRMED byte-identical -- but the missing half
 *    was DEFINING that shared local BEFORE the switch, which is 42 -> 3.]
 *
 * Also settled: the two `return -1` paths cross-jump into one shared exit that
 * skips the trailing `ldr r0, [sp, #8]`, which is what writing them as two
 * ordinary `return -1;` statements produces.
 *)

*/
#endif
