/* Func_8078144 (0x08078144) -- asm/rom_77000/rom_77320_a_c_c.s (3 functions).
 *
 * PARK HELD AT 4 of 103 encodings.  SIZE EXACT (103 against 103), per-opcode
 * memory profile the reference's exactly (ldr=4 ldrb=2 ldrh=1 ldrsh=6 strh=5).
 * The figure is unchanged; THE DIAGNOSIS IS CORRECTED, in the direction the
 * batch-321 park rejected.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_77000/8078144.c \
 *     asm/rom_77000/rom_77320_a_c_c.s --func Func_8078144
 *
 * INSTALLED PATH, if it ever lands: src/rom_77000/rom_77320_a_c_c_b.c.
 * Split shape: TEXT-ONLY, tools/datacheck.py prints nothing.
 *   tools/split_s.py asm/rom_77000/rom_77320_a_c_c.s Func_8078144 --dry-run:
 *     _a.s (2 functions, 237 lines), _b.s Func_8078144 (1 function, 115 lines).
 * All THREE functions in this .s are parked -- see the note in 807808c.c.
 * PINS: 0.  No shim, no fakematch row, no flag group.  No device.
 *
 * ---------------------------------------------------------------------------
 * THE RELOCATION DELTA IS OUR OWN SPLITTING ARTIFACT, NOT A DIFFERENCE
 *
 * All nine relocations sit at identical offsets; the only delta is `.L7a828`
 * (ref) against `_TBL_7a828` (ours) at 0xd8.  `.L7a828` is an ASSEMBLER-LOCAL
 * label in asm/rom_77000/rom_77320_c_c_c_b.s that our tree has already promoted
 * to `.global` because the split put its user in another object -- one of the
 * 1,544 such promotions.  A C file cannot name a `.L` symbol, and in the
 * ORIGINAL object a reference to a same-object local label is a SECTION-relative
 * relocation, not a named-symbol one.  So the names can never agree while the
 * table lives in a different object than its user, and the delta says nothing
 * about the C.  **The 4 IS a distance.  Stop reporting the relocation line as a
 * difference.**  (`_TBL_7a828` stays out of label.sym on the COMPLETION test,
 * exactly as the park argues -- that part is unchanged and correct.)
 *
 * ---------------------------------------------------------------------------
 * WHAT THE 4 ENCODINGS ARE, READ FROM THE MACHINE DESCRIPTION
 *
 * Indices 42-45, two `ldrsh`es and their two scratch `mov`s:
 *
 *      ref                          ours
 *  47  movs  r2, #0x38              movs  r3, #0x38
 *  48  ldrsh r0, [r5, r2]           ldrsh r0, [r5, r3]
 *  49  movs  r3, #0x34              movs  r2, #0x34
 *  50  ldrsh r1, [r5, r3]           ldrsh r1, [r5, r2]
 *
 * The scratch is NOT a local-alloc pseudo.  `.17.lreg` still carries the address
 * as `(plus (reg 32) (const_int 56))`, and `.19.flow2` has
 *
 *   (insn 141 (parallel [(set (reg/v:SI 1 r1)
 *        (sign_extend:SI (mem:HI (plus:SI (reg/v:SI 5 r5) (const_int 52)) 8)))
 *       (clobber (reg:SI 2 r2))]) 162 {*thumb_extendhisi2_insn}
 *
 * and `*thumb_extendhisi2_insn` (arm.md:3239-3242) declares that operand as
 * `(clobber (match_scratch:SI 2 "=&l"))`.  **A `match_scratch` is a RELOAD
 * register**, so this goes through `allocate_reload_reg` -- layer 3 of batch
 * 325's settled model, the round-robin from `last_spill_reg` (reload1.c:5003).
 * `last_spill_reg` is initialised to -1 exactly once per function
 * (reload1.c:821) and advanced only at :4937, so **the cursor is FUNCTION-WIDE,
 * not per-insn.**  Read from source.
 *
 * And this function reproduces batch 325's decisive observable.  `.18.greg`:
 *
 *     Spilling for insn 141.   Using reg 3 for reload 0
 *     Spilling for insn 146.   Using reg 3 for reload 0
 *
 * `find_reg` printed r3 for BOTH; the emitted pair is (r2, r3).  A `Using reg`
 * line is not the register you get -- confirm in `.19.flow2`.
 *
 * ---------------------------------------------------------------------------
 * THE BATCH-321 REFUTATION IS ITSELF REFUTED: THE RESIDUES DO TRADE
 *
 * This park has said, since batch 321:
 *
 *   "SO THE WHOLE RESIDUE IS ONE REGISTER-ALLOCATION CHOICE ... There is no
 *    trade and no scheduling question left -- 4 is a pure allocation residue"
 *
 * reached by reproducing the batch-319 park's `swap-ldrsh-order` at 6 and
 * declaring the matter closed.  MEASURED HERE: swapping the two source
 * statements so the 0x38 read comes first,
 *
 *     r0 = *(short *)((char *)r5 + 0x38);
 *     r1 = *(short *)((char *)r5 + 0x34);
 *
 * makes ALL FOUR of those encodings EXACT -- `movs r2,#0x38 / ldrsh r0,[r5,r2] /
 * movs r3,#0x34 / ldrsh r1,[r5,r3]`, the ROM's pair -- and costs 6 because
 * `strh r3,[r5,#0x3a]` then sinks below the first `ldrsh` and `lsls r0,r0,#14`
 * rises past the second `movs`.
 *
 * So 4 and 6 are COMPLEMENTARY CORNERS, which is what the batch-319 park said
 * and what batch 321 rejected.  Batch 321 refuted the WORDING ("no source order
 * can satisfy both") by observing that the 4-body has the store position and the
 * load order right -- without noticing that the 4-body has the REGISTERS wrong
 * and that the 6-body is its exact complement.
 *
 * THIS IS WHY THE SWEEPS WERE FLAT.  Three crossfire runs at depth 2 over 11
 * local edits and every pair -- getunit-uint, gstate-flat-index, flag-88-flat,
 * flag-89-flat, drop-unused-k, t-uchar, ok-decl-first, decl-r0-before-r1,
 * shift-in-load-stmt, div-expanded, copy36-one-stmt -- read **exactly 4 on every
 * single row**, 56 rows.  The park's own 16-row sweep was flat for the same
 * reason.  The one dimension that moves this function is the ORDER OF THE TWO
 * `ldrsh` STATEMENTS, and none of those 27 edits touches it.
 *
 * MEASURED ON THE SWAPPED BASE (crossfire depth 2, 10 edits):
 *   6   the swapped base itself
 *   5   `strh [0x3a]` moved after both loads -- scratch 1 becomes r2 (correct)
 *       but scratch 2 becomes r2 again, so the pair is (r2,r2) not (r2,r3)
 *   4   `shift-in-load` and its seven crossings -- but that edit's `new` text
 *       puts the 0x34 read back first, i.e. it UN-SWAPS; this 4 is the base 4,
 *       not a new corner.  Recorded so nobody reads it as progress.
 *
 * WHAT TO TRY NEXT, with the mechanism attached: on the swapped base the defect
 * is the 0x3a store's SCHEDULER PRIORITY, which comes from whichever later insn
 * overwrites its source register r3 -- in the swapped body that is the SECOND
 * scratch `movs r3,#0x34` instead of the first, so the store inherits a lower
 * priority and sinks.  (The same mechanism, in the same bank, is what is left of
 * Func_807808c -- see src/non_matching/rom_77000/807808c.c.)  The question is
 * therefore how to lift a store whose source register nothing overwrites early,
 * NOT how to persuade local-alloc to swap two registers.
 *
 * FREE CORRECTNESS DIVIDEND, measured exactly inert: the real callee signatures
 * in this tree are `int GetPartySize(void)` (src/rom_77000/rom_79460_b.c:10),
 * `int GetFlag(int idx)` (src/rom_77000/rom_79338_a.c:68) and
 * **`void *GetUnit(unsigned int id)`** (src/rom_77000/rom_77320_a_a_c_c_a_b.c:136).
 * The park declared `GetUnit(int)`; corrected below.
 *
 * The typed-struct rewrite the park records as byte-identical still is; it
 * remains worth adopting on code-quality grounds whenever this lands.
 */
extern int GetPartySize(void);
extern void *GetUnit(unsigned int id);
extern int GetFlag(int id);
extern unsigned char gState[];
extern unsigned char L7a828[] __asm__("_TBL_7a828");

void Func_8078144(void)
{
    void *r5;
    int r0;
    int r1;
    int r3;
    int i;
    int n;
    int k;
    int id;
    int t;
    int ok;

    n = GetPartySize();
    for (i = 0; i < n; i++) {
        id = gState[(0xfc << 1) + i];
        t = L7a828[id];
        ok = 0;
        if (t == 0) {
            if (GetFlag(0x88 << 1) != 0) {
                ok = 1;
            } else if (GetFlag(0x89 << 1) != 0) {
                ok = 1;
            }
        } else {
            if (GetFlag(0x111) != 0) {
                ok = 1;
            } else if (GetFlag(0x113) != 0) {
                ok = 1;
            }
        }
        if (ok != 0) {
            r5 = GetUnit(id);
            r3 = *(unsigned short *)((char *)r5 + 0x36);
            *(unsigned short *)((char *)r5 + 0x3a) = r3;
            r1 = *(short *)((char *)r5 + 0x34);
            r0 = *(short *)((char *)r5 + 0x38);
            r0 <<= 14;
            r0 /= r1;
            r3 = 0x80;
            r3 <<= 7;
            if (r0 > r3) {
                r3 = 0x80 << 7;
            } else {
                if (r0 < 0) {
                    r3 = 0;
                } else {
                    r3 = r0;
                }
            }
            *(short *)((char *)r5 + 0x14) = r3;
            if ((r3 << 16) != 0) {
                goto label_0x3a;
            }
            r3 = *(short *)((char *)r5 + 0x38);
            if (r3 == 0) {
                goto label_0x3a;
            }
            r3 = 1;
            *(short *)((char *)r5 + 0x14) = r3;
        label_0x3a:
            r0 = *(short *)((char *)r5 + 0x3a);
            r1 = *(short *)((char *)r5 + 0x36);
            r0 <<= 14;
            r0 /= r1;
            r3 = 0x80;
            r3 <<= 7;
            if (r0 > r3) {
                r3 = 0x80 << 7;
            } else {
                if (r0 < 0) {
                    r3 = 0;
                } else {
                    r3 = r0;
                }
            }
            *(short *)((char *)r5 + 0x16) = r3;
            if ((r3 << 16) != 0) {
                goto label_next;
            }
            r3 = *(short *)((char *)r5 + 0x3a);
            if (r3 == 0) {
                goto label_next;
            }
            r3 = 1;
            *(short *)((char *)r5 + 0x16) = r3;
        }
    label_next:
        ;
    }
}
