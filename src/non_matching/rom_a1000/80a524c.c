/* Func_80a524c -- ConfirmItemAction -- 0x080a524c, asm/rom_a1000/rom_a4f08_c.s
 * NON-MATCHING, 7 encodings of 134.  316 bytes against 316, 134 encodings against 134.
 * UNCHANGED at 7 after batch 295 -- see THE NEGATIVE below, which is the result.
 *
 * FOUR of the 7 are code; the other THREE are the .word pool placeholders for
 * _MSG_182 / _MSG_ad4 / _MSG_b2c and are an ARTEFACT of comparing one function in
 * isolation.  objcmp's ref assembles `ldr r3, =0x182` to a literal; ours emits a zero
 * word plus an R_ARM_ABS32, which is why objcmp also prints a RELOCATIONS line with
 * exactly three extra ABS32 entries.  The linker folds the `.equ` and both sides agree,
 * so THE TRUE FUNCTION-LEVEL RESIDUE IS 4 -- but only once _MSG_ad4 and _MSG_b2c exist.
 * _MSG_182 = 0x0182 is already admitted (message.sym:291); the other two are NOT.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a524c.c \
 *     asm/rom_a1000/rom_a4f08_c.s --func Func_80a524c
 *
 * objcmp, verbatim:
 *   XX ENCODINGS differ in 7 place(s) (ref 134, ours 134)
 *      first at index 64: ref 2201  ours 2301
 *   XX RELOCATIONS differ      (ours has 23, ref 20: the three _MSG_* ABS32 entries)
 *
 * TWO STALE FACTS CORRECTED IN THIS HEADER, both in the previous revision:
 *  - Its `Verify with:` recipe named a /tmp/claude-0/... scratchpad path from batch 287,
 *    which no longer exists.  parkcheck cannot have been running that recipe.
 *  - It said "asm/rom_a1000/rom_a4f08.s holds 4 functions + .rodata".  That file is GONE
 *    (only a stale .d and .o remain); it was split, and the reference is now
 *    rom_a4f08_c.s, which holds THREE functions + .rodata.
 *
 * THE SPLIT, from tools/datacheck.py asm/rom_a1000/rom_a4f08_c.s:
 *     data sections : .rodata
 *     functions     : Func_80a51d0, Func_80a524c, Func_80a5388
 *     EXPORTS       : .Laf08c   (already global -- NOT the set a split needs)
 *     -> converting a function here needs a TEXT/DATA SPLIT; the data keeps its own object
 *     Func_80a51d0   reads no data label -> split needs NO new export
 *     Func_80a524c   reads no data label -> split needs NO new export
 *     Func_80a5388   reads no data label -> split needs NO new export
 * So this function needs ZERO new exports, and the .s does NOT convert whole (the other
 * two functions are unattempted).
 *
 * SHIMS: none.  Zero `register ... __asm__` declarations, zero `__asm__(".equ ...")`
 * lines.  The three `extern char _MSG_*[]` declarations are the tree's ordinary
 * symbol-address technique, not shims.
 *
 * ============================================================================
 * THE SYMBOL EVIDENCE, RE-DERIVED PER SYMBOL.  This is new and it is strong.
 * ============================================================================
 * The previous revision asserted "0xad4 / 0xb2c must be SYMBOLS ... 19 differing with
 * literals" as one claim.  Measured symbol-by-symbol from this file, each a single drop:
 *
 *   all three symbolic (this file)                            7   (4 code + 3 pool words)
 *   _MSG_ad4 alone as the CONST_INT 0xad4                    13   (+6, at indices 20-26)
 *   _MSG_b2c alone as the CONST_INT 0xb2c                    13   (+6, at indices 32-38)
 *   BOTH ad4 and b2c as CONST_INTs                           19   (+12 -- ADDITIVE)
 *   _MSG_182 alone as the CONST_INT 0x182                   117
 *   all three as CONST_INTs                                 116
 *
 * So the park's "19" is confirmed and it was the ad4+b2c pair.  The two symbols are
 * INDEPENDENTLY load-bearing, each in its own DISJOINT index range around its own
 * _Func_801e7c0 call, and the effects add.  That is a cleaner control than a single
 * combined figure: neither is carrying the other.
 *
 * THE MECHANISM, verified against the side-by-side rather than argued:
 * with 0xad4 written as a CONST_INT, sched2 hoists the pool load ABOVE the preceding
 * `bl _Func_801e7c0`.  Indices 20-26, ref against ours:
 *     ref   add r1,r7 / mov r2,#0x18 / mov r3,#0 / bl / LDR r5,[pc] / add r1,r7 / add r0,r5
 *     ours  LDR r5,[pc] / add r1,r7 / mov r2,#0x18 / mov r3,#0 / bl / add r0,r5 / add r1,r7
 * A constant set of a call-saved hard register has no dependence on the call after
 * reload, so sched2 is free to move it; the SYMBOL_REF spelling is not moved.
 * WHERE MY VERIFICATION STOPS: I did not isolate WHY a SYMBOL_REF pool load and a
 * CONST_INT pool load schedule differently -- both are plain constant sets at RTL and
 * both assemble to `ldr rd, [pc, #N]`.  Do not treat a priority/cost explanation as
 * established.  The measurement is solid and reproduces; the cause is not pinned.
 *
 * _MSG_182's own control is worth recording because it is the strongest in the file:
 * 0x182 IS shiftable (0xc1 << 1), so as a CONST_INT gcc builds it with mov+lsl instead
 * of pooling it and the whole function unravels (117 of 134).  That is exactly the
 * pool tell message.sym's own "shiftable __MessageID IDs" section exists for.
 * NOTE FOR WHOEVER DECIDES ON _MSG_ad4 / _MSG_b2c: neither is shiftable
 * (0xad4 = 0x2b5 << 2 and 0xb2c = 0x2cb << 2, both multipliers > 255), so NEITHER HAS
 * THE SHIFTABILITY POOL TELL that admitted _MSG_182 and _MSG_b20.  Their evidence is
 * the scheduling tell above instead -- a POSITIONAL difference, not a size one -- plus
 * the fact that they feed the same sink (_Func_801e7c0) in the same function as the
 * already-admitted _MSG_182 and sit beside the already-admitted _MSG_ad0 = 0x0ad0.
 * The shiftability table, computed rather than asserted (imm8 << N reachable by
 * `mov rd,#imm8 / lsl rd,#N`, which is what makes gcc NOT pool a value):
 *     _MSG_182  0x0182 =  386   SHIFTABLE  193 << 1   <- admitted, pool tell present
 *     _MSG_ad0  0x0ad0 = 2768   SHIFTABLE  173 << 4   <- admitted, cited as the precedent
 *     _MSG_b20  0x0b20 = 2848   SHIFTABLE  178 << 4   <- admitted, the section's exemplar
 *     _MSG_ad4  0x0ad4 = 2772   NOT shiftable         <- PROPOSED, no pool tell
 *     _MSG_b2c  0x0b2c = 2860   NOT shiftable         <- PROPOSED, no pool tell
 * So ALL THREE admitted neighbours carry the shiftability pool tell and NEITHER proposal
 * does.  gcc pools 0xad4 and 0xb2c whether they are literals or symbols, which is why
 * the size and the encoding count are identical either way and the only evidence is
 * positional.
 * That is a different argument from the one message.sym's criteria are written around,
 * and it is the owner's call.  It does NOT complete the function either way.
 *
 * ============================================================================
 * THE BLOCKER, SHARPENED, AND A CLEAR NEGATIVE.
 * ============================================================================
 * THE 4 CODE ENCODINGS ARE TWO SITES, AND BOTH WANT r2 WHERE WE GIVE r3:
 *   [64] mov r2,#1   against ours mov r3,#1    ) the Up block's `moved = 1`
 *   [67] mov r8,r2   against ours mov r8,r3    )
 *   [86] mov r2,r8   against ours mov r3,r8    ) the `if (moved)` test
 *   [87] cmp r2,#0   against ours cmp r3,#0    )
 * The Down block's `moved = 1` (r3) and the `moved = 0` (r3) MATCH.  `moved` lives in
 * r8, a hi register, so every touch of it needs a low reload register.  In address
 * order the ROM's four reload registers are r2, r3, r2, r3; ours are r3, r3, r3, r3.
 *
 * THE PREVIOUS REVISION'S DIAGNOSIS IS CORRECT -- unusually, given three consecutive
 * batches of wrong park blockers -- AND IT IS NOW CONFIRMED BY GCC'S OWN DUMP RATHER
 * THAN INFERRED.  `-da`, x.c.18.greg, verbatim:
 *     Spilling for insn 13.
 *     Spilling for insn 109.
 *     Using reg 3 for reload 0
 *     Spilling for insn 230.
 *     Using reg 3 for reload 0
 *     Spilling for insn 252.
 *     Using reg 3 for reload 0
 *     Spilling for insn 133.
 *     Using reg 3 for reload 0
 *     Spilling for insn 138.
 *     Using reg 3 for reload 0
 * FIVE reload needs, r3 five times out of five.  So used_spill_regs == { r3 } and
 * n_spills == 1, and allocate_reload_reg's round robin (reload1.c:5003-5013,
 * `i = last_spill_reg` then `i++ ... if (i >= n_spills) i -= n_spills`) has nothing to
 * rotate over.  The ROM had n_spills == 2.  spill_regs is built ASCENDING by hard
 * register number (reload1.c:3527-3536), so used_spill_regs == { r2, r3 } gives
 * spill_regs = { r2, r3 }, and with `last_spill_reg = -1` at entry (reload1.c:821) the
 * FIRST reload of the function takes index 0 == r2.  r2, r3, r2, r3 -- THE ROM EXACTLY.
 * The two hypotheses are now one arithmetic identity, which is why this is worth
 * carrying even though it did not close.
 *
 * *** WHAT IS NEW: WHY find_reg NEVER RETURNS r2 HERE, AND THE THREE ROUTES. ***
 * The previous revision said "somewhere in the ROM's function find_reg had to pick r2,
 * i.e. r3 was busy at some reload.  Not found which insn."  find_reg is reload1.c:1618-
 * 1658 and it decides like this:
 *     this_cost = spill_cost[regno];
 *     ... if (this_cost < best_cost
 *             || (this_cost == best_cost
 *                 && inv_reg_alloc_order[regno] < inv_reg_alloc_order[best_reg]))
 * REG_ALLOC_ORDER on this target is { 3, 2, 1, 0, 12, 14, 4, 5, ... } (arm.h:989), so
 * **r3 IS FIRST AND WINS EVERY ZERO-COST TIE**.  spill_cost[regno] is nonzero only when
 * a PSEUDO allocated to that hard register is live at the insn (count_spilled_pseudo
 * over chain->live_throughout and chain->dead_or_set).  At both of our sites r3 is free:
 * it held the `*keys & mask` result, which dies at the `cmp`/branch just above.  Cost 0
 * for r2 and r3 alike, tie, r3 by REG_ALLOC_ORDER.  There are exactly three ways r2 can
 * win, and all three are properties of the SURROUNDING code, not of these statements:
 *   (i)  spill_cost[3] > spill_cost[2] -- a pseudo global-alloc put in r3 is live across
 *        the site.  Requires a long-lived value in r3, which costs instructions here.
 *   (ii) r3 in used_by_other_reload -- ONE INSN NEEDING TWO RELOADS, anywhere in the
 *        function.  That single insn would put r2 in used_spill_regs and the round robin
 *        would then fix all four encodings at once.  This is the route to look for.
 *   (iii) r3 in bad_spill_regs / bad_spill_regs_global.  Not reachable from source.
 * Note the discounts in find_reg (`rl->in`/`rl->out` already in that regno costs one
 * less) do not apply: both reloads here have r8 on the other side.
 *
 * NO FLAG REACHES IT.  Measured on this file, production flag group otherwise:
 *   -fno-rerun-cse-after-loop, -fno-regmove, -fno-strength-reduce,
 *   -fomit-frame-pointer, -fno-caller-saves                       7 (inert)
 *   -fno-expensive-optimizations                                 19, 312 bytes
 *   -fno-schedule-insns2                                         37
 *   -fno-force-mem                                               20
 *   -fno-gcse                                                   137, 328 bytes
 *   -fno-gcse AND -fno-expensive-optimizations (the batch-294 pair)  136, 324 bytes
 * So the pair that paid on OvlFunc_924_2008a24 is destructive here; recorded because a
 * flag inert alone can be load-bearing in a pair and the converse needs recording too.
 *
 * ADDED TO THE INERT LIST (each a single drop from this file, all still 7)
 *   `moved` typed unsigned char / char / short / unsigned short / unsigned int / long
 *     -- re-measured, since the previous revision's inert entry predates nothing in
 *        particular; still inert, and now with the reason: the mode of `moved` does not
 *        change that it lands in a hi register, so the reload need is unchanged.
 *   `moved` declared FIRST of the locals, and declared LAST                   inert
 *   `moved` read into a named temp before `if (moved)`                        inert
 *   `*keys` read into a named temp in each test                               inert
 *   `register int id` on the parameter                                        inert
 *   the 0x40 / 0x80 masks in a named local                                    20 (worse)
 *   the gKeyPress masks 1 / 2 in a named local                                inert
 *   `moved = n - 2` added inside the `if (moved)` block                       13 (worse)
 *   a shared `one = 1` local feeding all three `moved = 1` assignments        inert
 *   `keys` and gKeyRepeat non-volatile                                        14 (worse)
 *
 * *** NOT A NEW FINDING -- AND THE TREE ALREADY HAS A CURE, WHICH DOES NOT FIT HERE. ***
 * docs/elevation.md:19794, "A RELOAD SCRATCH REGISTER IS ROUND ROBIN OVER A SET THE
 * SOURCE CONTROLS", already records this entire mechanism: spill_cost[] zeroed per insn,
 * find_reg's tie-break on inv_reg_alloc_order so r3 wins every tie, used_spill_regs
 * sorted ascending into spill_regs[], allocate_reload_reg round robin over that array,
 * and the discriminator "count `Using reg N` in .18.greg; if they are all ONE register
 * while the reference alternates two, the SPILL SET IS TOO NARROW.  Add a reload
 * somewhere else and never touch the differing site."  What is new here is only the
 * confirmation on this function (five of five) and the enumeration of the three routes
 * above.  Do not file the mechanism again.
 *
 * THE DOCUMENTED CURE CANNOT BE APPLIED TO THIS FUNCTION, and the reason is worth
 * recording so nobody spends the round.  That cure works by writing an IWRAM WHOLE-WORD
 * STORE as a single expression so the offset leaves thumb `str` range: reload then needs
 * a register for the ADDRESS as well as one for the VALUE, two reloads on one insn, so
 * r3 goes to the first and r2 to the second and the spill set widens.  That is route (ii).
 * *** Func_80a524c CONTAINS NO DATA STORE AT ALL. ***  Its only store is `str r3, [sp]`,
 * the fifth argument of _CreateUIBox, which is sp-relative and in range and cannot be
 * pushed out of it.  Every other memory reference is a LOAD (`ldr r3, [r5]` through
 * gKeyRepeat / gKeyPress), and an out-of-range load needs a register for the address
 * only -- ONE reload, which find_reg gives r3 at zero cost, widening nothing.  So the
 * only route that does not cost instructions is unavailable.
 *
 * ROUTE (i) IS STRUCTURALLY BLOCKED, not merely expensive.  It needs a pseudo that
 * global-alloc put in r3 to be LIVE AT one of the two reload insns.  r3 is call-clobbered
 * (and the build's -fcall-used-r4 makes r4 call-used too, so the call-saved low registers
 * are only r5, r6, r7).  Both sites sit immediately before a `bl` -- `moved = 1` is
 * followed by `bl _PlaySound`, and `if (moved)` by `bl __modsi3` -- so any value live at
 * either site must either die before that call or be call-saved, and a call-saved value
 * cannot be in r3.  A value that dies before the call would have to be computed and
 * consumed inside the two- or three-instruction window around the reload, and there is
 * nothing there to compute.  So r3 is unavoidably free at both sites, spill_cost[3] is 0,
 * and find_reg's inv_reg_alloc_order tie-break hands it r3 every time.
 *
 * ALL FIGURES IN THIS HEADER ARE PRODUCTION-FLAG FIGURES.  batch.py reports
 * `flags-adjust=[]` for this .s, i.e. the plain GCC296_CFLAGS group
 * (-O2 -mthumb -mthumb-interwork -mcpu=arm7tdmi -fno-builtin -nostdinc -ffreestanding
 * -fcall-used-r4), with no per-file Makefile substitution.  Nothing here is
 * flag-conditional.
 *
 * CONTRADICTION WORTH CARRYING: none found.  Both of this park's technical claims --
 * the reload round robin and the literal-vs-symbol scheduling -- survived re-derivation.
 * That is the first park in four batches whose stated blocker was right.
 */
extern char _MSG_182[], _MSG_ad4[], _MSG_b2c[];
extern volatile int gKeyRepeat;
extern volatile int gKeyPress;
extern int _CreateUIBox(int, int, int, int, int);
extern unsigned char *_GetItemInfo(int item);
extern void _Func_801e7c0(int id, int win, int x, int y);
extern void Func_80a1ac0(int x, int y);
extern void Func_80a1a40(int x, int y);
extern void _PlaySound(int id);
extern void WaitFrames(int n);
extern int _GetFlag(int id);
extern void _CloseUIBox(int win, int a);

int Func_80a524c(int id)
{
    volatile int *keys;
    int win;
    int sel;
    int moved;
    int n;

    win = _CreateUIBox(0xd, 3, 0x11, 0xa, 2);
    id &= 0x1ff;
    _GetItemInfo(id);
    id += (int)_MSG_182;
    _Func_801e7c0(id, win, 0x18, 0);
    id = (int)_MSG_ad4;
    _Func_801e7c0(id, win, 0, 0x10);
    id++;
    _Func_801e7c0(id, win, 0, 0x18);
    id = (int)_MSG_b2c;
    _Func_801e7c0(id, win, 0x18, 0x28);
    id++;
    _Func_801e7c0(id, win, 0x18, 0x38);
    n = 2;
    sel = 1;
    moved = 1;
    Func_80a1ac0(0x68, 0x56);
    while (!_GetFlag(0x150)) {
        if (moved) {
            moved = 0;
            sel = (sel + n) % n;
        }
        if (gKeyPress & 1) {
            _PlaySound(0x70);
            break;
        }
        if (gKeyPress & 2) {
            _PlaySound(0x71);
            sel = 1;
            break;
        }
        Func_80a1a40(0x68, sel * 16 + 0x46);
        keys = &gKeyRepeat;
        if (*keys & 0x40) {
            sel--;
            moved = 1;
            _PlaySound(0x6f);
        }
        if (*keys & 0x80) {
            sel++;
            moved = 1;
            _PlaySound(0x6f);
        }
        WaitFrames(1);
    }
    if (_GetFlag(0x150))
        sel = 1;
    _CloseUIBox(win, 1);
    return sel;
}
