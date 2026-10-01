/* Func_80a2680  --  0x080a2680  --  TRIAGE PARK, NO CANDIDATE (batch 313, brief B)
 *
 * NON-MATCHING, NO FIGURE CLAIMED.  No candidate was written; there is no
 * objcmp number and none is implied below.
 *
 * Verify (once a candidate exists) with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/Func_80a2680.c \
 *     asm/rom_a1000/rom_a1814_c_a_c_c_a_c_c_c_c_a.s --func Func_80a2680
 *
 * ===================== SHAPE, SPLIT, FRAME, VENEER =======================
 * 1268 instructions, 87 labels -- BRANCH-DENSE, ~15 instructions per block, so
 * the ordinary 500-instruction lever set applies and band-800plus.md's
 * straight-line material does not.
 *
 * SPLIT SHAPE: A SPLIT IS REQUIRED.  `grep -c thumb_func_start` on
 * asm/rom_a1000/rom_a1814_c_a_c_c_a_c_c_c_c_a.s is 2 -- Func_80a24d0 (lines
 * 21-193) then Func_80a2680 (lines 214-1604).  The target is the LAST of the
 * two, so by the positional asymmetry recorded in docs/elevation.md this needs
 * only a TWO-WAY cut, not a three-way, which makes it cheaper than a
 * middle-of-three sibling.  DRY-RUN BOTH ORDERS before committing
 * (`tools/split_s.py --dry-run`, which is honoured).  tools/datacheck.py is
 * SILENT on the reference, so no `.global` export list is required.
 *
 * FRAME, ALL FOUR GREPS:
 *   g1  `sub sp, #0x28` / `add sp, #0x28`  -- 40 bytes
 *   g2  `(add|sub) sp, rN`                 -- ZERO
 *   g3  `mov rX, sp` ZERO and `add rX, sp, #K` ZERO -- NO STACK AGGREGATES
 *   g4  `str rX, [sp]` 1 with `ldr rX, [sp]` ZERO -- *** UNPAIRED ***
 *       -> genuine OUTGOING ARGUMENT SPACE for a 5-or-more-argument call.
 *          The brief's `sp0 1` is CORRECT here (unlike on Func_8090a5c and
 *          UpdateActors, where the same count is a paired spill slot).
 *
 * VENEER: ZERO inline sites, counted scoped and dot-anchored.  The 4
 * `bl _call_via_rN` in range are gcc's own output.  Nothing to install.
 *
 * ============== THE TRIAGE PREMISE IS FALSE HERE TOO, AND IN A SECOND WAY ==
 * The brief ranked this function TOP of the tree on wide-constant reuse, read
 * off 191 high-register mentions -- the highest figure in the band -- and put
 * it in the "mixed" population for the pin question.  The partition of its
 * 55 `mov rlo,rhigh` copies by the ROOT of the high-register value says
 * otherwise:
 *
 *     root class                 count    per-register detail
 *     MEM (a plain load)            28    *** ALL 28 ARE r9 ***
 *     IMM8                           7    r10 x5, r8 x2
 *     COMPUTED from a load           7    r10 x4, r11 x2, r8 x1
 *     callret                        6    r11 x3, r8 x3
 *     arg/prologue                   4
 *     POOL-CONST                     3    r8 x2, r11 x1
 *     ---------------------------------
 *     PURE-CONSTANT ROOTED          10  of 55
 *
 * TWENTY-EIGHT OF THE 55 COPIES ARE ONE QUANTITY: a single memory-loaded value
 * parked in r9 and copied out 28 times.  This is the exact defect
 * docs/elevation.md records when it retracted the raw copy count as a screen
 * ("ten of another function's sixteen copies are one pointer") -- here it is
 * 28 of 55, the strongest instance found so far.  The raw reuse count of 55,
 * and the 191 high-register mentions it was ranked on, are dominated by ONE
 * long-lived pointer plus its 28 reads.
 *
 * So the pin pass's domain here is at most 10 of 55 copies, and by the
 * zero-sum argument any subset of those ten is a different allocation problem.
 * THE LIVE LEVER IS LEVER 1 IN ITS POINTER FORM -- merge the pointer ranges,
 * which raises a reference count and buys a register, and which paid +28 -> -4
 * on size and dropped a frame from five slots to three elsewhere.  That is a
 * band-ENTRY lever and this function's count is nowhere near exact, which is
 * exactly its stated precondition.
 *
 * ================= THE CHEAPEST REAL LEVER HERE, UNSPENT =================
 * The pooled multiset carries a signal that costs one command and was not
 * acted on for want of budget.  TWO CONSECUTIVE RUNS:
 *
 *     0xb7c 0xb7d 0xb7f 0xb80 0xb81 0xb82 0xb84 0xb85   (plus 0xb7c x3)
 *     0xad8 0xad9 0xadb 0xadc 0xadd
 *
 * A RUN OF CONSECUTIVE IDs IS A NAMED BASE PLUS LITERAL OFFSETS.  Thirteen
 * pool words across two runs is the largest instance of that shape in the four
 * functions of this brief.  THE PRECONDITION MUST BE CHECKED FIRST, because
 * docs/elevation.md bounds this lever twice over: the evidence for a walked
 * base is POOL MULTIPLICITY, not value adjacency (two functions held
 * consecutive message ids that were each pooled exactly once and were NOT
 * walked bases), and relocation parity must have room for any symbol spelling
 * before one is adopted.  Here each run member is pooled ONCE, which is the
 * negative side of that bound -- so read the thirteen sites before spending a
 * round, and expect the lever to be inert.
 *
 * The other multiset entries are the reload signature: 0x21a x17, 0x21b x11,
 * 0x1ff x10, 0xb7c x3, 0x5ff x3, 0x222 x3, 0xbef x2, 0x25a x2, and single
 * 0xfdff / 0x200 / 0x25d.  Seventeen reloads of one value is the strongest
 * reload signal of the four, and `.La26bc` plus `iwram_3001f2c` complete the
 * set.  26 distinct pool entries.
 *
 * WHAT TO BRIEF: the two-way split first (it is the cheap half of this file and
 * unblocks Func_80a24d0 as a 173-instruction sibling at the same time), then
 * the r9 pointer range as lever 1, then read the thirteen consecutive-id sites.
 * Do NOT brief the pin pass on this function.
 */
