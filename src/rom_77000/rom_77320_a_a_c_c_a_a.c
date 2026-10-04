/* Func_8077348  --  0x08077348
 *
 * ===== BATCH 322g -- BYTE-IDENTICAL.  12 of 34 -> 0 of 34. =====
 * 76 bytes, 34 encodings and 4 relocations identical.  ZERO PINS, ZERO SHIMS.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_77000/rom_77320_a_a_c_c_a_a.c \
 *     asm/rom_77000/rom_77320_a_a_c_c_a_a.s --whole
 *
 * LANDING SHAPE: WHOLE FILE, NO SPLIT.  asm/rom_77000/rom_77320_a_a_c_c_a_a.s
 * holds exactly one `.thumb_func_start` (Func_8077348) and
 * tools/datacheck.py prints nothing -- no data section.  The split that made
 * this possible was done in batch 316c, which cut Func_8077348 into _a.s and
 * GetUnit into _b.s; _b.s is already src/rom_77000/rom_77320_a_a_c_c_a_b.c.
 * So split_s.py is NOT needed again -- installed path is
 * src/rom_77000/rom_77320_a_a_c_c_a_a.c, stage1.ld unchanged (it already
 * names rom_77320_a_a_c_c_a_a.o), no new export.
 *
 * ---------------------------------------------------------------------------
 * THE EDIT IS ONE STATEMENT MOVED: `sum = 0;` GOES BEFORE `n = GetPartySize();`
 *
 * The park's body had
 *      n = GetPartySize();
 *      sum = 0;
 * and this has
 *      sum = 0;
 *      n = GetPartySize();
 * Nothing else differs.  Semantically identical; worth all twelve encodings.
 *
 * THE PARK'S OBSERVATIONS WERE RIGHT AND ITS VERDICT WAS WRONG, as usual.
 *
 * It said: "REGISTER-ROLE SWAP across the loop's call ... rom r7 = n, the
 * walking pointer spilled to [sp]; ours r7 = the walking pointer, n spilled to
 * [sp]", and "NEXT: nothing source-level outstanding.  This wants whatever
 * cracks the register-role-swap class."
 *
 * The swap is real and the decomposition below confirms it exactly.  The
 * verdict -- that nothing source-level is outstanding -- is false, and the
 * reason is that the park never read .17.lreg/.18.greg.  The swap is decided by
 * ONE allocno-priority comparison with a 0.8% MARGIN, and a single statement
 * reorder moves it.
 *
 * DECOMPOSITION OF THE 12 (counts 34/34, so every index is a distance):
 *
 *   SEVEN indices are the role swap itself, r7 <-> r2:
 *     3  adds r7,r0,#0 / adds r2,r0,#0      14 adds r5,r7,#0 / adds r5,r2,#0
 *     6  cmp  r7,#0    / cmp  r2,#0         15 ldrb r0,[r2]  / ldrb r0,[r7]
 *     8  cmp  r6,r7    / cmp  r6,r2         26 adds r1,r7,#0 / adds r1,r2,#0
 *     13 adds r2,r3,r1 / adds r7,r3,r1
 *
 *   FIVE indices (16..20) are ONE instruction in a different slot, and they are
 *   DOWNSTREAM of the swap -- the park called this correctly.  The ROM has
 *        ldrb r0,[r2] / adds r2,#1 / str r2,[sp] / bl GetUnit
 *   and ours has the `adds` three slots later, after `subs r5,#1`.  Cause:
 *   `str r2,[sp]` is not a spill, it is caller-save.c's SAVE of a
 *   call-clobbered hard register around `bl GetUnit`, inserted by reload AFTER
 *   the register choice.  It depends on `adds r2,#1`, so sched2 cannot sink the
 *   increment past it.  When the pointer instead gets the CALLEE-SAVED r7 there
 *   is no save, the increment has no successor in the block, and sched2 puts it
 *   with the other end-of-loop IV updates.  Fix the register and the five
 *   indices fix themselves.  There is no second cause.
 *
 * ---------------------------------------------------------------------------
 * THE DECIDING RUNG, WITH THE NUMBERS.  global.c's allocno_compare.
 *
 * `.18.greg` prints `;; 4 regs to allocate:` -- N != 0, so GLOBAL-alloc decided
 * it and the priority formula DOES carry floor_log2 (batch 321 correction 1):
 *
 *      prio = floor_log2 (n_refs) * n_refs / live_length      (* 10000 * size)
 *
 * Four allocnos cross the call and there are three callee-saved registers
 * (r5, r6, r7 -- r4 is call-USED under -fcall-used-r4), so the fourth-ranked
 * allocno is the one that gets a call-clobbered register and a caller-save
 * pair.  From .17.lreg:
 *
 *   PARK          refs  live  floor_log2  prio      got
 *     34 count       7    11      2       1.2727    r5
 *     33 sum         9    38      3       0.7105    r6
 *     48 pointer     7    24      2       0.5833    r7   <-- 3rd
 *     32 n           5    18      2       0.5556    r2   <-- 4th, caller-saved
 *   greg order: `34 33 48 32`.
 *
 *   THIS FILE
 *     34 count       7    11      2       1.2727    r5
 *     33 sum         9    42      3       0.6429    r6
 *     32 n           5    17      2       0.5882    r7   <-- 3rd
 *     48 pointer     7    24      2       0.5833    r2   <-- 4th, caller-saved
 *   greg order: `34 33 32 48`.
 *
 * `sum = 0;` before the call moves `mov r6,#0` OUT of n's live range, so n goes
 * from 18 insns to 17 and its priority from 0.5556 to 0.5882 -- which clears
 * the pointer's 0.5833 by 0.8%.  That is the whole function.  `sum` pays for it
 * by crossing two calls instead of one (live 38 -> 42, prio 0.7105 -> 0.6429),
 * which costs nothing because it keeps r6 either way.
 *
 * THE GENERAL LEVER, and it is cheap enough to try first every time:
 *
 *   > TO RE-RANK TWO CALL-CROSSING ALLOCNOS, MOVE A STATEMENT ACROSS THE CALL
 *   > THAT BIRTHS ONE OF THEM.  An initialisation hoisted above the call that
 *   > defines its rival shortens the RIVAL'S live_length by one insn and
 *   > lengthens its own -- a one-insn change to the denominator of a formula
 *   > whose margins here are under one percent.
 *
 * This is the same family as batch 316c's GetUnit (its file-mate, out of the
 * same original .s), where the cure was also "make one side ineligible" -- but
 * a rung up: GetUnit's was local-alloc's dest/dying-source combine, this is
 * global-alloc's priority order.  Neither is a tie-break; both are arithmetic
 * with a readable margin.  Read `;; N regs to allocate:` to know which.
 *
 * MEASURED, ALL SIX VARIANTS, screened on .17.lreg numbers as well as figures:
 *   sum = 0 before the call (this file)            34/34, 0   <-- LANDS
 *   park base (sum = 0 after the call)             34/34, 12
 *   `return sum / n;` instead of `sum = sum / n`   34/34, 17  (worse: sum's
 *        refs drop 9->7, prio 0.7105->0.4118, so sum falls BELOW the pointer
 *        and the order becomes 34 48 32 33 -- the right idea on the wrong reg)
 *   explicit `p` + `for`, `GetUnit(*p++)`          32/34, 27  (loses 2 insns:
 *        a user-var pointer is not strength-reduced, so the countdown r5 and
 *        the pointer collapse into one IV.  Confirms the park's reading that
 *        only the INDEX form keeps two induction variables.)
 *   same, `p` declared first                       32/34, 27  (declaration
 *        order renumbers the pseudos and NOTHING else -- inert, a free
 *        code-quality dividend, not a lever)
 *   same, `*p` and `p++` as separate statements    32/34, 27
 *   same, `sum +=` instead of `sum = sum +`        32/34, 27
 *
 * WHAT THE PARK GOT RIGHT AND IS KEPT HERE: the two separate guards
 * (`if (n == 0) return 0;` then `for (i = 0; i < n; i++)`, which gives the
 * ROM's `cmp r7,#0 / beq` followed by `cmp r6,r7 / bge` reusing the
 * accumulator's zero), the index form over `gState[K + i]`, and assigning the
 * quotient back to the accumulator before returning it.  All verified
 * unchanged.
 *
 * -- scratch_elev/b322/G, v2_C_sum_first.c
 */

/* Func_8077348  --  0x08077348
 *
 * Mean of byte 0xf across the active party: walks the party-member id table at
 * gState + 0x1f8, maps each id to its unit record with GetUnit, sums one byte
 * out of each record and divides by the party size.  Returns 0 for an empty
 * party rather than dividing by zero.
 */

extern int GetPartySize(void);
extern unsigned char *GetUnit(int id);
extern unsigned char gState[];

int Func_8077348(void)
{
    int n;
    int sum;
    int i;

    sum = 0;
    n = GetPartySize();
    if (n == 0) {
        return 0;
    }
    for (i = 0; i < n; i++) {
        sum = sum + GetUnit(gState[(0xfc << 1) + i])[0xf];
    }
    sum = sum / n;
    return sum;
}
