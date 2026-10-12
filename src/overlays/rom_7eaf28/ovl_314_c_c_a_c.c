/* OvlFunc_960_2008d24 and OvlFunc_960_2008dc8 -- the two halves of one area-a5
 * cutscene hook.  Both guard on gState[0x1c0] == area 0xa5 and then reposition
 * actor 0xe, reset two of its fields and re-aim the camera through
 * __Func_8010704; 2008d24 tears the sequence down (__StopTask plus one halfword
 * poke at 0x500019e) and 2008dc8 starts it (__StartTask at priority 0xc80).
 *
 * Converted from asm/overlays/rom_7eaf28/ovl_314_c_c_a_c.s, the WHOLE FILE --
 * two functions, both parked, both landed together, no split needed.  Landed by
 * batch 339 brief C; batch 338 had recovered both bodies from scratch/.
 *
 * ONE RELOCATION FORM, AND IT IS THE ESTABLISHED ONE.  objcmp reports a single
 * differing word per function -- ref 0x000000a5 against our 0x00000000 carrying
 * R_ARM_ABS32 `_AREA_a5`.  area.sym:130 defines `_AREA_a5 = 0xa5`, so the linker
 * fills that word with 0xa5 and the bytes agree; the symbol exists precisely
 * because the ROM POOL-LOADS 0xa5 where a literal would give `cmp r2, #0xa5`.
 * Four already-matching files in this same directory ship the identical form
 * (ovl_314_a_c.c, ovl_314_c_a_c_a_b.c, ovl_314_c_c_a_a_b.c, ovl_314_c_c_c_b.c),
 * and 193 landed files use an `_AREA_` symbol.  `make compare` is the authority.
 *
 * WHAT CLOSED 2008dc8, which was at "ref 52 instructions against ours 51":
 * THE MISSING INSTRUCTION WAS THE EPILOGUE'S `pop {r5}`.  `push {r5, r14}` and
 * `push {r14}` are both one insn, so the prologue hid the gap entirely; the ROM
 * spends a callee-saved register on the zero stored at b[0x55] and therefore
 * pays for a second pop.  The park had diagnosed the r5 correctly and recorded
 * "hoisting `z = 0;` into the dominating block -- no change", which is true and
 * was the wrong block: the zero has to be born INSIDE the guarded body, before
 * the FIRST __MapActor_GetActor call, so its live range crosses two calls and
 * global.c cannot give it r0-r3.  Before the area test it is still sinkable and
 * gcc sinks it.  One statement, 51 -> 52 instructions and 15 -> 1 differing.
 *
 * > A value that must land in a callee-saved register needs its live range to
 * > cross a CALL, and the dominating block is not automatically on the far side
 * > of one.  Place the assignment immediately before the first call it must
 * > outlive, not at the top of the function.
 *
 * THE OTHER HALF OF 2008dc8 IS WHY THE PIECE-MATE MATTERED.  The two
 * stack-argument pairs needed the piece-mate's spelling, not this park's: a pair
 * of named locals born IMMEDIATELY BEFORE EACH CALL (`e1 = 0xf; f1 = 0x2c;`)
 * reproduces the ROM's "materialise both, then store both"
 * (`mov r3,#0xf / mov r2,#0x2c / str r3,[sp] / str r2,[sp,#4]`), where the same
 * locals DECLARED AND ASSIGNED AT THE TOP of the function give the register
 * reuse (`str r3,[sp] / mov r3,#0x2c / str r3,[sp,#4]`).  2008d24 already had
 * the per-site form and already matched that shape; 2008dc8 had the hoisted
 * form.  Sharing one pair of locals between the two sites scores identically to
 * two pairs, so it is the BIRTHPLACE that matters, not the count.
 *
 * WHAT CLOSED 2008d24, which was at 9 of 69 with the lengths already in phase:
 *   1. THE RETURN TYPE.  `pop {r1} / bx r1` against our `pop {r0} / bx r0`.
 *      `thumb_exit` (config/arm/arm.c:8305-8320) offers r0|r1|r2 as the pop
 *      scratch only when `mode == VOIDmode`; a return of size <= 4 offers r1|r2.
 *      So both of these functions are declared NON-VOID, and per
 *      docs/elevation.md the spelling is `int` with NO `return` statement
 *      anywhere -- adding `return 0;` materialises a value the ROM never
 *      produces.  Worth 2 encodings in each function.
 *   2. THE TAIL STORE.  The ROM reads the value first and REUSES r3 for the
 *      destination (`ldr r3,=.L1a00 / ldrh r2,[r3] / ldr r3,=0x500019e /
 *      strh r2,[r3]`); the single-expression store loads both addresses first.
 *      An `int` temporary for the loaded halfword gives the ROM's order.  An
 *      `unsigned short` temporary does NOT -- it adds an insn, which is the park
 *      header's own recorded negative and it is correct.
 *   3. THE LAST CALL'S ARGUMENT ORDER, `__Func_8010704(0xb, 0x47, 1, 1, q0, q1)`:
 *      rom `mov r1,#0x47 / mov r2,#1 / mov r3,#1 / mov r0,#0xb`, ours r3,r2,r0,r1.
 *      This came out on a `do { } while (0);` ANCHOR placed AFTER the following
 *      __StopTask call -- i.e. DOWNSTREAM of the contested window.  That reads
 *      backwards and is not: INSN_PRIORITY is computed over the whole basic
 *      block, so splitting the block later changes the priorities of everything
 *      in it, and the four movs are decided at the bottom of `rank_for_schedule`
 *      (haifa-sched.c:4111-4114) by INSN_LUID once priority, class and dependent
 *      count all tie.  The park had measured six spellings OF THE CALL ITSELF,
 *      all exactly 8 differing, and concluded the position was "decided below
 *      the source"; the axis it had not varied was the block boundary.
 *
 * > When a contested argument window is an INSN_LUID tie, an anchor placed
 * > DOWNSTREAM of it can still reorder it, because the priorities it perturbs
 * > are computed over the entire basic block.  Do not restrict an anchor sweep
 * > to the statements around the residue.
 *
 * The anchor is device-free: it emits no instruction and tools/shimcount.py
 * prints no row for this file.  No fakematch row is needed.
 *
 * MEASURED (ref 56 and 52 instructions):
 *   2008d24, the recovered park body                  56 insns, 9 differing
 *   ... + `int` return, no return statement           56 insns, 7
 *   ... + `int` temporary for the tail halfword       56 insns, 4
 *   ... + anchor after __StopTask                     56 insns, 1  <- pool word
 *   ... + `unsigned short` temporary instead          57 insns, 16  (worse)
 *   ... + six respellings of the last call's args     56 insns, 4   (all inert)
 *   2008dc8, the recovered park body                  51 insns, 15
 *   ... + pairs born per call site                    51 insns, 7
 *   ... + `z = 0;` in the dominating block            51 insns, 7   (inert)
 *   ... + `z = 0;` before the first GetActor          52 insns, 3
 *   ... + `int` return, no return statement           52 insns, 1  <- pool word
 *   the eight per-file FLAG groups on both best bodies           no improvement;
 *       -O1, -fno-gcse and -fno-schedule-insns2 are all much worse.
 *
 * Verified as a merged translation unit with objcmp --whole: 1 of 69 and 1 of 63,
 * each the `_AREA_a5` pool word, every relocation else identical.
 */
extern unsigned char gState[];
extern int _AREA_a5;
extern unsigned short L1a00[] __asm__(".L1a00");

extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_808edac(int a, int b, int c);
extern void __Func_808ee0c(void);
extern void __StopTask(void *f);
extern void __StartTask(void *fn, int pri);
extern void OvlFunc_960_2008ce4(void);

int OvlFunc_960_2008d24(void)
{
    unsigned char *g;
    int p0, p1;
    int q0, q1;
    int v;

    g = gState;
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_a5)) {
        __MapActor_GetActor(0xe)[0x23] = 2;
        __MapActor_GetActor(0xe)[0x55] = 3;
        __MapActor_SetPos(0xe, 0, 0);
        p0 = 0xf;
        p1 = 0x2c;
        __Func_8010704(0x10, 0x2c, 1, 1, p0, p1);
        __Func_808edac(0x64, 0, 0);
        __Func_8010704(0xc, 0x47, 1, 1, 0x7f, 0x7f);
        q0 = 0xc;
        q1 = 0x47;
        __Func_8010704(0xb, 0x47, 1, 1, q0, q1);
        __StopTask(OvlFunc_960_2008ce4);
        do { } while (0);
        v = L1a00[0];
        *(unsigned short *)0x500019e = v;
    }
}

int OvlFunc_960_2008dc8(void)
{
    unsigned char *a;
    unsigned char *b;
    unsigned int q;
    unsigned int off;
    int x, y;
    int m1, m2;
    int pri;
    int e1, f1, e2, f2;
    int z;

    x = 0xf8 << 16;
    y = 0xb2 << 18;
    m1 = -1;
    m2 = -1;
    pri = 0xc8 << 4;
    q = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    q += off;
    if (*(short *)q == (int)(&_AREA_a5)) {
        z = 0;
        a = __MapActor_GetActor(0xe);
        a[0x23] = 2;
        b = __MapActor_GetActor(0xe);
        b[0x55] = z;
        __MapActor_SetPos(0xe, x, y);
        e1 = 0xf;
        f1 = 0x2c;
        __Func_8010704(0x1f, 0x5f, 1, 1, e1, f1);
        __Func_808edac(0x64, m1, m2);
        __Func_808ee0c();
        e2 = 0xc;
        f2 = 0x47;
        __Func_8010704(0x7f, 0x7f, 1, 1, e2, f2);
        __StartTask(OvlFunc_960_2008ce4, pri);
    }
}
