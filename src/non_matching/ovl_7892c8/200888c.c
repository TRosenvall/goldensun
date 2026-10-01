/* OvlFunc_888_200888c  --  0x0200888c
 *
 * NOT RECONSTRUCTED, NO objcmp FIGURE EXISTS.  No candidate in this file, so no
 * "N of M" line to carry.  What follows is measurement of the REFERENCE.
 *
 * Reference sliced to ref_OvlFunc_888_200888c.s in this directory, from
 * asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_a_c_a_a.s, where it is the THIRD
 * of three functions (1,524 instructions, offsets 152-1546 of that file).  The
 * first two are at lines 11 and 112.  Its overlay already has a landed sibling,
 * src/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_a_a.c, so the directory and the
 * build rows are known ground.
 *
 * SPLIT SHAPE, if it ever lands: the file holds three functions and this is the
 * last, so the natural split is a-half (the two short ones) and b-half (this
 * one).  Check for a .data run in the file before writing the linker rows --
 * OvlFunc_964_200a59c's file in this batch turned out to carry the whole
 * object's .data plus thirteen four-digit `.L` externs, which made its split a
 * three-way one.
 *
 * Once a candidate exists:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7892c8/200888c.c \
 *     asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_a_c_a_a.s --func OvlFunc_888_200888c
 *
 * ================== THE ONE WITH LOOPS, AND NO FRAME AT ALL ==================
 *
 * Two facts separate this from the other four, and both are structural rather
 * than statistical.
 *
 * 1. IT HAS NO FRAME.  `push {r5, lr}` and NO `sub sp` anywhere: zero
 *    `str rX,[sp,#K]` and zero `ldr rX,[sp,...]` in 1,524 instructions.  So not
 *    one of its 490 calls passes a fifth argument.  Every callee here takes four
 *    or fewer, which removes brief D's named-stack-argument-pair lever entirely
 *    and removes any question of a slot map.  Of brief D's five this is the only
 *    one with no frame; the other four all have `sub sp,#8` with 2, 4, 58 and 80
 *    argument-staging stores and no reload.
 *
 * 2. IT HAS TWO REAL LOOPS, and they are the same loop written twice.  Of its 11
 *    branches, 4 are pool skips and TWO ARE BACKWARD -- the only backward
 *    branches in the three straight-line references.  Both loops have identical
 *    shape, r5 is the counter, and the bound is `cmp r5,#5 / bls`, i.e. six
 *    iterations:
 *        i = 0;  do { OvlFunc_888_200a750(8, 0x90 << 5); __CutsceneWait(8); }
 *                while (++i <= 5);
 *    written at slice lines 652-666 and 883-893.  The `bls` with the counter
 *    initialised to 0 immediately before means a `do/while` or a `for` whose
 *    first test gcc removed; which of the two the source used is a real question
 *    and is settled by whether the candidate reproduces the entry branch, not by
 *    preference.
 *
 * THE LOOPS ARE ALSO WHERE A COUNT I PUBLISHED WAS WRONG, recorded here because
 * this function is what caught it.  I first measured commoning by which register
 * each wide constant is BUILT into, concluded the references never common a wide
 * argument constant, and was wrong.  In this function the value 0x90<<5 is built
 * into r5 at slice lines 636, 667 and 894 and fed to arguments as `mov r1,r5` at
 * 642, 647, 671, 676, 831, 836, 898 and 903 -- eight copies -- while the loop
 * BODIES rebuild the same value into r1 every iteration (lines 658-660, 885-887).
 * One build and eight copies counts as one build, which is why the build-register
 * measure missed it.  The corrected instrument is the COPY count, and the
 * retraction and corrected table live in repro_commoning.c.
 *
 * That contrast inside one function is itself the cleanest statement of the
 * mechanism available in this batch: the SAME constant is rebuilt into the
 * argument register inside the loop and commoned into r5 outside it, because
 * inside the loop each iteration's build is the only one in its block.
 *
 * CONSTANT SET: 43 pool loads of just 10 distinct values -- 0x101 nineteen
 * times, 0xcccc seven, 0x6666 seven, iwram_3001ebc four, and one each of
 * OvlFunc_888_2008848, 0x201, 0x1162, 0x1138, 0x105, 0x10002.  A ratio of 4.3
 * uses per distinct value, the highest of the five, and brief D's reading of
 * that ratio is correct here: the same few repeated means COMMONING, and naming
 * them cannot help.  Checked the sharp way -- zero `add rX, rSAVED, #k` and zero
 * `mov rX,rSAVED / add rX,#k` in the whole function, so the named-base reading
 * is excluded outright.  Compare OvlFunc_959_200b054 in this directory, which
 * has 75 such uses and is a source-base function despite a similar ratio.
 *
 * 0x101 NINETEEN TIMES, each a separate pool load, is the single highest-value
 * thing to look at.  Pooled constants land in a pseudo (repro_commoning.c
 * step 1), so our compiler will common it; the reference reloads it nineteen
 * times.  Expect that one value to account for most of the candidate's excess.
 *
 * 490 calls across 38 distinct callees, and the distribution is extreme: 160
 * `__CutsceneWait`, 57 `__Func_809280c`, 41 `__ActorMessage`, 39
 * `__MapActor_Emote`, 27 `__Func_80925cc`, 26 `__MapActor_SetAnim`, 25
 * `__MapActor_DoAnim`.  At 3.1 instructions per call this is dialogue-and-beat
 * script, and the transcription risk is high while the per-statement difficulty
 * is low.  The relocation sequence is the instrument that catches a dropped or
 * duplicated call -- it counts calls and symbols, which arithmetic cannot fake --
 * and at 490 calls it should be read before any hunk.
 *
 * 56 wide-constant builds of only 12 distinct values, 52 of them rebuilds.
 * Combined with 43 pool loads of 10 distinct values, essentially every constant
 * in this function is one our compiler will common and the reference will not.
 *
 * RECOMMENDED ORDER OF WORK:
 *   1. The constant set first: 10 pooled values and 38 callees settle in one
 *      `grep | sort | uniq -c` (band doc section 5).
 *   2. The two loops next, because they are the only control flow that is not a
 *      pool skip and because getting `do/while` versus `for` wrong moves the
 *      entry branch and nothing else will line up after it.
 *   3. The relocation sequence, for the 490 calls.
 *   4. Last, the commoning remainder -- and expect it to be large here in
 *      proportion to size, with 0x101's nineteen reloads leading it.
 
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
