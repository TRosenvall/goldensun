// fakematch
/* OvlFunc_891_200905c  --  0x0200905c
 *
 * Was the whole of asm/overlays/rom_78c76c/ovl_30_c_c_a_c_c_a_a_c_a.s (ONE
 * function by the anchored .thumb_func_start pattern), so NO SPLIT IS NEEDED;
 * tools/datacheck.py prints nothing -- text-only, no data exports.
 * ONE overlay.ld row: overlays/rom_78c76c/overlay.ld:28 (asm/ -> src/).
 *
 * MATCH: 1116 bytes, 472 encodings and 74 relocations identical --
 * BUT ONLY WITH CSE_CFLAGS.  It needs a Makefile rule:
 *
 *   src/overlays/rom_78c76c/ovl_30_c_c_a_c_c_a_a_c_a.o: src/overlays/rom_78c76c/ovl_30_c_c_a_c_c_a_a_c_a.c
 *      $(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
 *      ...same recipe tail as the other CSE_CFLAGS rules...
 *
 * At plain -O2 it is 29 differing of 472, 94.7% aligned, 1120 bytes: gcc hoists
 * the save-bit constants 0x30a and 0x30b into callee-saved registers and copies
 * them (`adds r0, r6, #0`) where the ROM reloads each from the pool a second
 * time -- the textbook pool-constant CSE blocker with A BRANCH between the two
 * uses, which docs/elevation.md ("Pool-constant CSE: the complete rule") records
 * as reachable ONLY by -fno-rerun-cse-after-loop.  The 4 bytes it costs also
 * moves every pc-relative pool offset, which is why the -O2 figure looks like
 * 24 hunks rather than 4.  With the flag the second 0x30a reappears as a second
 * pool word and the whole thing is exact.
 *
 * SHIMS: 19 register pins in 17 PIN3 blocks (tools/shimcount.py).  NEEDS A
 * fakematch.txt ROW.  MINIMISED TO A FIXPOINT: all 17 were tested by dropping
 * each one and remeasuring -- NONE is inert.  The cheapest drop costs 3
 * encodings, the dearest 16 bytes (`drop pin 7` reads 1132 against 1116).
 *
 * ============================================================
 * WHAT IT TOOK -- 447 differing of 472 down to 0, in four passes.
 *
 * 1. THE PINNED ARGUMENT FILLS, 447 -> 136.  __MapActor_SetPos is called with a
 *    shifted 16.16 coordinate pair sixteen times and several pairs REPEAT
 *    (0xe8<<16 and 0xf0<<15 three times each, 0xac<<17 twice).  Unpinned, gcc
 *    commons them into r5/r6/r10, which costs the function TWO EXTRA CALLEE-SAVED
 *    PUSHES in the prologue (`mov r6,sl / mov r5,r9 / push {r5,r6}`) and a
 *    `adds r1,r6,#0` copy at every later site; the ROM rematerialises each
 *    `mov/lsl` pair.  Written UNIFORMLY -- `q0 = slot; q1 = X; q2 = Y;` ascending,
 *    whole value per statement -- sched2 reproduces the ROM's own variations,
 *    which put r0 last at some sites and first at others.  The two SetPos calls
 *    whose arguments are all 8-bit immediates (`5,0,0` and `1,0,0`) need NO pin:
 *    gcc already emits them ascending.
 *
 * 2. THE DESTRUCTIVE POINTER WALK AND THE NAMED OFFSET, and TWO gState BASES.
 *    - `*(int *)(p + 0x1c0) = 0x204` is ONE held value: the ROM computes
 *      0xe0<<1 into r2, adds it to the pointer, then ADDS 0x44 TO THE SAME
 *      REGISTER and stores it.  `n = 0xe0 << 1; q = (int *)(iwram_3001ebc + n);
 *      n += 0x44; *q = n;` -- naming the pointer is what stops gcc using a
 *      register-offset `str r3,[r2,r1]` and keeping n alive.
 *    - the actor's two byte fields: `a[0x59] = 0;` then `a += 0x23;` --
 *      DESTRUCTIVELY.  Indexing both off `a` lets CSE derive the second from the
 *      first (`subs r2, #54`), which is 0x59 - 0x23 and not in the ROM.
 *    - `gState + (0xe1 << 1)` must come from a NAMED BASE POINTER or gcc folds
 *      the addend into the pool word (`.word gState+450`) and loses the ROM's
 *      `mov r2,#0xe1 / lsl r2,#1 / add r3,r2`.  And it needs TWO SEPARATE
 *      LOCALS, one per region: shared, the pseudo spans the whole function and
 *      global-alloc gives it the callee-saved r5 at the FIRST site, where the
 *      ROM uses the scratch r3 and only reloads into r5 much later.
 *
 * 3. THE RANGE TEST IS IN HImode AND THE POINTER TYPE DECIDES IT, and this is
 *    the one real discovery here.  The ROM's
 *        ldrh r3,[r3] / mov r2,#0x80 / sub r3,#3 / lsl r3,#16 / lsl r2,#9 / cmp r3,r2 / bls
 *    is fold's range check for `v != 3 && v != 4` done in `short unsigned int`:
 *    build_range_check uses the type of the tested expression, so a SHORT type
 *    gives a HImode comparison, and Thumb has no HImode compare -- gcc shifts
 *    BOTH SIDES left by 16, which is where the otherwise unexplained
 *    `lsl r3,#16` and the 1<<16 on the right come from.
 *    READ IT THROUGH `short *`, NOT `unsigned short *`.  Both give a `ldrh`
 *    (only the low 16 bits are live, so combine narrows the load either way),
 *    but with an UNSIGNED short the -3 folds to the HImode constant 0xfffd,
 *    which is not an 8-bit immediate: gcc pools it and then derives the
 *    right-hand 0x10000 from the pool word with `add r2,r2,#3`.  Signed, the
 *    constant stays -3 and `sub r3,#3` is emitted, exactly as in the ROM.
 *    Two pool words and two instructions turn on the signedness of a cast that
 *    cannot change the program's value.
 *
 * 4. THE SIXTH ARGUMENT PAIR: THREE THINGS HAVE TO BE RIGHT AT ONCE, 136 -> 0.
 *    Six calls pass their 5th and 6th (stacked) arguments as constants that the
 *    ROM materialises into TWO LIVE REGISTERS -- `mov r3,#0x11 / mov r2,#7 /
 *    str r3,[sp] / str r2,[sp,#4]` -- where plain literals make gcc reuse ONE
 *    register (`mov r3,#0x11 / str r3,[sp] / mov r3,#7 / str r3,[sp,#4]`).  The
 *    two-live-registers-means-two-named-locals reading is right, but the naming
 *    alone is not enough and the two failures look nothing alike:
 *      - ONE SHARED PAIR FOR ALL SIX SITES gets the REGISTERS backwards (r2 for
 *        the 5th argument, r3 for the 6th).  Six sites make the pseudo span the
 *        function, so global-alloc rather than local-alloc assigns it.
 *      - A DISTINCT PAIR PER SITE, ASSIGNED 6th-THEN-5th, gets the registers
 *        right and the ORDER backwards -- 12 encodings, size and count exact.
 *      - A DISTINCT PAIR PER SITE, ASSIGNED 5th-THEN-6th, is exact.
 *    And naming only ONE of the two (5th named, 6th left a literal) is WORSE
 *    than naming neither: 18 differing, because gcc goes back to reusing r3.
 *    So: one local per stacked argument, one set per call site, assigned in
 *    ASCENDING argument order.
 *
 * ============================================================
 * OTHER THINGS READ OUT OF THE ROM.
 *
 *  - IT RETURNS int AND RETURNS 0 (`mov r0,#0` immediately before `add sp,#8`).
 *  - The 0x818 arm and the 0x816-and-0x817 arm END IN THE SAME CALL with
 *    different arguments; gcc's cross-jumping merges the tail (`mov r2,#2 /
 *    mov r3,#1 / bl __Func_8010704`) on its own from two plain calls -- the
 *    `b .L122a` in the reference is that, not a source shape.
 *  - The (2,2) stacked pair at `__CopyMapTiles(8,0x3c,0x11,0x27,...)` is ONE
 *    register in the ROM (`mov r3,#2 / str r3,[sp] / str r3,[sp,#4]`), so THAT
 *    one is literals -- the same pair of values, the opposite reading.  Count
 *    the registers at the `str [sp]` pair, never the values.
 *  - In the 0x80d and 0x80e blocks the 6th argument (1) is used a THIRD time by
 *    the block's last call, and that extra reference is what makes the ROM give
 *    it r5 and the 5th argument (2) r6 -- the reverse of the 0x80b and 0x80c
 *    blocks, where both are used twice.  Separate locals per block reproduce it.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];

extern void OvlFunc_891_20094b8(void);
extern void OvlFunc_891_2008150(void);
extern void OvlFunc_891_2008614(void);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __Func_8092b08(int a, int b);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8091ff0(int a);
extern void __Func_8012330(int a, int b, int c);
extern void __StartEarthquake(void);

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

int OvlFunc_891_200905c(void)
{
    unsigned char *a;
    unsigned char *gs1;
    unsigned char *gs2;
    int *q;
    int n;
    int m1, m2;
    int e, f;
    int g, h, i;
    int t5, t6, w5, w6, x5, x6, y5, y6;
    int b1, b2, b5, b6;
    int c1, c2, c5, c6;
    int d1, d2;
    int e1, e2;

    n = 0xe0 << 1;
    q = (int *)(iwram_3001ebc + n);
    n += 0x44;
    *q = n;
    OvlFunc_891_20094b8();
    __SetFlag(0xa2 << 1);
    a = __MapActor_GetActor(0x12);
    a[0x59] = 0;
    a += 0x23;
    m1 = 2;
    m1 |= *a;
    *a = m1;
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x12), 0);
    a = __MapActor_GetActor(0x12) + 0x23;
    m2 = 0xfe;
    m2 &= *a;
    *a = m2;
    __Func_8092b08(0x12, 1);
    gs1 = gState;
    if (*(short *)(gs1 + (0xe1 << 1)) != 3 && *(short *)(gs1 + (0xe1 << 1)) != 4) {
        __MapActor_SetPos(5, 0, 0);
        __MapActor_SetPos(1, 0, 0);
    }
    if (__GetFlag(0x818) != 0) {
        { PIN3; q0 = 0x12; q1 = 0x90 << 17; q2 = 0xb2 << 16; __MapActor_SetPos(q0, q1, q2); }
        { PIN3; q0 = 0x11; q1 = 0xc9 << 19; q2 = 0xc9 << 19; __MapActor_SetPos(q0, q1, q2); }
        { PIN3; q0 = 0xa; q1 = 0xe8 << 16; q2 = 0xf0 << 15; __MapActor_SetPos(q0, q1, q2); }
        { PIN3; q0 = 0xc; q1 = 0xac << 17; q2 = 0xf0 << 15; __MapActor_SetPos(q0, q1, q2); }
        { PIN3; q0 = 0xa; q1 = 0xe8 << 16; q2 = 0xf0 << 15; __MapActor_SetPos(q0, q1, q2); }
        e = 4;
        f = 3;
        __CopyMapTiles(0, 0x3b, 0xf, 0x26, e, f);
        { PIN3; q0 = 0xc; q1 = 0xac << 17; q2 = 0xf0 << 15; __MapActor_SetPos(q0, q1, q2); }
        __CopyMapTiles(4, 0x3b, 0x11, 0x26, e, f);
        __CopyMapTiles(8, 0x3c, 0x11, 0x27, 2, 2);
        t5 = 0x11;
        t6 = 7;
        __Func_8010704(0, 1, 2, 1, t5, t6);
    } else if (__GetFlag(0x816) != 0 && __GetFlag(0x817) != 0) {
        { PIN3; q0 = 0xa; q1 = 0xe8 << 16; q2 = 0xf0 << 15; __MapActor_SetPos(q0, q1, q2); }
        { PIN3; q0 = 0xc; q1 = 0xac << 17; q2 = 0xf0 << 15; __MapActor_SetPos(q0, q1, q2); }
        g = 2;
        __CopyMapTiles(0, 0x1c, 0x11, 8, g, 1);
        { PIN3; q0 = 0xa; q1 = 0xe8 << 16; q2 = 0xf0 << 15; __MapActor_SetPos(q0, q1, q2); }
        h = 4;
        i = 3;
        __CopyMapTiles(0, 0x3b, 0xf, 0x26, h, i);
        { PIN3; q0 = 0xc; q1 = 0xac << 17; q2 = 0xf0 << 15; __MapActor_SetPos(q0, q1, q2); }
        __CopyMapTiles(4, 0x3b, 0x11, 0x26, h, i);
        __CopyMapTiles(8, 0x3c, 0x11, 0x27, g, g);
        w5 = 0x11;
        w6 = 8;
        __Func_8010704(0, 0, 2, 1, w5, w6);
    } else {
        if (__GetFlag(0x816) != 0) {
            { PIN3; q0 = 0xa; q1 = 0xe8 << 16; q2 = 0xf0 << 15; __MapActor_SetPos(q0, q1, q2); }
            x5 = 4;
            x6 = 3;
            __CopyMapTiles(0, 0x3b, 0xf, 0x26, x5, x6);
        }
        if (__GetFlag(0x817) != 0) {
            { PIN3; q0 = 0xc; q1 = 0xac << 17; q2 = 0xf0 << 15; __MapActor_SetPos(q0, q1, q2); }
            y5 = 4;
            y6 = 3;
            __CopyMapTiles(4, 0x3b, 0x11, 0x26, y5, y6);
        }
    }
    if (__GetFlag(0x80b) != 0) {
        { PIN3; q0 = 9; q1 = 0xfc << 17; q2 = 0x98 << 16; __MapActor_SetPos(q0, q1, q2); }
        b1 = 2;
        b2 = 1;
        __CopyMapTiles(2, 0x1c, 0x22, 0xa, b1, b2);
        __CopyMapTiles(2, 0x1e, 0x10, 0xa, b1, b2);
        b5 = 4;
        b6 = 3;
        __CopyMapTiles(0, 0x37, 0x20, 0x28, b5, b6);
    }
    if (__GetFlag(0x80c) != 0) {
        { PIN3; q0 = 0xb; q1 = 0xa2 << 18; q2 = 0x98 << 16; __MapActor_SetPos(q0, q1, q2); }
        c1 = 2;
        c2 = 1;
        __CopyMapTiles(4, 0x1c, 0x24, 0xa, c1, c2);
        __CopyMapTiles(4, 0x1e, 0x12, 0xa, c1, c2);
        c5 = 4;
        c6 = 3;
        __CopyMapTiles(4, 0x37, 0x24, 0x28, c5, c6);
    }
    if (__GetFlag(0x80d) != 0) {
        { PIN3; q0 = 0xd; q1 = 0xfc << 17; q2 = 0xc8 << 16; __MapActor_SetPos(q0, q1, q2); }
        d2 = 1;
        d1 = 2;
        __CopyMapTiles(2, 0x1d, 0x22, 0xb, d1, d2);
        __CopyMapTiles(2, 0x1f, 0x10, 0xb, d1, d2);
        __CopyMapTiles(0, 0x3a, 0x20, 0x2b, 4, d2);
    }
    if (__GetFlag(0x80e) != 0) {
        { PIN3; q0 = 0xf; q1 = 0xa2 << 18; q2 = 0xc8 << 16; __MapActor_SetPos(q0, q1, q2); }
        e2 = 1;
        e1 = 2;
        __CopyMapTiles(4, 0x1d, 0x24, 0xb, e1, e2);
        __CopyMapTiles(4, 0x1f, 0x12, 0xb, e1, e2);
        __CopyMapTiles(4, 0x3a, 0x24, 0x2b, 4, e2);
    }
    gs2 = gState;
    if (*(short *)(gs2 + (0xe1 << 1)) == 3) {
        if (__GetFlag(0x30a) != 0) {
            __MapActor_SetPos(1, 0, 0);
            __MapActor_SetPos(5, 0, 0);
        } else if (__GetFlag(0x109) == 0) {
            OvlFunc_891_2008150();
            __SetFlag(0x30a);
        }
    }
    if (*(short *)(gs2 + (0xe1 << 1)) == 4) {
        if (__GetFlag(0x30b) != 0) {
            __MapActor_SetPos(1, 0, 0);
            __MapActor_SetPos(5, 0, 0);
        } else if (__GetFlag(0x109) == 0) {
            OvlFunc_891_2008614();
            __SetFlag(0x30b);
        }
    }
    if (__GetFlag(0x814) != 0) {
        __Func_8091ff0(0x8d);
        { PIN3; q0 = 0x80 << 9; q1 = 0x80 << 9; q2 = 0x80 << 9; __Func_8012330(q0, q1, q2); }
        __StartEarthquake();
    }
    return 0;
}
