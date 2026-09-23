/* Func_80a96d8 -- NON-MATCHING, 4 encodings of 319 (was 10; advanced in batch 282), SIZE DELTA ZERO (728 = 728).
 * Relocation lists identical in symbol AND order except two entries, both
 * consequences of the single residue below.
 *
 * ADVANCED IN BATCH 282 FROM 10 TO 4 OF 319, and the file below is the 4.  ONE LINE
 * did it:
 *
 *     StopTask(Func_80a19a0);
 *     __asm__ volatile ("");
 *
 * That empty volatile asm stops sched2 hoisting the NEXT statement's argument
 * precompute above the StopTask call.  It takes window 2 from 5 encodings to 2 AND
 * FIXES THE POOL-LITERAL ORDER -- which proves the two pool-word differences at
 * indices 314/315, and the offset at 299, were CONSEQUENCES OF THE SCHEDULE rather
 * than a pool problem.  The same barrier placed before `msg = 0xb06;` measures
 * identically at 4.
 *
 * (An empty `__asm__ volatile ("")` is a lighter tool than `do { } while (0);` --
 * batch 282 established that the latter plants TWO TOTAL barriers via
 * NOTE_INSN_LOOP_BEG/LOOP_END, see docs/elevation.md.  Reach for the empty asm when
 * one directional barrier is wanted.)
 *
 * BOTH REMAINING WINDOWS ARE AT THE LIST-SCHEDULING FLOOR, re-derived from
 * haifa-sched.c and the dumps:
 *   - idx 27-28: insns 41 and 55 are prio 3, depend_count 2, same class -- a PURE
 *     LUID TIE, and precompute_register_parameters iterates args[] FORWARD, so arg0's
 *     `add` always precedes arg3's `mov`.  New negative: the two-statement constant
 *     trick that closed Func_80b3050 does NOT transfer here (`r = 1; r <<= 4;` gives
 *     10 and a reloc mismatch), because 15 is a one-insn constant and
 *     const-propagation re-creates it at the fill.
 *   - idx 44-45: both prio 42, and one has depend_count 3 against the other's 2, so
 *     DEPEND_COUNT DECIDES BEFORE LUID and no source ordering reaches it.
 *     `prio(ldr r5) = prio(ldr r0) + 1` is structural: both anchor on the next call's
 *     fill insns, and arm_adjust_cost's load-feeding-a-call cost of 1 is what costs
 *     the pool load its point.
 *
 * Blocker class: sched2 ready-list ranking in the prologue -- two slips, both in
 * haifa-sched.c's rank_for_schedule.  NOT structural, NOT pool, NOT register
 * allocation, all of which match.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_a1000/80a96d8.c \
 *     asm/rom_a1000/rom_a8604_c_a_a.s --func Func_80a96d8
 * The reference holds THREE functions and this is the last, so a split is required.
 * Its pool is at the end and the candidate reproduces that; there was never a pool
 * problem.
 *
 * ================================================================
 * THE BIGGEST LEVER OF THE SESSION: iwram_3001f2c IS A STRUCT, NOT AN
 * `unsigned char *` -- WORTH ABOUT 290 ENCODINGS
 * ================================================================
 *
 * Under -fstrict-aliasing (ON at -O2 in this gcc; measured),
 * `*(unsigned int *)(p+8)` and `*(unsigned short *)(p+off)` land in DIFFERENT alias
 * sets, so cse merges two textual `ldrh`s across the intervening `str`.  A STRUCT
 * MEMBER ACCESS TAKES THE RECORD'S ALIAS SET, so `s->f08 = s->arr[i]` does NOT merge
 * and the ROM's three separate `ldrh r3,[r7,r2]` reproduce.
 *
 * The struct also fixes the ADDRESSING FORM for free: `s->arr[i]` emits
 * `mov r3,#0x82 / lsl r3,#2 / lsl r2,r5,#1 / add r2,r2,r3 / ldrh r3,[r7,r2]` --
 * offset in a register with the base in the addressing mode -- whereas a hand-cast
 * `state + (0x208 + i*2)` gets REASSOCIATED by fold into `(state+i*2)+0x208` and
 * materialises the address.
 *
 * Ladder: v1 raw-cast 290 differing / +20 bytes; v2 offset-locals 305; v3 loop
 * rotation 298 / size exact; v4 structs 302 -> 299 -> 11; final 10.
 *
 * A POINTER FIELD MUST POINT AT A STRUCT WITH NO `int` MEMBER for a later `int` load
 * to hoist above its store -- a distinct lever from the above, same machinery,
 * opposite direction.  The reference hoists `ldr r1,[ctx+0x10]` above
 * `strb r6,[s->f14+5]`.  With `unsigned char *f14` the store is alias set 0 (char)
 * and conflicts with everything, so no hoist (16 differing).  With
 * `struct W { unsigned char pad[5]; unsigned char f5; } *f14`, W's set does not
 * conflict with `int` and the hoist happens (10).  THE CONTROL WAS MEASURED: adding
 * ONE `int` MEMBER to W re-records int as a subset of W and kills the hoist again.
 *
 * ================================================================
 * THE OUTER-LOOP ROTATION LEVER, AND WHY IT INVALIDATES AN INNER LOOP ON PURPOSE
 * ================================================================
 *
 * expand_end_loop (stmt.c ~2360) scans from the loop top for the LAST jump to
 * end_label, stops at the first nested NOTE_INSN_LOOP_BEG, then moves everything up
 * to that jump to the bottom.  Two plain `if (x) break;` tests move only the tests,
 * leaving the refresh block at the loop top.  Writing the guard as
 * `if (done == 0 && _GetFlag(0x150) == 0) { refresh; i = 0; } else { break; }` puts
 * an UNCONDITIONAL `goto end_label` AFTER the refresh, so the refresh and the
 * `i = 0` move too.
 *
 * The back edge then lands on the copy loop's TEST -- a jump INTO THE MIDDLE of that
 * loop.  That gives it multiple entry points, loop.c marks it invalid, and it gets
 * NO invariant hoisting, NO strength reduction and NO reversal.  Which is exactly
 * the ROM's raw indexed copy loop, while the SECOND copy loop, a normal nested for,
 * is fully strength-reduced and reversed.
 *
 * **TWO COPY LOOPS WITH IDENTICAL SOURCE SHAPE COMPILING DIFFERENTLY IS THE TELL
 * THAT ONE OF THEM IS JUMPED INTO.**  It also removed a fifth spill slot (gcc had
 * been hoisting &buf[0] and spilling it, frame 88 -> 84).
 *
 * ================================================================
 * TWO MORE MECHANISMS, both verified in the compiler source in the image
 * ================================================================
 *
 * FRAME_GROWS_DOWNWARD IS 1 FOR ARM HERE (arm.h:1297), SO THE FIRST-DECLARED LOCAL
 * GETS THE HIGHEST sp OFFSET.  `buf` at sp+0x34 and `ctx` at sp+0x18 required
 * `short buf[15];` declared BEFORE `int ctx[7];`.  Swapping them costs 5 encodings.
 *
 * A DEAD ONE-LINE TEMP BEFORE `ret = 0` FLIPS THE PROLOGUE'S HARD-REGISTER
 * ASSIGNMENT AND ENABLES reload_cse_move2add -- 299 -> 17, the largest single edit
 * after the struct.  The ROM derives 0x208 as `subs r0,#18` from the register
 * already holding 0x21a, which is postreload's move2add and only fires if both
 * constants get the SAME hard reg.  Controls measured: DECLARATION-ORDER PERMUTATION
 * IS WORTHLESS HERE (all 17 positions of `int ret;` gave 298 or 299, no spread), but
 * WHICH VARIABLE THE TEMP IS MATTERS -- a dedicated `int t` gave 17, reusing `cur`
 * 19, `redraw` 19, i/n 20, `c` 24, and done/r 294/295.
 *
 * ================================================================
 * THE RESIDUE -- two prologue windows, both sched2
 * ================================================================
 *
 * Window 1, ref +0x038, 2 encodings: the ROM emits `movs r3,#15 / adds r0,#48`
 * where ours emits `adds r0,#48 / movs r3,#15`.  Both candidates are prio 3 with
 * depend_count 2, so the tie falls to INSN_LUID -- and `add r0,#48` ALWAYS has the
 * earlier LUID because precompute_register_parameters (calls.c) forces arg0 to a
 * pseudo whenever rtx_cost(value,SET) > 2, and RTX_COSTS returns
 * COSTS_N_INSNS(1) = 4 for any PLUS, unconditionally.  LOAD_ARGS_REVERSED is NOT
 * defined for ARM in this tree (grepped), so arg0's load can never get a later LUID.
 *
 * Window 2, ref +0x05a..0x060, 8 encodings plus the pool-literal order swap.  Read
 * from -fsched-verbose=6: `ldr r0,=Func_80a19a0` has prio 41 while `ldr r5,=0xb06`
 * and `mov r6,#24` have prio 42, SO OURS IS THE CORRECT LIST-SCHEDULING ANSWER.
 * The asymmetry is arm_adjust_cost (arm.c:2429, "Call insns don't incur a stall,
 * even if they follow a load"), which returns cost 1 for a load feeding a CALL_INSN
 * while `ldr r5 -> mov r0,r5` keeps cost 2.  So prio(ldr r0) = 1 + 40 and
 * prio(ldr r5) = 2 + 40, and the ROM's order requires the opposite.
 *
 * ONE SPELLING IN THE C BELOW IS FLAGGED BY ITS OWN AUTHOR AS UNSATISFYING:
 * `n = 0xc80; StartTask(Func_80a19a0, n);` is worth exactly 1 encoding (11 -> 10) by
 * giving the `mov r1,#200 / lsl r1,#4` pair an earlier LUID than `ldr r0,[pc]`, and
 * it is the same LUID mechanism as window 1.  IF SOMEONE FINDS THE REAL CAUSE OF
 * THE TWO PROLOGUE SLIPS, THAT LINE SHOULD COME OUT WITH IT.  Everything else in
 * the file reads as ordinary C.
 *
 * `msg = 0xb06; f(msg,...); f(msg + 3,...)` reproduces the ROM's one pool entry plus
 * `add r5,#3` in a callee-saved register; two literals give two pool entries.  The
 * inverse spelling (`msg = 0xb09 ... msg - 3`) fuses one insn away and is wrong.
 *
 * ABLATIONS PROVING EVERY REMAINING LEVER IS LOAD-BEARING, from 10: drop `c = 0xf0`
 * -> 13; declare ctx before buf -> 15; f14 back to `unsigned char *` -> 16; drop
 * `n = 0xc80` -> 11; two plain `if (...) break;` -> 241 and +8 bytes; drop the `t`
 * temp -> 299.
 *
 * NEGATIVE RESULTS, so nobody re-spends them: `pp = &s->f30` as a temp;
 * `(unsigned char *)s + 0x30`; first-param type `unsigned int *` vs `void *`; 0xf
 * written as 15; a variable for the 4th argument in NINE different variable choices;
 * StopTask/StartTask prototypes as `void (*)(void)` vs `void *` and StopTask
 * returning int; _Func_801e7c0's msg param unsigned; an explicit `y = -0x18` in
 * either order; hoisting msg and/or y above the objs loop; two separate variables
 * for msg and msg+3.  Also: the `one = 1` unification this bank sometimes needs
 * BECAME UNNECESSARY once the struct and struct W landed -- the plain literal now
 * gives the same 10, so it is not in the final C.
 *
 * No symbol tells.  No per-file Makefile override applies to this stem.
 */
/* Func_80a96d8 -- NON-MATCHING, 4 encodings of 319 (was 10; advanced in batch 282), SIZE DELTA ZERO (728 = 728).
 * Relocation lists identical in symbol AND order except two entries, both
 * consequences of the single residue below.
 *
 * ADVANCED IN BATCH 282 FROM 10 TO 4 OF 319, and the file below is the 4.  ONE LINE
 * did it:
 *
 *     StopTask(Func_80a19a0);
 *     __asm__ volatile ("");
 *
 * That empty volatile asm stops sched2 hoisting the NEXT statement's argument
 * precompute above the StopTask call.  It takes window 2 from 5 encodings to 2 AND
 * FIXES THE POOL-LITERAL ORDER -- which proves the two pool-word differences at
 * indices 314/315, and the offset at 299, were CONSEQUENCES OF THE SCHEDULE rather
 * than a pool problem.  The same barrier placed before `msg = 0xb06;` measures
 * identically at 4.
 *
 * (An empty `__asm__ volatile ("")` is a lighter tool than `do { } while (0);` --
 * batch 282 established that the latter plants TWO TOTAL barriers via
 * NOTE_INSN_LOOP_BEG/LOOP_END, see docs/elevation.md.  Reach for the empty asm when
 * one directional barrier is wanted.)
 *
 * BOTH REMAINING WINDOWS ARE AT THE LIST-SCHEDULING FLOOR, re-derived from
 * haifa-sched.c and the dumps:
 *   - idx 27-28: insns 41 and 55 are prio 3, depend_count 2, same class -- a PURE
 *     LUID TIE, and precompute_register_parameters iterates args[] FORWARD, so arg0's
 *     `add` always precedes arg3's `mov`.  New negative: the two-statement constant
 *     trick that closed Func_80b3050 does NOT transfer here (`r = 1; r <<= 4;` gives
 *     10 and a reloc mismatch), because 15 is a one-insn constant and
 *     const-propagation re-creates it at the fill.
 *   - idx 44-45: both prio 42, and one has depend_count 3 against the other's 2, so
 *     DEPEND_COUNT DECIDES BEFORE LUID and no source ordering reaches it.
 *     `prio(ldr r5) = prio(ldr r0) + 1` is structural: both anchor on the next call's
 *     fill insns, and arm_adjust_cost's load-feeding-a-call cost of 1 is what costs
 *     the pool load its point.
 *
 * Blocker class: sched2 ready-list ranking in the prologue -- two slips, both in
 * haifa-sched.c's rank_for_schedule.  NOT structural, NOT pool, NOT register
 * allocation, all of which match.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_a1000/80a96d8.c \
 *     asm/rom_a1000/rom_a8604_c_a_a.s --func Func_80a96d8
 * The reference holds THREE functions and this is the last, so a split is required.
 * Its pool is at the end and the candidate reproduces that; there was never a pool
 * problem.
 *
 * ================================================================
 * THE BIGGEST LEVER OF THE SESSION: iwram_3001f2c IS A STRUCT, NOT AN
 * `unsigned char *` -- WORTH ABOUT 290 ENCODINGS
 * ================================================================
 *
 * Under -fstrict-aliasing (ON at -O2 in this gcc; measured),
 * `*(unsigned int *)(p+8)` and `*(unsigned short *)(p+off)` land in DIFFERENT alias
 * sets, so cse merges two textual `ldrh`s across the intervening `str`.  A STRUCT
 * MEMBER ACCESS TAKES THE RECORD'S ALIAS SET, so `s->f08 = s->arr[i]` does NOT merge
 * and the ROM's three separate `ldrh r3,[r7,r2]` reproduce.
 *
 * The struct also fixes the ADDRESSING FORM for free: `s->arr[i]` emits
 * `mov r3,#0x82 / lsl r3,#2 / lsl r2,r5,#1 / add r2,r2,r3 / ldrh r3,[r7,r2]` --
 * offset in a register with the base in the addressing mode -- whereas a hand-cast
 * `state + (0x208 + i*2)` gets REASSOCIATED by fold into `(state+i*2)+0x208` and
 * materialises the address.
 *
 * Ladder: v1 raw-cast 290 differing / +20 bytes; v2 offset-locals 305; v3 loop
 * rotation 298 / size exact; v4 structs 302 -> 299 -> 11; final 10.
 *
 * A POINTER FIELD MUST POINT AT A STRUCT WITH NO `int` MEMBER for a later `int` load
 * to hoist above its store -- a distinct lever from the above, same machinery,
 * opposite direction.  The reference hoists `ldr r1,[ctx+0x10]` above
 * `strb r6,[s->f14+5]`.  With `unsigned char *f14` the store is alias set 0 (char)
 * and conflicts with everything, so no hoist (16 differing).  With
 * `struct W { unsigned char pad[5]; unsigned char f5; } *f14`, W's set does not
 * conflict with `int` and the hoist happens (10).  THE CONTROL WAS MEASURED: adding
 * ONE `int` MEMBER to W re-records int as a subset of W and kills the hoist again.
 *
 * ================================================================
 * THE OUTER-LOOP ROTATION LEVER, AND WHY IT INVALIDATES AN INNER LOOP ON PURPOSE
 * ================================================================
 *
 * expand_end_loop (stmt.c ~2360) scans from the loop top for the LAST jump to
 * end_label, stops at the first nested NOTE_INSN_LOOP_BEG, then moves everything up
 * to that jump to the bottom.  Two plain `if (x) break;` tests move only the tests,
 * leaving the refresh block at the loop top.  Writing the guard as
 * `if (done == 0 && _GetFlag(0x150) == 0) { refresh; i = 0; } else { break; }` puts
 * an UNCONDITIONAL `goto end_label` AFTER the refresh, so the refresh and the
 * `i = 0` move too.
 *
 * The back edge then lands on the copy loop's TEST -- a jump INTO THE MIDDLE of that
 * loop.  That gives it multiple entry points, loop.c marks it invalid, and it gets
 * NO invariant hoisting, NO strength reduction and NO reversal.  Which is exactly
 * the ROM's raw indexed copy loop, while the SECOND copy loop, a normal nested for,
 * is fully strength-reduced and reversed.
 *
 * **TWO COPY LOOPS WITH IDENTICAL SOURCE SHAPE COMPILING DIFFERENTLY IS THE TELL
 * THAT ONE OF THEM IS JUMPED INTO.**  It also removed a fifth spill slot (gcc had
 * been hoisting &buf[0] and spilling it, frame 88 -> 84).
 *
 * ================================================================
 * TWO MORE MECHANISMS, both verified in the compiler source in the image
 * ================================================================
 *
 * FRAME_GROWS_DOWNWARD IS 1 FOR ARM HERE (arm.h:1297), SO THE FIRST-DECLARED LOCAL
 * GETS THE HIGHEST sp OFFSET.  `buf` at sp+0x34 and `ctx` at sp+0x18 required
 * `short buf[15];` declared BEFORE `int ctx[7];`.  Swapping them costs 5 encodings.
 *
 * A DEAD ONE-LINE TEMP BEFORE `ret = 0` FLIPS THE PROLOGUE'S HARD-REGISTER
 * ASSIGNMENT AND ENABLES reload_cse_move2add -- 299 -> 17, the largest single edit
 * after the struct.  The ROM derives 0x208 as `subs r0,#18` from the register
 * already holding 0x21a, which is postreload's move2add and only fires if both
 * constants get the SAME hard reg.  Controls measured: DECLARATION-ORDER PERMUTATION
 * IS WORTHLESS HERE (all 17 positions of `int ret;` gave 298 or 299, no spread), but
 * WHICH VARIABLE THE TEMP IS MATTERS -- a dedicated `int t` gave 17, reusing `cur`
 * 19, `redraw` 19, i/n 20, `c` 24, and done/r 294/295.
 *
 * ================================================================
 * THE RESIDUE -- two prologue windows, both sched2
 * ================================================================
 *
 * Window 1, ref +0x038, 2 encodings: the ROM emits `movs r3,#15 / adds r0,#48`
 * where ours emits `adds r0,#48 / movs r3,#15`.  Both candidates are prio 3 with
 * depend_count 2, so the tie falls to INSN_LUID -- and `add r0,#48` ALWAYS has the
 * earlier LUID because precompute_register_parameters (calls.c) forces arg0 to a
 * pseudo whenever rtx_cost(value,SET) > 2, and RTX_COSTS returns
 * COSTS_N_INSNS(1) = 4 for any PLUS, unconditionally.  LOAD_ARGS_REVERSED is NOT
 * defined for ARM in this tree (grepped), so arg0's load can never get a later LUID.
 *
 * Window 2, ref +0x05a..0x060, 8 encodings plus the pool-literal order swap.  Read
 * from -fsched-verbose=6: `ldr r0,=Func_80a19a0` has prio 41 while `ldr r5,=0xb06`
 * and `mov r6,#24` have prio 42, SO OURS IS THE CORRECT LIST-SCHEDULING ANSWER.
 * The asymmetry is arm_adjust_cost (arm.c:2429, "Call insns don't incur a stall,
 * even if they follow a load"), which returns cost 1 for a load feeding a CALL_INSN
 * while `ldr r5 -> mov r0,r5` keeps cost 2.  So prio(ldr r0) = 1 + 40 and
 * prio(ldr r5) = 2 + 40, and the ROM's order requires the opposite.
 *
 * ONE SPELLING IN THE C BELOW IS FLAGGED BY ITS OWN AUTHOR AS UNSATISFYING:
 * `n = 0xc80; StartTask(Func_80a19a0, n);` is worth exactly 1 encoding (11 -> 10) by
 * giving the `mov r1,#200 / lsl r1,#4` pair an earlier LUID than `ldr r0,[pc]`, and
 * it is the same LUID mechanism as window 1.  IF SOMEONE FINDS THE REAL CAUSE OF
 * THE TWO PROLOGUE SLIPS, THAT LINE SHOULD COME OUT WITH IT.  Everything else in
 * the file reads as ordinary C.
 *
 * `msg = 0xb06; f(msg,...); f(msg + 3,...)` reproduces the ROM's one pool entry plus
 * `add r5,#3` in a callee-saved register; two literals give two pool entries.  The
 * inverse spelling (`msg = 0xb09 ... msg - 3`) fuses one insn away and is wrong.
 *
 * ABLATIONS PROVING EVERY REMAINING LEVER IS LOAD-BEARING, from 10: drop `c = 0xf0`
 * -> 13; declare ctx before buf -> 15; f14 back to `unsigned char *` -> 16; drop
 * `n = 0xc80` -> 11; two plain `if (...) break;` -> 241 and +8 bytes; drop the `t`
 * temp -> 299.
 *
 * NEGATIVE RESULTS, so nobody re-spends them: `pp = &s->f30` as a temp;
 * `(unsigned char *)s + 0x30`; first-param type `unsigned int *` vs `void *`; 0xf
 * written as 15; a variable for the 4th argument in NINE different variable choices;
 * StopTask/StartTask prototypes as `void (*)(void)` vs `void *` and StopTask
 * returning int; _Func_801e7c0's msg param unsigned; an explicit `y = -0x18` in
 * either order; hoisting msg and/or y above the objs loop; two separate variables
 * for msg and msg+3.  Also: the `one = 1` unification this bank sometimes needs
 * BECAME UNNECESSARY once the struct and struct W landed -- the plain literal now
 * gives the same 10, so it is not in the final C.
 *
 * No symbol tells.  No per-file Makefile override applies to this stem.
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
extern void Func_80a10d0(void *p, int a, int b, int c, int d, int e);
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
