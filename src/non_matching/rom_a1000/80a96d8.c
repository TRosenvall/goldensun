/* Func_80a96d8 -- NON-MATCHING, 2 ENCODINGS OF 319.  SIZE DELTA ZERO (728 = 728),
 * 319 instructions both sides, relocation lists identical in symbol AND order.
 * CLOSED, AND AS OF BATCH 315 CLOSED WITH A STRUCTURAL ARGUMENT RATHER THAN A
 * COUNT OF ATTEMPTS.  DO NOT SPEND A BRIEF ON THIS WINDOW AGAIN.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_a1000/80a96d8.c \
 *     asm/rom_a1000/rom_a8604_c_a_a_c.s --func Func_80a96d8
 *   XX ENCODINGS differ in 2 place(s) (ref 319, ours 319)
 *      first at index 44: ref 2618  ours 4d99
 *
 * THE REFERENCE HOLDS THIS FUNCTION ALONE -- no split, no data work
 * (`python3 tools/datacheck.py asm/rom_a1000/rom_a8604_c_a_a_c.s` is silent).
 * SHIMS: none of either class.  The empty `__asm__ volatile ("")` is a
 * scheduling barrier, not a pin.
 *
 * THE RESIDUE, ONE WINDOW, BOTH ENCODINGS REAL CODE (checked in batch 315 by
 * listing EVERY differing index, not just the first -- there is no `.word` among
 * them, so the figure is NOT pool-inflated):
 *     idx 44  ref 2618 `mov r6,#0x18`   ours 4d99 `ldr r5,[pc,...]`
 *     idx 45  ref 4d99 `ldr r5,=0xb06`  ours 2618 `mov r6,#0x18`
 * A straight swap.  insn 107 is the 0xb06 materialisation, insns 1133/1134 are
 * reload's split of -0x18.  Both sides then agree on `neg r6,r6`.
 *
 * ============ THE FULL rank_for_schedule LADDER, VERIFIED AGAINST SOURCE ============
 * haifa-sched.c rank_for_schedule, in order, each rung consulted only on a tie:
 *     1. INSN_PRIORITY
 *     2. INSN_REG_WEIGHT          -- guarded by `!reload_completed`, SO IT NEVER
 *                                    RUNS AT sched2; ignore it here
 *     3. interblock keys           -- same bb, no effect
 *     4. CLASS RELATIVE TO last_scheduled_insn: 3 = independent of it OR
 *        insn_cost == 1; 1 = data dependence; 2 = anti/output.  HIGHER WINS.
 *     5. dependent count (length of INSN_DEPEND).  MORE WINS.
 *     6. INSN_LUID.  LOWER WINS.
 * The brief for batch 315 gave this ladder without rung 4.  THIS PARK ALREADY
 * HAD RUNG 4 and its treatment of it is correct; what follows upgrades that
 * treatment from an observation into an argument.
 *
 * MEASURED FROM .23.sched2 (-fsched-verbose=6), basic block 4, at the decision
 * point t = 34 where the ready list is `118 110 1133 107`:
 *      insn   code  dep  prio  cost   INSN_DEPEND
 *      107    173    1    42    2     797 124 114          -> 3 dependents
 *     1133    173    1    42    1     797 1134             -> 2 dependents
 *      110    173    2    40    2     797 126 121          -> loses on priority
 *      118    173    2    40    1     797 134 121          -> loses on priority
 * So 107 vs 1133 is the real competition, the two others are excluded at rung 1.
 *
 * RUNG 1 TIES BY IDENTITY (unchanged, and the derivation stands): writing C for
 * the first `_Func_801e7c0` call, prio(1133) = 1 + prio(1134) = 2 + prio(120)
 * = 3 + prio(C), and prio(107) = 2 + prio(114) >= 3 + prio(C) because
 * arm_adjust_cost pins any true dependence into a CALL_INSN at 1.  107 can never
 * LOSE rung 1.
 *
 * RUNG 4 TIES BY CONSTRUCTION, AND THIS IS THE NEW ARGUMENT.  Neither competitor
 * can EVER be class 1: both are CONSTANT MATERIALISATIONS WITH NO INPUT
 * REGISTERS, so neither can carry a data dependence on anything, whatever the
 * source says.  Each can therefore only be class 3 or class 2, and the dumps show
 * both carry the SAME note on the SAME insn --
 *     (insn 107 ... (insn_list:REG_DEP_OUTPUT 104 (nil)))
 *     (insn 1133 ... (insn_list:REG_DEP_OUTPUT 104 (nil)))
 * -- an output dependence on insn 104, the empty `__asm__ volatile ("")`, which
 * is the last-scheduled insn at that point.  Same relation, same class, tie.
 * To break rung 4 the last-scheduled insn would have to touch r5 and NOT r6 (or
 * the reverse), i.e. a prior writer or reader of the 0xb06 carrier placed
 * immediately before the pair.  Without the barrier the last-scheduled insn is
 * the StopTask CALL, which touches neither callee-saved register, so link == 0
 * for both and rung 4 ties again -- consistent with the measured "drop the
 * barrier -> 8".  RUNG 4 IS PINNED EQUAL FOR EVERY SOURCE SHAPE THAT KEEPS THE
 * ROM'S TWO QUANTITIES.
 *
 * RUNG 5 IS FIXED BY THE ROM'S OWN STREAM (unchanged): the pooled 0xb06 has two
 * users in this block (`mov r0,r5` for the first call's arg0 and `add r5,#3` for
 * the second's) and the 0x18 has exactly one (`neg r6,r6`).  3 against 2, 107
 * wins, and INSN_LUID is never reached -- equalising at 2-2 still loses it,
 * because `(insn 107 104 1133 ...)` puts 107 first in the chain and rung 6
 * prefers the LOWER LUID.
 *
 * THE BRIEF'S ALIAS-SET LEVER CANNOT REACH THIS WINDOW, AND NOW BY DUMP RATHER
 * THAN BY ASSERTION.  The lever needs a MEM in the tie.  `ldr r5,=0xb06` IS NOT A
 * MEM AT sched2: at .19.flow2 insn 107 is `(set (reg/v:SI 5 r5) (const_int 2822))`
 * with `REG_EQUIV (const_int 2822)`, and it only becomes
 * `(mem:SI (const (plus (label_ref 1153) (const_int 12))))` at .26.mach -- the
 * constant pool is built by machine_dependent_reorg, which runs AFTER sched2.
 * BOTH COMPETITORS ARE REGISTER-ONLY WHEN THE DECISION IS MADE.  (Worth keeping
 * bank-wide: no pool load is ever a MEM for scheduling purposes in this compiler.)
 *
 * NOT A STATEMENT BOUNDARY EITHER, so the barrier route does not apply: the two
 * insns come from `msg = 0xb06;` and the arg3 fill of the call on the next line,
 * and a barrier can only put the whole pool load before or after the whole fill
 * group -- it cannot produce the ROM's INTERLEAVING (`mov r6` / `ldr r5` /
 * `neg r6`).  Measured, all from 2:
 *   y = 0x18; barrier; msg = 0xb06; yn = -y;              2 (same window)
 *   y = 0x18; barrier; msg = 0xb06; ... -y at both uses   2 (same window)
 *   y = 0x18; msg = 0xb06; yn = -y; (no second barrier)   2 (same window)
 *   yn = -0x18; barrier; msg = 0xb06;                     4
 *   y = 0x18; __asm__ ("" : "+r" (y)); msg = 0xb06;       8
 *   y = -0x18 with the "+r" probe                         8
 * Constant propagation defeats the first three: `y = 0x18; yn = -y` folds back to
 * `yn = -24` and reload re-splits it into mov/neg in the same place, AFTER the
 * barrier.  insns 1133/1134 ARE RELOAD OUTPUT and have no source position at all.
 *
 * Blocker class: sched2 ready-list ranking, haifa-sched.c rank_for_schedule.
 * CLOSED ON RUNGS 1, 4 AND 5 TOGETHER, each by a property of the ROM's own
 * instruction stream or of the competitors' RTL form rather than by exhaustion.
 * This window needs a different rank_for_schedule or a different arm_adjust_cost.
 *
 * ============ BATCH 315: EVERY `extern void` RE-SWEPT FROM THIS BASELINE ============
 * All EIGHTEEN void callees swept to `extern int` (tools/sweep_variants.py, one
 * container, 3 seconds).  INERT at 2: Func_80a9cbc, Func_80a19a0, StopTask,
 * StartTask, WaitFrames, Func_80a33d4, _Func_8016498, _Func_80164ac,
 * Func_80a1e38, Func_80a9a5c, Func_80a3e28, Func_80a9598, Func_80a93a4,
 * Func_80a1a40, _PlaySound, Func_80a1804, Func_80a345c.  WORSE: _Func_801e7c0
 * at 4, confirming the park's recorded regression.  So the batch-315
 * callee-return-type lever is EXHAUSTED on this function -- which matters,
 * because that lever is exactly what took this park from 4 to 2 on Func_80a10d0.
 *
 * ============ THE 4 -> 2 ADVANCE AND ITS MECHANISM, both unchanged ============
 *     extern int Func_80a10d0(void *p, int a, int b, int c, int d, int e);
 * -- `int`, not `void`.  A VOID CALL IS `*call_insn` AND NEVER **SETS** r0, it
 * only `(use)`s it, so after the call `reg_last_sets[r0]` still names the call's
 * OWN r0 argument fill and the next r0 definer takes a REG_DEP_OUTPUT on that
 * fill -- giving the r0 fill one MORE INSN_DEPEND entry than the call's other
 * fills, which decides rung 5.  Declared `int` the call is `*call_value_insn`,
 * it SETS r0, the output dependence attaches to the CALL, the counts tie and
 * rung 6 decides.  Measured on window 1's pair, insn 41 `add r0,r0,#48` against
 * insn 55 `mov r3,#15`, both priority 3:
 *     void  41 -> {58,56} = 2, 55 -> {58,56} = 2, LUID breaks it, `add r0`
 *           first -- wrong
 *     int   41 -> {56} = 1, 55 -> {58,56} = 2, `mov r3,#15` first -- the ROM
 * WHY THE LUID COULD NEVER HAVE BEEN FIXED: precompute_register_parameters
 * (calls.c) and load_register_parameters both walk args[] FORWARD;
 * PUSH_ARGS_REVERSED is 0 here (calls.c defines it only under PUSH_ROUNDING,
 * which arm.h does not define); LOAD_ARGS_REVERSED is not defined for ARM.  An
 * argument-0 fill can never get a later LUID than argument 3's.  Confirmed in
 * .20.ce2: 38(call) 1011 41 43 44 46 47 51 53 55.
 *
 * ============ EVERY REMAINING LEVER, RE-ABLATED FROM 2 IN BATCH 295 ============
 *   drop the empty `__asm__ volatile ("")` after StopTask     8
 *   drop `n = 0xc80;` (pass the literal)                      4
 *   drop `c = 0xf0;` (inline the literal)                     5
 *   declare `int ctx[7]` before `short buf[15]`               7
 *   `f14` back to `unsigned char *`                         151, +4 bytes
 *   drop the dead one-line `t` temp                         299
 *
 * ============ THE EARLIER LEVERS, unchanged and still load-bearing ============
 * iwram_3001f2c IS A STRUCT, NOT AN `unsigned char *` -- worth about 290
 * encodings.  Under -fstrict-aliasing (on at -O2 here) a hand-cast
 * `*(unsigned int *)(p+8)` and `*(unsigned short *)(p+off)` land in DIFFERENT
 * alias sets so cse merges two textual `ldrh`s across the intervening `str`;
 * A STRUCT MEMBER ACCESS TAKES THE RECORD'S ALIAS SET, so `s->f08 = s->arr[i]`
 * does not merge and the ROM's three separate `ldrh r3,[r7,r2]` reproduce.  The
 * struct also fixes the addressing form for free, where a hand-cast
 * `state + (0x208 + i*2)` gets REASSOCIATED by fold.
 * A POINTER FIELD MUST POINT AT A STRUCT WITH NO `int` MEMBER for a later `int`
 * load to hoist above its store -- same machinery, opposite direction; adding ONE
 * `int` member to W re-records int as a subset of W and kills the hoist again.
 * THE OUTER-LOOP ROTATION LEVER: expand_end_loop scans from the loop top for the
 * LAST jump to end_label, stops at the first nested NOTE_INSN_LOOP_BEG, and moves
 * everything up to that jump to the bottom.  Writing the guard as
 * `if (done == 0 && _GetFlag(0x150) == 0) { refresh; i = 0; } else { break; }`
 * puts an UNCONDITIONAL `goto end_label` after the refresh so the refresh and the
 * `i = 0` move too; the back edge then lands on the copy loop's TEST, giving it
 * multiple entry points, so loop.c marks it invalid and it gets no invariant
 * hoisting, no strength reduction and no reversal -- the ROM's raw indexed copy
 * loop -- while the second copy loop is fully strength-reduced.  TWO COPY LOOPS
 * WITH IDENTICAL SOURCE SHAPE COMPILING DIFFERENTLY IS THE TELL THAT ONE OF THEM
 * IS JUMPED INTO.  Two plain `if (...) break;` measured 241 and +8 bytes.
 * FRAME_GROWS_DOWNWARD IS 1 (arm.h), so the first-declared local gets the highest
 * sp offset: `short buf[15];` BEFORE `int ctx[7];`.
 * A DEAD ONE-LINE TEMP BEFORE `ret = 0` flips the prologue's hard-register
 * assignment and enables reload_cse_move2add -- 299 -> 17.  Declaration-order
 * permutation is worthless for register-resident quantities (all 17 positions of
 * `int ret;` gave 298/299) but WHICH variable the temp is matters (`int t` 17,
 * `cur` 19, `redraw` 19, i/n 20, `c` 24, done/r 294/295).
 * `msg = 0xb06; f(msg,...); f(msg + 3,...)` reproduces the ROM's one pool entry
 * plus `add r5,#3`; two literals give two pool entries; the inverse spelling
 * (`0xb09 ... msg - 3`) fuses one insn away and is wrong.
 *
 * NEGATIVE RESULTS, so nobody re-spends them: `pp = &s->f30` as a temp;
 * `(unsigned char *)s + 0x30`; first-param type `unsigned int *` vs `void *`;
 * 0xf written as 15; `r = 1; r <<= 4;` for the 4th argument; a variable for the
 * 4th argument in NINE choices; StopTask/StartTask as `void (*)(void)` vs
 * `void *`; _Func_801e7c0's msg param unsigned; an explicit `y = -0x18` in either
 * order; hoisting msg and/or y above the objs loop; two separate variables for
 * msg and msg+3.
 */
struct W {
    unsigned char pad00[5];
    unsigned char f5;
};

struct Unit {
    unsigned char pad00[0xd8];
    unsigned short items[15];
};

struct State {
    unsigned char pad00[8];
    unsigned int f08;
    unsigned char pad0c[0x14 - 0x0c];
    struct W *f14;
    unsigned char pad18[0x1c - 0x18];
    signed char f1c;
    unsigned char pad1d[0x24 - 0x1d];
    unsigned int f24;
    unsigned char pad28[0x2c - 0x28];
    unsigned int f2c;
    unsigned int f30;
    unsigned char pad34[0x48 - 0x34];
    unsigned char *objs[32];
    unsigned char padc8[0x10c - 0xc8];
    unsigned int f10c;
    unsigned char pad110[0x1c8 - 0x110];
    unsigned short list[(0x208 - 0x1c8) / 2];
    unsigned short arr[(0x218 - 0x208) / 2];
    unsigned char f218;
    unsigned char f219;
    unsigned char f21a;
    unsigned char pad21b[0x260 - 0x21b];
    unsigned char f260[4];
};

extern struct State *iwram_3001f2c;
extern volatile unsigned int gKeyPress;
extern volatile unsigned int gKeyRepeat;
extern struct Unit *_GetUnit(int id);
extern int Func_80a10d0(void *p, int a, int b, int c, int d, int e);
extern void Func_80a9cbc(void);
extern void Func_80a19a0(void);
extern void StopTask(void *task);
extern void StartTask(void *task, int priority);
extern void _Func_801e7c0(int msg, unsigned int win, int x, int y);
extern void WaitFrames(int n);
extern void Func_80a33d4(struct State *s, unsigned int a);
extern int _GetFlag(int id);
extern void _Func_8016498(unsigned int win);
extern void _Func_80164ac(unsigned int win);
extern int Func_80a3ddc(struct Unit *unit, unsigned short *p, int f);
extern void Func_80a1e38(unsigned short *p, int f);
extern int Func_80a8b8c(int *dest, int cursor);
extern void Func_80a9a5c(unsigned int win, unsigned int sel, int a);
extern void Func_80a3e28(unsigned short *p, int f);
extern void Func_80a9598(unsigned int win, int a, int *ctx);
extern void Func_80a93a4(unsigned int win, int a, int *ctx);
extern void Func_80a1a40(int a, int b);
extern void _PlaySound(int id);
extern int Func_80a1fd4(int swap, int total, int cols, int *col, int *row);
extern void Func_80a1804(struct State *s, unsigned int id);
extern void Func_80a345c(void);

int Func_80a96d8(void)
{
    short buf[15];
    int ctx[7];
    struct State *s;
    struct Unit *u;
    unsigned char **q;
    unsigned char *x;
    int ret;
    int done;
    int redraw;
    int full;
    int r;
    int i;
    int cur;
    int n;
    int msg;
    int c;
    int t;

    s = iwram_3001f2c;
    t = s->f21a;
    ret = 0;
    _GetUnit(s->arr[t]);
    Func_80a10d0(&s->f30, 0, 0xa, 0xf, 0xa, 2);
    Func_80a9cbc();
    c = 0xf0;
    q = s->objs;
    i = 0x1f;
    do {
        x = *q++;
        if (x != 0)
            x[0xf] = c;
        i--;
    } while (i >= 0);
    StopTask(Func_80a19a0);
    __asm__ volatile ("");
    msg = 0xb06;
    _Func_801e7c0(msg, s->f24, 0x40, -0x18);
    _Func_801e7c0(msg + 3, s->f24, 0, -0x18);
    Func_80a9cbc();
    WaitFrames(1);
    Func_80a33d4(s, s->f10c);
    done = 0;
    while (1) {
        if (done == 0 && _GetFlag(0x150) == 0) {
            Func_80a9cbc();
            _Func_8016498(s->f24);
            u = _GetUnit(s->f21a);
            i = 0;
        } else {
            break;
        }
        while (i <= 0xe) {
            buf[i] = u->items[i];
            i++;
        }
        s->f218 = Func_80a3ddc(u, s->list, 0);
        Func_80a1e38(s->list, 0);
        Func_80a8b8c(ctx, 0);
        _Func_8016498(s->f30);
        Func_80a9a5c(s->f30, s->f21a, 1);
        WaitFrames(1);
        Func_80a3e28(s->list, 0);
        redraw = 1;
        full = 1;
        while (_GetFlag(0x150) == 0) {
            if (redraw != 0) {
                _Func_80164ac(s->f2c);
                redraw = 0;
                if (full != 0) {
                    full = 0;
                    Func_80a9598(s->f24, 0, ctx);
                }
                Func_80a93a4(s->f24, 0, ctx);
            }
            s->f14->f5 = 1;
            Func_80a1a40(0x60, ctx[4] * 16 + 0x34);
            WaitFrames(1);
            r = Func_80a1fd4(0, ctx[5], 5, &ctx[4], &ctx[2]);
            if (r == 1) {
                full = 1;
                redraw = 1;
            }
            if (r == 0)
                redraw = 1;
            if (r == -1)
                redraw = 0;
            if ((gKeyPress & 1) != 0) {
                _PlaySound(0x70);
                ret = 1;
                done = 1;
                break;
            }
            if ((gKeyPress & 2) != 0) {
                _PlaySound(0x71);
                ret = -1;
                done = 1;
                break;
            }
            if ((gKeyRepeat & 0x100) != 0 || (gKeyRepeat & 0x200) != 0) {
                _PlaySound(0x6f);
                cur = s->f1c;
                s->f260[s->arr[cur]] = ctx[6];
                if ((gKeyRepeat & 0x100) != 0)
                    cur++;
                else
                    cur--;
                for (i = 0; i <= 0xe; i++)
                    u->items[i] = buf[i];
                n = s->f219;
                cur = (cur + n) % n;
                s->f08 = s->arr[cur];
                s->f21a = s->arr[cur];
                s->f1c = cur;
                Func_80a1804(s, s->arr[cur]);
                break;
            }
        }
    }
    _Func_80164ac(s->f2c);
    _Func_8016498(s->f2c);
    _Func_80164ac(s->f10c);
    Func_80a345c();
    _Func_8016498(s->f24);
    n = 0xc80;
    StartTask(Func_80a19a0, n);
    return ret;
}
