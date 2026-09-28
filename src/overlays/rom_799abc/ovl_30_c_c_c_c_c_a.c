// fakematch
/* OvlFunc_905_200921c  --  0x0200921c
 *
 * Cut out of goldensun/asm/overlays/rom_799abc/ovl_30_c_c_c_c.s (single
 * function, 163 instructions, confirmed by `grep -ci func_start`). The file's
 * `.data` tail and its ten already-global labels stay with the sibling `.s`;
 * this function reads none of them, so the split needs no new exports.
 *
 * EXACT: 396 bytes, 169 encodings and 27 relocations identical (objcmp --func).
 *
 * TWO OFFSETS WERE MISREAD FIRST TIME and it cost 30 differing. The `== 0x63`
 * test and the unsigned `(u16)(v - 2) <= 1` test above it read THE SAME slot,
 * gState + (0xe1 << 1) -- one through `ldrh`, one through `ldrsh`. Only the
 * `!= 0` test at the end reads (0x8d << 2). Reading the two `mov r2,#K / lsl`
 * offset builds as if they were the same offset in both places is the easy
 * mistake here; they are not.
 *
 * FOUR LEVERS, each with its single-drop measurement (tryc --align, of 170):
 *
 *  1. THE STACK-ARG-PAIR LEVER (src/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_c_b.c)
 *     is decisive: 38 -> 21. All three __Func_8010704 calls take two stack
 *     arguments and the ROM builds both into separate registers before storing
 *     either. Naming both, adjacent to the call, in the ROM's order, with the
 *     value that appears BOTH as a register argument and as a stack argument
 *     named once and used twice. Here that shared value is argument 3 == 6 in
 *     the first pair of calls and argument 1 == argument 5 in the other two.
 *
 *  2. THE ORR SITES NEED THE DESTINATION NAMED, per this overlay's own
 *     ovl_30_c_c_c_c_b.c: the ROM accumulates into the CONSTANT's register
 *     (`mov r3,#0x10 / orr r3,r2`) and `|= K` accumulates into the loaded
 *     byte's. Writing `t = *q; k = 0x10; k |= t; *q = k;` fixes it. The
 *     addition here is that NAMING THE POINTER IS FREE IF IT CARRIES THE
 *     OFFSET: `q = __MapActor_GetActor(9) + 0x59;` keeps the ROM's
 *     `add r0,#0x59` where that sibling's `p = ...; p[0x23] &= ...` cost
 *     `mov r1,r0`. The address is still a temp that dies in the statement.
 *
 *  3. WHERE THE SECOND ORR'S CONSTANT IS BORN DECIDES r5 vs r6, worth 21 -> 7.
 *     The ROM pushes {r5, r6, lr} and we pushed {r5, lr}: it has one more
 *     callee-saved quantity than we did. `k2 = 2;` written at the accumulate
 *     site is block-local, takes r3 (REG_ALLOC_ORDER starts 3, 2, 1, 0) and
 *     leaves r5 for the gState pointer. Assigned BEFORE the
 *     __MapActor_GetActor(0xa) call it crosses, it must be callee-saved, takes
 *     r5, and the gState pointer moves to r6 -- the ROM's allocation exactly.
 *     Hoisting it one statement FURTHER, above the `[0x22] = 2` store, is
 *     wrong by one instruction: cse then shares the single register between the
 *     two constant 2s and the ROM's separate `mov r3,#2` disappears. So the
 *     assignment has to sit between that store and the accumulate.
 *
 *  4. THE TWO __MapActor_SetPos CALLS WITH SHIFTED CONSTANTS need the argument
 *     order pinned, and this is the finding worth keeping. `calls.c:855`
 *     precomputes an argument whose `rtx_cost > 2` -- which a thumb shiftable
 *     CONST_INT is -- into a pseudo BEFORE any hard argument register is
 *     loaded, so `mov r0,#K` always lands last in the pre-sched stream and
 *     sched2 cannot pull it forward. The ROM has it in the MIDDLE of the
 *     mov/lsl group at both sites. Writing the first argument through a pin
 *     puts its `mov` where the source puts it.
 *
 *     THE EXACT SPELLING IS `p1 = K; p0 = S; p1 <<= N;` AND NOTHING ELSE
 *     REACHES IT. Measured, each against the finished rest of the function:
 *
 *       both operands pinned, `p1 = K; p2 = K2; p0 = S; p1 <<= N; p2 <<= N2;`
 *                                        exact at one site, 2 at the other
 *       one operand pinned, the other a literal           MATCH at both
 *       plain `int` locals in the same statement order        4 differing
 *       pin on r1 only, plain int for the slot                4 differing
 *       pin on r0 only, plain int for the value               6 differing
 *       no pins at all (literal arguments)                    2 differing
 *
 *     WHY THE SECOND SITE NEEDS THE SHORTER FORM, and it is an impossibility
 *     argument, not a preference. combine folds `p = K; p <<= N;` into one
 *     `(set p (const_int K<<N))` AT THE SHIFT'S POSITION, and split2 re-expands
 *     it there, so a pinned pair always ends up ADJACENT in the pre-sched
 *     stream. The ROM's order at that site is
 *         mov r1 / mov r2 / lsl r2 / mov r0 / lsl r1
 *     which needs LUID(mov r1) < LUID(mov r2) for the priority-2 tie AND
 *     LUID(lsl r2) < LUID(mov r0) < LUID(lsl r1) for the priority-1 tie. With
 *     both pairs adjacent those two conditions are contradictory: the first
 *     wants r1's pair first, the second wants r1's shift last. Leaving the r2
 *     operand as a literal breaks the pair (it goes through precompute, which
 *     does not fold) and the contradiction with it.
 *
 * INERT HERE, measured and recorded so the next agent does not retry them:
 *   - `g = gState;` written ONCE at the top of the function, or dropped
 *     entirely in favour of subscripting `gState` at all three sites: 21 -> 29
 *     both ways, and the second also loses 7 instructions. The ROM's two
 *     `ldr r6, =gState` loads, one per predecessor of the merge block, are
 *     what `g = gState;` in BOTH arms of the 0x301 `if` produces.
 *   - naming the `6` shared by the first two calls as a FUNCTION-SCOPE local
 *     (rather than one pair per call site): it takes r5 for the whole body and
 *     costs the prologue -- 21 -> 140 positional. The lever's "adjacent to the
 *     call" is not decoration.
 *   - separate m2/n2 locals for the fourth call rather than reusing m/n: 21,
 *     unchanged either way. Kept because it is clearer, not because it pays.
 *   - naming the gOvl_020098ec store's ADDRESS (`int *gp = &gOvl...; *gp = 0;`)
 *     or naming the __StartTask priority: 5 -> 8, the wrong direction. Naming
 *     the stored ZERO is what does it, 5 -> 2: `int z = 0; gOvl_020098ec = z;`
 *     gives the ROM's `ldr r3,=gOvl / mov r2,#0 / str r2,[r3]` where the plain
 *     literal store swaps the two registers.
 *
 * SHIMS: two `register ... __asm__` pins (r0 and r1), both load-bearing at the
 * first __MapActor_SetPos call -- see the table above. No `__asm__(".equ ...)`
 * lines and no other inline asm.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];
extern int gOvl_020098ec;

extern int __GetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8092158(int a, int b, int c);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __CutsceneWait(int n);
extern int __StartTask(void (*fn)(void), int prio);
extern void OvlFunc_905_2008ce0(void);
extern void OvlFunc_905_20090c8(void);

int OvlFunc_905_200921c(void)
{
    unsigned char *p;
    unsigned char *g;
    unsigned char *q;
    int t, k;
    int m, n;
    register int p0 __asm__("r0");
    register int p1 __asm__("r1");

    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x204;
    *(int *)(p + 0x1c8) = 0x18;
    q = __MapActor_GetActor(9) + 0x59;
    t = *q;
    k = 0x10;
    k |= t;
    *q = k;
    if (__GetFlag(0x302)) {
        p1 = 0xac; p0 = 8; p1 <<= 17;
        __MapActor_SetPos(p0, p1, 0xd0 << 15);
        m = 0x12;
        n = 6;
        __Func_8010704(0x18, 0x28, n, 3, m, n);
    } else {
        m = 0x12;
        n = 6;
        __Func_8010704(0x12, 0x28, n, 3, m, n);
    }
    if (__GetFlag(0xc0 << 2)) {
        __MapActor_SetPos(9, 0, 0);
        m = 0x15;
        n = 0xb;
        __Func_8010704(m, 0x2d, 4, 2, m, n);
    }
    if (__GetFlag(0x301)) {
        int v;
        p1 = 0x9a; p0 = 0xa; p1 <<= 18;
        __MapActor_SetPos(p0, p1, 0xe8 << 16);
        g = gState;
        v = *(unsigned short *)(g + (0xe1 << 1));
        if ((unsigned short)(v - 2) <= 1) {
            int m2, n2, t2, k2;
            __MapActor_GetActor(0xa)[0x22] = 2;
            k2 = 2;
            ((int *)__MapActor_GetActor(0xa))[3] -= 1;
            q = __MapActor_GetActor(0xa) + 0x23;
            t2 = *q;
            k2 |= t2;
            *q = k2;
            m2 = 0x24;
            n2 = 0xe;
            __Func_8010704(m2, 0x30, 5, 1, m2, n2);
        }
    } else {
        g = gState;
    }
    if (*(short *)(g + (0xe1 << 1)) == 0x63) {
        __MapTransitionIn();
        __WaitMapTransition();
        __MapActor_SetPos(9, 0xc0 << 17, 0xc0 << 16);
        __CutsceneWait(0x3c);
        __MapActor_GetActor(9)[0x22] = 2;
        __Func_8092158(9, 0xcc << 1, 0xc0);
        __CutsceneWait(0x3c);
        OvlFunc_905_2008ce0();
    }
    if (*(short *)(g + (0x8d << 2)) != 0) {
        int z = 0;
        gOvl_020098ec = z;
        __StartTask(OvlFunc_905_20090c8, 0xc8 << 4);
    }
    return 0;
}
