/* OvlFunc_969_200a360  --  0x0200a360  --  TRIAGE PARK, NO CANDIDATE
 *                                          (batch 313, brief B)
 *
 * NON-MATCHING, NO FIGURE CLAIMED.  No candidate was written; there is no
 * objcmp number and none is implied below.
 *
 * Verify (once a candidate exists) with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7f6e64/200a360.c \
 *     asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_a.s --func OvlFunc_969_200a360
 *
 * ===================== SHAPE, SPLIT, FRAME, VENEER =======================
 * 1822 instructions, FIFTEEN labels, 433 `bl` calls.  This is a CUTSCENE
 * SCRIPT -- roughly three instructions per call, almost everything is argument
 * fill -- and it belongs to the CALL-SCRIPT population, which the 800+ band
 * axis does not cover and which WORK DENSITY orders correctly.  It is the same
 * population and the same overlay as OvlFunc_969_20088b4, whose park is the
 * closest sibling evidence available and should be read beside this one.
 *
 * SPLIT SHAPE: NONE.  `grep -c thumb_func_start` on the reference is 1, so this
 * is a WHOLE-FILE conversion to
 * src/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_a.c when it lands.
 * tools/datacheck.py is SILENT on the reference.  Note the overlay path hazard:
 * the install path above is `ovl_7f6e64`, the reference lives under
 * `asm/overlays/rom_7f6e64`, and one linker row under
 * overlays/rom_7f6e64/ will name the object.
 *
 * FRAME, ALL FOUR GREPS:
 *   g1  `sub sp, #0x88` / `add sp, #0x88`  -- 136 bytes, under the 508 cap
 *   g2  `(add|sub) sp, rN`                 -- ZERO, no register-built frame
 *   g3  `mov rX, sp` ZERO; `add rX, sp, #K` TWO -- TWO STACK AGGREGATES, both
 *       in the `add rX,sp,#K` idiom.  Aggregate declaration order is REVERSED
 *       relative to the frame.
 *   g4  `str rX, [sp]` 3 with `ldr rX, [sp]` ZERO -- *** UNPAIRED ***
 *       -> genuine OUTGOING ARGUMENT SPACE for 5-or-more-argument calls.
 *          The brief's `sp0 3` is CORRECT here.
 *
 * VENEER: ZERO inline sites and ZERO `bl _call_via_rN`.  Nothing to install.
 *
 * ========== THIS IS THE ONE TARGET OF BRIEF B WHERE THE PIN QUESTION
 * ========== ACTUALLY LIVES -- AND THE PARTITION PREDICTS IT WILL RESIST
 *
 * Of brief B's four, this is the ONLY function whose high-register content is
 * constants.  Partitioning its 58 `mov rlo,rhigh` copies by the ROOT of the
 * value in the high register:
 *
 *     root class                 count
 *     MULTI-INSN-CONST (mov+lsl)    29
 *     POOL-CONST                    13
 *     arg/prologue                   4
 *     SP                             4
 *     callret                        4
 *     POOL-SYM                       2
 *     IMM8                           2
 *     ---------------------------------
 *     PURE-CONSTANT ROOTED          44  of 58    <-- the pin pass's DOMAIN
 *     MEM-ROOTED                     0
 *
 * 44 of 58, and ZERO memory-rooted.  By the class that docs/elevation.md shows
 * PREDICTS the pin's reach (0 -> 2 opcodes of residue, 11 -> 17), 42 in the
 * two constant sub-classes is far and away the largest figure measured, so the
 * pin pass has more to reach here than on anything in the doc's table.
 *
 * BUT THE TWO CONSTANT SUB-CLASSES ARE *BOTH* POPULATED -- 29 BUILT AND 13
 * POOLED -- AND THAT IS THE MIXED SIGNATURE.  The reference's own high-register
 * value map, read off the defining writes:
 *
 *     r8   <- BUILT 0x8000 (0x80<<8)      r8   <- POOL 0x2013
 *     r8   <- BUILT 0x80000 (0x80<<12)    r9   <- POOL 0 (x2)
 *     r9   <- BUILT 0xa000 (0xa0<<8)      r10  <- POOL 0x2014
 *     r10  <- BUILT 0x3000 (0xc0<<6)      r10  <- POOL 0xbb
 *     r10  <- BUILT 0xa000 (0xa0<<8)      r11  <- POOL gScript_969__0200e074
 *     r11  <- BUILT 0x3000 (0xc0<<6)
 *     r11  <- BUILT 0x6000 (0xc0<<7)
 *     (plus r8/r9/r10 <- callret and <- str/add/cmp-rooted values)
 *
 * Compare its file-mate OvlFunc_969_20088b4, the one function docs/elevation.md
 * names as the mixed case that resisted selective pinning.  Its partition is
 * 11 BUILT and 11 POOLED -- the SAME both-sub-classes-populated shape -- while
 * every function the doc records as a PURE REBUILD has one sub-class or none:
 *
 *     function    BUILT  POOL-CONST   doc's verdict
 *     20095dc         0           0   pure rebuild, pin residue 2 opcodes
 *     2009410         0           1   pure rebuild
 *     20088ec         7           0   pure rebuild
 *     200b4c8        11           0   pin residue 17 opcodes
 *     20088b4        11          11   *** MIXED, resisted selective pinning ***
 *     200a360        29          13   *** predicted MIXED, same shape ***
 *
 * THE PREDICTION, STATED SO IT CAN BE FALSIFIED: a blanket pin pass over this
 * function's wide-literal calls will NOT take both axes exact, and it will fail
 * the same way 20088b4 does, because a pin forces the REGISTER and not the
 * REBUILD -- so it can settle the 29 BUILT values and cannot settle the 13
 * POOLED ones, which the reference loads once into a high register and HOLDS.
 * THE PLACEMENT RULE TO APPLY INSTEAD is the one recovered from
 * src/non_matching/ovl_7f6e64/20088b4.c and recorded in this batch's report:
 *   * values the reference RELOADS at every site  -> PIN;
 *   * values the reference HOLDS in a register    -> the TWO-STEP COMPUTED
 *     form `q = 0x80; q <<= 8;`, whose two sets defeat the REG_EQUIV that
 *     makes a one-set `int q = 0x80 << 8;` inert.
 * Those two lists are DISJOINT and together cover every value, so applying
 * both is NOT a partial constant set and the zero-sum objection to selecting
 * over constants does not bite.  THAT is the experiment this function is for.
 *
 * The reload list is read straight off the pooled multiset (37 distinct
 * entries, the largest of the four):
 *     0x101 x9   0x1999 x6   iwram_3001ebc x4   0x3333 x4   0x103 x4
 *     0xcccc x3  0x135 x3    OvlFunc_969_20083a0 x2   0xffff0000 x2
 *     0x4063ff x2  0x19999 x2   0x1090000 x2   =0 x2
 *     and 24 further entries at multiplicity 1.
 * The 0x1999 / 0x19999 / 0xcccc / 0x9999 / 0x6666 / 0xccc family is the
 * __MapActor_SetSpeed fixed-point set that band-800plus.md s2 traced on
 * OvlFunc_881_2008c28, so that section's cse1-commons-constants-across-calls
 * mechanism applies to this function essentially unchanged.
 *
 * The HOLD list is the value map above.
 *
 * ===================== TWO SITES THAT NEED READING FIRST =================
 *   * `ldr r2, =0` and `ldr r0, =0` -- TWO POOLED ZEROS, and both reach a HIGH
 *     register (r9 twice).  Zero is eight-bit-movable, so `*thumb_movsi_insn`
 *     would emit `mov` and could never pool it: the site is therefore EITHER a
 *     relocation (a symbol whose address is zero) OR `force_const_mem` on a
 *     SPILLED CONSTANT PSEUDO.  Both branches are documented and the screen
 *     cannot distinguish them, so NO `.sym` entry may be written on this
 *     evidence.  The adjudicator is RELOCATION PARITY: read the reference's
 *     relocation count before adopting any symbol spelling, because a
 *     `_AREA_00` / `_CONST_0` carrier ADDS a relocation the reference lacks,
 *     and on a function whose relocations are already at parity that makes the
 *     spelling inadmissible whatever it does for the pool word.
 *     Note the file-mate precedent cuts the other way -- on 20088b4
 *     `z2 = (int)&_AREA_00;` is what BOUGHT the exact count -- so this must be
 *     measured here and not inherited.
 *   * `0x101 / 0x103 / 0x105`-style near-runs appear across this overlay's
 *     scripts and look exactly like a named base plus literal offsets.  The
 *     evidence for a walked base is POOL MULTIPLICITY, NOT VALUE ADJACENCY,
 *     and here 0x101 is pooled NINE times and 0x103 FOUR -- which is the
 *     reload signature, not the walked-base signature.  Treat them as reload
 *     constants and pin them; do not spend a round on a base.
 *
 * ==================== WHY NO CANDIDATE WAS ATTEMPTED =====================
 * 1822 instructions at ~3 per call is on the order of 450 call statements to
 * transcribe before any figure exists, which does not fit beside a reconstruction
 * taken to depth in the same brief.  Brief B took Func_8090a5c to a real figure
 * instead, per its own instruction that depth on two beats shallowness on four,
 * and spent the remaining budget establishing the partition instrument that
 * tells you -- before a line is written -- which of these four the pin question
 * even applies to.  It applies to this one.
 *
 * WHAT TO BRIEF: this function alone, with the disjoint PIN list and HOLD list
 * above handed over as data, and the prediction above as the thing to falsify.
 * It is the right target for the question brief B was given; the other three
 * were not, and the partition is why.
 */
