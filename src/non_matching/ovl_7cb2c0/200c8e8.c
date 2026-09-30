/* NON-MATCHING, 6 of 696 encodings differ
 *
 * OvlFunc_945_200c8e8  --  0x0200c8e8
 *
 * SIZE AND COUNT ARE BOTH EXACT (1728 bytes, 696 encodings, both sides), so
 * objcmp's 6 IS a distance and not a saturated figure.  aligncmp separately:
 * aligned-equal 690, 99.1% of ref, 6 differing in 6 hunks -- the two tools agree
 * because there is no insert/delete left anywhere in the function.
 *
 * shimcount.py: PIN-FREE.  No inline asm, no "+r" barrier, no flag override.
 * Nothing for fakematch.txt.
 *
 * SPLIT SHAPE: asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_c_c_a_c_a.s holds
 * this function ALONE (one .thumb_func_start) and datacheck.py reports no data,
 * so NO text/data split is needed and split_s.py has nothing to do.  No label
 * needs `.global` either: the four field-actor tables this function loads
 * (.L72a0, .L7300, .L7360, .L73c0) are ALREADY `.global` in
 * asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_c_c.s:12-15, so the
 * `__asm__(".L72a0")` externs below link as they stand.  The 26-entry jump table
 * is gcc's own and needs nothing.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7cb2c0/200c8e8.c \
 *     asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_c_c_a_c_a.s --func OvlFunc_945_200c8e8
 * FINAL INSTALLED PATH: src/non_matching/ovl_7cb2c0/200c8e8.c
 * (If it ever matches, it belongs at
 *  src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_c_c_a_c_a.c.)
 *
 * THE BLOCKER: global.c's allocno processing order, case 0x13 only.
 * The residue is ONE class and every instruction of it is a high-register name:
 *     ref  mov sl, r2 / mov r1, sl / mov r8, r2 / mov r3, r8 (x2) / mov r1, sl
 *     ours mov r8, r2 / mov r1, r8 / mov sl, r2 / mov r3, sl (x2) / mov r1, r8
 * t8 (0xc0 << 6) and t10 (0xe0 << 1) are SWAPPED between r8 and r10; t9
 * (0xd0 << 8) lands in r9 in both.  Thumb REG_ALLOC_ORDER gives a call-crossing
 * value r5, r6, r7, r8, r10, r9, r11, so the ROM processed t8 first and we
 * process t10 first.  `allocno_compare`'s formula
 * floor_log2(n_refs)*n_refs/live_length explains OUR answer exactly and predicts
 * that only an n_refs or live_length change can flip it: t10 has FOUR refs
 * (def + `t10` and `t10 - 0x60` in the same call + the case's last call) against
 * t8's three, so floor_log2(4)*4 = 8 beats floor_log2(3)*3 = 3 and t10 wins
 * despite the longer range.  For the ROM's order t8 must out-rank t10, which
 * needs t10 down to three refs or t8 up to four, and neither is expressible
 * here: the third t10 reference IS the `sub r2, #0x60` the ROM emits, and there
 * is no third site for t8.
 *   MEASURED INERT against this, all still 6 of 696:
 *     declaration order t8/t9/t10/m instead of m/t8/t9/t10
 *     a named temp for the derived value: `w = t10 - 0x60;` before the call
 *     a separate scratch base: `w = 0xe0 << 1; t10 = w; ... w - 0x60`
 *       (this one reproduces the ROM's r2-holds-0x1c0-then-subtracts shape
 *        LITERALLY and still does not move the allocation)
 *     one-statement `t8 = 0xc0 << 6;` vs the split `t8 = 0xc0; t8 <<= 6;`
 * This is the register-rotation class of elevation.md's "REGISTER ALLOCATION: it
 * is systematic, and it is not reachable from C", and the targeting rule there
 * names it in advance: the ROM prologue saves r8/r9/r10 (`mov r7, r10 / push`),
 * which is the tell.
 *
 * THE LEVERS THAT PAID, in the order they paid, with figures.  First draft:
 * 631 of 696, 709 encodings (13 OVER), 1760 bytes -- a saturated figure.
 *
 * 1. ONE VARIABLE PER SWITCH ARM (block-scoped declarations), 709 -> 688
 *    encodings and the aligned figure 332 -> 179.  The first draft declared one
 *    `unsigned char *e` and one `int m/t8/t9/t10` at the top of the function and
 *    reused them across cases 5, 6, 7, 0xb, 0xc and 0x12, 0x13.  Each became ONE
 *    allocno with a live range spanning the whole 700-instruction body, so `e`
 *    took r7 and produced an `adds r7, r0, #0` copy at every actor store where
 *    the ROM uses r0 directly, and parameter `b` was pushed out of r6 into r8
 *    with a `mov r2, r8` at every use.  Per-arm scope gives each its own allocno.
 *    This is batch 295's "one-variable-per-region extends to switch arms", and
 *    the cost of getting it wrong is not local -- it moved a PARAMETER.
 * 2. THE DOMINATING-BLOCK CONSTANT LEVER, WITH THE SWITCH DISPATCH AS THE
 *    BRANCH, 688 -> 690 and the aligned figure 179 -> 155, then 155 -> 52 when
 *    applied to every repeated coordinate.  Case 3 calls __MapActor_Surprise
 *    four times with 0x81 << 1 and case 0 passes 0x80 << 8 twice, straight-line
 *    in both, and cse1 commons them into r5 with an `adds r1, r5, #0` per site
 *    where the ROM rebuilds `mov r1, #0x81 / lsl r1, #1`.  NEITHER
 *    -fno-rerun-cse-after-loop NOR -fno-gcse NOR both together moves ONE byte of
 *    this (631/709 in all three) -- exactly what the two-part precondition
 *    predicts for the no-branch case, and worth recording because the same flag
 *    is what makes OvlFunc_936_2009930 byte-exact in this same batch.
 *    What DOES reach it: declare the constant as a local ABOVE the switch and
 *    pass it by name.  The jump-table dispatch is the branch the lever needs, so
 *    gcc will not keep the value live across it and rematerialises at each site
 *    -- and the rematerialised sequence carries the ROM's interleave
 *    (`mov r1, #0xdb / mov r2, #0x98 / mov r0, #0x1b / lsl r1, #17 / lsl r2, #16`)
 *    for free.  Applied to 0x80<<8, 0x81<<1, the eight case-0xb SetPos
 *    coordinates, 0xb0<<8, 0xd0<<8, 0xdb<<17, 0x98<<16 and the three -1s of
 *    case 0x18 (`mov`+`neg` is a split build exactly as `mov`+`lsl` is).
 *    A NEW BOUNDARY ON THE LEVER, measured here: it is PER SITE, not per
 *    constant.  __Func_8092adc(0xf, ab, 0) wants the named `ab`, and
 *    OvlFunc_945_200c880(0x11, ab) four instructions later wants the LITERAL --
 *    the ROM does NOT interleave at that one call (`mov r1,#0xb0 / lsl r1,#8 /
 *    mov r0,#17`), and the named form puts `mov r0, #17` one slot early.  Worth
 *    2 encodings and 2 hunks on its own.  Read each site's order separately.
 * 3. BUILD THE CONSTANT AFTER THE CALL, WHICH MEANS NAMING THE POINTER, 12 -> 6.
 *    `*(short *)(__MapActor_GetActor(0xd) + 6) = u;` with `u` assigned before
 *    the call makes `u` cross the call, so it takes r5 where the ROM uses the
 *    scratch r3.  Naming the call result in a per-site block --
 *    `{ unsigned char *e = __MapActor_GetActor(0xd); int u = 0xc0 << 6;
 *       *(short *)(e + 6) = u; }` -- puts the build after the `bl`, which is
 *    where the ROM has it, and the pointer stays in r0.  Note this is the
 *    OPPOSITE of lever 1's advice and both are right: the pointer must be named
 *    (so the constant can be built after the call) and it must be named PER
 *    SITE (so it does not become a function-wide allocno).
 * 4. STORE INSIDE EACH ARM, 694 -> 696 encodings and 360 -> 12 of 696 -- the
 *    single biggest step, and the one that made the count exact.  Case 0xc is
 *    `if (b) v = 0xc0 << 6; else v = 0xa0 << 7; *(short *)(e + 6) = v;`, and gcc
 *    hoists the then-arm's `mov r3,#0xc0 / lsl r3,#6` ABOVE the test and inverts
 *    the branch, losing the ROM's `cmp r6,#0 / beq` pair entirely (2 encodings
 *    short, which is why the count would not close).  `goto` around the arms and
 *    a ternary are both byte-identical to the plain if/else -- 360 either way.
 *    Duplicating the STORE into both arms is what fixes it; gcc cross-jumps the
 *    two `strh` back into the ROM's single one, so the duplication costs nothing.
 *    This is elevation.md's "Store INSIDE each arm, or gcc will speculate the
 *    cheap one", and it is the third recorded instance.
 *
 * Readings worth keeping:
 *  - `top:` before the switch plus `op = 0xe; goto top;` at the end of case 0x12
 *    reproduces the ROM's `mov r7,#0 / mov r0,#0xe / mov r6,#0 / b <dispatch>`.
 *    The dispatch label sits immediately after the parameter copies
 *    (`mov r7, r1 / mov r6, r2`), which is where a C label before the first
 *    statement lands, so no tail-recursion machinery is involved.
 *  - 26 dense cases 0..0x19 give the real jump table; the `cmp r0,#0x19 / bls`
 *    range check needs the switch value to be UNSIGNED.
 *  - `for (i = 0; i < a; i++)` (case 4) and `for (i = a; i <= b; i++)`
 *    (case 0x14) both produce the ROM's guarded do-while via
 *    duplicate_loop_exit_test; case 0x11's constant bound 8 loses the guard
 *    because gcc folds `0 < 8`, which is why it has none in the ROM either.
 *  - an `int` local then a store through `short *` for 0x3000/0x5000-class
 *    values; a bare literal there POOLS (HImode has no split build).
 */
extern unsigned char *iwram_3001ebc[];
extern unsigned char L72a0[] __asm__(".L72a0");
extern unsigned char L7300[] __asm__(".L7300");
extern unsigned char L7360[] __asm__(".L7360");
extern unsigned char L73c0[] __asm__(".L73c0");
extern unsigned char gScript_945__0200e840[];
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092504(int a);
extern void __MapActor_SetAnim(int slot, int n);
extern void __MapActor_SetAnimSpeed(int slot, int n);
extern void __MapActor_Surprise(int slot, int n);
extern void __MapActor_SetPos(int slot, int x, int z);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetBehavior(int slot, void *s);
extern void __CutsceneWait(int n);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __WaitFrames(int n);
extern void __Func_8091e9c(int a);
extern void __ClearFlag(int id);
extern void __DeleteFieldActor(int a);
extern void __Func_80933f8(int a, int b, int c, int d);
extern unsigned char *__Func_8093554(void);
extern void __LoadFieldActors(void *p);
extern void OvlFunc_945_200c7cc(int a);
extern void OvlFunc_945_200b7d8(int a);
extern void OvlFunc_945_200c890(int a, int b, int c, int d);
extern void OvlFunc_945_200c8ac(int a, int b, int c, int d);
extern void OvlFunc_945_200c880(int a, int b);
extern void OvlFunc_945_200c5d0(void);
extern void OvlFunc_945_200c8e8(unsigned int op, unsigned int a, unsigned int b);

void OvlFunc_945_200c8e8(unsigned int op, unsigned int a, unsigned int b)
{
    int c0;
    int c3;
    int px, pz1, pz2, pz3, pz4, qx;
    int ab, ad;
    int m1, m2, m3;
    int rx, rz;

    c0 = 0x80 << 8;
    c3 = 0x81 << 1;
    px = 0xcd << 17;
    qx = 0xeb << 17;
    pz1 = 0xae << 16;
    pz2 = 0xce << 16;
    pz3 = 0x8f << 17;
    pz4 = 0x9e << 17;
    ab = 0xb0 << 8;
    ad = 0xd0 << 8;
    rx = 0xdb << 17;
    rz = 0x98 << 16;
    m1 = -1;
    m2 = -1;
    m3 = -1;
top:
    switch (op) {
    case 0:
        __Func_8092adc(0, 0, 0);
        __Func_8092adc(1, c0, 0);
        __Func_8092adc(2, 0, 0);
        __Func_8092adc(3, c0, b);
        break;
    case 1:
        __Func_8092adc(0, a, 0);
        __Func_8092adc(1, a, 0);
        __Func_8092adc(2, a, 0);
        __Func_8092adc(3, a, b);
        break;
    case 2:
        __MapActor_SetAnim(0, 3);
        __MapActor_SetAnim(1, 3);
        __MapActor_SetAnim(2, 3);
        __MapActor_SetAnim(3, 3);
        if (a != 0)
            __Func_8092504(3);
        if (b != 0)
            __CutsceneWait(b);
        break;
    case 3:
        __MapActor_Surprise(0, c3);
        __MapActor_Surprise(1, c3);
        __MapActor_Surprise(2, c3);
        __MapActor_Surprise(3, c3);
        __CutsceneWait(b);
        break;
    case 4:
      { unsigned int i;
        for (i = 0; i < a; i++)
            __MapActor_SetPos(i + 0xa, 0, 0); }
        break;
    case 5:
      { unsigned char *e; int h;
        e = __MapActor_GetActor(a);
        h = 0xa0; h <<= 7;
        *(short *)(e + 6) = h; }
        __MapActor_SetAnim(a, 5);
        __MapActor_SetAnimSpeed(a, b);
        break;
    case 6:
      { unsigned char *e; int h;
        e = __MapActor_GetActor(a);
        h = 0xa0; h <<= 7;
        *(short *)(e + 6) = h;
        *(int *)(e + 0x18) = 0xffff0000; }
        __MapActor_SetAnim(a, 5);
        __MapActor_SetAnimSpeed(a, b);
        break;
    case 7:
      { unsigned char *e; int h;
        e = __MapActor_GetActor(a);
        h = 0xa0; h <<= 7;
        *(short *)(e + 6) = h; }
        OvlFunc_945_200c7cc(a);
        if (b == 0)
            __MapActor_SetAnimSpeed(a, 0);
        break;
    case 8:
        *(int *)(iwram_3001ebc[0] + (0xe0 << 1)) = 0x202;
        __MapTransitionIn();
        if (a != 0)
            __WaitMapTransition();
        __CutsceneWait(0);
        break;
    case 9:
        __MapTransitionOut();
        __WaitMapTransition();
        if (a != 0)
            __Func_8091e9c(a);
        break;
    case 0xa:
        OvlFunc_945_200c8e8(0x18, 1, 0);
        OvlFunc_945_200c8e8(0x19, 0, 0);
      { int h6;
        h6 = 0x80;
        OvlFunc_945_200b7d8(0);
        h6 <<= 7;
        OvlFunc_945_200c890(0, 0xd8 << 1, 0xa8, h6);
        OvlFunc_945_200c890(1, 0xe0 << 1, 0xa8, h6);
        OvlFunc_945_200c890(2, 0xd4 << 1, 0x98, h6);
        OvlFunc_945_200c890(3, 0xe5 << 1, 0x98, h6); }
        break;
    case 0xb:
      { int hA, hC, hS;
        if (a != 0) {
            __MapActor_SetAnim(0xd, 1);
          { unsigned char *e = __MapActor_GetActor(0xd);
            int u1 = 0xc0 << 6;
            *(short *)(e + 6) = u1; }
          { unsigned char *e = __MapActor_GetActor(0xd);
            int u2 = 0x80 << 9;
            *(int *)(e + 0x18) = u2; }
        }
        __MapActor_SetAnim(0xe, 1);
      { unsigned char *e = __MapActor_GetActor(0xe);
        hA = 0xa0; hA <<= 7;
        *(short *)(e + 6) = hA; }
        __MapActor_SetAnim(0xf, 1);
      { unsigned char *e = __MapActor_GetActor(0xf);
        hC = 0xc0; hC <<= 6;
        *(short *)(e + 6) = hC; }
      { unsigned char *e = __MapActor_GetActor(0xf);
        hS = 0x80; hS <<= 9;
        *(int *)(e + 0x18) = hS; }
        __MapActor_SetAnim(0x10, 1);
        *(short *)(__MapActor_GetActor(0x10) + 6) = hA;
        __MapActor_SetAnim(0x11, 1);
        *(short *)(__MapActor_GetActor(0x11) + 6) = hC;
        *(int *)(__MapActor_GetActor(0x11) + 0x18) = hS;
        __MapActor_SetPos(0x1c, px, pz1);
        __MapActor_SetPos(0x1d, qx, pz1);
        __MapActor_SetPos(0x1e, px, pz2);
        __MapActor_SetPos(0x1f, qx, pz2);
        __MapActor_SetPos(0x20, px, pz3);
        __MapActor_SetPos(0x21, qx, pz3);
        __MapActor_SetPos(0x22, px, pz4);
        __MapActor_SetPos(0x23, qx, pz4);
        __WaitFrames(1);
        if (a != 0)
            __Func_8092adc(0xd, ab, 0);
        __Func_8092adc(0xe, ad, 0);
        __Func_8092adc(0xf, ab, 0);
        __Func_8092adc(0x10, ad, 0);
        OvlFunc_945_200c880(0x11, 0xb0 << 8); }
        break;
    case 0xc:
      { unsigned char *e; int v; int w;
        e = __MapActor_GetActor(a);
        __MapActor_SetAnim(a, 1);
        if (b != 0) {
            v = 0xc0 << 6;
            *(short *)(e + 6) = v;
        } else {
            v = 0xa0 << 7;
            *(short *)(e + 6) = v;
        }
        w = 0x80 << 9;
        *(int *)(e + 0x18) = w; }
        break;
    case 0xd:
        __MapActor_SetPos(9, 0, 0);
        __MapActor_SetPos(0xc, 0, 0);
        __MapActor_SetPos(0xb, 0, 0);
        __MapActor_SetPos(0xd, 0, 0);
        __MapActor_SetPos(0xa, 0, 0);
        break;
    case 0xe:
        __MapActor_SetPos(0xe, 0, 0);
        __MapActor_SetPos(0xd, 0, 0);
        break;
    case 0xf:
        OvlFunc_945_200c8e8(0x18, 1, 0);
        __MapActor_SetPos(9, 0, 0);
        __MapActor_SetPos(0xa, 0, 0);
        OvlFunc_945_200c890(8, 0xde << 1, 0x266, 0xd0 << 8);
        __MapActor_SetPos(0, 0, 0);
        if (a != 0)
            OvlFunc_945_200c5d0();
        OvlFunc_945_200c8ac(0xe0 << 17, 0x80 << 14, 0x9c << 18, 0x1000001);
        if (b != 0) {
            *(int *)(iwram_3001ebc[0] + (0xe0 << 1)) = 0x202;
            __MapTransitionIn();
            __WaitMapTransition();
            __CutsceneWait(0x14);
        }
        break;
    case 0x10:
        __MapActor_SetPos(8, 0, 0);
        __MapActor_SetPos(9, 0, 0);
        __MapActor_SetPos(0x1b, rx, rz);
        break;
    case 0x11:
      { unsigned int i;
        for (i = 0; i < 8; i++)
            __MapActor_SetPos(i + 0x1c, 0, 0); }
        break;
    case 0x12:
      { int m; int t8;
        m = 0xf5;
        OvlFunc_945_200c890(0xc, 0x98, 0x85 << 2, 0xb0 << 8);
        m <<= 1;
        t8 = 0xc0; t8 <<= 6;
        OvlFunc_945_200c890(8, 0x86, m, t8);
        OvlFunc_945_200c890(9, 0xa6, m, 0xa0 << 7);
        m += 0xe;
        OvlFunc_945_200c890(0xa, 0xb6, m, 0xa0 << 7);
        OvlFunc_945_200c890(0xb, 0x76, m, t8); }
        a = 0;
        op = 0xe;
        b = 0;
        goto top;
    case 0x13:
      { int m; int t8; int t9; int t10;
        OvlFunc_945_200c890(8, 0xd0 << 1, 0xa4 << 1, 0);
        t10 = 0xe0; t10 <<= 1;
        t9 = 0xd0; t9 <<= 8;
        OvlFunc_945_200c890(9, t10, t10 - 0x60, t9);
        t8 = 0xc0; t8 <<= 6;
        m = 0xcc;
        OvlFunc_945_200c890(0xa, 0xe3 << 1, 0xf8, t8);
        m <<= 1;
        OvlFunc_945_200c890(a, m, 0x91 << 1, 0);
        OvlFunc_945_200c890(b, m, 0xab << 1, 0);
        OvlFunc_945_200c890(0xd, 0xd2 << 1, 0xb2 << 1, t9);
        OvlFunc_945_200c890(0xe, m, 0x98 << 1, 0);
        m -= 0x1e;
        OvlFunc_945_200c890(0xf, 0xd1 << 1, m, t9);
        OvlFunc_945_200c890(0x10, 0xdc << 1, 0x83 << 1, t8);
        OvlFunc_945_200c890(0x11, t10, m, t9); }
        break;
    case 0x14:
      { unsigned int i;
        for (i = a; i <= b; i++)
            __ClearFlag(i); }
        break;
    case 0x15:
        OvlFunc_945_200c8e8(0x14, 0x92c, 0x93d);
        OvlFunc_945_200c8e8(0x14, 0x917, 0x91f);
        OvlFunc_945_200c8e8(0x14, 0x99 << 4, 0x998);
        __ClearFlag(0xc0 << 2);
        __ClearFlag(0x301);
        __ClearFlag(0x302);
        break;
    case 0x16:
        __WaitFrames(1);
        OvlFunc_945_200c8e8(0x17, 0, 0);
        __MapActor_SetBehavior(0xc, gScript_945__0200e840);
        break;
    case 0x17:
        __DeleteFieldActor(1);
        __DeleteFieldActor(2);
        __DeleteFieldActor(3);
        break;
    case 0x18:
        __Func_80933f8(m1, m2, m3, 0);
        __WaitFrames(1);
        if (a != 0) {
            unsigned char *e;
            e = __Func_8093554();
            e[0x55] = 0;
        }
        break;
    case 0x19:
        __LoadFieldActors(L72a0);
        __WaitFrames(1);
        if (a == 1) {
            __LoadFieldActors(L7300);
            __WaitFrames(1);
        } else if (a == 2) {
            __LoadFieldActors(L7360);
            __WaitFrames(1);
        } else if (a == 3) {
            __LoadFieldActors(L73c0);
            __WaitFrames(1);
        }
        break;
    }
}
