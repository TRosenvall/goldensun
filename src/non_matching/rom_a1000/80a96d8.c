/* Func_80a96d8 -- NON-MATCHING, 2 encodings of 319 (was 4; advanced in batch 295,
 * and 10 before batch 282).  SIZE DELTA ZERO (728 = 728), 319 instructions both
 * sides, relocation lists identical in symbol AND order.
 *
 * ADVANCED FROM 4 TO 2 BY ONE WORD, and it was a park bug rather than a lever:
 *
 *     extern int Func_80a10d0(void *p, int a, int b, int c, int d, int e);
 *
 * -- `int`, not `void`.  The callee IS defined in this tree as
 * `int Func_80a10d0(void **slot, int a, int b, int c, int d, int e)`
 * (src/rom_a1000/rom_a1050_c_a_b.c:4), and src/rom_a1000/rom_a47b4_a_b.c already
 * records the lever for this very callee -- batch 99 corrected that file to "the
 * RETURN TYPE decides it: `int f(int,int)`, `int f()` and no declaration all
 * match; `void f(int,int)` and `void f()` do not."  This park declared it void
 * and then derived, correctly but pointlessly, why the resulting order was
 * unreachable.
 *
 * THE MECHANISM, which the a47b4 note states without explaining, read out of
 * .23.sched2 and haifa-sched.c.  It generalises, so it is worth having:
 *
 *   A VOID CALL IS `*call_insn` (code 239) AND NEVER **SETS** r0 -- it only
 *   `(use)`s it.  So after the call `reg_last_sets[r0]` still names the call's OWN
 *   r0 ARGUMENT FILL, and the next insn defining r0 takes a REG_DEP_OUTPUT on
 *   that fill.  The r0 fill therefore carries ONE MORE INSN_DEPEND entry than the
 *   other fills of the same call.  rank_for_schedule compares priority, then
 *   class relative to last_scheduled_insn, then DEPENDENT COUNT, then INSN_LUID --
 *   and every argument fill of a call has priority `1 + priority(call)`, because
 *   arm_adjust_cost returns 1 for ANY true dependence into a CALL_INSN whatever
 *   the producer's latency.  Priority ties, so the dependent count decides and r0
 *   goes first.  Declared `int` the call is `*call_value_insn` (code 240), it SETS
 *   r0, the output dependence attaches to the CALL, the counts tie, and INSN_LUID
 *   decides.  MEASURED HERE on window 1's pair, insn 41 `add r0,r0,#48` against
 *   insn 55 `mov r3,#15`, both priority 3:
 *     void  depend_count 41 -> {58,56} = 2, 55 -> {58,56} = 2, LUID breaks it and
 *           LUID(41) < LUID(55) ALWAYS (see below), so `add r0` first -- wrong
 *     int   depend_count 41 -> {56} = **1**, 55 -> {58,56} = 2, so `mov r3,#15`
 *           first -- the ROM
 *
 * WHY THE LUID COULD NEVER HAVE BEEN FIXED, so nobody re-spends it: both
 * precompute_register_parameters (calls.c:805, `for (i = 0; i < num_actuals;
 * i++)`) and load_register_parameters walk args[] FORWARD; PUSH_ARGS_REVERSED is
 * 0 here (calls.c:67-74 defines it only under PUSH_ROUNDING, which arm.h does not
 * define, so the `#ifndef` default 0 applies) so args[0] IS arg0; and
 * LOAD_ARGS_REVERSED is not defined for ARM.  An argument-0 fill can therefore
 * never get a later LUID than argument 3's.  Confirmed in the pre-sched2 dump
 * (.20.ce2): 38(call) 1011 41 43 44 46 47 51 53 55.
 *
 * -- Also correcting this park: THE REFERENCE NOW HOLDS THIS FUNCTION ALONE, so
 * NO SPLIT IS REQUIRED.  The header said "holds THREE functions and this is the
 * last, so a split is required"; a later batch split the file.
 * `python3 tools/datacheck.py asm/rom_a1000/rom_a8604_c_a_a_c.s` is silent -- no
 * data section, no export list.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_a1000/80a96d8.c \
 *     asm/rom_a1000/rom_a8604_c_a_a_c.s --func Func_80a96d8
 *
 * ================================================================
 * THE RESIDUE -- ONE WINDOW, AND IT IS ARITHMETICALLY CLOSED
 * ================================================================
 *
 * idx 44/45, ref +0x05a.  ROM `mov r6,#0x18 / ldr r5,=0xb06 / neg r6,r6`; ours
 * `ldr r5,=0xb06 / mov r6,#0x18 / neg r6,r6`.  Insn 107 is the pool load, insns
 * 1133/1134 are reload's split of the -0x18 constant.  From .23.sched2 (bb 4):
 *
 *     107 (ldr r5)    prio 42  depend_count 3  {797, 124, 114}
 *     1133 (mov r6)   prio 42  depend_count 2  {797, 1134}
 *
 * THE PRIORITY EQUALITY IS AN IDENTITY, NOT A COINCIDENCE.  Write C for the
 * first `_Func_801e7c0` call (insn 121):
 *     prio(1133) = 1 + prio(1134) = 1 + 1 + prio(120) = 2 + (1 + prio(C))
 *                = 3 + prio(C)            -- 120 is `mov r3,r6`, and it feeds
 *                                            ONLY C, so prio(120) = 1 + prio(C)
 *     prio(107)  = 2 + prio(114) >= 2 + (1 + prio(C)) = 3 + prio(C)
 *                                         -- 114 is `mov r0,r5`, it feeds C, and
 *                                            arm_adjust_cost pins that cost at 1
 * So prio(107) >= prio(1133) for every possible shape: the pool load can never
 * LOSE the first key.  On the tie the class key is 3 for both (both depend on the
 * empty-asm insn 104, whose INSN_CODE is -1, so insn_cost returns 1 and
 * rank_for_schedule's `== 1` test makes both class 3).  The next key is dependent
 * count, and BOTH COUNTS ARE FIXED BY THE ROM'S OWN INSTRUCTION STREAM: the
 * pool-loaded 0xb06 has two users in this block (`mov r0,r5` for the first call's
 * arg0 and `add r5,#3` for the second call's), while the 0x18 has exactly one
 * (`neg r6,r6`).  There is no source construct that removes a use the ROM has or
 * adds one it does not.  INSN_LUID is never reached.
 *
 * NOT A STATEMENT BOUNDARY, WHICH IS WHY THE BARRIER ROUTE DOES NOT APPLY.  The
 * sibling park ovl_77a7c8/200a8e8.c closed an equally "proven" sched2 tie in this
 * batch with an empty `__asm__ volatile ("")` between the two STATEMENTS the
 * competing insns came from.  Here the two insns are `msg = 0xb06;` and the arg3
 * fill of the call on the NEXT line, and a barrier can only put the whole pool
 * load before the whole fill group or after it -- it cannot produce the ROM's
 * INTERLEAVING (`mov r6` / `ldr r5` / `neg r6`).  Measured, all from 2:
 *   y = 0x18; barrier; msg = 0xb06; yn = -y;              2 (same window)
 *   y = 0x18; barrier; msg = 0xb06; ... -y at both uses   2 (same window)
 *   y = 0x18; msg = 0xb06; yn = -y; (no second barrier)   2 (same window)
 *   yn = -0x18; barrier; msg = 0xb06;                     4
 *   y = 0x18; __asm__ ("" : "+r" (y)); msg = 0xb06;       8 -- the "+r" probe
 *        does NOT lengthen the chain; it only swaps the register roles (msg to
 *        r6, -0x18 to r5) and the pool load still goes first
 *   y = -0x18 with the "+r" probe                         8
 * Constant propagation is what defeats the first three: `y = 0x18; yn = -y` is
 * folded back to a single `yn = -24` and reload re-splits it into mov/neg in the
 * same place, AFTER the barrier.
 * int-return on `_Func_801e7c0` (the consumer) is a REGRESSION: 6 alone, 4 when
 * combined with the Func_80a10d0 fix.
 *
 * Blocker class: sched2 ready-list ranking, haifa-sched.c rank_for_schedule,
 * dependent-count key.  NOT structural, NOT pool, NOT register allocation, all of
 * which match.  A CLEAR NEGATIVE: this window needs a compiler with a different
 * rank_for_schedule or a different arm_adjust_cost, and should not be
 * re-attempted from source.
 *
 * ================================================================
 * EVERY REMAINING LEVER, RE-ABLATED FROM 2 IN BATCH 295
 * ================================================================
 *
 *   drop the empty `__asm__ volatile ("")` after StopTask     8
 *   drop `n = 0xc80;` (pass the literal)                      4
 *   drop `c = 0xf0;` (inline the literal)                     5
 *   declare `int ctx[7]` before `short buf[15]`               7
 *   `f14` back to `unsigned char *`                         151, +4 bytes
 *   drop the dead one-line `t` temp                         299
 * The park's author flagged `n = 0xc80;` as unsatisfying and asked for it to come
 * out with the real cause of the prologue slips.  HALF THE CAUSE IS NOW FOUND and
 * the line is STILL worth 2, so it stays; it is the same LUID mechanism as window
 * 1 and the same argument-order identity above says nothing shorter reaches it.
 *
 * SHIMS: none of either class -- no `register ... __asm__` declaration, no
 * `__asm__(".equ ...")`.  The empty `__asm__ volatile ("")` is a scheduling
 * barrier, not a pin; it names no register and defines no symbol.  No symbol
 * tells.  No per-file Makefile override applies to this stem; every figure above
 * is on the production flag group.
 *
 * ================================================================
 * THE EARLIER LEVERS, unchanged from the batch-282 park and still load-bearing
 * ================================================================
 *
 * iwram_3001f2c IS A STRUCT, NOT AN `unsigned char *` -- WORTH ABOUT 290
 * ENCODINGS.  Under -fstrict-aliasing (ON at -O2 in this gcc; measured),
 * `*(unsigned int *)(p+8)` and `*(unsigned short *)(p+off)` land in DIFFERENT
 * alias sets, so cse merges two textual `ldrh`s across the intervening `str`.  A
 * STRUCT MEMBER ACCESS TAKES THE RECORD'S ALIAS SET, so `s->f08 = s->arr[i]` does
 * NOT merge and the ROM's three separate `ldrh r3,[r7,r2]` reproduce.  The struct
 * also fixes the ADDRESSING FORM for free: `s->arr[i]` emits the ROM's
 * offset-in-a-register form, whereas a hand-cast `state + (0x208 + i*2)` gets
 * REASSOCIATED by fold into `(state+i*2)+0x208` and materialises the address.
 * Ladder: v1 raw-cast 290 differing / +20 bytes; v2 offset-locals 305; v3 loop
 * rotation 298 / size exact; v4 structs 302 -> 299 -> 11; then 10, 4, 2.
 *
 * A POINTER FIELD MUST POINT AT A STRUCT WITH NO `int` MEMBER for a later `int`
 * load to hoist above its store -- same machinery, opposite direction.  The
 * reference hoists `ldr r1,[ctx+0x10]` above `strb r6,[s->f14+5]`.  With
 * `unsigned char *f14` the store is alias set 0 (char) and conflicts with
 * everything, so no hoist.  With `struct W { unsigned char pad[5]; unsigned char
 * f5; } *f14`, W's set does not conflict with `int` and the hoist happens.  THE
 * CONTROL WAS MEASURED: adding ONE `int` MEMBER to W re-records int as a subset
 * of W and kills the hoist again.
 *
 * THE OUTER-LOOP ROTATION LEVER, AND WHY IT INVALIDATES AN INNER LOOP ON PURPOSE.
 * expand_end_loop (stmt.c ~2360) scans from the loop top for the LAST jump to
 * end_label, stops at the first nested NOTE_INSN_LOOP_BEG, then moves everything
 * up to that jump to the bottom.  Two plain `if (x) break;` tests move only the
 * tests.  Writing the guard as `if (done == 0 && _GetFlag(0x150) == 0) { refresh;
 * i = 0; } else { break; }` puts an UNCONDITIONAL `goto end_label` AFTER the
 * refresh, so the refresh and the `i = 0` move too.  The back edge then lands on
 * the copy loop's TEST -- a jump INTO THE MIDDLE of that loop.  That gives it
 * multiple entry points, loop.c marks it invalid, and it gets NO invariant
 * hoisting, NO strength reduction and NO reversal, which is exactly the ROM's raw
 * indexed copy loop, while the SECOND copy loop, a normal nested for, is fully
 * strength-reduced and reversed.  TWO COPY LOOPS WITH IDENTICAL SOURCE SHAPE
 * COMPILING DIFFERENTLY IS THE TELL THAT ONE OF THEM IS JUMPED INTO.  It also
 * removed a fifth spill slot (frame 88 -> 84).  Two plain `if (...) break;`
 * measured 241 and +8 bytes.
 *
 * FRAME_GROWS_DOWNWARD IS 1 FOR ARM HERE (arm.h:1297), SO THE FIRST-DECLARED
 * LOCAL GETS THE HIGHEST sp OFFSET.  `buf` at sp+0x34 and `ctx` at sp+0x18
 * required `short buf[15];` declared BEFORE `int ctx[7];`.
 *
 * A DEAD ONE-LINE TEMP BEFORE `ret = 0` FLIPS THE PROLOGUE'S HARD-REGISTER
 * ASSIGNMENT AND ENABLES reload_cse_move2add -- 299 -> 17, the largest single
 * edit after the struct.  The ROM derives 0x208 as `subs r0,#18` from the
 * register already holding 0x21a, which is postreload's move2add and only fires
 * if both constants get the SAME hard reg.  Controls: DECLARATION-ORDER
 * PERMUTATION IS WORTHLESS HERE (all 17 positions of `int ret;` gave 298 or 299,
 * no spread -- note this is declaration order for a REGISTER-RESIDENT quantity,
 * which is the case batch 294's spill-slot-order correction scopes OUT; the two
 * ARRAYS above are spilled for their whole lives and their order IS decisive, at
 * 7 encodings), but WHICH VARIABLE THE TEMP IS MATTERS -- a dedicated `int t`
 * gave 17, reusing `cur` 19, `redraw` 19, i/n 20, `c` 24, and done/r 294/295.
 *
 * `msg = 0xb06; f(msg,...); f(msg + 3,...)` reproduces the ROM's one pool entry
 * plus `add r5,#3` in a callee-saved register; two literals give two pool
 * entries.  The inverse spelling (`msg = 0xb09 ... msg - 3`) fuses one insn away
 * and is wrong.
 *
 * NEGATIVE RESULTS, so nobody re-spends them: `pp = &s->f30` as a temp;
 * `(unsigned char *)s + 0x30`; first-param type `unsigned int *` vs `void *`;
 * 0xf written as 15; `r = 1; r <<= 4;` for the 4th argument (10 and a reloc
 * mismatch -- 15 is a one-insn constant and const-propagation re-creates it at
 * the fill); a variable for the 4th argument in NINE different variable choices;
 * StopTask/StartTask prototypes as `void (*)(void)` vs `void *` and StopTask
 * returning int; _Func_801e7c0's msg param unsigned; an explicit `y = -0x18` in
 * either order; hoisting msg and/or y above the objs loop; two separate variables
 * for msg and msg+3.  Also: the `one = 1` unification this bank sometimes needs
 * BECAME UNNECESSARY once the struct and struct W landed.
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
