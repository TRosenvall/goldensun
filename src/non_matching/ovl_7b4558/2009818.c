/* ===== DUPLICATE PARK RETIRED INTO THIS ONE, batch 349 =====
 * src/non_matching/overlays/2009818.c was a SECOND park file for this same
 * function, created by batch 338's scratchfill recovery without noticing this
 * file already existed -- the same error class that produced a duplicate for
 * 80916b0/916b0 in that batch.  piece.py has been warning about the pair ever
 * since ("SAME BASENAME IN TWO OVERLAY DIRECTORIES"), and it inflated the park
 * count by one and docs/directory/01-parked.md by a row.
 *
 * IT WAS NOT A COPY.  Measured before retiring it, both against
 * asm/overlays/rom_7b4558/ovl_30_c_c_a_c_c_c_b.s --func OvlFunc_927_2009818:
 *
 *     this file                      3 of 38, instructions 24/24
 *     overlays/2009818.c (retired)   3 of 38, instructions 24/24
 *
 * SAME FIGURE FROM TWO DIFFERENT PROGRAMS -- the retired body declared an
 * `unsigned char *a` this one does not, and named __MapActor_SetPos's third
 * parameter `z` where this names it `y`.  So the 3 is reachable by at least two
 * distinct spellings, which bounds the residue: it is not sensitive to either of
 * those differences.  (A brief reported the two as bit-identical programs; they
 * are not -- the bodies differ by 38 characters of code.)
 *
 * The retired file is at toDelete/2009818-overlays-dup.c if its prose is wanted.
 */
/* OvlFunc_927_2009818 -- PARK at 3 differing encodings of 38, PIN-FREE.
 *   0 of 38 IS IN HAND AT TWO PINS -- see "THE LANDING" in the carried-forward
 *   header.  CLOSED FOR PASS 2.  BODY UNCHANGED THIS BATCH.
 *
 * ===== BATCH 349 BRIEF D: THE CLOSURE IS CORRECT, AND ITS EVIDENCE BASE IS
 * ===== ONE PROGRAM.  PLUS: THIS PARK EXISTS TWICE IN THE TREE.
 *
 * THE FOUR NUMBERS, measured this batch (not inherited):
 *   ENCODINGS           3 differing of 38   (ref 38 / ours 38), first index 29
 *                       ref 2011  ours 0449
 *   16-BIT INSTRUCTIONS 24 / 24
 *   BYTES               equal (--func prints no SIZE line; --whole agrees)
 *   POOL / 32-BIT       equal (the two pool words are indices 36-37 and match)
 *   RELOCATIONS         clean.   PINS 0 (tools/shimcount.py).
 * In phase on all three length dimensions, so the figure IS a real distance.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7b4558/2009818.c \
 *     asm/overlays/rom_7b4558/ovl_30_c_c_a_c_c_c_b.s --func OvlFunc_927_2009818
 *
 * SPLIT: NONE NEEDED, re-verified.  tools/datacheck.py SILENT (exit 0).
 * tools/piece.py: asm/overlays/rom_7b4558/ovl_30_c_c_a_c_c_c_b.s holds only
 * OvlFunc_927_2009818 and reports NO non-function exports, so it converts whole.
 * No frame at all (`push {lr}' / `pop {r0}' / `bx r0').
 * MANIFEST carries "split": null -- this is a park, not a landing.
 *
 * ===== THIS PARK IS IN THE TREE TWICE, AS TWO ROWS OF THE DIRECTORY =====
 *
 * tools/piece.py opens with a warning that is NOT about overlay address reuse,
 * although it is phrased that way:
 *     !! SAME BASENAME IN TWO OVERLAY DIRECTORIES
 *        OvlFunc_927_2009818: src/non_matching/overlays/2009818.c,
 *                             src/non_matching/ovl_7b4558/2009818.c
 * and piece.py then attributes the park to the FIRST of those -- i.e. NOT the
 * file this park's own `Verify with' line names.  Both exist, both are parks for
 * the same function against the same reference, and docs/directory/01-parked.md
 * carries BOTH as separate rows (lines 75 and 76), so the parked count is
 * inflated by one here.
 *
 * MEASURED: they are not two candidates, they are ONE PROGRAM.
 *   src/non_matching/overlays/2009818.c      3 of 38, 24/24
 *   src/non_matching/ovl_7b4558/2009818.c    3 of 38, 24/24
 *   ... and the first is BIT-IDENTICAL TO THE SECOND (object fingerprint,
 *       tools/sweep_variants.py).
 * Their bodies DO differ textually -- the overlays/ copy names the actor
 * (`a = __MapActor_GetActor(0xe); __Actor_SetSpriteFlags(a, 0);') where this one
 * inlines the call, and it carries an `#if 0' block of the original prose park --
 * yet the objects are identical, which is the same canonicalisation result as
 * the whole negative list below.
 *   The overlays/ copy is the STALE one: its prose names
 *   `asm/overlays/rom_7b4558/ovl_30_c_c_a_c_c_c.s' as its reference and THAT FILE
 *   NO LONGER EXISTS (the current reference is the `_b' split of it).  It also
 *   predates batches 328/329/336, so it carries none of the closure work.
 * FOR THE COORDINATOR: this is a tree-hygiene item, not a figure. The overlays/
 * copy should be deleted and docs/directory regenerated (never hand-edited).
 * I did not touch it -- outside my directory.
 *
 * ===== THE RECORDED NEGATIVE LIST IS ONE PROGRAM, WHICH STRENGTHENS THE PARK
 *
 * This header records: "MEASURED, ALL EXACTLY INERT AT 3 (earlier batches, 5
 * edits at depth 3, 22 live subsets): callee without a prototype;
 * __MapActor_SetPos declared to return a value; __CutsceneEnd declared to return
 * a value; slot in a named local; coordinates cast at the call; every pair and
 * triple of those; varargs declaration; unsigned coordinate parameters; unsigned
 * char slot parameter; coordinates in named locals immediately before the call;
 * __CutsceneEnd with no prototype."  Plus batch 329's ten pin-free bodies.
 *
 * THIRTEEN OF THOSE REBUILT AND SWEPT WITH OBJECT FINGERPRINTING
 * (tools/sweep_variants.py).  **ALL THIRTEEN BIT-IDENTICAL TO THE BASE.**
 *     __MapActor_SetPos declared to return a value            bit-identical
 *     __CutsceneEnd declared to return a value                bit-identical
 *     __CutsceneEnd with no prototype                         bit-identical
 *     __MapActor_SetPos with no prototype                     bit-identical
 *     varargs declaration (int slot, ...)                     bit-identical
 *     unsigned coordinate parameters                          bit-identical
 *     unsigned char slot parameter                            bit-identical
 *     slot in a named local                                   bit-identical
 *     coordinates cast at the call                            bit-identical
 *     coordinates in named locals before the call             bit-identical
 *     slot local + both coordinate locals (a TRIPLE)          bit-identical
 *     slot in a local SET TWICE (0x1e then 0x11)              bit-identical
 *     one-member union, u.i = 0x11                            bit-identical
 *
 * SO "22 LIVE SUBSETS, ALL EXACTLY INERT AT 3" IS ONE PROGRAM, NOT 22
 * MEASUREMENTS.  That does not weaken this park -- IT IS WHAT ITS OWN VERDICT
 * PREDICTS, and it is now the right kind of evidence for it.  The park's reading
 * (batch 329 (B)) is that `arm_rtx_costs' returns 0 for an 8-bit constant in a
 * SET while `cse.c''s rtx_cost returns 1 for a pseudo, so THE CONSTANT IS
 * CHEAPER THAN A REGISTER and cse substitutes it at the use, killing every early
 * def.  A body whose every named-slot spelling is bit-identical to the unnamed
 * one is exactly that mechanism observed at the object level.  The park reasoned
 * from the cost table to the conclusion; the fingerprints now confirm the
 * conclusion independently.
 *
 * NOTHING ELSE WAS RE-SCREENED, per the brief: the flag axis (all eight per-file
 * groups) and the anchor axis (11 placements, 7 of them bit-identical, so 4
 * distinct programs) are recorded closed in this header and were left alone.
 * -fno-schedule-insns2 stays REJECTED at 9 of 38.
 *
 * ===== THE VERDICT, RECONFIRMED A THIRD TIME =====
 *
 * The residue is indices 29-31: the ROM issues `mov r0, #0x11' BEFORE the two
 * `lsl' materialisations and we issue it after.  The free variable is the
 * INSN_LUID at haifa-sched.c:4113-4115, the tie being closed above LUID by
 * construction (batch 336 audited every rung), and the pin-free route is
 * CORPUS-REFUTED -- 0 pin-free against 83 pinned for this instruction shape in a
 * function with no branch, and this function is straight-line.
 *
 * STILL 3 of 38 PIN-FREE.  CLOSED FOR PASS 2.  The 0 at two pins is the pass-3
 * landing and needs no further search.  DO NOT RE-OPEN AS A SPELLING QUESTION --
 * and after this batch there is a bit-identical set saying why: no declaration-
 * or naming-level spelling of this function reaches the back end at all.
 * -- scratch_elev/b349/D
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void OvlFunc_927_2008ea8(int a, int b);
extern void OvlFunc_927_2008d90(int a, int b, int c, int d);
extern void OvlFunc_927_2008e18(int a);
extern void __Func_8092950(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void __SetFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int y);

void OvlFunc_927_2009818(void)
{
    __CutsceneStart();
    OvlFunc_927_2008ea8(0xe, 1);
    OvlFunc_927_2008d90(0xe, 0xd4 << 1, 0xf0 << 1, 0x79999);
    __CutsceneWait(2);
    OvlFunc_927_2008e18(0xe);
    __Func_8092950(0xe, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xe), 0);
    __CutsceneWait(0x1e);
    __SetFlag(0x305);
    __MapActor_SetPos(0x11, 0xd4 << 17, 0xf0 << 17);
    __CutsceneEnd();
}
