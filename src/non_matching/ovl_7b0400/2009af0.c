/* OvlFunc_925_2009af0  --  0x02009af0
 *
 * NOT RECONSTRUCTED, NO objcmp FIGURE EXISTS.  No candidate in this file, so no
 * "N of M" line to carry.  What follows is measurement of the REFERENCE.  Its
 * value is that this is the PUREST case of the commoning blocker in brief D's
 * five, and therefore the right function to test any future fix against.
 *
 * Reference sliced to ref_OvlFunc_925_2009af0.s in this directory, from
 * asm/overlays/rom_7b0400/ovl_314_c_c_c_a_c_c_a_a_c.s (1,876 instructions).
 * Note the function NAME's bank and the FILE it lives in disagree with the two
 * other rom_7b0400 objects that merely branch to it -- the park directory for
 * this one is derived from the reference's own file, which is the only safe
 * derivation (every overlay loads at the same base).
 *
 * Once a candidate exists:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7b0400/2009af0.c \
 *     asm/overlays/rom_7b0400/ovl_314_c_c_c_a_c_c_a_a_c.s --func OvlFunc_925_2009af0
 *
 * ============== THE CLEANEST COMMONING TEST CASE IN THE BAND =================
 *
 * 164 wide-constant builds -- the most of brief D's five, over three times
 * OvlFunc_964_200a59c's 41 -- covering 50 distinct values, of which 141 ARE
 * REBUILDS of a value the function already built.  And the reference makes only
 * TWO `mov rARG, rSAVED` copies in all 1,876 instructions.
 *
 *   reference            insns  wide builds  rebuilds  arg copies  saved defs
 *   OvlFunc_925_2009af0   1876      164        141          2          6
 *   OvlFunc_959_200b054   2123      116         97         57          3
 *   OvlFunc_924_200bd20   1210       73         42          5         14
 *   OvlFunc_888_200888c   1524       56         52          8          6
 *   OvlFunc_964_200a59c    993       41         38         10         21
 *
 * 141 rebuilds against 2 copies is as close to "rebuild everything, common
 * nothing" as the band offers, and it is the maximum contrast with what our
 * compiler does: on OvlFunc_964_200a59c, at a quarter the wide-build count, our
 * best candidate still makes 33 copies against that reference's 10.  Scaled by
 * wide builds, 925 is where the excess should be largest and easiest to read.
 * Any future flag, cost-model or expander change aimed at the mechanism in
 * repro_commoning.c should be measured HERE first.
 *
 * AND IT IS NOT A SOURCE-BASE FUNCTION, checked the sharp way.  Zero
 * `add rX, rSAVED, #k` and zero `mov rX,rSAVED / add rX,#k` in 1,876
 * instructions, so brief D's named-base reading is excluded outright rather
 * than merely unlikely -- the two readings really are converses and this one
 * sits entirely on the commoning side.  Contrast OvlFunc_959_200b054 in this
 * directory, which has 75 such uses and wants a completely different lever.
 *
 * TWO SYMBOLS ARE HELD IN REGISTERS, AND NOT CONSISTENTLY, which is a real
 * source-shape question to settle before transcription rather than after:
 *   r7 = iwram_3001ebc  (one `ldr r7, =iwram_3001ebc`)
 *   r6 = iwram_3001e70  (one `ldr r6, =iwram_3001e70`)
 * but iwram_3001ebc is ALSO reached through NINE separate pool loads elsewhere
 * in the same function, and iwram_3001e70 through two.  So the original does
 * not hold one function-scope pointer for either; it names a base over part of
 * the function and re-reads the global over the rest.  That is the same shape
 * OvlFunc_964_200a59c needed (where one gState base local was not enough and a
 * SECOND one had to be assigned deeper in, because the first's live range ended
 * at the dispatch).  Expect to need two or three scoped bases, not one, and
 * expect the `=gState+N` / `=iwram_3001ebc+N` fold if you use none.
 *
 * CONSTANT SET: 36 pool loads of 11 distinct values -- 0x101 twelve times,
 * iwram_3001ebc nine, 0xcccc four, 0x6666 three, iwram_3001e70 twice, and one
 * each of 0xffff0000, 0xffe00000, 0x881, 0x1999, 0x15d4, 0x105.  Eleven values
 * and 41 callees is one `grep | sort | uniq -c` away from settled (band doc
 * section 5), and that check should come first at this size.
 *
 * 0x101 TWELVE TIMES, EACH A SEPARATE POOL LOAD, is worth a note: it is pooled
 * rather than built, so by the mechanism in repro_commoning.c it lands in a
 * pseudo and OUR compiler will common it into a call-saved register.  The
 * reference reloads it twelve times.  Expect that single value to be worth
 * roughly eleven copies plus a register of pressure, and check it early -- it is
 * the most-repeated constant in the function and the cheapest thing to look at.
 *
 * 500 calls across 41 distinct callees.  `push {r5,r6,r7,lr}`, `sub sp,#8` with
 * FOUR `str rX,[sp,#K]` and ZERO `ldr rX,[sp,...]`: argument staging only, no
 * spill slot, a fourth confirmation of brief D's frame finding.
 *
 * TWELVE branches, EIGHT of them pool skips, none backward -- FOUR real branches
 * in 1,876 instructions.  Straight-line in the strict sense; the control flow is
 * not the work and should not be budgeted for.
 *
 * ONE EXCEPTIONAL BUILD, inspected so it is not misread as commoning:
 * `mov r5,#0x80 / lsl r5,#1 / str r5,[r3]` near the top is a STORE operand, not
 * a call argument, so the expander gives it a pseudo for the ordinary reason --
 * exactly what repro_commoning.c's f_narrow_pseudo control predicts.  It is one
 * of only five callee-saved wide builds across all five references and none of
 * the five is a counter-example to the mechanism.
 
 *
 * *** RECIPE REFERENCE PATH CORRECTED ON INSTALL. *** This park's `Verify with:` line named
 * scratch_elev/b311d/ref_<NAME>.s -- the agent's own workspace copy of the reference. That
 * directory is GITIGNORED, so the recipe would have become unrunnable the moment the workspace
 * was cleaned, and parkcheck reported TOOLING on all five parks of this brief.  A recipe must
 * name a DURABLE path on both sides: the installed .c and the tracked asm/ reference.  This is a
 * third flavour of the same defect, after a literal `<this file>` placeholder (six parks) and a
 * recipe naming a file that does not exist -- all three make a park unverifiable, which means its
 * figure can never be caught lying.
 */
