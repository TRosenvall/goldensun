/* Func_8078144 (0x08078144) -- asm/rom_77000/rom_77320_a_c_c.s (3 functions).
 *
 * PARK HELD AT 4 differing encodings of 103.  SIZE EXACT (103 against 103),
 * per-opcode memory profile the reference's exactly
 * (ldr=4 ldrb=2 ldrh=1 ldrsh=6 strh=5).  Figure re-derived in batch 327 G.
 * The figure is unchanged; THE MECHANISM IS NOW COMPLETE AND VALIDATED BY AN
 * INSTRUMENT, and it names a corner at 3 that nothing has reached.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_77000/8078144.c asm/rom_77000/rom_77320_a_c_c.s --func Func_8078144
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
 * (unchanged, and re-confirmed this batch).  All nine relocations sit at
 * identical offsets; the only delta is `.L7a828` (ref) against `_TBL_7a828`
 * (ours) at 0xd8.  `.L7a828` is an assembler-local label in
 * asm/rom_77000/rom_77320_c_c_c_b.s that our tree has already promoted to
 * `.global` because the split put its user in another object -- one of 1,544
 * such promotions.  A C file cannot name a `.L` symbol, and in the original
 * object a same-object local reference is SECTION-relative.  **The 4 IS a
 * distance.  Stop reporting the relocation line as a difference.**
 *
 * ---------------------------------------------------------------------------
 * THE FOUR ENCODINGS ARE A RELOAD CURSOR, AND THE CURSOR IS ONE NUMBER
 *
 * Indices 42-45, two `ldrsh`es and their two scratch `mov`s:
 *
 *      ref                          ours
 *  47  movs  r2, #0x38              movs  r3, #0x38
 *  48  ldrsh r0, [r5, r2]           ldrsh r0, [r5, r3]
 *  49  movs  r3, #0x34              movs  r2, #0x34
 *  50  ldrsh r1, [r5, r3]           ldrsh r1, [r5, r2]
 *
 * The scratch is `*thumb_extendhisi2_insn`'s `(clobber (match_scratch:SI 2
 * "=&l"))` (arm.md:3239-3242) -- a RELOAD register, so `allocate_reload_reg`
 * (reload1.c:4962) decides it, round-robin from `last_spill_reg` (:5003).
 * `last_spill_reg` is initialised to -1 once per function (:821) and set ONLY on
 * acceptance (:4937), so **the cursor is FUNCTION-WIDE.**
 *
 * This function's six ldrsh scratches, read out of `.19.flow2`, are a PERFECT
 * round robin over the ascending spill set [r1, r2, r3] (n_spills 3):
 *
 *   insn 141 0x34=r2 | 146 0x38=r3 | 220 0x38=r1 | 242 0x3a=r2 | 247 0x36=r3 | 321 0x3a=r1
 *
 * i.e. r2, r3, r1, r2, r3, r1, with the cursor sitting at idx(r1)=0 when reload
 * reaches insn 141.  Insn 29's reload is the only earlier ACCEPTANCE and it took
 * r1 ("Spilling for insn 136" prints with NO "Using reg" line, i.e. accepts
 * nothing).  Read figures off `.19.flow2`, never off `.18.greg`: find_reg's
 * `Using reg` printf says 3 for BOTH 141 and 146 and the emitted pair is (r2,r3).
 *
 * THE ROM WANTS 0x38 -> r2 and 0x34 -> r3, i.e. (r3, r2) for insns (141, 146).
 * That IS reachable from the round robin: with the cursor one step further on,
 * 141 takes r3; then 146 wraps to r1, which is NOT free (141 has just set r1 and
 * it is live as the divisor), and takes r2.  Groups 2 and 3 then stay put,
 * because r3 is insn 220's own DESTINATION and the scratch constraint is "=&l"
 * (earlyclobber), so 220 cannot take r3 and the cursor re-synchronises.
 *
 * MEASURED WITH AN INSTRUMENT (labelled; NOT a proposal).  Read 0x36 as
 * `*(short *)` so that read becomes an ldrsh and consumes one extra accepted
 * reload before insn 141.  Clobbers come out
 *   134:0x36=r2 143:0x34=r3 148:0x38=r2 222:0x38=r1 244:0x3a=r2 249:0x36=r3 323:0x3a=r1
 * -- the prediction exactly, re-synchronisation included.  Its figure is 59,
 * which is a figure ABOUT THE BLOCKER: the instrument costs the 0x36 encoding
 * and shifts the stream.
 *
 * **WHAT THE ROTATED CURSOR WOULD BE WORTH: 3, NOT 0.**  Traced through the
 * scheduler: with (141 0x34=r3, 146 0x38=r2) the store's anti dependence moves
 * from 146 (prio 36) to 141 (prio 34), so the store drops to 34 and sinks ONE
 * place below the 0x38 load, giving `movs r2,#0x38 / ldrsh r0 / strh [0x3a] /
 * movs r3,#0x34 / ldrsh r1 / lsl r0,#14` -- three positions rotated against the
 * ROM.  So the whole function is a FOUR-CORNERED knot at 3/4/5/6 and 0 is not
 * among the corners.  See the bound below.
 *
 * ---------------------------------------------------------------------------
 * WHY THE BASE'S MATCHING STORE POSITION AND ITS WRONG REGISTERS ARE ONE FACT
 *
 * `.23.sched2` block 11, call __divsi3 priority 33:
 *   132 r3=zxn([r5+0x36])  38 | 136 [r5+0x3a]=r3  36 | 141 0x34-ldrsh  34
 *   146 0x38-ldrsh  36 | 149 r0=r0<<0xe  34 | 155 call  33
 * Insn 136's dependent list is `169 445 155 146`: the store depends on **146**,
 * the 0x38 load, purely because in OUR base 146 CLOBBERS r3.  Anti, cost 0,
 * exact transfer.  **The store is held in place BY the wrong register
 * assignment.**  A load feeding the call directly gets cost 1 (141 = 33+1);
 * a load feeding a non-call gets its real cost 2 (146 = 34+2).
 *
 * THE BOUND -- the same one that closes Func_807808c and Func_8077f70 in this
 * object, derived in full in src/non_matching/rom_77000/807808c.c:
 * `arm_adjust_cost` (arm.c:2430-2432) returns 1 for any true dependence whose
 * CONSUMER is a CALL_INSN, so a pre-call store is capped at prio(call)+1, and
 * `rank_for_schedule` returns on the priority rung (haifa-sched.c:4040-4042).
 * In the ROM's register assignment nothing of priority >= 36 overwrites the
 * store's r3, so the store cannot be first.  **The ROM's exact order at this
 * site is not reachable by sched2 under ANY of the four register/LUID
 * combinations.**  Taking that seriously is what makes 4 the right park.
 *
 * ---------------------------------------------------------------------------
 * WHY THE SWAPPED BASE IS 6 -- IT IS THE CLASS RUNG, NOT PRIORITY
 *
 * Swapping the two source statements so the 0x38 read comes first DOES give the
 * ROM's register pair (clobbers 141:0x38=r2, 146:0x34=r3) and reads 6.  Its
 * `.23.sched2` schedules 132, 141(0x38), 136(store), **149(lsl r0)**, 146(0x34),
 * 155.  At the contested cycle 146 and 149 are BOTH priority 34 (both capped by
 * the call rule), so `rank_for_schedule` falls to its `last_scheduled_insn`
 * CLASS rung (haifa-sched.c:4069-4095).  last_scheduled_insn is the store, and
 * 146 IS in INSN_DEPEND(store) by the anti dependence on r3 -> insn_cost 0, not
 * 1 -> class 2, while 149 is unrelated -> class 3.  Higher class wins, so the
 * shift jumps the load.  **The store's own anti dependence demotes the load that
 * should follow it.**  Without that demotion the final rung is INSN_LUID
 * (haifa-sched.c:4115, lower luid preferred) and 146 would go first -- which is
 * the 3-corner above.
 *
 * THE FOUR CORNERS, all 103 instructions against 103:
 *   4   THIS BODY (0x34 read first)        store right, registers wrong
 *   3   base order + rotated reload cursor NOT REACHED; needs one extra accepted
 *                                          reload before insn 141 at zero
 *                                          instruction cost, and none exists
 *   5   swapped + store after both loads   scratches come out (r2, r2)
 *   6   swapped                            registers right, store and shift both wrong
 *
 * MEASURED FLAT THIS BATCH, AND SCREENED BY THE CLOBBERS RATHER THAN BY THE
 * FIGURE -- which is why these rows now mean something.  Ten declaration-level
 * edits (drop-k, id-uchar, id-uint, t-merged-with-ok, decl reorderings,
 * extra-local, r3-declared-last, a block-local temp for the 0x36 value) ALL read
 * 4 and ALL produce the base's clobber list.  Eight index-form edits (idx-flat,
 * idx-i-first 4; idx-via-k 17; idx-ptr-local and idx-base-hoist 96 at 99
 * instructions; loop-ne 7; n-unsigned, i-unsigned 6) move the cursor only AFTER
 * the first group, never before it.  **No declaration-level or index-level edit
 * moves the cursor before insn 141.  The park's earlier flat sweeps were flat
 * for the right reason.**
 *
 * SCHED2_CFLAGS IS REFUTED FOR THE WHOLE OBJECT: OBJCMP_EXTRA=-fno-schedule-insns2
 * gives this function 4 -> 8 with relocations going dirty, Func_807808c 3 -> 8
 * and Func_8077f70 3 -> 17.  Three witnesses in one object.
 *
 * FREE CORRECTNESS DIVIDEND, measured exactly inert: the real callee signatures
 * are `int GetPartySize(void)` (src/rom_77000/rom_79460_b.c:10),
 * `int GetFlag(int idx)` (src/rom_77000/rom_79338_a.c:68) and
 * `void *GetUnit(unsigned int id)` (src/rom_77000/rom_77320_a_a_c_c_a_b.c:136).
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
