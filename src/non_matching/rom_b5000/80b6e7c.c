/* GetWeaponType  --  NOT MATCHING
 *
 * NON-MATCHING, 26 of 26 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 26, ours 22) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   RELOCATIONS differ, but THE SAME SYMBOLS AT A SHIFTED OFFSET -- which
 *   this project treats as a CONSEQUENCE of the length difference, not a
 *   separate blocker.  Batch 322's re-classification of this figure is WITHDRAWN. *** THAT CLAIM WAS FALSE HERE. *** Re-measured by the coordinator after batch 348: ref 24 / ours 20 16-bit instructions, a gap of -4, so this figure is a POSITIONAL count measuring PHASE, not distance. Batch 322's re-classification sweep made this claim on 54 parks and was WRONG ON 34 OF THEM, this one included; the 18 it got right keep the claim. Close the length gap before reading the figure, then rank on tools/aligncmp.py rather than on the raw count.
 *   SIZE ref 56 bytes, ours 48.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b5000/80b6e7c.c \
 *     asm/rom_b5000/rom_b6e7c.s --func GetWeaponType
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 *
 * Source asm: goldensun/asm/rom_b5000/rom_b6e7c.s
 * Best screen: 18 instructions in disagreeing regions, of 27 (rom 27, ours 23).
 *
 * BLOCKER CLASS: the index is strength-reduced away.
 *
 * The ROM keeps a counter and recomputes the byte offset each iteration:
 *
 *      lsl r1, r4, #1 / ldrh r2, [r0, r1]      ... add r4, #1
 *
 * gcc turns the counter into a pointer walk and drops the shift, which is four
 * instructions cheaper over the loop. `j = i << 1;` inside the loop with `i++`
 * at the bottom -- written below, and exactly the ROM's shape -- is what gets
 * strength-reduced.
 *
 * `-fno-strength-reduce` was NOT tried here because it has been byte-identical
 * to the default on every function it has been tried on in this corpus
 * (Func_80a9d84, batch 63). If someone wants to spend a screen on it, that is
 * the reason it was skipped rather than an oversight.
 *
 * The rest is right: 0x1ff is legitimately pooled (it does not fit `mov #imm8`,
 * so this is NOT the pool tell and the constant needs no name), `m = 0x1ff;
 * m &= v;` makes the constant the AND's destination, and the sentinel is built
 * as `n = 1; n = -n;` to match `mov r2, #1 / neg r2, r2`.
 *
 * LATER: re-derived independently before the park file was noticed, which is
 * how tools/unparked_candidates.py came to match EVERY identifier in a park
 * rather than only Func_/OvlFunc_ shapes -- this function's real name is what
 * slipped through. The rediscovery reached the same conclusion, so the note
 * above is confirmed rather than revised. Three things it did add:
 *
 *   - AT -O1 the streams are the same LENGTH (27 against 27) and 20 differ,
 *     against 27 differing and a short stream at -O2. That is suggestive but
 *     NOT evidence of an -O1 rule: no Makefile rule covers rom_b6e7c, and 20
 *     differing is nowhere near a match, so nothing was changed. Recorded so
 *     the next reader does not re-run the same three flag screens.
 *     --no-rerun-cse and --no-sched2 are both identical to the default.
 *
 *   - The ROM sign-extends the ALREADY LOADED halfword (`lsl r3, r2, #16 /
 *     asr r3, #16`) instead of re-reading it with `ldrsh`. That is the tell
 *     for a variable WIDER than short holding the loaded value and being cast
 *     down -- with `unsigned short v`, gcc knows the cast is just a compare
 *     against 0xffff and never emits the shift pair.
 *
 *   - Acting on that and declaring `int v` makes it WORSE, not better: 22
 *     differing at -O1 against 20, and 25 at -O2 against 27 with a stream two
 *     lines short. So the sign-extension tell is real but is not reachable
 *     from the variable's type while the index is still being strength-reduced.
 *     The strength reduction is upstream of it and has to go first.

 * ===================== BATCH 343 BRIEF A =====================
 *
 * CURRENT FIGURE, in stream terms, re-measured at the head of this batch:
 *   AT PRODUCTION FLAGS  26 of 26 encodings (ref 26, ours 22); 16-bit
 *     INSTRUCTIONS ref 24, ours 20; SIZE ref 56, ours 48.  Relocations differ
 *     only as .Lc593c at a SHIFTED OFFSET (ref 0x30, ours 0x28) -- a
 *     consequence of the length gap, not a separate blocker.
 *   UNDER -fno-strength-reduce (STRENGTH_CFLAGS)  22 of 26 encodings
 *     (ref 26, ours 26); INSTRUCTIONS ref 24, ours 23; SIZE EQUAL at 56;
 *     RELOCATIONS IDENTICAL.  The function is now exactly ONE INSTRUCTION
 *     short and the figure measures MISALIGNMENT, not distance.
 *
 * *** THE PARK'S REASON FOR SKIPPING -fno-strength-reduce IS REFUTED. ***
 * The text above says the flag "has been byte-identical to the default on
 * every function it has been tried on in this corpus (Func_80a9d84, batch
 * 63)".  On THIS function it is the single biggest lever in the file: it
 * closes the four-byte length gap, equalises the relocations and takes the
 * figure 26 -> 22.  One function's negative was generalised into a corpus
 * rule and it cost this park twenty-four batches.  tools/autosweep.py found
 * it in one command.  (-O1 also reads 22 of 26, but its stream stays SHORT,
 * so it is misalignment, not an improvement.)
 *
 * THE BLOCKER CLASS IS CONFIRMED AND THE REMAINING RESIDUE IS NAMED EXACTLY.
 * Aligned against the reference under the flag, the whole function matches
 * except for TWO things, and they are independent:
 *
 *   (a) THE SECOND LOAD.  The reference re-reads the table halfword in the
 *       then-arm (`ldrh r3, [r0, r1] / lsr r0, r3, #9`); we reuse the already
 *       loaded register (`lsr r0, r0, #9`).  That is the missing instruction.
 *   (b) THE SENTINEL IS HOISTED.  The reference rebuilds -1 inside the loop
 *       (`mov r2, #1 / neg r2, r2` between the `lsl` and the `asr`); loop.c
 *       hoists ours into the prologue.  Net count neutral -- two instructions
 *       move from the loop body to the preheader -- so it does not show in the
 *       count, only in the positions.
 *
 * (a) IS KILLED BY cse1, AND NO TYPE-BASED SPELLING CAN REACH IT.  Counting
 * `(mem:HI` per dump: 2 at .00.rtl and .02.jump, 1 at .03.cse.  The two loads
 * are distinct in RTL -- one is `(zero_extend:SI (mem:HI (plus ...)))`, the
 * other a bare `(mem:HI (plus ...))` -- and cse1 commons them.  The reason no
 * spelling helps is structural: **gcc-2.96's cse.c contains ZERO references to
 * MEM_ALIAS_SET** (`grep -n "MEM_ALIAS_SET\|alias_set" cse.c` is empty) and
 * `exp_equiv_p`'s MEM path compares only code, mode and operands.  So the
 * alias-set lever that works against gcse (gcse.c:1546-1551) and against the
 * post-reload scheduler has NO EFFECT on cse1.  Measured, not just argued: the
 * second read through a distinct struct tag is EXACTLY INERT, 22 of 26, 24/23.
 *
 * ALSO MEASURED AND EXACTLY INERT, all 22 of 26 with 24/23 instructions:
 *   - a plain `-1` sentinel instead of `n = 1; n = -n;`
 *   - the second read through a distinct struct tag (see above)
 *   - `do { } while (0);` INSIDE the loop before the `return`
 *   - `do { } while (0);` INSIDE the loop before the sentinel build
 *   - a second index name `k = i << 1;` for the then-arm's read
 *   (tools/autosweep.py only sweeps TOP-LEVEL statement boundaries, so the two
 *   in-loop anchor placements are a genuinely new negative, not a re-screen.)
 *
 * `unsigned short v` with `s = (short)v` is NOT a near miss and must not be
 * read as one: it reaches 24/24 instructions but by a different program --
 * gcc drops the pooled 0x1ff entirely and extracts the field with
 * `lsl #7 / lsr #23`.  Twenty-two still differ.
 *
 * THE volatile PROBE, run as a MEASUREMENT and deliberately not shipped.
 * docs/elevation.md's "blocked sub-class" note says only `volatile` keeps both
 * loads.  It does -- and it is not even a fakematch solution here: under the
 * flag it reads 27 of 26 at 25 instructions, ONE LONG, because the volatile
 * load needs its own pseudo and gcc adds `mov r2, r3`.  AND IT STILL HOISTS
 * THE SENTINEL.  That is the useful part: it proves (a) and (b) are
 * independent and that closing (a) alone does NOT land this function.  Anyone
 * reaching for a second-load device should know they still owe loop.c an
 * answer afterwards.
 *
 * NEXT, if someone spends another round: attack (b) first, because it is the
 * one with an unexplored pass.  loop.c's `move_movables` is the hoister and
 * the park has never looked at its threshold; (a) is a hard structural
 * negative with a citation, and there is no point paying for it while (b)
 * would still leave the function misaligned.
 *
 * AUTOSWEEP, recorded so nobody re-screens it:
 *   FLAG AXIS    -O1 22 (short stream), -fno-strength-reduce 22 (EQUAL -- see
 *     above), -fno-gcse / -fno-schedule-insns2 / -ffixed-r7 /
 *     -fno-strict-aliasing / -fno-rerun-cse-after-loop / -fno-rerun-loop-opt
 *     all 26, i.e. inert.
 *   ANCHOR AXIS  15 of 15 top-level boundaries; best 25 with dsize=-4, which
 *     is misalignment.  CLOSED.
 */
extern unsigned char Lc593c[] __asm__(".Lc593c");

int GetWeaponType(int id)
{
    unsigned char *t;
    int key;
    int i;
    unsigned int j;
    int v;
    int m;
    int s;
    int n;

    key = id;
    i = 0;
    t = Lc593c;
    do {
        j = i << 1;
        v = *(unsigned short *)(t + j);
        m = 0x1ff;
        m &= v;
        if (key == m)
            return *(unsigned short *)(t + j) >> 9;
        s = (v << 16) >> 16;
        n = 1;
        n = -n;
        i++;
    } while (s != n);
    return 6;
}
