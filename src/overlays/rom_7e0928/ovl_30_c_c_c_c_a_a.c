/* WHOLE-FILE CONVERSION of asm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_a.s --
 * OvlFunc_956_2008da4 is its only function, no data section, so no split and no
 * linker change.  599 instructions, 1496 bytes, 623 encodings and 116 relocations
 * identical.
 *
 * NEEDS `_FILE_e6 = 0xe6;`, WHICH THIS COMMIT ADDS to file_table.sym and
 * include/file_table.h.  It COMPLETES this function: with the symbol, byte-exact;
 * with the literal, 6 differing encodings and 4 bytes short.  Evidence is in the
 * file_table.sym entry -- eleven small constants in this same function reproduce as
 * plain literals and five larger ones as mov+lsl, so it synthesises everything it
 * can and pools exactly this one value.  No shim here: a verification shim belongs
 * in the scratch candidate only.
 *
 * ================================================================
 * AN `int` RETURN TYPE ON A `void` CALLEE IS AN "r0 LAST" LEVER, AND IT IS NOT THE
 * SAME THING AS DROPPING THE PROTOTYPE
 * ================================================================
 *
 * The last 2 of 623 were `mov r2,#6` / `mov r0,#0` swapped at OvlFunc_common1_1ecc.
 * Measured at that one site: full prototype plus pins 2; prototype dropped to `()`
 * 3, and with pins 2; `extern int` instead of `extern void` -> 0.
 *
 * FOURTEEN OTHER SPELLINGS AT THAT SITE ALL FLOORED AT 2 -- five pin masks, three
 * barrier placements, r2-only pins, named carriers for arguments 3 and 4.  So the
 * return type is a DISTINCT handle from the no-prototype lever already on file, and
 * it is the one that reaches this shape.  The file-neighbour
 * src/overlays/rom_7b4558/ovl_30_c_c_c_a_c.c repeats it twice, worth 7 of 661 when
 * reverted.
 *
 * "NAME THE STACK ARGUMENTS" IS A CLASS RESULT, NOW MEASURED AT SCALE.  This
 * function has SIXTEEN six-argument __Func_8010704 calls.  The ROM materialises both
 * stack arguments into two registers and stores both; a bare literal pair reuses one
 * register and stores between the movs.  Naming them took 351 -> 134 of 623 AT THE
 * SAME INSTRUCTION COUNT.  Reverting all sixteen jointly costs 40; individually TEN
 * of sixteen are load-bearing (2-6 each) and six are inert -- and the greedy ladder
 * accepted all six sequentially, so the inert ones are not free in combination.
 *
 * LOOP-SETUP STATEMENT ORDER IS WORTH 8-18 ENCODINGS AND HAS TO BE SWEPT.  The two
 * five-actor loops hoist three invariants each (0, 2, and a stack argument) into
 * r8/r9/r10, and sched2 interleaves the induction-variable and accumulator
 * initialisations BETWEEN the high-register moves.  A 20-way sweep over the first
 * loop ranged 15-19 and picked `zero, i, two, k, six`; a 6-way sweep on the second
 * picked `i, zero2, two2` and took 15 -> 4.  Not deducible -- sweep it.
 *
 * `x >>= 20;` AS ITS OWN STATEMENT rather than `x >> 20` at the use gives the ROM's
 * two-operand in-place `asr r5, #0x14` against gcc's three-operand form.
 *
 * TWO `0x100`-SHAPED STORES IN TWO SIBLING FUNCTIONS WANT OPPOSITE SPELLINGS.  Here
 * the ROM re-uses the 0x1c0 offset register (`sub r2,#0xc0`) and a bare `= 0x100`
 * reproduces it; in src/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_a_c_a.c the value
 * must survive to a later call so the ROM builds it fresh and a named local is
 * actively harmful (142 -> 327).  BARE LITERAL WHEN THE VALUE DIES AT THE STORE.
 *
 * No per-file Makefile rule matches this stem.
 */
extern int _FILE_e6;

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];
extern unsigned char ActorCmd_ARRAY_956__0200cbec[];

extern void OvlFunc_956_2008658(void);
extern void OvlFunc_956_200804c(void);
extern void OvlFunc_common1_148(void);
extern void OvlFunc_common1_0(void);
extern void OvlFunc_common1_78(int a);
extern void OvlFunc_common1_488(void);
extern void OvlFunc_common1_ea0(int a);
extern void OvlFunc_common1_1608(int a, int b);
extern int OvlFunc_common1_1ecc(int a, int b, int c, int d, int e, int f, int g);
extern void OvlFunc_common1_1fb4(int a);
extern void OvlFunc_956_2009474(int a);
extern void OvlFunc_956_2009a0c(int a);

extern void __SetFlag(int id);
extern int __GetFlag(int id);
extern int __GetFlagByte(int id);
extern void __PlaySound(int id);
extern void __DeleteFieldActor(int slot);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetExtra(int slot, int v);
extern void __Actor_SetSpriteFlags(unsigned char *e, int f);
extern void __Actor_SetAnim(unsigned char *e, int anim);
extern void __Actor_SetScript(unsigned char *e, unsigned char *s);
extern void __StartTask(void (*f)(void), int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8010788(int a, int b, int c, int d, int e, int f);
extern int __Func_8011f54(int a, int b, int c);
extern void __Func_80118c0(int a);
extern void __Func_8091e9c(int a);
extern void __Func_8092950(int a, int b);
extern void __Func_809ad90(int a);

int OvlFunc_956_2008da4(void)
{
    unsigned char *p;
    unsigned char *g;
    int i, k, n, r;
    int two, zero, six;
    int two2, zero2;

    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x100;
    __SetFlag(0xa2 << 1);
    { register int q1 __asm__("r1"); q1 = 0xc8; q1 <<= 4;
      __StartTask(OvlFunc_956_2008658, q1); }
    { int s1a = 0x78; int s1b = 0x3c;
      __Func_8010704(0x4a, 0x3c, 8, 6, s1a, s1b); }
    p = __MapActor_GetActor(9);
    __Actor_SetSpriteFlags(p, 0);
    p = __MapActor_GetActor(0xa);
    __Actor_SetSpriteFlags(p, 0);
    p[0x55] = 0;
    *(int *)(p + 0xc) = 0x80 << 14;
    p = __MapActor_GetActor(0xb);
    __Actor_SetSpriteFlags(p, 0);
    p[0x55] = 0;
    *(int *)(p + 0xc) = 0x80 << 11;
    if (__GetFlag(0x362)) {
        __MapActor_SetAnim(9, 5);
        p = __MapActor_GetActor(0xa);
        *(int *)(p + 0xc) = 0x80 << 11;
        p = __MapActor_GetActor(0xb);
        *(int *)(p + 0xc) = 0x80 << 14;
        { int s2a = 0xd; int s2b = 0xc;
          __Func_8010704(0xf, 0xc, 1, 1, s2a, s2b); }
    } else {
        p = __MapActor_GetActor(9);
        *(int *)(p + 0x18) = 0xc0 << 9;
        *(int *)(p + 0x1c) = 0xc0 << 9;
        if (__GetFlag(0x367))
            { int s3a = 9; int s3b = 0xc;
              __Func_8010704(0, 0x18, 1, 1, s3a, s3b); }
        else
            { int s4a = 9; int s4b = 0xc;
              __Func_8010704(0, 0x19, 1, 1, s4a, s4b); }
    }
    if (__GetFlag(0xda << 2)) {
        __Func_8010704(0xf, 0xc, 1, 1, 0xd, 0xc);
        __Func_8010704(1, 0x19, 1, 1, 9, 0xc);
        p = __MapActor_GetActor(0xc);
        __Actor_SetSpriteFlags(p, 0);
        p[0x55] = 0;
        *(int *)(p + 0xc) = 0x80 << 10;
        p[0x23] = 2;
        p = __MapActor_GetActor(0xa);
        *(int *)(p + 0xc) = 0x80 << 11;
        p[0x23] = 2;
        p = __MapActor_GetActor(0xb);
        *(int *)(p + 0xc) = 0x80 << 14;
    }
    n = __GetFlagByte(0xdc << 2);
    if (n == 0)
        n = 0x13;
    p = __MapActor_GetActor(0xd);
    *(int *)(p + 8) = (n << 20) + (0x80 << 12);
    p[0x55] = 0;
    p[0x23] = 2;
    { int s7a = 0x12; int s7b = 0xb;
      __Func_8010704(0x12, 0xa, 3, 1, s7a, s7b); }
    __Func_8010704(0x11, 0xb, 1, 1, n, 0xb);
    for (i = 0xf; i <= 0x11; i++) {
        p = __MapActor_GetActor(i);
        r = __Func_8011f54(0, *(int *)(p + 8), *(int *)(p + 0x10));
        if (*(int *)(p + 0xc) == 0 && r == 0) {
            p[0x23] = 2;
            p[0x55] = r;
            { int s9a = *(int *)(p + 8) >> 20; int s9b = *(int *)(p + 0x10) >> 20;
              __Func_8010704(0x53, 0xd, 1, 1, s9a, s9b); }
            { int s10a = *(int *)(p + 8) >> 20; int s10b = (*(int *)(p + 0x10) >> 20) + 0x34;
              __Func_8010704(0x53, 0xd, 1, 1, s10a, s10b); }
        }
    }
    if (__GetFlag(0x361)) {
        zero = 0;
        i = 0x12;
        two = 2;
        k = 0x21;
        six = 0xb;
        for (; i <= 0x16; i++) {
            p = __MapActor_GetActor(i);
            p[0x23] = two;
            __Actor_SetAnim(p, 2);
            p = __MapActor_GetActor(i + 5);
            p[0x23] = two;
            p[0x55] = zero;
            *(int *)(p + 0xc) = 0x80 << 14;
            __Actor_SetAnim(p, 0xa);
            __Func_8010704(0x4a, 0xc, 1, 1, k, six);
            k += 2;
        }
        __MapActor_SetAnim(0x1c, 0xa);
        __Func_809ad90(0x1c);
    } else {
        i = 0x12;
        zero2 = 0;
        two2 = 2;
        for (; i <= 0x16; i++) {
            p = __MapActor_GetActor(i);
            p[0x23] = two2;
            p = __MapActor_GetActor(i + 5);
            p[0x23] = two2;
            p[0x55] = zero2;
            *(int *)(p + 0xc) = 0x80 << 14;
        }
        __StartTask(OvlFunc_956_200804c, 0xc8 << 4);
    }
    if (__GetFlag(0xd8 << 2)) {
        __MapActor_SetAnim(0x1d, 4);
        { int s12a = 0x31; int s12b = 0x3d;
          __Func_8010704(0x2f, 0x3d, 1, 4, s12a, s12b); }
    }
    if (__GetFlag(0x363)) {
        __Func_80118c0(1);
        p = __MapActor_GetActor(0x1e);
        p[0x55] = 0;
        *(int *)(p + 8) = 0x46a0000;
        *(int *)(p + 0x10) = 0xb8 << 16;
        __Actor_SetSpriteFlags(p, 0);
        __Actor_SetAnim(p, 3);
        __Actor_SetScript(p, ActorCmd_ARRAY_956__0200cbec);
    } else {
        __Func_80118c0(2);
    }
    if (__GetFlag(0x369)) {
        p = __MapActor_GetActor(0x1f);
        __Actor_SetAnim(p, 8);
        p[0x23] = 2;
        __Func_8010704(0x56, 0xa, 1, 2, 0x54, 0xa);
        __Func_8010704(0x56, 9, 1, 1, 0x54, 0xc);
    } else {
        p = __MapActor_GetActor(0x1f);
        { int s15a = *(int *)(p + 8) >> 20; int s15b = 9;
          __Func_8010704(0x55, 9, 1, 4, s15a, s15b); }
        { int s16a = *(int *)(p + 8) >> 20; int s16b = 0x3d;
          __Func_8010704(0x55, 9, 1, 4, s16a, s16b); }
    }
    p = __MapActor_GetActor(9);
    p[0x55] = 0;
    p[0x23] = 2;
    p = __MapActor_GetActor(0xa);
    p[0x55] = 0;
    p[0x23] = 2;
    p = __MapActor_GetActor(0xb);
    p[0x55] = 0;
    p[0x23] = 2;
    __MapActor_SetAnim(8, 9);
    g = gState;
    g[0xf9 << 1] = 0;
    { PIN2; q1 = 3; q0 = 0x27; OvlFunc_common1_1608(q0, q1); }
    { PIN2; q1 = 0x11; q0 = 0x28; OvlFunc_common1_1608(q0, q1); }
    __Func_8092950(8, 2);
    switch (*(short *)(g + (0xe1 << 1))) {
    case 1:
        { PIN2; int w; w = 0xbd << 19; q1 = 8; q0 = 0;
          OvlFunc_common1_1ecc(q0, q1, 6, w, 0xc0 << 16, 0x27, 0x28); }
        { int u1 = 5; int u2 = 2; __Func_8010788(0x7f, 0, 1, 2, u1, u2); }
        __DeleteFieldActor(0x20);
        __DeleteFieldActor(0x21);
        __DeleteFieldActor(0x22);
        __DeleteFieldActor(0x23);
        __DeleteFieldActor(0x24);
        __DeleteFieldActor(0x25);
        __DeleteFieldActor(0x26);
        if (!__GetFlag(0x109)) {
            __PlaySound(0x11);
            OvlFunc_common1_78(0);
            OvlFunc_common1_0();
            __MapActor_SetExtra(1, 0);
            OvlFunc_common1_ea0(3);
        }
        __MapActor_SetExtra(1, 0);
        __MapActor_SetExtra(2, 0);
        __MapActor_SetExtra(3, 0);
        OvlFunc_common1_1fb4((int)&_FILE_e6);
        break;
    case 2:
        __StartTask(OvlFunc_common1_148, 0xc8 << 4);
        __DeleteFieldActor(0x27);
        __DeleteFieldActor(0x28);
        if (!__GetFlag(0x109)) {
            OvlFunc_common1_0();
            OvlFunc_common1_78(1);
            OvlFunc_common1_ea0(0);
        }
        break;
    case 3:
        if (!__GetFlag(0x109)) {
            OvlFunc_956_2009a0c(0x20);
            OvlFunc_common1_488();
        }
        break;
    case 4:
        OvlFunc_956_2009474(1);
        __Func_8091e9c(4);
        __SetFlag(0x95 << 4);
        __SetFlag(0x951);
        break;
    case 5:
        OvlFunc_956_2009474(-1);
        __Func_8091e9c(5);
        __SetFlag(0x95 << 4);
        break;
    }
    return 0;
}
