/* ========= BATCH 328, BRIEF F -- VERDICT HOLDS; RESIDUE LOCATED; =========
 * ========= AND THE BARRIER IS PROVED LOAD-BEARING BY ITS CONTROL =========
 *
 * FIGURE RE-DERIVED: 2 differing encodings of 319 (ref 319, ours 319).  No
 * objcmp SIZE line, no INSTRUCTION COUNT line.  `--whole` agrees: "2 of 319
 * differ (ours 319), first at index 44".  THE FIGURE IS A DISTANCE, and the
 * park's verdict survives.
 *
 * (1) THE RESIDUE IS ONE ADJACENT TRANSPOSITION, pinned for the first time.
 *     Immediately after `bl StopTask`, at indices 44/45:
 *         ROM    movs r6, #24    /  ldr r5, =0xb06
 *         ours   ldr r5, =0xb06  /  movs r6, #24
 *     r6 is then `negs r6,r6`, i.e. it is the shared `-0x18` y-argument of both
 *     `_Func_801e7c0` calls; r5 is `msg`.  So the contest is `msg = 0xb06`
 *     against the `-0x18` argument -- the SAME construct, in the same position,
 *     that the landed module-mate `src/rom_a1000/rom_a8604_c_c_a_a_a_b.c`
 *     (Func_80a9a5c, `msg = 0xb24`) settled.  It is not a separate mechanism.
 *
 * (2) THE MODULE-MATE CONTRAST, RESOLVED: THEY ARE ON THE SAME SIDE.
 *     The bare `__asm__ volatile ("")` in this body already sits exactly where
 *     Func_80a9a5c's does -- immediately before the `msg = ...` pool constant --
 *     and does exactly the same job (supply a predecessor via
 *     `reg_pending_sets_all` so the constant is not `dep 0` and ready at t = 0).
 *     THE CONTROL NOBODY HAD RUN IS REMOVING IT:
 *         barrier REMOVED .................. **8 of 319** (ref 319, ours 319)
 *         barrier present (as installed) ... 2 of 319
 *     **THE BARRIER IS WORTH SIX ENCODINGS AND IS LOAD-BEARING.**  What is inert
 *     here is the park's PROPOSED `__asm__ volatile ("" ::: "r5")` clobber-list
 *     escape, which is a different construct; the bare barrier is the same lever
 *     with the same sign as the landing.  BOTH FIGURES FOR PASS 3/4:
 *     **2 with the barrier, 8 without it.**
 *
 * (3) THE EXPAND-ORDER / LUID DIMENSION IS NOW CLOSED FROM THE SOURCE SIDE.
 *     The obvious attack on a transposition is to write the loser's value first.
 *     All three positions BREAK THE COUNT, so none of them is even a figure
 *     about ordering:
 *       `n = -0x18;` BEFORE `msg = 0xb06;` ..... 123 of 319, 275 insns (ref 273)
 *       `n = -0x18;` AFTER  `msg = 0xb06;` ..... 123 of 319, 275 insns
 *       `n = -0x18;` then the barrier then msg . 125 of 319, 275 insns
 *     All three SIZE 732 vs 728 with relocations differing.  Naming the shared
 *     `-0x18` in an existing local costs TWO INSTRUCTIONS whatever its position,
 *     so source order cannot reach the transposition.  Combined with batch 327's
 *     durable fact -- a volatile asm as `last_scheduled_insn` FLATTENS RUNG 3
 *     for the whole ready list, which is why the clobber-list escape measured
 *     exactly inert -- the barrier that buys the 6 is also what removes the rung
 *     that would separate these two insns.  That is the real shape of the
 *     blocker: **the lever and the obstacle are the same insn.**
 *     Pass 3/4 should ask whether the predecessor can be supplied by something
 *     that is NOT a volatile asm (the landing's route (a): a genuine MEM
 *     dependence), since that would restore rung 3 while keeping the 6.
 * ========================================================================  *
 * Func_80a96d8 @ 0x080a96d8 -- NON-MATCHING, 2 differing encodings of 319.
 *
 * FIGURE RE-DERIVED batch 327 brief E, not inherited.  SIZE 728/728, 319
 * instructions both sides, no objcmp INSTRUCTION COUNT line, relocations
 * identical in symbol and order.  PIN COUNT 0 (tools/shimcount.py reports only
 * "empty asm : 1 (NOT treated as a fakematch in this tree)").
 *
 * Verify with -- NAMES THE INSTALLED PATH, ONE LINE:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a96d8.c asm/rom_a1000/rom_a8604_c_a_a_c.s --func Func_80a96d8
 *     -> XX ENCODINGS differ in 2 place(s) (ref 319, ours 319), first at index 44
 *
 * SPLIT SHAPE: NONE.  datacheck.py silent; split_s.py --dry-run says the
 * reference "holds only Func_80a96d8 and no data; convert it directly".
 *
 * THE RESIDUE, ONE ADJACENT SWAP, both halves real code (no pool word):
 *   idx 44  ref 2618 `mov r6,#0x18`   ours 4d99 `ldr r5,=0xb06`
 *   idx 45  ref 4d99 `ldr r5,=0xb06`  ours 2618 `mov r6,#0x18`
 *
 * ===== THE PARK'S VERDICT STANDS. ITS RUNG-3 REASONING DOES NOT. =====
 *
 * Re-read from my own `.23.sched2`, block 4 (`from 862 to 797`):
 *    99  (173)  dep 0 prio 43 cost  2  deps {797,104,102}
 *    102 (239, a CALL) dep 1 prio 42 cost 32
 *    104 (-1, THE ASM BARRIER) dep 2 prio 42 cost 1, unit `none`, 17 dependents
 *    107 (173)  dep 1 prio 42 cost  2  deps {797,124,114} = 3  <- ldr r5,=0xb06
 *    1133(173)  dep 1 prio 42 cost  1  deps {797,1134}    = 2  <- mov r6,#0x18
 *    1134(125 neg) dep 1 prio 41       deps {797,136,120} = 3
 *  Schedule 99@t0, 102@t2, 104@t3, then a 30-CYCLE STALL (102 is a call,
 *  cost 32), so `last_scheduled_insn` is STILL 104 at the contest:
 *    t=34 Ready: 118 110 1133 107 -> picks 107.  (ROM picks 1133.)
 *    t=36 picks 1133, t=37 1134 -- the two land adjacent either way.
 *
 *  rung 1 `:4041` priority ....... 42 == 42                        TIES
 *  rung 2 `:4046` REG_REG_WEIGHT . dead, gated `!reload_completed`
 *  rung 3 `:4067-4094` CLASS vs 104:  both 107 and 1133 take a REG_DEP_OUTPUT
 *         from the asm, because a volatile asm sets `reg_pending_sets_all` in
 *         sched_analyze, so EVERY later register write gets one.  SAME note kind
 *         on the SAME insn => SAME class.                           TIES
 *  rung 4 `:4100-4110` dependent count  3 vs 2, MORE WINS -> 107.  ** DECIDES **
 *  rung 5 `:4115` INSN_LUID ...... not reached; and it returns
 *         LUID(tmp)-LUID(tmp2), so the LOWER LUID WINS, and the dependence table
 *         lists 107 before 1133 -- so equalising rung 4 at 2-2 or 3-3 STILL
 *         loses it.  The park was right about this.
 *
 * *** WHAT THE PARK GOT WRONG, AND IT MATTERS FOR OTHER FUNCTIONS, NOT THIS ONE.
 * The park explains rung 3 as "both links are REG_DEP_OUTPUT on the SAME insn,
 * and arm_adjust_cost prices an output dependence at 0, so both land in class 2".
 * The arithmetic is right and the GENERALISATION it invites is wrong.  The
 * durable fact is stronger and simpler:
 *
 *   > A VOLATILE ASM AS `last_scheduled_insn` FLATTENS RUNG 3 FOR THE WHOLE
 *   > READY LIST.  It sets `reg_pending_sets_all`, so every ready insn that
 *   > writes a register carries the SAME kind of link to it and lands in the
 *   > SAME class.  The barrier does not merely fail to discriminate -- it
 *   > removes rung 3's ability to discriminate at all.
 *
 * That is why the park's own proposed escape (`__asm__ volatile ("" ::: "r5")`,
 * to make the last-scheduled insn "touch r5 and not r6") measured EXACTLY INERT
 * rather than merely unhelpful: the clobber list is irrelevant twice over.
 * AND IT IS THE MIRROR IMAGE OF THIS BATCH'S 80a9a5c LANDING, where adding a
 * barrier WON, because there it gave a dep-0 pool load a PREDECESSOR -- see
 * src/rom_a1000/rom_a8604_c_c_a_a_a_b.c.  Same construct, opposite sign:
 * a barrier HELPS when the problem is a dep-0 insn being ready too early, and
 * HURTS when the problem is rung 3 needing to tell two competitors apart.
 *
 * A CAUTION I had to apply to myself, recorded so nobody repeats it: I first
 * read `insn_cost` (`haifa-sched.c:3060`) as returning 1 unconditionally for an
 * asm (`:3072-3076`, `INSN_CODE(insn) < 0 => INSN_COST = 1; return 1;`), which
 * would make every ready insn class 3.  THAT EARLY RETURN IS GATED ON
 * `INSN_COST(insn) == 0` AND INSN_COST IS CACHED -- the priority pass has already
 * set INSN_COST(104) = 1 before rank_for_schedule runs, so the call falls through
 * to ADJUST_COST instead.  Rung 3 ties either way, so the verdict is unaffected;
 * but only one of the two mechanisms is true.
 *
 * REMAINING TARGET, unchanged and narrow: deps(1133) > deps(107), i.e. 1133 at
 * >= 3 while 107 is at <= 2.  The park's reasons both survive re-reading:
 * 107 cannot drop below 2 because the ROM's own stream gives the pooled 0xb06 two
 * users (`mov r0,r5` = insn 114 and `add r5,#3` = insn 124), and 1133 cannot rise
 * because 1133/1134 are reload output materialising a COMPILE-TIME CONSTANT, so
 * every expression that could relate 0x18 to another value in this block relates
 * two compile-time constants and cse/cprop folds it before sched2.
 *
 * MEASURED, inherited from the batch-321 run and NOT re-run (319 encodings):
 *   base 2; `__asm__ volatile ("" ::: "r5")` 2 inert; `msg = 0xb06` moved above
 *   the barrier 2 inert; `msg += 3` then pass `msg` 2 inert; named `int y = 0x18`
 *   with `-y` at both calls 2 inert; those four crossed pairwise 2 inert;
 *   `msg` above the barrier + clobber "r5" 8; `__attribute__((packed))` on
 *   struct State 552 of 319 at 563 insns, RELOC + MEM.  Re-ablations from 2:
 *   dropping the barrier 8, dropping `n = 0xc80` 4, inlining `c = 0xf0` 5,
 *   `int ctx[7]` first 7, `f14` back to `unsigned char *` 151, dropping the dead
 *   `t` 299.  `packed` is a lever for a struct whose layout gcc is CHOOSING; on
 *   this pad-array struct it can only drop alignment, so it is inert-at-best.
 *
 * Blocker class: sched2 ready-list ranking, rank_for_schedule RUNG 4 (dependent
 * count).  Closed on rungs 1, 3, 4 and 5, each by a property of the ROM's own
 * instruction stream or of the competitors' RTL form.  Needs a different
 * rank_for_schedule.
 *
 * Still load-bearing from the batch-315 park, and the record of how the body
 * reached 2: iwram_3001f2c IS A STRUCT, worth ~290 encodings; a pointer field
 * must point at a struct with no `int` member; the outer-loop rotation lever;
 * FRAME_GROWS_DOWNWARD so `short buf[15]` is declared before `int ctx[7]`; the
 * dead one-line `t` temp, 299 -> 17; `msg = 0xb06` with `msg + 3`; every
 * `extern void` re-swept to `extern int` in batch 315 -- all inert at 2 except
 * `_Func_801e7c0`, which is 4.
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
