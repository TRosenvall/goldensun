/* Task_SpinCamera @ 0x080d6504  [asm/rom_c9000/rom_d6504_a_a.s]
 * Byte-identical.  88 bytes, 41 encodings, 1 relocation.  PIN-FREE, DEVICE-FREE,
 * no per-file flag, no fakematch row.  The reference .s holds exactly one
 * function and no data, so this is a whole-file conversion with no split.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_c9000/rom_d6504_a_a.c asm/rom_c9000/rom_d6504_a_a.s --whole
 *
 * Annotation ported from the reference .s:
 *   StepScreenShake.  Takes no arguments.  Advances the battle screen shake one
 *   frame using the amount at [iwram_1eec]+0x77AC and the mode at +0x77B0,
 *   adding the offset into the view's y at [iwram_1e80]+0x36.  Mode 1 applies
 *   the full amount once and then clears the mode -- a single jolt.  Any other
 *   mode applies half the amount and alternates between 2 and 0, giving the
 *   oscillating shake.
 *
 * ================= HOW THE LAST TWO ENCODINGS CLOSED (batch 330 B) ============
 * The park's residue was ONE ADJACENT TRANSPOSITION at the end of the mode-1 arm:
 *     ours  mov r2,#0 | str r2,[r0] | strh r3,[r1,#0x36]
 *     ref   mov r2,#0 | strh r3,[r1,#0x36] | str r2,[r0]
 * Everything else -- the negative-offset global head, the signed halving, the
 * two-store inner update, the cross-jumped shared `str` -- was already exact.
 *
 * THE FIX: write the mode store LAST in source order, and materialise its zero
 * into a NAMED LOCAL one statement earlier (`int z = 0;` placed after `yv`).
 * Two things fall out of that one edit:
 *   - cse cannot propagate the 0 into the store, because a thumb store needs a
 *     register operand, so the `mov #0` insn STAYS WHERE IT IS WRITTEN.  The
 *     zero pseudo is therefore born BEFORE the halfword store and overlaps
 *     `yv`, which holds r3; r3 is first in REG_ALLOC_ORDER (arm.h:989,
 *     {3,2,1,0,12,14,...}) but unavailable, so the zero takes r2.
 *   - with the if-arm's store spelled `str r2,[r0]` it no longer matches the
 *     else-arm's shared `str r3,[r0]`, so the post-sched2 cross-jump pass
 *     (toplev.c:3515, which runs AFTER sched2 at toplev.c:3481) cannot merge
 *     them.  That merge is why every earlier store-last variant came out short.
 * DECLARATION POSITION IS THE LEVER, not the mere existence of the local:
 *     `int z = 0;` after  `yv`   ->  0  (this body)
 *     `int z = 0;` before `yv`   -> 19 of 41
 * A function-scope `int z;` assigned in the same place is ALSO byte-identical,
 * so no dominating-block rule is needed here; the block-scoped form is kept
 * because it matches the two `amt` locals the park already established.
 *
 * ===== THE PARK'S STATED BLOCKER WAS REFUTED, AND HOW TO RE-CHECK IT =====
 * The park wrote: "The mode store is alias set 0, so it conflicts with
 * everything and sched2 CANNOT reorder it against the halfword store -- source
 * order decides."  The conclusion is right and the reason is wrong, which is
 * why seven crosses against it all failed.  MEASURED: a body that keeps the
 * one-member union for the two mode READS but writes all three mode STORES
 * through a plain `int *` -- so the stores no longer conflict with the
 * `unsigned short` store at all -- compiles to an object BYTE-IDENTICAL to the
 * park's, same two differing encodings at the same index.  Removing the
 * conflict changes nothing.
 *
 * What actually fixed the order is `rank_for_schedule`'s LAST rung,
 * `return INSN_LUID (tmp) - INSN_LUID (tmp2);` (haifa-sched.c:4113-4116, comment
 * "so that we make the sort stable.  This minimizes instruction movement, thus
 * minimizing sched's effect on debugging and cross-jumping") -- i.e. plain
 * source order, reached because every rung above it ties:
 *   - the register-pressure rung (haifa-sched.c:4046-4048) is guarded by
 *     `!reload_completed` AND THIS BUILD NEVER RUNS sched1.  Compiling any file
 *     here with `-da` writes `.23.sched2` and no sched1 dump at all, so the only
 *     scheduling pass is post-reload and that rung is dead code for this port.
 *     Any plan that reasons about INSN_REG_WEIGHT is reasoning about a pass that
 *     does not run.
 *   - both stores are the last insns of their block, so the priority rung
 *     (:4040-4043) ties at 0 and the dependent-count rung (:4098-4110) ties at 0;
 *   - the last-scheduled-insn class rung (:4069-4096) puts both in class 3,
 *     because the `mov`->`str` link costs 1 (`link == 0 || insn_cost (...) == 1`).
 * COROLLARY, and it generalises: with no sched1, REGISTER ALLOCATION SEES SOURCE
 * ORDER, so in this port "which register does this value get" and "where do I
 * write the statement" are the same question.
 *
 * The one-member `union Word` is the per-MEM alias escape docs/elevation.md
 * documents; it has no unread member, so it is not a device.  It is still
 * REQUIRED: it is what stops gcc commoning the else-arm's re-read of the mode
 * word with the entry read across the halfword store.
 */
extern char *iwram_3001eec;

/* A ONE-MEMBER union: lang_get_alias_set (c-common.c:3329-3345) returns alias
 * set 0 for a COMPONENT_REF taken directly through a UNION_TYPE, which is what
 * keeps the two reads of the mode word apart across the halfword store.  No
 * unread member, so nothing here is a device -- see docs/elevation.md,
 * "A one-member UNION is a per-MEM alias escape". */
typedef union { int i; } Word;

void Task_SpinCamera(void)
{
    char *st;
    char *view;
    Word *mode;
    st = iwram_3001eec;
    view = *(char **)((char *)&iwram_3001eec - 0x6c);
    mode = (Word *)(st + 0x77b0);
    if (mode->i == 1) {
        int amt = *(int *)(st + 0x77ac);
        int yv = *(unsigned short *)(view + 0x36) + amt;
        int z = 0;
        *(unsigned short *)(view + 0x36) = yv;
        mode->i = z;
    } else {
        int amt = *(int *)(st + 0x77ac);
        amt = amt / 2;
        *(unsigned short *)(view + 0x36) = *(unsigned short *)(view + 0x36) + amt;
        if (mode->i == 2)
            mode->i = 0;
        else
            mode->i = 2;
    }
}
