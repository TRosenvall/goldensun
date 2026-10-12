/* Func_80f0614 (FeedNextCreditLine) -- 0x080f0614
 * asm/rom_f0000/rom_f0254_c_c_a.s
 *
 * 17 of 45 stream encodings, AND THE INSTRUCTION COUNTS NOW AGREE: 40 against
 * the ROM's 40, 96 bytes against 96, relocations IDENTICAL.  The figure is a
 * DISTANCE for the first time.  It was 38 of 45 at 39/40 instructions -- ONE
 * SHORT -- with the relocations shifted, from batch 319 to batch 343.
 * Pin-free and device-free (`tools/shimcount.py` prints no rows).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_f0000/80f0614.c \
 *     asm/rom_f0000/rom_f0254_c_c_a.s --func Func_80f0614
 *   ... and tools/aligncmp.py with the same three arguments plus -v.
 *
 * Returns early if a status halfword is non-zero; divides two signed halfwords
 * by 8 and returns early if they agree; otherwise copies one to the other,
 * indexes a table, and stores the result of Func_80f07f0.
 *
 * STILL TRUE AND CARRIED FORWARD: ewram_2004c00 is loaded BOTH signed and
 * unsigned from the same address -- `ldrsh` for the division and `ldrh` for the
 * halfword copy -- which is the documented `short` rule, and one `short`
 * declaration gives both.  Also, r4 is CALL-USED in this tree's build, so the
 * reference writing r4 without pushing it says nothing about the source.
 *
 * ============================================================
 * BATCH 344, BRIEF B.  THE GAP IS CLOSED AND THE PARK'S BOUND IS REFUTED.
 *
 * GAP ARITHMETIC (16-bit instructions; 5/5 32-bit entries = 4 pool words plus
 * one `bl`).  No loop, so the regions are the guard, each divide, and the tail:
 *
 *     region                                    ref   old park   this body
 *     prologue + first guard  push .. `bne`       6       6          6
 *     FIRST divide  `ldr r3,=c00` .. `add #7`     8       7 (-1)     8
 *     second divide + compare .. `beq`           10      10         10
 *     tail  `ldr r2,=.Lf1220` .. `strh r0,[r5]`  13      13         13
 *     epilogue                                    3       3          3
 *     TOTAL                                      40      39         40
 *
 * The whole -1 was ONE instruction in the FIRST divide: the ROM's `mov r3, r2`,
 * the copy that `expand_divmod` makes of the dividend before biasing it.
 *
 *     ROM    mov r3,r2 | cmp r2,#0 | bge | add r3,r2,#7 | ... | asr r1,r3,#3
 *     park   cmp r2,#0 | bge | add r2,#7 | asr r0,r2,#3
 *
 * ---- THE PARK'S DIAGNOSIS WAS RIGHT AND ITS BOUND WAS WRONG ----
 *
 * The park had the pass right -- "it is lost to cse, three passes earlier" --
 * and then stated the bound: *"WHAT WOULD KEEP IT: a SECOND USE of the
 * sign-extended value, so that cse cannot make the copy dead.  This function
 * has none to give"* and *"NEXT: nothing at the expression level."*  **That is
 * refuted: no second use is needed.**
 *
 * THE TRANSFORMATION IS cse.c:5960-6005, and it is not a liveness rule.  Its
 * own comment is "Special handling for (set REG0 REG1) where REG0 is the
 * cheapest ... change this insn to (set REG1 REG0) and replace REG1 with REG0
 * in the previous insn that computed their value."  The gate at cse.c:5992-5994
 * is the one that matters:
 *
 *     rtx prev = prev_nonnote_insn (insn);
 *     if (prev != 0 && GET_CODE (prev) == INSN
 *         && GET_CODE (PATTERN (prev)) == SET
 *         && SET_DEST (PATTERN (prev)) == SET_SRC (sets[0].rtl))
 *
 * **The insn IMMEDIATELY BEFORE the copy must be the very insn that set the
 * dividend.**  It is a textual-adjacency test on the insn chain, nothing more.
 * The dumps show it firing exactly: at `.00.rtl` insn 26 is
 * `(set (reg:SI 41) (ashiftrt (reg:SI 43) 16))` with
 * `REG_EQUAL (sign_extend (mem:HI ...))` and insn 28 is `(set (reg:SI 44)
 * (reg:SI 41))`; at `.03.cse` the two DESTINATIONS HAVE SWAPPED -- insn 26 now
 * sets reg44 and insn 28 is `(set (reg:SI 41) (reg:SI 44))` with reg41 used
 * nowhere but a REG_EQUAL note.  The copy is then a dead store and flow deletes
 * it (reg-to-reg copies in the dump go 7 at `.02.jump` -> 5 at `.03.cse` -> 3
 * at `.12.life`).
 *
 * In the ROM the SAME transformation half-fired: its `cmp` and its
 * `add r3,r2,#7` both READ the dividend (cse canonicalised the reads), but the
 * add's DEST and the `asr r1,r3,#3` are still the copy -- the shift is in the
 * join block, a new extended basic block, so cse could not reach it.  That is
 * only consistent with the copy insn having survived, i.e. with the gate above
 * having failed in the ROM's compile.
 *
 *   >> THE FIX IS ONE STATEMENT: name the dividend in a local so the divide's
 *      op0 is ALREADY in a pseudo and `prev_nonnote_insn` of the copy is the
 *      previous statement's last insn instead of the sign-extension.
 *
 *          t = ewram_2004c00;
 *          a = t / 8;
 *
 *      38 of 45 at 39/40 -> **23 of 45 at 40/40, relocations clean**. <<
 *
 * AND THE PARK'S OWN MEASUREMENT OF THAT SPELLING IS WRONG.  Its table records
 * "the dividend named in an `int` local  44.4 [39]" -- 39 instructions, i.e.
 * still short.  Re-measured here it is 40/40 and 23 of 45.  The difference is
 * that `t` must be the SAME local that later carries `(a + 0x10) & 0x1f`; a
 * fresh name for the dividend measures 38 of 45 at 39/40 (that row is in the
 * inert list below).  So the park's row was measuring a different body than its
 * wording suggests, and the recorded negative cost this park ten batches.
 *
 * ---- SECOND EDIT: THE TAIL, 23 -> 17 ----
 *
 * The ROM loads the table entry BEFORE it builds the ring index:
 *
 *     ROM   ldr r2,=.Lf1220 | lsl r3,r1,#2 | strh r4,[r0] | ldr r0,[r2,r3]
 *           | mov r3,r1 | mov r2,#0x1f | add r3,#0x10 | and r3,r2
 *     23    mov r4,r1 | mov r3,#0x1f | add r4,#0x10 | and r4,r3
 *           | strh r0,[r2] | ldr r2,=.Lf1220 | ...
 *
 * Naming the table entry in its OWN local, assigned before the mask, fixes the
 * order:  `s = Lf1220[a]; t = (a + 0x10) & 0x1f;` then
 * `Func_80f07f0(s, t * 24, 1)`.  23 -> **17 of 45**.  Inlining the mask into
 * the call argument instead goes the wrong way (36 of 45 at gap -1 -- it loses
 * the dividend copy again, because the argument expansion changes what
 * `prev_nonnote_insn` sees).
 *
 * ---- WHAT THE REMAINING 17 IS: ONE ALLOCATION, THREE REGISTER PAIRS ----
 *
 * Every remaining encoding is a register NAME, and the aligned hunks are now
 * pure replaces of equal length.  The ROM/ours pairs are
 * r2/r4 (the zero for the register-offset `ldrsh`), r0/r2 (&ewram_2004c08),
 * r2/r3 and r3/r4 in the ring-index build -- plus `push {r5, r6, lr}` against
 * `push {r5, lr}` and the matching `pop`, 2 of the 17.
 *
 * The push mask is the handle.  The ROM spends a CALLEE-SAVED register (r6) on
 * the zero that the register-offset `ldrsh` needs, twice, and that value never
 * crosses the call -- so the ROM's compile had every call-used low register
 * (r0-r3, and r4 because this tree builds with -fcall-used-r4) busy at that
 * point and ours does not.  The park's reading that "the push mask is a symptom
 * of the copy, not a second blocker" is now TESTABLE AND FALSE: the copy is
 * present in this body and the push mask is still {r5}.  So there is a second,
 * independent pressure difference, and it is the next thing to find.
 *
 * MEASURED INERT ON THIS BODY, with figures (all at 40/40, relocations clean,
 * unless noted):
 *   all NINE anchor placements .............................. 17 (two of the
 *     nine land after a `return` and produce nonsense rows -- 45 of 45 at a
 *     truncated body; they are not variants and were discarded)
 *   ALL EIGHT per-file flag groups .......................... 17, except
 *     -fno-schedule-insns2 18 and -O1 43 at gap +2
 *   the second dividend named in its own local .............. 17
 *   the store's halfword named and stored from the name ..... 17
 *   the store moved after the table load .................... 17
 *   the store moved after the mask .......................... 17
 *   the mask written as `t = a + 0x10; t &= 0x1f;` .......... 17
 *   the mask written as three statements .................... 17
 *   `t = t * 24;` hoisted out of the call ................... 17
 *   a fresh name for the mask (`s`), dividend in `t` ........ 38, gap -1
 *   the mask inlined into the call argument ................. 36, gap -1
 *   the unsigned halfword read placed BETWEEN the dividend
 *     and the divide (`u = ewram_2004c00;`) ................. 38, gap -1
 *     -- NOTE: this is the one that SHOULD have worked by the cse.c gate and
 *     does not, because gcc expands the two reads of one address into ONE
 *     HImode load and the sign-extension still ends up adjacent to the copy.
 *   the second dividend loaded between ...................... 42, gap +1
 *   both dividends named, second divide first ............... 30
 *
 * BOTH CHEAP AXES WERE CLOSED ON THE OLD BODY FIRST (tools/autosweep.py): all
 * eight flag groups at 38 or 43, all 8 anchor placements at 38.
 */
extern short ewram_2004c00;
extern short ewram_2004c04;
extern short ewram_2004c08;
extern int Lf1220[] __asm__(".Lf1220");
extern short Func_80f07f0(int a, int b, int c);

void Func_80f0614(void)
{
    int a;
    int b;
    int t;
    int s;

    if (ewram_2004c04 != 0)
        return;
    t = ewram_2004c00;
    a = t / 8;
    b = ewram_2004c08 / 8;
    if (a == b)
        return;
    ewram_2004c08 = ewram_2004c00;
    s = Lf1220[a];
    t = (a + 0x10) & 0x1f;
    ewram_2004c04 = Func_80f07f0(s, t * 24, 1);
}
