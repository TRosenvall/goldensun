/* Task_SpinCamera -- 0x080d6504, 37 ROM instructions (41 encodings).
 *
 * NON-MATCHING, 2 differing encodings of 41.   [batch 327B: was 18]
 *
 * MEASUREMENT.  SIZE IS EXACT (objcmp prints no SIZE line), the instruction
 * COUNT IS EXACT (41 / 41) and RELOCATIONS ARE IDENTICAL (no RELOCATIONS
 * line), so the 2 is a TRUE DISTANCE.  Confirmed with --whole as well as
 * --func: "Task_SpinCamera  2 of 41 differ (ours 41), first at index 16".
 * aligncmp: 40 of 41 aligned-equal (97.6%), 2 differing in 2 hunks.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_c9000/80d6504.c asm/rom_c9000/rom_d6504_a_a.s --func Task_SpinCamera
 *
 * SHIMS: NONE.  tools/shimcount.py is clean.  No register pin, no barrier, no
 * per-file flag override, no fakematch.txt row, no .equ, no fictitious symbol.
 * The figure above is a PRODUCTION-FLAG figure.  The one-member `union Word`
 * is the alias escape docs/elevation.md documents at length ("A one-member
 * UNION is a per-MEM alias escape, and it beats -fno-strict-aliasing", and the
 * controls table under "A UNION MEMBER ACCESS IS ALIAS SET 0, AND THE MEMBER
 * LIST IS IRRELEVANT"): it has NO unread member, so it is not the never-read
 * -union-member device.  UNION-FREE THE BODY READS 15 -- see the ladder below.
 *
 * SPLIT SHAPE: none needed.  asm/rom_c9000/rom_d6504_a_a.s holds exactly ONE
 * function (`grep -c func_start` = 1), so elevating this is a file rename with
 * no split and no exports.
 *
 * ================================================================
 * BATCH 327B -- 18 -> 2, AND THREE OF THE PARK'S CLAIMS WERE WRONG
 * ================================================================
 *
 * WRONG CLAIM 1, AND IT WAS A BOUND: "`lang_get_alias_set` hands back 0 in
 * exactly two reachable cases -- a COMPONENT_REF taken DIRECTLY through a
 * UNION_TYPE (:3344), and any reference of char precision (:3348) -- and an
 * `int`-width field can be neither, so THERE IS NO SOURCE SPELLING THAT
 * REACHES ALIAS SET 0 HERE."
 *
 * An int-width field CAN be a union member.  `union { int i; }` and
 * `union { int i; unsigned short h[2]; }` measure IDENTICALLY (12 / 10 / 7 / 2
 * across the four bodies below), which is the member-list-is-irrelevant result
 * docs/elevation.md already records with a struct control.  The union
 * reproduces `-fno-strict-aliasing` EXACTLY on this function: the park records
 * 12 for this body and 10 for its alternative WITH the flag, and the union
 * gives 12 and 10 WITHOUT it.  So the blocker was never TU-wide and the
 * ALIAS_CFLAGS request this park raised is MOOT -- there is no owner-facing
 * flag decision here any more.
 *
 * WRONG CLAIM 2: the park's "WHAT THE REMAINING 18 IS -- TWO CAUSES ...
 * A. `view` and `mode` over r0/r1" describes a body that was not installed.
 * Measured, the 18 has NO r0/r1 swap at all -- ref and ours both address the
 * mode word as `[r0,#0]`.  All 7 of its aligncmp hunks trace to the ONE
 * aliasing defect: the commoned re-read forces the mode value into
 * callee-saved r4, which forces `push {r5,lr}` / `pop {r5}` and rotates
 * r4 -> r5.  The r0/r1 swap only EXISTS once the aliasing is fixed; it is the
 * with-union body's residue, not the 18's.
 *
 * WRONG CLAIM 3, and it is the one that paid: "the POSITION wants the
 * statement after the y update, the cross-jump wants it before ... Give the
 * outer zero r2 and the statement can go back to its natural place."  The
 * position and the register are the SAME FACT and neither is the lever.  The
 * lever is CONSUMING `amt` INTO A NAMED TEMP so that r2 is free at the moment
 * the zero is materialised:
 *
 *     int amt = *(int *)(st + 0x77ac);
 *     int yv  = *(unsigned short *)(view + 0x36) + amt;   // amt dies HERE
 *     mode->i = 0;                                        // zero takes r2
 *     *(unsigned short *)(view + 0x36) = yv;
 *
 * The ROM's shape is `ldrh r3 / add r3,r2 / mov r2,#0 / strh r3 / str r2,[r0]`
 * -- the zero is materialised AFTER the add consumes amt, which is why it gets
 * r2, and a different value register is ALSO why it does not cross-jump onto
 * the inner shared `str r3,[r0]`.  Cross-jumping needs TWO MATCHING INSNS
 * (jump.c:675), so the register and the merge are one question.
 *
 * THE LADDER, all production flags, pin-free, 41 = 41 encodings both sides:
 *
 *     body                                   union-free   one-member union
 *     ------------------------------------   ----------   ----------------
 *     installed (view-first, no temp)            18             12
 *     mode-first, no temp (the park's alt)       23             10
 *     mode-first + yv temp                       20              7
 *     view-first + yv temp   <-- THIS BODY       15          **  2 **
 *     view-first + yv temp, no `amt` local       28             24 (1 insn LONG)
 *
 * So the two levers are INDEPENDENT and MULTIPLICATIVE, and neither is visible
 * alone: the temp alone is 18 -> 15, the union alone is 18 -> 12, together 2.
 * The park's lever (4) ("the one that TRADES: it fixes the head completely but
 * exchanges r0/r1") is ALSO a missing-prerequisite row -- with the temp in,
 * view-first gives the ROM's head AND the right r0/r1, and there is no trade.
 *
 * ================================================================
 * THE REMAINING 2 -- ONE SLOT, AND IT IS A FIXED POINT
 * ================================================================
 *   ours  mov r2,#0 | str r2,[r0] | strh r3,[r1,#0x36]
 *   ref   mov r2,#0 | strh r3,[r1,#0x36] | str r2,[r0]
 * The mode store is alias set 0, so it conflicts with everything and sched2
 * CANNOT reorder it against the halfword store -- source order decides.  But
 * writing the strh first puts the mode store LAST in the block, where
 * cross-jumping merges it onto the inner shared `str` and the body comes out
 * 2 INSTRUCTIONS SHORT.  Position -> register -> cross-jump -> position.
 *
 * SEVEN CROSSES MEASURED AGAINST IT, every head order, both inner-arm orders,
 * with and without the union prerequisite, with and without the temp -- all
 * 39 instructions against 40, i.e. 2 SHORT:
 *   mode store after the y update x {view-first, mode-first}
 *     x {arms normal, arms swapped} x {no union, union}           2 SHORT (x6)
 *   the same with the yv temp and the union (`sc_w14`)            2 SHORT
 * ALSO MEASURED, 41 instructions but worse: a named `unsigned short *yp` for
 * the y word, store last 35, store before the mode store 27.
 *
 * MEASURED INERT (this batch, at this body's figure):
 *   - the inner arms written `if (mode->i != 2) mode->i = 2; else mode->i = 0;`
 *     -- BUT NOTE: union-free and temp-free that variant ALSO reads 18 while
 *     emitting `beq` + `mov #2` first against the ROM's `mov #0` first.  It is
 *     a SECOND BODY AT THE SAME FIGURE and the WORSE corner; do not start from
 *     it (batch 326's `InitMapActors` warning, reproduced here).
 *   - a named `int off = 0x77b0;` for the offset (10 against 10 with the union)
 * MEASURED WORSE (this batch):
 *   - `mode = (Word *)st;` then re-assigned after `view`   1 insn LONG, 34
 *   - `view`'s load before `st`                            2 SHORT, 40
 *
 * ================================================================
 * KEPT FROM THE PARK, STILL RIGHT
 * ================================================================
 *   (1) the two-store inner update, not a temp -- blocks ifcvt.  `noce_*` only
 *       handles a REGISTER destination, so two direct stores cannot be
 *       if-converted and cross-jumping merges them onto the ROM's one shared
 *       `str` (ref idx 31-34).
 *   (2) `amt` declared BLOCK-SCOPED in each branch.
 *   (3) the second global reached as an offset FROM THE FIRST SYMBOL'S ADDRESS,
 *       which is why the head is `ldr r3,=iwram_3001eec / ldr r2,[r3] /
 *       sub r3,#0x6c / ldr r1,[r3]` and not two pool loads.
 * The signed halving (ref idx 22-27) is EXACT and needed no work.
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
        mode->i = 0;
        *(unsigned short *)(view + 0x36) = yv;
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
