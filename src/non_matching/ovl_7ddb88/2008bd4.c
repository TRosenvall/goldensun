/* OvlFunc_955_2008bd4 -- NON-MATCHING, 236 encodings of 537, size 1292 against the
 * ROM's 1288 (+4), 539 instructions against 537.  517 instructions in the reference.
 *
 * READ BOTH NUMBERS: 236 positionally under objcmp, but 78 ALIGNMENT-ROBUST
 * differences.  See the measurement note below -- on this function the positional
 * count actively MIS-RANKS candidates.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7ddb88/2008bd4.c \
 *     asm/overlays/rom_7ddb88/ovl_30_c_c_c_c_a_c_a.s
 * ONE function, no data section -- CONVERTS WHOLE when it lands.
 *
 * It is a STRUCTURAL SIBLING of the landed
 * src/overlays/rom_7e0928/ovl_30_c_c_c_c_a_a.c and EVERY lever from it transferred:
 * the `return 0` epilogue read, named stack arguments, PIN2 on OvlFunc_common1_1608,
 * the `int` return type on OvlFunc_common1_1ecc, direct-call indexing, and the
 * loop-order sweep.  Start there, not from the disassembly.
 *
 * ================================================================
 * BLOCKER: LOOP-INVARIANT MOTION OF A POOLED CONSTANT THAT THE ROM DOES NOT PERFORM
 * -- and loop.c's own arithmetic says no spelling of a literal can decline it
 * ================================================================
 *
 * Residue window is the else arm's five-iteration loop at .Lf0c:
 *
 *     ours (outside the loop)          ROM
 *     mov r7,#0x80                     mov r7,#0x80
 *     ldr r3,=0x3333 / mov r10,r3      lsl r7,#8
 *     lsl r7,#8                        --
 *     (inside)  mov r3,r10             (inside)  ldr r3,=0x3333
 *
 * Three instructions against two, plus one copy inside -- EXACTLY the +2 / +4 bytes.
 *
 * MEASURED, and every spelling still gets it hoisted:
 *   `v = 0x3333` named outside + literal inside      80 real, hoisted to r10
 *   both bare literals, either store order           86 / 82, both hoisted
 *   `v = 0x80<<8` named outside, 5 statement orders  85-86, still hoisted
 *   one variable assigned TWICE inside the loop      84 real but 1284 bytes (-4) --
 *       kills the movable (n_times_set != 1) so NEITHER constant is hoisted, where
 *       the ROM hoists one
 *
 * READ OUT OF loop.c: move_movables hoists when
 * `threshold * savings * m->lifetime >= insn_count` (line 1803), with
 * `threshold = (loop_info->has_call ? 1 : 2) * (1 + n_non_fixed_regs)` (line 651) and
 * `threshold -= 3` after each move (1900, 2104).  With calls in the loop that is
 * 1 + n_non_fixed_regs, about 14, against an insn_count of ~16 -- so EVEN THE SECOND
 * MOVABLE CLEARS IT at lifetime >= 2.
 *
 * THE COST MODEL IS NOT REACHABLE FROM THE SOURCE AT THIS LOOP SIZE, which is why
 * the sweep is flat.  So either the ROM's source makes the 0x34 store value
 * NON-INVARIANT in a way a plain constant cannot be, or its loop is not the loop
 * written here.  That is the open question, and it is the only COUNT defect -- the
 * other ~76 differences are r1-vs-r2 scratch choices downstream of it and of the
 * `mov r2,#0xe0` offset-register rotation.
 *
 * ================================================================
 * A FOURTH ANGLE ON THE MEASUREMENT RULE, AND IT IS THE FIRST WITH *AGREEING* COUNTS
 * ================================================================
 *
 * Batch 281's three angles were a moved pool, a jump table, and disagreeing counts.
 * THIS ONE HAS AGREEING SIZE AND AGREEING ENCODING COUNT AND NO JUMP TABLE, and the
 * positional count still mis-ranks.  Three candidates with identical size and
 * identical count scored 238 / 393 / 503 positionally while their alignment-robust
 * differences were 87 / 106 / 89.  Picking by the objcmp number would have chosen
 * the 89 over the 87 AND ranked the 106 as better than the 89.
 *
 * WHERE SIZE AND COUNT ALREADY AGREE, THE POSITIONAL COUNT IS NO LONGER ORDERING
 * INFORMATION; a shift-tolerant diff is.  scratch_elev/b282/C/sdiff2.py is one --
 * a difflib opcode diff over normalised instruction streams with pool references
 * resolved to their words.
 *
 * `_FILE_e5 = 0xe5;` is the same tell this function's landed siblings carry (see the
 * file_table.sym entry for _FILE_e6, added in batch 282).  It does not complete this
 * function, so it is reported and withheld.
 *
 * No per-file Makefile rule matches this stem.
 *
 * NEXT: the loop-invariant question above.  Do not sweep spellings -- the cost-model
 * arithmetic says they are all on the wrong side of the threshold.
 */
__asm__(".equ _FILE_e5, 0xe5");
extern int _FILE_e5;

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];

extern void OvlFunc_955_200862c(void);
extern void OvlFunc_955_2008714(void);
extern void OvlFunc_955_2008b38(int a);
extern void OvlFunc_955_20090dc(int a);
extern void OvlFunc_common1_0(void);
extern void OvlFunc_common1_78(int a);
extern void OvlFunc_common1_148(void);
extern void OvlFunc_common1_488(void);
extern void OvlFunc_common1_ea0(int a);
extern void OvlFunc_common1_1608(int a, int b);
extern int OvlFunc_common1_1ecc(int a, int b, int c, int d, int e, int f, int g);
extern void OvlFunc_common1_1fb4(int a);

extern void __SetFlag(int id);
extern int __GetFlag(int id);
extern int __GetFlagByte(int id);
extern void __PlaySound(int id);
extern void __DeleteFieldActor(int slot);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetExtra(int slot, int v);
extern void __Actor_SetAnim(unsigned char *e, int anim);
extern void __StartTask(void (*f)(void), int n);
extern void __Func_8004358(void (*f)(void), int a);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8010788(int a, int b, int c, int d, int e, int f);
extern void __Func_8091e9c(int a);
extern void __Func_8092950(int a, int b);
extern void __Func_809ad90(int a);

int OvlFunc_955_2008bd4(void)
{
    unsigned char *p;
    unsigned char *g;
    int i, k, n;
    int two, zero;
    int two2, zero2, ten;
    int two3, v, thirteen;

    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0;
    __SetFlag(0xa2 << 1);
    { int s1a = 0x64; int s1b = 0xb;
      __Func_8010704(0xe, 0xb, 0xc, 4, s1a, s1b); }
    { int s2a = 0x78; int s2b = 0xa;
      __Func_8010704(0x30, 0xa, 5, 6, s2a, s2b); }
    i = 0x1a;
    zero = 0;
    two = 2;
    for (; i <= 0x1e; i++) {
        p = __MapActor_GetActor(i);
        __Actor_SetAnim(p, 4);
        p[0x55] = zero;
        *(int *)(p + 0xc) = zero;
        p[0x23] = two;
    }
    __MapActor_GetActor(0x12)[0x23] = 2;
    if (__GetFlag(0xcc << 2)) {
        p = __MapActor_GetActor(0x1e);
        *(int *)(p + 8) = 0xa8 << 17;
        *(int *)(p + 0xc) = 0xfff80000;
        *(int *)(p + 0x10) = 0x84 << 17;
        { int s3a = 0x14; int s3b = 0x10;
          __Func_8010704(0x13, 0x10, 1, 1, s3a, s3b); }
        { int s4a = 0x15; int s4b = 0x50;
          __Func_8010704(0x14, 0x50, 1, 1, s4a, s4b); }
    } else {
        p = __MapActor_GetActor(0x1e);
        __Actor_SetAnim(p, 3);
        *(int *)(p + 0xc) = 0x80 << 13;
    }
    two2 = 2;
    __MapActor_GetActor(0xb)[0x23] = two2;
    zero2 = 0;
    if (__GetFlag(0x335)) {
        { int s5a = 0x23; int s5b = 0x4d;
          __Func_8010704(0x23, 0x4e, 1, 1, s5a, s5b); }
    }
    if (__GetFlag(0x333)) {
        __MapActor_SetAnim(0x13, 4);
        { int s6a = 0x20; int s6b = 0x4d;
          __Func_8010704(0x20, 0x25, 1, 4, s6a, s6b); }
    }
    if (__GetFlag(0x331)) {
        __MapActor_GetActor(0x14)[0x55] = zero2;
        __MapActor_GetActor(0x14)[0x23] = two2;
        __MapActor_SetAnim(0x14, 5);
        { int s7a = 0x2c; int s7b = 0x11;
          __Func_8010704(0x2e, 0x11, 1, 1, s7a, s7b); }
    }
    if (__GetFlag(0x332)) {
        __MapActor_GetActor(0x15)[0x55] = zero2;
        __MapActor_GetActor(0x15)[0x23] = two2;
        __MapActor_SetAnim(0x15, 5);
        { int s8a = 0x32; int s8b = 0x11;
          __Func_8010704(0x2e, 0x11, 1, 1, s8a, s8b); }
    }
    p = __MapActor_GetActor(0x20);
    p[0x55] = zero2;
    p[0x23] = two2;
    { int s9a = *(int *)(p + 8) >> 20; int s9b = 0xa;
      __Func_8010704(0x34, 0x1c, 1, 3, s9a, s9b); }
    p = __MapActor_GetActor(0x21);
    p[0x55] = zero2;
    p[0x23] = two2;
    { int sAa = *(int *)(p + 8) >> 20; int sAb = 0xd;
      __Func_8010704(0x34, 0x1c, 1, 3, sAa, sAb); }
    n = __GetFlagByte(0xd0 << 2);
    if (n == 0)
        n = 0x49;
    p = __MapActor_GetActor(0xc);
    *(int *)(p + 8) = (n << 20) + (0x80 << 12);
    p[0x55] = zero2;
    p[0x23] = two2;
    ten = 0x10;
    __Func_8010704(0x47, 0x10, 1, 1, n, ten);
    n = __GetFlagByte(0xd2 << 2);
    if (n == 0)
        n = 0x4c;
    p = __MapActor_GetActor(0xd);
    *(int *)(p + 8) = (n << 20) + (0x80 << 12);
    p[0x55] = zero2;
    p[0x23] = two2;
    __Func_8010704(0x47, 0x10, 1, 1, n, ten);
    n = __GetFlagByte(0xd4 << 2);
    if (n == 0)
        n = 0x4f;
    p = __MapActor_GetActor(0xe);
    *(int *)(p + 8) = (n << 20) + (0x80 << 12);
    p[0x55] = zero2;
    p[0x23] = two2;
    __Func_8010704(0x47, 0x10, 1, 1, n, ten);
    OvlFunc_955_200862c();
    __MapActor_SetAnim(0x1f, 0xa);
    if (__GetFlag(0xcd << 2)) {
        thirteen = 0xd;
        i = 0x16;
        k = 0x3a;
        for (; i <= 0x19; i++) {
            p = __MapActor_GetActor(i);
            p[0x23] = 2;
            __Actor_SetAnim(p, 4);
            __Func_8010704(0x38, 0xd, 1, 1, k, thirteen);
            k += 2;
        }
        __MapActor_SetAnim(0x1f, 0xa);
        __Func_809ad90(0x1f);
    } else {
        v = 0x3333;
        i = 0x16;
        two3 = 2;
        for (; i <= 0x19; i++) {
            p = __MapActor_GetActor(i);
            p[0x23] = two3;
            __Actor_SetAnim(p, 4);
            *(int *)(p + 0x30) = 0x80 << 8;
            *(int *)(p + 0x34) = v;
        }
        __StartTask(OvlFunc_955_2008714, 0xc85);
        __Func_8004358(OvlFunc_955_2008714, 1);
    }
    __MapActor_SetAnim(8, 9);
    g = gState;
    g[0xf9 << 1] = 0;
    { PIN2; q1 = 0x59; q0 = 0x29; OvlFunc_common1_1608(q0, q1); }
    { PIN2; q1 = 0x4d; q0 = 0x28; OvlFunc_common1_1608(q0, q1); }
    { PIN2; q1 = 1; q0 = 8; __Func_8092950(q0, q1); }
    switch (*(short *)(g + (0xe1 << 1))) {
    case 1:
        { PIN2; int w; w = 0xd0 << 15; q1 = 8; q0 = 0;
          OvlFunc_common1_1ecc(q0, q1, 5, w, 0x80 << 17, 0x28, 0x29); }
        { int u1 = 0x4f; int u2 = 6; __Func_8010788(0x7f, 0, 1, 2, u1, u2); }
        __DeleteFieldActor(0x22);
        __DeleteFieldActor(0x23);
        __DeleteFieldActor(0x24);
        __DeleteFieldActor(0x25);
        __DeleteFieldActor(0x26);
        __DeleteFieldActor(0x27);
        if (!__GetFlag(0x109)) {
            __PlaySound(0x11);
            OvlFunc_common1_78(0);
            OvlFunc_common1_0();
            OvlFunc_common1_ea0(2);
        }
        __MapActor_SetExtra(1, 0);
        __MapActor_SetExtra(2, 0);
        __MapActor_SetExtra(3, 0);
        OvlFunc_common1_1fb4((int)&_FILE_e5);
        break;
    case 2:
        __StartTask(OvlFunc_common1_148, 0xc8 << 4);
        __DeleteFieldActor(0x28);
        __DeleteFieldActor(0x29);
        if (!__GetFlag(0x109)) {
            OvlFunc_common1_0();
            OvlFunc_common1_78(1);
            OvlFunc_common1_ea0(0);
        }
        break;
    case 3:
        if (!__GetFlag(0x109)) {
            OvlFunc_955_20090dc(0x22);
            OvlFunc_common1_488();
        }
        break;
    case 4:
        OvlFunc_955_2008b38(2);
        __Func_8091e9c(4);
        break;
    case 5:
        OvlFunc_955_2008b38(-2);
        __Func_8091e9c(5);
        break;
    }
    return 0;
}
