/* Func_80f62b8 -- NON-MATCHING, 17 of 191 encodings differ.
 * Unattempted before batch 298.  Reference asm/rom_f6000/rom_f6008_c_a_e_a.s.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_f6000/80f62b8.c \
 *       asm/rom_f6000/rom_f6008_c_a_e_a.s --func Func_80f62b8
 *
 * A TRUE DISTANCE: no SIZE line (392 both) and count 191 == 191, relocations
 * identical.  PIN-FREE.  Text-only split, 2 functions in the file.
 *
 * THIS IS THE AXIS-NAMED BODY, and it is the better NUMBER but not the better
 * ANALYSIS.  The role-named form -- naming the SCANNED and DITHERED coordinates
 * rather than x and y -- measures 28, and four separate pseudos measure 69.
 * Axis-naming wins its 11 encodings BY ACCIDENT: gcc makes one fixed choice (x to
 * r0, y to r1) which is right for the x-major arm, so only the y-major arm is
 * wrong here.  Role-naming makes the whole residue ONE uniform cause.
 *
 * The one cause is PROVED, not asserted: a BLIND textual r0/r1 swap over the two
 * loop regions takes the role-named body from 28 to 6, and `introduced 0` is the
 * proof -- the swap rewrites every r0 and r1 in those ranges regardless of content,
 * so if either register held anything else the count would have RISEN.  The
 * surviving 6 are the x-major divisor block order.  Reconstruct the role-named form
 * by renaming the two walking variables; it is a two-variable change.
 *
 * WHY IT DOES NOT CLOSE, read off the ROM: per arm the scanned coordinate has six
 * instruction references and the dithered one five, and IN-LOOP THEY ARE EQUAL AT
 * FOUR EACH -- the entire difference is the out-of-loop guard compare.  So
 * allocno_compare correctly ranks the scanned one higher, allocates it first, and
 * REG_ALLOC_ORDER {3,2,1,0,...} hands it r1.  ONE REFERENCE APART, which is why none
 * of sixteen spellings reaches it: `if (y0 != y1)` is inert because both names hold
 * the same value, and `while` is inert because loop rotation manufactures the guard.
 *
 * FALSIFIABLE PREDICTION, recorded so it can be struck: rebuilding with
 * REG_ALLOC_ORDER starting {3, 2, 0, 1, ...} should take the role-named body to 6
 * and make THIS body worse, its accidental win becoming an accidental loss.  That
 * makes this function the cheapest instrument in the corpus for HANDOFF.md's open
 * REG_ALLOC_ORDER question -- one clean exchange, no confounding shape difference.
 *
 * CAVEAT on the correction to the sibling park rom_c9000/80cde90.c: its
 * double-counted reference argument is real, but the pseudo-to-axis identities behind
 * that reading are INFERRED from conflict sets, `preferences` lines and the emitted
 * registers -- global_alloc prints no pseudo-to-hard-register table, only reload
 * notes.  The allocation-ORDER line is verbatim and solid; the axis labelling is a
 * lead, not a fact.
 */
/* Func_80f62b8 -- asm/rom_f6000/rom_f6008_c_a_e_a.s, 0x080f62b8, 189 ROM lines.
 *
 * NON-MATCHING: 17 encodings of 191 differ (objcmp).
 *
 * 17 IS A TRUE DISTANCE.  objcmp prints no SIZE line and no RELOCATIONS line:
 * size is 392 bytes in both, instruction count is 191 in both, and the four
 * relocations are the same set in the same order at the same offsets
 * (__divsi3 at 0x68, 0x76, 0x10e; iwram_3001ef0 at 0x180).  Its count is
 * therefore meaningful here.
 *
 * SHIMS: none.  tools/shimcount.py reports no pins, no .equ shims and no "+r"
 * barriers, so no fakematch.txt row is needed if the last 17 ever close.
 * SPLIT: text-only.  tools/datacheck.py prints nothing for this .s.  The file
 * holds TWO functions (Func_80f62b8 and Func_80f6440, both by the anchored
 * .thumb_func_start pattern), so landing this one needs a split.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_f6000/80f62b8.c \
 *     asm/rom_f6000/rom_f6008_c_a_e_a.s --func Func_80f62b8
 *
 * ============================================================
 * WHAT IS EXACT.  Everything except two regions.  The prologue, the four
 * parameter copies AND THEIR REGISTERS (x0 -> r8, y0 -> r7, x1 -> r10,
 * y1 -> r4, dx -> r6, dy -> r5, acc -> r9, fb -> r11, step -> r12), both
 * absolute values, the y-major coordinate swap, both numerators, BOTH of the
 * y-major arm's __divsi3 calls with the `str r4,[sp] / ldr r4,[sp]` spill of
 * y1 around each, the whole x-major loop body, and the epilogue are
 * instruction-for-instruction and register-for-register identical.
 *
 * THE TWO RESIDUES.
 *
 *  1. 14 encodings: THE Y-MAJOR LOOP'S TWO COORDINATES ARE IN SWAPPED
 *     REGISTERS.  The ROM puts the SCANNED coordinate in r0 and the DITHERED
 *     one in r1; ours has them the other way.  The loop is otherwise identical
 *     including the `mov r7, lr` that reloads 0x100 from a high register --
 *     37 instructions in the y-major arm against 36 in the x-major one, and we
 *     reproduce both counts.
 *     PASS: global_alloc, and the .18.greg dump names it exactly.  Its
 *     allocation-order line reads
 *         ;; 23 regs to allocate: 49 46 44 47 48 41 39 38 33 32 35 34 ...
 *     with pseudo 47 = the y coordinate and 48 = the x coordinate.  47 is
 *     ordered FIRST by allocno_compare, and find_reg walks REG_ALLOC_ORDER
 *     { 3, 2, 1, 0, 12, 14, 4, ... }, so with r2/r3 already conflicted 47 takes
 *     r1 and 48 falls to r0.  The ROM wants the reverse.
 *
 *  2. 3 encodings: THE X-MAJOR DIVISOR'S TWO BLOCKS ARE IN THE OPPOSITE ORDER.
 *     Same seven instructions, same registers, arms exchanged:
 *         rom   cmp r6,#0 / blt L / mov r1,r6        / b D / L: mov r3,r8 /
 *               mov r6,r10 / sub r1,r3,r6 / D: bl __divsi3
 *         ours  cmp r6,#0 / bge L / mov r3,r8 / mov r6,sl / sub r1,r3,r6 /
 *               b D / L: mov r1,r6 / D: bl __divsi3
 *     PASS: fold / expand_expr on the COND_EXPR -- gcc inverts the condition
 *     and emits the FALSE arm in the fall-through.  Writing the conditional the
 *     other way round (`dx < 0 ? x0 - x1 : dx`) is INERT: fold canonicalises
 *     both spellings to the same tree, byte for byte.
 *
 * ============================================================
 * THE MECHANISM THAT GOT THIS FROM 170/191 TO 17/191, AND IT CORRECTS A
 * SIBLING PARK.
 *
 * THE WALKING VARIABLES ARE NAMED BY ROLE, NOT BY AXIS.  The ROM's two arms
 * put DIFFERENT AXES in r0: the y-major loop reads `lsr r2,r0,#3 / lsr r3,r1,#3`
 * with y in r0, and the x-major loop reads `lsr r2,r1,#3 / lsr r3,r0,#3` with x
 * in r0.  src/non_matching/rom_c9000/80cde90.c (DrawLine -- the same Bresenham
 * into a 128-wide buffer) concluded from exactly this that "the ROM's four
 * coordinate values CANNOT be two pseudos with fixed registers" and went
 * looking for four.  THAT IS WRONG, and four pseudos measure 69 of 191 here
 * (variant `o`) because the two extra allocnos reshuffle dx, dy and y0 out of
 * r6, r5 and r7.
 *
 * Two pseudos do it, if they are the SCANNED and the DITHERED coordinate rather
 * than x and y.  The tile-address formula still spells the axes correctly --
 * `((y>>3)*32 + (x>>3))*8 + (y&7))*8 + (x&7)` -- so it reads (sc,dt) in the
 * y-major arm and (dt,sc) in the x-major one, which is the ROM's two orders
 * from ONE pair.  Written that way (variant `q`) the residue becomes a single
 * UNIFORM r0<->r1 exchange on one pseudo pair, 24 encodings in both loops plus
 * residue 2, and nothing else differs anywhere in the function.
 *
 * THAT UNIFORMITY IS THE POINT, and it is a clean datapoint for HANDOFF.md's
 * open REG_ALLOC_ORDER question.  The scanned coordinate has SIX references
 * per arm (def, guard compare, the shift, the mask, the increment, the
 * back-edge compare) and the dithered one FIVE (def, the shift, the mask, and
 * the two ±1 arms), so allocno_compare correctly ranks the scanned one higher,
 * and REG_ALLOC_ORDER's { ... 1, 0 ... } hands the higher-ranked allocno r1.
 * The ROM has the higher-ranked one in r0, in BOTH arms, from ONE pair of
 * pseudos.  A REG_ALLOC_ORDER that reaches r0 before r1 reproduces the ROM
 * with no source change at all.  80cde90.c's ref-count argument -- that the
 * dithered coordinate has MORE references (7) than the scanned one (6) and so
 * "should rank higher and take r1 with the scanned one falling to r0" -- counted
 * the two ±1 arms twice; the dithered coordinate has FEWER.  Its conclusion
 * that this is a global_alloc floor survives; its arithmetic does not.
 *
 * WHICH VARIANT IS PARKED.  The axis-named pair below, because it measures 17:
 * gcc's fixed choice (x -> r0, y -> r1) happens to be RIGHT for the x-major arm
 * and wrong only for the y-major one.  The role-named spelling is the honest
 * reading of the ROM and measures 27, all of it one register exchange.  Anyone
 * re-attempting this should start from the role-named form, not this one.
 *
 * ============================================================
 * WHAT WAS READ OUT OF THE ROM AND IS LOAD-BEARING.
 *
 *  - THE WALKING COORDINATES ARE `unsigned`.  `lsr rN,#3` with no `cmp/add #7`
 *    correction.  The tile stride is 32, not 80cde90's 16: a 256-pixel-wide
 *    8x8-tile surface.
 *  - `col` IS THE FIFTH, STACKED ARGUMENT and it is `int`: `ldrb / ldr
 *    r7,[sp,#0x24] / cmp / bge` is a SIGNED compare against a zero-extended
 *    byte, reloaded from the stack on every iteration in both loops.
 *  - THE TWO ABSOLUTE VALUES AT THE MAJOR-AXIS TEST ARE WRITTEN ON THE
 *    VARIABLES: `adx = dx; if (dx < 0) adx = -dx;` gives `mov r2,r6 /
 *    cmp r6,#0 / bge / neg r2,r6`.
 *  - THE TWO ABSOLUTE VALUES AT THE DIVISION ARE WRITTEN ON THE EXPRESSIONS,
 *    which is why they print as REVERSED SUBTRACTIONS and not as `neg`:
 *    `num = dx << 8; if (dx < 0) num = (x0 - x1) << 8;` -- fold rewrites
 *    -(x1 - x0) to x0 - x1, and the `lsl` before the test proves the
 *    conditional OVERWRITE rather than an if/else.
 *  - THE TWO ARMS' DIVISORS HAVE DIFFERENT SHAPES AND MUST NOT BE TIDIED.
 *    The y-major arm is an if/else over the WHOLE CALL -- two __divsi3 sites,
 *    each with its own `str r4,[sp] / ldr r4,[sp]`.  The x-major arm is a
 *    conditional expression feeding ONE call.  Writing the x-major one as an
 *    if/else into a named variable is what kept this at 189 instructions:
 *    gcc collapses it to a conditional overwrite and drops the `b`, losing two
 *    instructions and, as a side effect, moving dx from r6 to r4 and y1 out of
 *    r4 -- which cost 153 further encodings.  Measured: a fresh local 189,
 *    reusing the dead `adx` 186, reusing `ady` 189, reusing the swap temp `t`
 *    187; the conditional expression 191.
 *  - BOTH SWAPS RECOMPUTE BOTH DELTAS.  `sub r6,r2,r3 / sub r5,r4,r7` after the
 *    register rotate, in both arms -- not `neg`.
 *  - y1 LIVES IN r4, WHICH IS CALL-USED BY FLAG (-fcall-used-r4, Makefile line
 *    113), and that is why the y-major arm spills it around both divides.  The
 *    `sub sp,#4` exists only for that spill.
 *  - `iwram_3001ef0` is read WHOLE (`ldr r3,=sym / ldr r3,[r3]`), no offset and
 *    no .sym entry.
 *
 * MEASURED, 22 spellings.  Only the x-major divisor moved anything:
 *   x-major divisor as a conditional expression      17   <-- this file
 *   x-major divisor as if/else into a new local     189 insns, 170 enc
 *   role-named walking variables                     27  (one exchange, uniform)
 *   four walking pseudos (a pair per arm)            69
 *   abs from expressions instead of variables        inert
 *   ady computed before adx                          128 (worse)
 *   dy assigned before dx                            inert
 *   fb before acc / two swap temps / u before v      inert
 *   declaration order of the coordinate pair         inert
 *   guard on the parameter instead of the walker     inert
 *   `while` instead of `if` + do-while               inert
 *   `dt = dt + 1` instead of `dt++`                  inert
 *   dither guard inverted                            121 (worse)
 *   y-major divisor if/else inverted                 121 (worse)
 *
 * NEXT: nothing source-level for residue 1.  It belongs with the other
 * global_alloc parks, and it is the cheapest test in the corpus for the
 * REG_ALLOC_ORDER hypothesis: ONE pseudo pair, ONE exchange, both arms, 24
 * encodings, everything else byte-exact.  Re-screen it together with
 * src/non_matching/rom_c9000/80cde90.c, which is the same routine.
 */
extern unsigned char *iwram_3001ef0;

void Func_80f62b8(int x0, int y0, int x1, int y1, int col)
{
    unsigned char *fb;
    int dx, dy, adx, ady, step, acc, num, t;
    unsigned int u, v;
    int o;

    dx = x1 - x0;
    dy = y1 - y0;
    acc = 0x80;
    fb = iwram_3001ef0;
    adx = dx;
    if (dx < 0)
        adx = -dx;
    ady = dy;
    if (dy < 0)
        ady = -dy;
    if (adx < ady) {
        if (dy < 0) {
            t = x0;
            x0 = x1;
            x1 = t;
            t = y0;
            y0 = y1;
            y1 = t;
            dx = x1 - x0;
            dy = y1 - y0;
        }
        num = dx << 8;
        if (dx < 0)
            num = (x0 - x1) << 8;
        if (dy >= 0)
            step = num / dy;
        else
            step = num / (y0 - y1);
        v = y0;
        u = x0;
        if (v != y1) {
            do {
                o = (((v >> 3) * 32 + (u >> 3)) * 8 + (v & 7)) * 8 + (u & 7);
                if (fb[o] < col)
                    fb[o] = col;
                acc += step;
                if (acc & 0x100) {
                    if (dx > 0)
                        u++;
                    else
                        u--;
                    acc &= ~0x100;
                }
                v++;
            } while (v != y1);
        }
    } else {
        if (dx < 0) {
            t = x0;
            x0 = x1;
            x1 = t;
            t = y0;
            y0 = y1;
            y1 = t;
            dx = x1 - x0;
            dy = y1 - y0;
        }
        num = dy << 8;
        if (dy < 0)
            num = (y0 - y1) << 8;
        step = num / (dx < 0 ? x0 - x1 : dx);
        u = x0;
        v = y0;
        if (u != x1) {
            do {
                o = (((v >> 3) * 32 + (u >> 3)) * 8 + (v & 7)) * 8 + (u & 7);
                if (fb[o] < col)
                    fb[o] = col;
                acc += step;
                if (acc & 0x100) {
                    if (dy > 0)
                        v++;
                    else
                        v--;
                    acc &= ~0x100;
                }
                u++;
            } while (u != x1);
        }
    }
}
