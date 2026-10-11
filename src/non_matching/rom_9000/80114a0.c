/* Func_80114a0 -- NON-MATCHING, 55 encodings of 97 differ (objcmp: ref 97 / ours 95,
 * ref 200 bytes / ours 192).  tryc: rom 100 lines, ours 98, 53 differ.
 *
 * Verify with:
 *   python3 tools/objcmp.py /tmp/claude-0/-home-user-goldensun/ad08b1ee-c1a0-56c8-b3c2-c0ff6481844c/scratchpad/L/Func_80114a0.park.c \
 *     asm/rom_9000/rom_108e4_c.s --func Func_80114a0
 *
 * FRESH TARGET (batch 286).  The streaming twin of Func_80113e4 (same file, not yet
 * elevated either): same 2x2x2 quadrant walk, force=0, return on the first load.
 *
 * WHAT IS RIGHT: prologue, the ldmia pos read (`x = *p++; z = p[1];` -- the p[0]/p[2]
 * spelling gives two plain ldr), the two signed >>25 biases (x - 0x1000000 must be
 * written with a signed constant; 0xff000000 is unsigned and gives lsr), the
 * struct-member table read `state->quad[idx]` (gives the ROM's
 * lsl/add #0x138/ldrh [state, off] association; a u8* + 0x138 cast gives
 * (idx*2 + state) + 0x138 instead), and the z giv: writing `z + j` in BOTH the call
 * and the index makes loop.c reduce it into r6 and leave the ROM's `mov r8, r6` copy.
 *
 * BLOCKER: THE 0xF MASK CONSTANT.  ROM rebuilds `mov #0xf` in the k-loop body and at the
 * j-level (with the hoisted row), never in a register.  Ours: cse (pass 03) merges the
 * row's and the column's 15 into one pseudo; loop.c moves it with the row to the j
 * preheader, then (life 30, "halved since already moved") on to the outermost preheader
 * in r11.  That steals the high register the ROM spends on its bias copy, so bias stays
 * a plain register giv in r9 (the ROM keeps it at [sp+4] and copies it into r11 at each
 * layer), and the j counter lands in r8/r10 differently.  Writing the row at j-level instead keeps the
 * k-loop's 15 separate ("not desirable") BUT combine_movables (loop.c) merges the two
 * equal-constant movables in the j-loop scan (savings 2), moves them, and cse2 folds the
 * k-loop one into it.  So neither placement reproduces the ROM; the ROM's two 15s were
 * never unified, which no spelling tried here achieves.
 *
 * INERT / WORSE (measured): layer*0x140 inline (80 differ), bias as a user var
 * incremented (84), `zq` as its own biv (84), row local at j-level (77-84), all-goto
 * loops with explicit copies (88; copies coalesce), 0xff000000 constant (unsigned).
 *
 * ============ BATCH 334, BRIEF C: RE-MEASURED, AND THE SAME PIECE-LEVEL
 * ============ BLOCKER AS ITS PIECE-MATE
 *
 * RE-MEASURED and the claim line at the top of this file REPRODUCES: 55 of 97
 * encodings, ref 97 / ours 95, instructions ref 94 / ours 92, size ref 200 bytes
 * / ours 196.  COUNT DIFFERS, so that positional figure measures MISALIGNMENT and
 * the honest statement is WE ARE TWO 16-BIT INSTRUCTIONS SHORT.  Read the count
 * before the figure; the figure is not a distance here and the real distance is
 * unknown until the lengths agree.
 *
 * ==================== THE PIECE CANNOT BE LANDED AT ALL YET ====================
 *
 * asm/rom_9000/rom_108e4_c.s holds SEVEN functions and ALL SEVEN ARE PARKED --
 * Func_8010e14, Func_8010ff0, Func_80110e0, Func_8011164, Func_80111b4,
 * Func_80113e4 and this one.  A .c is one TU, so a match here installs nothing
 * without a split.  Its piece-mate Func_8011164 is also parked two instructions
 * short and is NOT adjacent to this function, so taking both off the count needs
 * TWO byte-neutral cuts of the same .s plus a linker-script change.  Schedule the
 * two as a pair, not as standalone singles.
 *
 * ALSO: the header above names Func_80113e4 as "the streaming twin of this
 * function (same file, not yet elevated either)".  That is still true, and it is
 * a THIRD member of the same piece -- so a split that carves out this function
 * and Func_8011164 leaves the twin behind in the remainder, where it stays
 * reachable as a separate target.  The twin is the first thing to read before any
 * further work here: the brief's standing rule is that when a park names another
 * function, that function's source is read first, and in this piece every named
 * relative is still a park, which is itself the reason this one has had no
 * labelled-corpus neighbour to query.
 *
 * >> ON THE STATED BLOCKER.  The 0xF-mask story above is an observation about
 *    where two equal constants are unified, and it is NOT yet an account of the
 *    two-instruction deficit -- the header never says which two instructions are
 *    missing, only which register the ROM spends differently.  The first thing a
 *    next round should produce is the DECOMPOSITION: count the ROM's and our
 *    instructions inside each of the three nested loops and in the straight-line
 *    head and tail separately, and say which region is short.  Func_8011164's
 *    header in this same piece is the model for that, and on a body of this size
 *    tools/aligncmp.py's hunk list gives it directly.  Without it, every mask
 *    spelling measured here is being screened against a figure that cannot move
 *    until the lengths do.
 *
 * ============ BATCH 336, BRIEF F: 55 of 97 HELD.  THE DECOMPOSITION THIS
 * ============ HEADER ASKED FOR, AND IT REFUTES THE STATED BLOCKER'S SIGN.
 *
 * RE-DERIVED FIRST, in stream terms: 55 of 97 (ref 97 encodings, ours 95),
 * instructions 94/92, SIZE ref 200 bytes against ours 196, relocations
 * differing only by the 4-byte shift the deficit causes (Func_80108e4 call at
 * the same offset, iwram_3001e70 word 4 bytes earlier).  COUNT DIFFERS, so the
 * 55 measures MISALIGNMENT -- we are TWO 16-BIT INSTRUCTIONS SHORT.
 * tools/aligncmp.py: 63 of 97 aligned-equal (64.9%), 38 differing/ins/del in
 * 18 hunks.
 *
 * ==== THE DECOMPOSITION, counted off both streams, loops split by their
 * ==== branch targets (ROM entries 0x46 / 0x50 / 0x5c)
 *
 *   region                                         ROM  ours  delta
 *   prologue + iwram_3001e70 + frame + if (p)
 *     + both >>25 biases            (ROM 0-31)      32    32     0
 *   layer-loop SETUP               (ROM 32-34)       3     5    +2
 *   layer-loop TOP        (0x46)   (ROM 35-39)       5     3    -2
 *   j-loop top            (0x50)   (ROM 40-45)       6     6     0
 *   k-loop body           (0x5c)   (ROM 46-69)      24    25    +1
 *   j-loop increment               (ROM 70-75)       6     6     0
 *   layer-loop increment           (ROM 76-85)      10     7    -3
 *   epilogue                       (ROM 86-94)       9     9     0
 *                                                   95    93    -2
 * (95/93 are the objdump streams: 94/92 sixteen-bit insns plus the one 32-bit
 * bl.  Add the 2 pool words for objcmp's 97/95.)
 *
 * ==== IT IS ONE ALLOCATION DECISION: WHICH VALUE GETS SPILLED ====
 *
 * Both bodies have FIVE long-lived values and FOUR callee-saved high registers.
 * r4 is NOT saved by this prologue, which pushes only {r5,r6,r7,lr} + {r5,r6,r7}
 * + {r7} -- that is why landing a value in r4 costs a frame round-trip.
 *
 *   value             ROM                             ours
 *   layer counter     r9                              sl
 *   j counter         sl                              r8
 *   z + j copy        r8                              r4  -> SPILLED round the call
 *   bias (the         THE FRAME, [sp,#4], copied       r9
 *     layer*0x140       into fp once per layer
 *     giv)
 *   the 0xF mask      NO REGISTER -- `movs r1,#15`    fp
 *                       / `movs r2,#15` rebuilt
 *                       in the loops
 *
 * ROM bias:  `movs r3,#0` / `str r3,[sp,#4]` (setup); `ldr r2,[sp,#4]` /
 *            `mov fp,r2` (layer top); `ldr r2,[sp,#4]` ... `adds r2,r2,r3` /
 *            `str r2,[sp,#4]` (layer inc).  A giv maintained IN MEMORY.
 * our bias:  `mov r9,r3`; `add r3,r9`; `add r9,r2`.  The same giv IN A REGISTER.
 *
 * THE THREE COUPLINGS SUM TO EXACTLY -2, AND THEIR SIGNS ARE THE POINT:
 *   bias in a register instead of the frame -- no layer-top reload+copy,
 *     no layer-inc load/store                                        -5
 *   the 0xF mask in fp instead of rebuilt -- `movs r1,#15` + `mov fp,r1`
 *     in the setup                                                   +2
 *   z + j displaced to caller-saved r4 -- `str r4,[sp,#4]` /
 *     `ldr r4,[sp,#4]` round the bl                                  +1
 *                                                                    -2
 *
 * ** >> THE STATED BLOCKER HAS THE WRONG SIGN, AND THAT IS THIS BATCH'S
 *    FINDING.  The header above names "THE 0xF MASK CONSTANT" as the blocker
 *    and treats keeping the two 15s separate as the goal.  THE MASK HOIST IS
 *    NOT THE DEFICIT -- IT IS PARTIALLY COMPENSATING FOR IT.  Remove it with
 *    nothing else changed and fp frees up, z+j returns to a high register, and
 *    BOTH the r4 spill pair AND the `movs #15`/`mov fp,r1` pair disappear --
 *    leaving the body at ROM - 5, i.e. THREE INSTRUCTIONS WORSE.
 *
 *    The load-bearing fact is the one this header mentions only in passing:
 *    THE ROM SPILLS THE bias GIV TO [sp,#4] AND WE DO NOT.  That single
 *    decision is worth +5; the mask hoist (-2) and the r4 spill (-1) are its
 *    knock-on consequences, because once bias occupies r9 the fourth high
 *    register is contested by three claimants instead of two. <<
 *
 *   >> NAMED CAUSE, REPLACING THE MASK STORY: make the layer*0x140 giv lose its
 *      global allocno so greg gives it the FRAME, and the other four values
 *      take r8/r9/sl/fp the way the ROM's do.  ADDING PRESSURE CANNOT DO IT --
 *      the ROM has the SAME five values, not more -- so this is an
 *      allocno_compare / find_reg ordering question about which of five
 *      equal-pressure claimants loses.  From .18.greg on this body:
 *        ;; 12 regs to allocate: 36 39 70 54 50 38 37 72 52 34 32 35
 *        dispositions: 37 in 10 (sl)  38 in 8  52 in 11 (fp)  72 in 9  50 in 4
 *        32, 34 and 35 appear in NO disposition -- they are the three values
 *        the ROM also keeps in the frame at [sp,#8] / [sp,#12] / [sp,#16],
 *        so that part of the allocation already agrees. <<
 *
 * ==== AND THIS HEADER'S WHOLE NEGATIVE LIST WAS SCREENED WITH THE WRONG
 * ==== INSTRUMENT, WHICH IS WHY IT CARRIES NO INFORMATION
 *
 * `layer*0x140 inline (80 differ)`, `bias as a user var incremented (84)`,
 * `zq as its own biv (84)`, `row local at j-level (77-84)`, `all-goto loops
 * (88)` -- EVERY ONE of those figures is a positional count on a stream whose
 * length differs from the reference, so each measures MISALIGNMENT and 80 vs 84
 * vs 77 says nothing about whether the edit helped.  This header says exactly
 * that about its own headline figure and then ranks its candidates on the same
 * broken scale.  Re-screening needs tools/aligncmp.py, which aligns first.
 *
 * RE-SCREENED THIS BATCH, 18 rows crossed 3 x 2 x 3 (bias-form x z+j-form x
 * mask-form), every row through BOTH tools.  Base: ndiff 55, 94/92 insns,
 * aligned-equal 63 (64.9%).  NOTHING BEATS THE BASE ON THE ALIGNED SCALE:
 *   bias as an accumulator declared outside the layer loop, alone      54, 63
 *   bias inline `layer * 0x140` at the use site                 80, 94/97, 55
 *     (NOTE: this one is ROM + 3, i.e. it OVERSHOOTS the length)
 *   named `rz`/`cx` mask locals (any bias form)              83-84, 94/88, 47
 *     (FOLDS: loses 4 instructions)
 *   named `zj = z + j` at j level (any bias form)            75-84,        47-48
 * So the mask spelling, the bias spelling and the z+j spelling are all inert or
 * harmful, consistent with the settled fact that only WHICH PSEUDOS ARE LIVE
 * moves an allocation.
 *
 * INSTRUMENT WARNING, AND IT INVALIDATES THE OBVIOUS NEXT PROBE: -ffixed-rN IS
 * NOT A USABLE INSTRUMENT ON THIS FUNCTION.  Denying any high register makes
 * the body FOUR ENCODINGS SHORTER, not longer, because the unused register
 * drops out of the prologue's `mov r7,fp` / `push` / `mov fp,r7` save-restore
 * block -- so the flag changes the very region the measurement is about.
 * Measured: -ffixed-r11 95 diffs 97/91 aligned 55; -ffixed-r10 94 / 91 / 56;
 * -ffixed-r8 92 / 91 / 59; -ffixed-r9 93 / 91 / 54.  None is comparable to the
 * production 55 / 95 / 63.
 *
 * ==== PIECE STATUS RE-VERIFIED, AND THE "PAIR" FRAMING IS WRONG ====
 *
 * asm/rom_9000/rom_108e4_c.s holds SEVEN .thumb_func_start and ALL SEVEN ARE
 * STILL PARKED; verified against src/non_matching/rom_9000/ and
 * docs/directory/01-parked.md, and none has a landed .c.  The batch-334
 * section above is CORRECT about that and its advice to schedule the two as a
 * pair is NOT.
 *
 * The piece is ALREADY sub-split into _a / _b / _c (stage1.ld:228-230 for
 * .text, :299-301 for .rodata).  This function is member 7 of 7 -- THE LAST --
 * so it needs exactly ONE cut to become its own TU, while Func_8011164 is
 * interior and needs TWO.  Once cut, each is its own TU and matches or fails
 * INDEPENDENTLY.
 *
 *   >> CORRECTED SCHEDULING STATEMENT: TWO INDEPENDENT SINGLES, not a pair.
 *      This one is the CHEAPER of the two to install (one byte-neutral
 *      tools/split_s.py cut plus a stage1.ld edit) and should not be gated on
 *      Func_8011164.  Landing both needs THREE cuts, not the two stated above:
 *      members 1-3 asm | 8011164 | members 5-6 asm | 80114a0. <<
 *
 * Func_80113e4, named above as this function's streaming twin, is member 6 and
 * so stays in the asm remainder under either cut -- still reachable as a
 * separate target, and still a park, so it remains unavailable as a
 * labelled-corpus neighbour.
 */
extern unsigned char *iwram_3001e70;
extern int Func_80108e4(int layer, int qx, int qz, int tileset, int force);

struct WorldMapState {
    int *pos;
    unsigned char pad[0x134];
    unsigned short quad[256];
};

void Func_80114a0(void)
{
    struct WorldMapState *state = (struct WorldMapState *)iwram_3001e70;
    int x = 0, z = 0;
    int *p;
    unsigned int layer, j, k;

    p = state->pos;
    if (p) {
        x = *p++;
        z = p[1];
    }
    x = (x - 0x1000000) >> 25;
    z = (z - 0x1400000) >> 25;
    for (layer = 0; layer <= 1; layer++) {
        int bias = layer * 0x140;
        for (j = 0; j <= 1; j++) {
            for (k = 0; k <= 1; k++) {
                if (Func_80108e4(layer, x + k, z + j,
                        state->quad[(((z + j) & 15) << 4) + ((x + k) & 15)] + bias, 0))
                    return;
            }
        }
    }
}
