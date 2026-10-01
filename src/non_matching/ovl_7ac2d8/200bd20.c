/* OvlFunc_924_200bd20  --  0x0200bd20
 *
 * NOT RECONSTRUCTED, NO objcmp FIGURE EXISTS.  There is no candidate in this
 * file, so there is no "N of M" line to carry.  Saying so plainly is the point:
 * brief D asked for five and this batch reconstructed one, on the brief's own
 * instruction that two well-understood functions beat five shallow ones.  What
 * is below is measurement of the REFERENCE only, and the one finding that
 * matters here is that this function should not have been in brief D at all.
 *
 * Reference sliced to ref_OvlFunc_924_200bd20.s in this directory, from
 * asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_c_a_c.s (1,210 instructions).
 *
 * Re-measure the reference-side figures with:
 *   python3 widecheck.py ref_OvlFunc_924_200bd20.s
 * and, once a candidate exists:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7ac2d8/200bd20.c \
 *     asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_c_a_c.s --func OvlFunc_924_200bd20
 *
 * ================ THIS FUNCTION IS MISCLASSIFIED IN BRIEF D =================
 *
 * Brief D selected its five targets by the REFERENCE's high-register use and
 * called them "the extreme straight-line population".  Counted properly, this
 * one is not straight-line and does not belong with the other four:
 *
 *   reference            insns  branches  pool-skips  backward  REAL branches
 *   OvlFunc_925_2009af0   1876      12         8          0            4
 *   OvlFunc_959_200b054   2123      11         7          0            4
 *   OvlFunc_888_200888c   1524      11         4          2            7
 *   OvlFunc_964_200a59c    993      40        12          0           28  + 2 jump tables
 *   OvlFunc_924_200bd20   1210      89        21          1           68
 *
 * 68 real branches over 1,210 instructions is about 18 instructions per block --
 * ORDINARY.  docs/band-800plus.md section 8.2 already says what to do with that:
 * "for the branch-dense ones, the existing 500-instruction lever set should apply
 * essentially unchanged -- their blocks are ordinary sized, so none of section 2
 * applies."  That is the brief for this function, and it is a different brief
 * from the one it was given.
 *
 * NOTE THE BRIEF'S OWN WARNING SURVIVES THIS CORRECTION.  Brief D was right that
 * branch counting is noisy and that pool skips inflate it -- 21 of this
 * function's 89 branches are pool skips, and 8 of 12 and 7 of 11 on the two
 * largest references.  The fix is not to abandon the branch axis but to SUBTRACT
 * THE POOL SKIPS, which is one pass over the .s: a forward branch with a `.pool`
 * or `.pool_aligned` directive strictly between it and its target.  The script
 * that produced the table above does exactly that and is eleven lines.  After
 * that subtraction the two populations separate cleanly -- 4, 4, 7 against 68 --
 * and the high-register axis agrees with it on four of the five.
 *
 * THE DISCRIMINATOR BRIEF D PROPOSED POINTS THE OTHER WAY HERE, and that is the
 * second reason to re-triage this one.  Brief D's rule for telling a source base
 * from commoning is to count pooled constants on both sides: far fewer in the
 * reference than uses means a SOURCE BASE, the same few repeated means
 * COMMONING.  This function has 70 pool loads of 43 DISTINCT values -- a ratio
 * of 1.6, nothing like the other four:
 *
 *   reference            pool loads  distinct  ratio   reading
 *   OvlFunc_888_200888c      43         10      4.3    repeated: commoning
 *   OvlFunc_925_2009af0      36         11      3.3    repeated: commoning
 *   OvlFunc_959_200b054      53         16      3.3    repeated: commoning
 *   OvlFunc_964_200a59c      41         27      1.5    mostly distinct
 *   OvlFunc_924_200bd20      70         43      1.6    mostly distinct
 *
 * So this function's constants are mostly used once and the commoning mechanism
 * (see repro_commoning.c) has little to work with.  Its 73 wide-constant builds
 * cover 43 distinct values with only 42 rebuilds -- the lowest rebuild fraction
 * of the five.
 *
 * WHAT IS ACTUALLY HERE, structurally.  1,210 instructions, 257 calls across 43
 * distinct callees, `push {r5,r6,r7,lr}` with r8 also used (4 high-register
 * mentions), `sub sp,#8` with 80 `str rX,[sp,#K]` and -- checked the decisive
 * way -- ZERO `ldr rX,[sp,...]`.  Every one of those 80 frame stores is
 * outgoing-argument staging and not one is ever read back, so there is no spill
 * slot in this function and no slot map to sort.  That confirms brief D's frame
 * finding on a fifth function and on the largest staging count of the five.
 *
 * The 1 backward branch plus the loop-invariant pair at lines 570-571
 * (`mov r7,#0x9e / lsl r7,#18` and `mov r6,#0xcc / lsl r6,#2`, both built
 * immediately before the loop label `.L42b6`) is the one loop.  Those two builds
 * are 2 of only 5 callee-saved wide builds in all five references, and they are
 * hoists out of that loop rather than cse commoning -- see repro_commoning.c,
 * where all five exceptions are accounted for.
 *
 * RECOMMENDED NEXT STEP: re-brief this one with the 500-instruction lever set,
 * NOT with brief D's mechanism.  Its 43 distinct pooled values are the work --
 * that is 43 constants, actor slots, script pointers and symbols to get right,
 * and docs/band-800plus.md section 5's single `grep | sort | uniq -c` against a
 * candidate's `.word` list settles all 43 at once.  Expect the residue to be
 * ordinary register allocation and ordinary call-site spelling.
 
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
