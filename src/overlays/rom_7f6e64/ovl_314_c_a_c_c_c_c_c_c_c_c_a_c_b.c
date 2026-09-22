/* OvlFunc_969_200c8d8 -- 244 instructions, 592 bytes, byte-identical (247
 * encodings, 48 relocations).  Split out of
 * asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_c_c_a_c.s; its file-mate
 * OvlFunc_969_200c23c did NOT land, so the file could not convert whole.  The
 * split was verified byte-neutral (make compare green) BEFORE this .c was written.
 *
 * ================================================================
 * local-alloc ORDERS QUANTITIES SHORTEST-LIVED FIRST, SO AN ACTOR POINTER REUSED
 * ACROSS N BLOCKS MUST BE N SEPARATE VARIABLES
 * ================================================================
 *
 * This is the decisive lever and it took 229 of 247 -> 10 of 247 in one edit.
 *
 * The formula was already on file (docs/elevation.md, local-alloc.c:1496):
 * QTY_CMP_PRI = floor_log2(n_refs) * n_refs * size / (death - birth).  What is new
 * is the CONSEQUENCE in a function with NO BRANCHES AT ALL.  Every pseudo is then
 * block-local, global-alloc never runs, and local-alloc's comment at
 * local-alloc.c:1480 -- "We give shorter-lived quantities higher priority" -- is
 * the whole allocation policy.
 *
 * One reused `p` is ONE LONG QUANTITY that sorts LAST and takes r7.  Four
 * p0..p3 are FOUR SHORT NON-CONFLICTING quantities that all get r5 -- the ROM's
 * register.
 *
 * THIS IS THE COUNTERPART OF BATCH 280'S "SPLIT A REUSED POINTER TO SHORTEN LIVE
 * RANGES", and the pair is worth holding together because the mechanisms are
 * opposite: there it was global_alloc, where LONGER live ranges win priority;
 * here it is local_alloc, whose preference is INVERTED.  Same entry point in the
 * diff either way -- the prologue saves one register too many, and the wrong one.
 * Check which pass owns the quantity (does the function branch at all?) before
 * reasoning about either formula.
 *
 * THREE SMALLER LEVERS, each measured:
 *
 *   __Func_80933f8(-1,-1,-1,0) unpinned is 151 of 247; { PIN4; q0=-1; q1=-1;
 *   q2=-1; q3=0; } is 8.  The pin is load-bearing, but the ORDER of the three
 *   `q=1; q=-q` spellings is INERT because sched2 decides -- so the plain `-1`
 *   form is correct and the elaborate negate form buys nothing.
 *
 *   A STORE ORDER THAT IS INVERTED BETWEEN SOURCE AND OUTPUT FIXED A REGISTER
 *   ASSIGNMENT TWO HUNDRED INSTRUCTIONS EARLIER.  Writing +0xc before +0x10 emits
 *   +0x10 before +0xc AND shortens `y` / lengthens `z` by one insn each, which
 *   flips their QTY_CMP_PRI and swaps r9/r10 to the ROM's.  8 -> 2.  Lengthening
 *   `z` directly, at three placements, got the registers right but moved the
 *   materialisation: 13, 87, 104.
 *
 *   __StartTask(fn, 0xc80) with the argument NAMED -- { int tk; tk = 0xc80; } --
 *   is worth the last 2.  A PIN2 form is equally exact; the named local is
 *   preferred as ordinary C with no extra scaffolding.
 *
 * Reused verbatim from the batch-280 donor: the e/f/g named-stack-argument pattern
 * for the four __Func_8010704 calls, and { int t = 4; t |= *s; *s = t; } for the
 * |= byte accumulator.
 *
 * -ffixed-r7 WAS PROBED TWICE ON TWO CANDIDATE SHAPES AND IS WRONG BOTH TIMES --
 * it forces a stack spill and grows the frame to #0xc.  See the caution added to
 * the Makefile's FIXEDR7_CFLAGS comment: the r7-vs-r9 prologue difference here is
 * a LIVE-RANGE problem, not a reservation problem.  No flag row is needed.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __DeleteFieldActor(int slot);
extern void __PlaySound(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_808bb2c(void);
extern void __Func_800fe9c(void);
extern void __WaitFrames(int n);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void OvlFunc_969_200d688(unsigned char *a);
extern void __Func_8092950(int a, int b);
extern void __StartTask(void *fn, int arg);
extern void OvlFunc_969_200da28(void);
extern void __MapTransitionIn(void);
extern void __Func_8091220(int a, int b);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern unsigned char *iwram_3001ebc;

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_969_200c8d8(void)
{
    unsigned char *p0;
    unsigned char *p1;
    unsigned char *p2;
    unsigned char *p3;
    int e, f, g;
    int m, n;
    int y, z, w, t;

    __DeleteFieldActor(0x14);
    __DeleteFieldActor(0x13);
    __PlaySound(0x8d);
    m = 0x11;
    n = 8;
    __Func_8010704(0x11, 0xa, 4, 2, m, n);
    m = 0x12;
    n = 0x17;
    __CopyMapTiles(0x66, 4, 0x4a, 4, m, n);
    m = 0x10;
    n = 0x15;
    __CopyMapTiles(0x27, 0x48, 0xb, 0x48, m, n);
    e = 0x16;
    f = 6;
    __Func_8010704(0x13, 6, 3, 7, e, f);
    g = 0xd;
    __Func_8010704(0x13, 6, 3, 7, g, f);
    __Func_8010704(0x13, 6, 3, 7, e, g);
    __Func_8010704(0x13, 6, 3, 7, g, g);
    { PIN3; q0 = 0x80; q1 = 0x80; q2 = 0x80; q0 <<= 10; q1 <<= 10; q2 <<= 9;
      __Func_8012330(q0, q1, q2); }
    { PIN4; q0 = -1; q1 = -1; q2 = -1; q3 = 0;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_808bb2c();
    __Func_800fe9c();
    __WaitFrames(1);
    __PlaySound(0x8a);
    __MapActor_SetAnim(0, 0x13);
    __MapActor_SetAnim(1, 0x12);
    __MapActor_SetAnim(2, 0x12);
    __MapActor_SetAnim(3, 0x12);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(1), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(2), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(3), 0);
    p0 = __MapActor_GetActor(0);
    *(int *)(p0 + 8) = 0xad << 17;
    y = 0x80 << 14;
    *(int *)(p0 + 0xc) = y;
    *(int *)(p0 + 0x10) = 0xcd << 16;
    OvlFunc_969_200d688(p0);
    p0[0x63] = 0;
    w = 0x80 << 10;
    *(int *)(p0 + 0x28) = w;
    p1 = __MapActor_GetActor(1);
    *(int *)(p1 + 8) = 0xb2 << 17;
    *(int *)(p1 + 0xc) = y;
    *(int *)(p1 + 0x10) = 0xc0 << 16;
    OvlFunc_969_200d688(p1);
    p1[0x63] = 0;
    *(int *)(p1 + 0x28) = w;
    p2 = __MapActor_GetActor(2);
    *(int *)(p2 + 8) = 0xb4 << 17;
    *(int *)(p2 + 0xc) = y;
    z = 0xde << 16;
    *(int *)(p2 + 0x10) = z;
    OvlFunc_969_200d688(p2);
    p2[0x63] = 0;
    *(int *)(p2 + 0x28) = w;
    p3 = __MapActor_GetActor(3);
    *(int *)(p3 + 8) = 0xa7 << 17;
    *(int *)(p3 + 0xc) = y;
    *(int *)(p3 + 0x10) = z;
    OvlFunc_969_200d688(p3);
    p3[0x63] = 0;
    *(int *)(p3 + 0x28) = w;
    *(__MapActor_GetActor(0x15) + 0x62) = 0;
    *(__MapActor_GetActor(6) + 0x62) = 0;
    {
        unsigned char *s;
        s = __MapActor_GetActor(0x17) + 0x55;
        t = 4;
        t |= *s;
        *s = t;
    }
    __Func_8092950(0x17, 4);
    { int tk; tk = 0xc80; __StartTask((void *)OvlFunc_969_200da28, tk); }
    {
        unsigned char *q;
        q = iwram_3001ebc;
        *(int *)(q + (0xe4 << 1)) = 1;
    }
    __MapTransitionIn();
    __Func_8091220(0x7fff, 0);
    __Func_8091200(0x80 << 9, 0);
    __Func_8091254(0x28);
    __WaitFrames(0x3c);
}
