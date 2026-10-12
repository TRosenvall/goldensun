/* DecodeMetatileset -- 0x0800f9f4.  *** MATCHING, byte-identical. ***
 *
 * LANDED batch 342 brief C.  objcmp verbatim, re-derived on this body:
 *     OK whole file -- 152 bytes, 73 encodings and 3 relocations identical
 *
 * *** REQUIRES GCSE_CFLAGS (-fno-gcse). ***  The flag is a per-file group that
 * already exists at Makefile:334 with several members; this file needs a rule
 * added beside them.  NOT a device and NOT a pin -- shimcount reports no rows.
 *
 * THE PARK IT REPLACES read 24 of 73 STREAM encodings, 70/70 16-bit
 * instructions (IN PHASE, a true distance), size 152/152, relocations
 * identical, PINS 0.  Both figures re-derived at the head of this batch and
 * both confirmed exactly; the batch-335 brief-D body and figure were right.
 *
 * INSTALL AS:  src/rom_9000/rom_f9cc_a_c_b.c
 *
 * NO SPLIT.  tools/piece.py: `asm/rom_9000/rom_f9cc_a_c_b.s -- 1 function(s)`,
 * so this converts the whole TU.  tools/datacheck.py exits 0 with no output.
 *
 * Verify with (AFTER the Makefile rule is added, on the installed path):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_9000/rom_f9cc_a_c_b.c \
 *     asm/rom_9000/rom_f9cc_a_c_b.s --whole
 * Verify BEFORE it (what was actually run to land it -- objcmp reads the
 * Makefile for the reference's flags, so the flag must be injected by hand):
 *   docker run ... -e OBJCMP_EXTRA=-fno-gcse goldensun-build \
 *     python3 tools/objcmp.py <this file> \
 *     asm/rom_9000/rom_f9cc_a_c_b.s --func DecodeMetatileset
 *
 * *** A DUPLICATE PARK MUST BE RETIRED WITH THIS LANDING. ***
 * src/non_matching/rom_9000/**80f9f4**.c (note the single 0) is a SECOND park
 * for this same function, written by the batch-338 prose-park recovery from
 * `scratch/hold_f9f4.c` without noticing that batch 335 brief D had already
 * installed src/non_matching/rom_9000/**800**f9f4.c.  The stale one has no
 * figure ("objcmp stops at the size check", ref 152 against ours 156) and a
 * completely different body.  **tools/piece.py resolves the reference to the
 * STALE file**, which is why this target was presented to this brief as
 * "(no claim)" when the real park sat at a verified 24 of 73.  Same class as
 * the duplicate batch 341 retired.  BOTH files must go.
 *
 * ======================================================================
 * WHAT CLOSED IT: FOUR LEVERS, NONE WORTH ANYTHING ALONE, AND EVERY PROPER
 * SUBSET IS WORSE THAN THE PARK AT DEFAULT FLAGS
 * ======================================================================
 *
 *   F  -fno-gcse
 *   P  a SECOND accumulator local, `prev2`, used by mode 2 only
 *   O  the three initial definitions in the order s, d, count
 *      (the park had count, s, d)
 *   R  mode 1's preheader as `i = 0; prev = 0; b2 = b + count;`
 *      (the park had b2 first -- its own recorded lever 3)
 *
 * MEASURED, every row ref 73 / ours 73 encodings, size 152 both, relocations
 * clean, so these are flat figures and not misalignment:
 *
 *            default flags   -fno-gcse
 *   (park)        24             24      <- F ALONE IS EXACTLY INERT
 *   R             22             22
 *   O             31             31
 *   O+R           50             29
 *   P             47             16      <- P alone is 23 WORSE than the park
 *   P+R           48             15
 *   P+O           41              2
 *   P+O+R         42          *** 0 ***
 *
 * Read the last two rows twice.  **P+O+R at default flags is 42, nearly double
 * the park**, and -fno-gcse on the parked body is BIT-IDENTICALLY INERT.  A
 * one-axis search cannot see this: the flag looks dead, the source change looks
 * like a bad regression, and together they are byte-identical.  This is the
 * sharpest instance of the crossing rule the project has: autosweep's flag
 * axis is a CROSSING PARTNER, not a verdict, and `-fno-gcse` reading 24 of 73
 * on the park was the single most misleading number in the file.
 *
 * ======================================================================
 * WHY -fno-gcse, AND THE PARK NAMED THE WRONG PASS
 * ======================================================================
 *
 * The park's root-cause paragraph said: "The ROM's mode-1 `b = ewram_2010002`
 * is a FRESH POOL LOAD ... where ours is a copy of the halfword pointer ...
 * cse1 substitutes the available register for the symbol_ref because a pseudo
 * is cheaper than a CONSTANT_P source, and nothing below reaches that choice."
 * The OBSERVATION is exact.  The ATTRIBUTION is wrong, and so was the question
 * it generated ("NEXT: ONE question -- what stops cse substituting the halfword
 * pointer into mode 1's pool load").
 *
 * IT IS NOT cse AT ALL.  Both pointers are loaded from the SAME literal-pool
 * slot `*.LC0`, and counting that slot's loads per dump pass
 * (scratch_elev/b342/C/c_dump.sh with `symbol_ref/u:SI \("\*\.LC0"\)`) gives:
 *     .00.rtl 2   .02.jump 2   .03.cse 2   .04.addressof 2   .07.gcse 2
 *     .08.loop 1   .09.cse2 1   ... .25.jump2 1
 * *** TWO SURVIVE cse1 AND gcse AND BECOME ONE AT .08.loop. ***  The loop pass
 * rewrites it, and its own dump says why -- `Biv 37 initialized at insn 80:
 * initial value (symbol_ref:SI ("ewram_2010002"))`.  Reg 37 is `b`, a basic
 * induction variable of mode 1's loop, and at .08.loop insn 80 has become
 *     (insn 80 228 83 (set (reg/v:SI 37) (reg/v:SI 35)) 173 {*thumb_movsi_insn})
 * i.e. the copy, where at .07.gcse it was still
 *     (set (reg/v:SI 37) (mem/u/f:SI (symbol_ref/u:SI ("*.LC0")) 8)).
 * So the park's "cse1" is three passes early and `-fcse-follow-jumps`, the
 * obvious lever for a cse table that survives into a switch arm, is measured
 * INERT (below).  `-fno-gcse` works because gcse is what makes the two loads
 * provably the same value for loop's biv machinery; with it off the pool load
 * SURVIVES and the emitted mode-1 preheader is the ROM's
 * `ldr r6, =ewram_2010002`, counted directly as two bare-pool `ldr`s against
 * one.  `-O1` also gives two, which is why the park's -O1 row at 32 was never
 * purely a regression.
 *
 * AND THE PARK'S "SINGLE CONFLICT FORCES THE WHOLE PERMUTATION" IS FALSE.
 * With -fno-gcse alone the copy is gone, the fresh pool load is in place, and
 * the register permutation does not move AT ALL -- 24 of 73 with byte-identical
 * register assignments (count r6, s r2, b r7, b2 r4, prev r0 in both arms).
 * The copy was never the root; it is one encoding, and it was already counted
 * among the 24.  The root is the ALLOCATION, and the three source levers above
 * are what reach it:
 *   P  the ROM puts mode 1's accumulator in r7 and mode 2's in r2 -- TWO
 *      DIFFERENT HARD REGISTERS FOR ONE SOURCE NAME, which is only possible if
 *      the ROM has TWO locals.  That is the batch-340 Func_80a33d4 result (a
 *      piece-mate of this brief's target 3) in its purest form: one name is
 *      commoned, two distinct ones are not.  The park's own table has
 *      "two separate accumulators, one per mode  +19 to +28" -- it measured the
 *      right edit and threw it away because it was tested alone.
 *   O  takes `count` from r5 to r4 and `d` from r4 to r5, the ROM's pair.
 *      REG_ALLOC_ORDER (arm.h:989-995) is {3,2,1,0,12,14,4,5,6,7,...}, so r4
 *      precedes r5 and whichever allocno sorts first takes it; the only source
 *      handle is WHERE THE DEFINITION SITS (batch 339 brief D, global.c:598-620
 *      scoring floor_log2(n_refs)*n_refs/live_length).  The park's
 *      "all ten declaration orders of the eight locals INERT, every one" is
 *      correct AND IRRELEVANT -- declaration order is not definition order, the
 *      distinction batch 339 had to make twice.
 *   R  the last two encodings, `mov r7,#0` against `add r0,r4,r6`: both are
 *      scheduling-region leaves in mode 1's preheader, so sched2 ranks them on
 *      INSN_LUID alone (haifa-sched.c:4029ff) and source order decides.  Three
 *      of the six permutations of {b2, i, prev} are 0 (any order with `prev`
 *      before `b2`), and the park's recorded lever 3 -- "mode 1 initialises its
 *      POINTERS BEFORE its SCALARS" -- is the one that is NOT.
 *
 * ======================================================================
 * ALSO MEASURED THIS BATCH, ALL INERT OR WORSE
 * ======================================================================
 * FLAGS BEYOND THE EIGHT GROUPS, on the parked body (c_flagprobe.py):
 *   -fno-cse-follow-jumps                24   inert
 *   -fno-cse-skip-blocks                 24   inert
 *   both together                        24   inert
 *   -fno-thread-jumps                    24   inert
 *   -fno-force-mem                       24   inert
 *   -fno-caller-saves                    24   inert
 *   -fno-strength-reduce                 24   inert, and it does NOT restore
 *                                             the pool load (still one load)
 *   -fno-reduce-all-givs                 24   inert, same
 *   -fno-move-all-movables               24   inert, same
 *   -fno-regmove                         26   worse
 *   -fno-expensive-optimizations         28   worse
 * SOURCE, crossed against -fno-gcse (c_sweepflag.py), all 73/73 and clean:
 *   split `i` per mode                   47 / 47     (and -4 bytes)
 *   split `i`, `t`, `prev` all per mode  47 / 47     (and -4 bytes)
 *   split `t` for mode 2 only            24 / 24     inert
 *   split `s` for mode 2 only            24 / 24     inert
 *   split `d` per mode                   59 / 59     (and +4 bytes)
 *   `count` computed after s and d but
 *     before the `if`                    31 / 31
 *   `(n - 1) >> 1` for the divide        69 / 69     (and -4 bytes)
 *   ONE pointer local for all modes      50 / 31
 *   ONE pointer local + split prev       47 / 17
 *   ONE short* pointer with byte casts   50 / 31
 *
 * AUTOSWEEP (tools/autosweep.py, run FIRST; raw in scratch_elev/b342/C):
 *   PARKED BODY: FLAG AXIS -fno-gcse 24, -fno-strict-aliasing 24,
 *   -fno-strength-reduce 24, -fno-rerun-cse-after-loop 24, -fno-rerun-loop-opt
 *   24 (inert); -O1 32, -fno-schedule-insns2 32, -ffixed-r7 55 (worse).
 *   ANCHOR AXIS 34 boundaries, NOT capped, 2 produced no row, the other 32 all
 *   24 or worse.  *** AND THE FLAG ROW THAT LOOKED DEADEST IS THE ONE THAT
 *   LANDS IT. ***
 *
 * KEPT FROM THE PARK -- its four crossed levers are ALL still load-bearing and
 * none was changed here: the explicit `t` in mode 1, the explicit `t` in mode 2,
 * mode 1's `hi` and `lo` temps, and `b2 = b + count` (rather than the inline
 * expression).  Its alignment-pad warning also still stands and is why this
 * header gives the 16-bit instruction count separately.
 *
 * PASS 4 (humanization): `prev` / `prev2` are placeholder names and the real
 * source almost certainly called them something per-mode; the REQUIREMENT is
 * only that mode 1 and mode 2 use two distinct automatics.  `s`/`d`/`b`/`b2`
 * likewise.  Recorded for docs/directory/03-humanize.
 */
extern unsigned char ewram_2010001;
extern unsigned char ewram_2010002[];
extern unsigned short ewram_2020000[];

void DecodeMetatileset(int n)
{
    int count;
    int i;
    unsigned short *s;
    unsigned short *d;
    unsigned char *b;
    unsigned char *b2;
    int prev;
    int prev2;
    int t;
    int hi;
    int lo;

    s = (unsigned short *)ewram_2010002;
    d = ewram_2020000;
    count = (n - 1) / 2;
    if (n & 1) {
        switch (ewram_2010001) {
        case 0:
            for (i = 0; i < count; i++) {
                *d = *s;
                s++;
                d++;
            }
            break;
        case 1:
            b = ewram_2010002;
            i = 0;
            prev = 0;
            b2 = b + count;
            while (i < count) {
                hi = *b;
                lo = *b2;
                t = ((hi << 8) | lo) ^ prev;
                i++;
                *d = t;
                b2++;
                b++;
                d++;
                prev = t;
            }
            break;
        case 2:
            prev2 = 0;
            if (count > 0) {
                i = count;
                do {
                    t = *s ^ prev2;
                    i--;
                    *d = t;
                    s++;
                    d++;
                    prev2 = t;
                } while (i != 0);
            }
            break;
        }
    }
}
