// fakematch
/* ovl_30_c_c_a_c_c_a_c_c.c  --  OvlFunc_944_2008564 AND OvlFunc_944_20087b0,
 *   the WHOLE of asm/overlays/rom_7ca63c/ovl_30_c_c_a_c_c_a_c_c.s.
 *
 *   OK WHOLE OBJECT -- 1312 bytes, 508 encodings and 135 relocations identical.
 *   Measured six times (three on the merged TU after the final cleanup, three
 *   on the two functions separately before the merge).
 *
 * NO SPLIT IS NEEDED.  `tools/asmfacts.py` says `2 functions  split first` and
 * `tools/split_asm.py` adds a BASENAME WARNING -- both are the verdict for
 * solving only the FIRST function.  BOTH functions are closed here, so the .s
 * is replaced outright and the basename stays.  overlay.ld carries exactly ONE
 * line for this object, rom_7ca63c/overlay.ld:29
 * `asm/overlays/rom_7ca63c/ovl_30_c_c_a_c_c_a_c_c.o(.text)`, and it stays
 * VERBATIM in the `asm/...o(SECTION)` form -- there is no `.data` line.
 *
 * objcmp prints no `(built with: ...)` line: `makefile_flags()` is the EMPTY
 * set, no rule or wildcard names this stem, so `asm/%.o: src/%.c` fires at
 * plain -O2.  NO MAKEFILE CHANGE (see the -ffixed-r7 entry below, which is the
 * main result of this batch).  FAKEMATCH: register-pin idiom, 28 pins, all on
 * r0-r3, so the recorded callee-saved pin-miscompile hazard does not apply.
 *
 * THREE CROSS-FILE `.L` SYMBOLS, ALL ALREADY EXPORTED.  `.L16f4`, `.L1930` and
 * `.L1938` are defined and `.global`-ised in
 * asm/overlays/rom_7ca63c/ovl_30_c_c_c_b.s (lines 4, 72-82); this file reaches
 * them with the `extern int L1930[] __asm__(".L1930");` idiom already used by
 * the landed src/overlays/rom_7ca63c/ovl_30_a_a_a.c.  There are NO `.L` branch
 * targets in this .s -- both functions are straight line.
 *
 * Two cutscenes: 207 and 276 instructions, 73 + 38 calls, message bases 0x1e3e
 * and 0x1e49, both setting save bit 0x8f0.
 *
 * ===================================================================
 * `-ffixed-r7` WAS A FALSE LEAD THAT MEASURED LIKE THE ANSWER.  A FOURTH ARM
 * FOR THAT DISCRIMINATOR.
 *
 * OvlFunc_944_20087b0's first differing encoding was the PUSH MASK:
 * `push {r5, r6, r7, lr}` against the ROM's `push {r5, r6, lr}`, with the ROM
 * spending r5, r6, r8 and r10 and SKIPPING r7 -- which is exactly the recorded
 * `-ffixed-r7` shape, and the flag is worth 268 -> 45 with the size becoming
 * exact.  It is still the WRONG LEVER.  The extra register was a SOURCE-LEVEL
 * live range that was too long: `__MapActor_GetActor` is called twice and both
 * results were written to one local, so the pseudo spanned the whole function.
 * Giving the first call its own local (`p`) is 45 -> 6 AND drops r7 from the
 * mask with NO FLAG.  At the finished source `-ffixed-r7` is INERT (still 0).
 *
 *   The recorded discriminator reads "same count, rotated -> the flag; one more
 *   -> a live range".  This is the third false positive on it and the first
 *   where the ROM's register set really does exclude r7: gcc spending ONE MORE
 *   callee-saved register than the ROM is a live-range symptom FIRST, and the
 *   flag will mask it at a number good enough to ship a Makefile rule on.
 *   Re-measure the flag after every structural change, not before.
 * ===================================================================
 *
 * A NAMED OFFSET LOCAL FED use_related_value AND COST AN INSTRUCTION 500 BYTES
 * AWAY.  OvlFunc_944_2008564 stores 0x203 and later 0x202 at a 0x1c0 offset the
 * ROM keeps in r10.  Written as `off = 0xe0 << 1; *(int *)(*q + off) = 0x203;`
 * gcc builds the VALUE from the OFFSET (`adds r3, #0x43`) where the ROM pools
 * it, and separately spends `mov sl, r3 / mov r1, sl` placing `off` in r10
 * before its first use where the ROM copies to r10 AFTER the store -- one
 * instruction short overall.  Writing `(0xe0 << 1)` out again at BOTH stores
 * and dropping the local fixes the length, the pool word AND the r2/r3 role
 * swap at the store: 192 (one short) -> 10 -> 5.  This is the recorded
 * "a named local is not a weaker version of the lever, it IS the defect",
 * reached from the constant-placement side rather than the CSE side.
 *
 * THE BYTE STORE'S ZERO, BORN AT THE TOP.  `b = __MapActor_GetActor(9);
 * b[0x55] = 0; L1930[1] = 0;` gives `movs r2, #0` -- a caller-saved register --
 * where the ROM has `movs r6, #0`.  `int z; z = 0;` as the FIRST statement of
 * the function, before the first call, is worth 5 -> 0.  Third instance of the
 * recorded rule ("a pushed callee-saved register means the pseudo is born
 * before the first call"), and the placement that works is the top, not the
 * statement before the call (that spelling measures 14).
 *
 * THE ROM DERIVES iwram_3001ebc FROM iwram_3001e70 AND THE SOURCE IS AN ARRAY
 * WALK.  `ldr r1, =iwram_3001e70 / mov r8, r1 / ... / mov r2, #0x4c /
 * add r8, r2` is one pool word, not two: 0x3001e70 + 0x4c = 0x3001ebc.
 * `extern char *iwram_3001e70[]; q = iwram_3001e70; ... q += 0x13;` reproduces
 * it, including the `mov r2, #0x4c / add r8, r2` pair, which is only how Thumb
 * adds a constant to a HIGH register -- not a tell about the source.
 *
 * TWO FILL ORDERS AT THE SAME SITE SHAPE, IN OPPOSITE DIRECTIONS.  The last six
 * encodings of OvlFunc_944_20087b0 were three transpositions of two `movs`,
 * at sites whose SHIFTS were already in the ROM's order.  sched2 reorders the
 * `movs` to agree with the shift order, so the source shift order that emits
 * the ROM is NOT the ROM's: `__MapActor_SetSpeed(9, 0x80 << 10, 0x80 << 9)`
 * wants `q1 <<= 10; q2 <<= 9` at the site whose ROM order is `lsl r2 / lsl r1`
 * -- and the SAME call with the SAME constants 100 instructions later wants the
 * same source order for the ROM's `lsl r1 / lsl r2`.  One source spelling, two
 * emissions, decided by what follows the call.  All 30 legal permutations were
 * swept at each site: 4 or 6 at the first, 2, 4 or 6 at the `__Actor_TravelTo`
 * pair; nothing under the best exists to find, so the sweep is complete.
 *
 * `-fno-schedule-insns2` REGRESSES: 6 -> 79 on the 20087b0 base and 0 -> 161 on
 * the finished merged TU.  sched2 is already producing the ROM's order and
 * ALIAS IS THE WRONG AXIS -- consistent with every other function in this
 * family.  `-fno-strict-aliasing` is INERT (still 0).  Neither is shipped.
 *
 * WHAT CLOSED IT
 *
 *   OvlFunc_944_20087b0 (283 encodings, 724 bytes)     differing
 *   ------------------------------------------------  ---------
 *   plain C, no pins                                        265  (-4 bytes)
 *   every pinnable site pinned                              268  (-8 bytes)
 *   ... + -ffixed-r7                                         45  (exact size)
 *   ... + the first GetActor given its own local              6
 *       (and -ffixed-r7 now INERT, dropped)
 *   ... + the SetSpeed shift order at ONE of its two sites     4
 *   ... + the two __Actor_TravelTo(.., 0, 0xa9<<16) orders     0
 *
 *   OvlFunc_944_2008564 (225 encodings, 588 bytes)     differing
 *   ------------------------------------------------  ---------
 *   every pinnable site pinned                              192  (-4 bytes)
 *   ... + the stored 0x203 named before the offset          180  (exact size)
 *   ... + the offset local dropped, expression repeated       10
 *   ... + the stored 0x203 written as a literal                5
 *   ... + the byte store's zero born at the top                0
 *
 * MEASURED WORSE / INERT (against the merged 508 encodings / 1312 bytes)
 *
 *   spelling                                            result
 *   -------------------------------------------------  ----------------
 *   all 87 pins dropped as one set                      452 (+4 bytes)
 *   -fno-schedule-insns2                                161
 *   -ffixed-r7                                          0 (INERT)
 *   -fno-strict-aliasing                                0 (INERT)
 *   the zero assigned just before its GetActor           14
 *   an eviction pin on the stored 0x203                 192 (INERT)
 *   __asm__ volatile ("" : "+r" (off)) on the offset    197 (WORSE)
 *   no pin at the ActorCmd __MapActor_SetBehavior        10 (INERT)
 *   INERT: 59 of the 87 pins, removed by a per-site
 *     fixpoint and then re-checked AS A SET; 28 kept.
 */
extern char *iwram_3001e70[];
extern unsigned char gScript_944__0200939c[];
extern unsigned char ActorCmd_ARRAY_944__02009314[];
extern unsigned char L16f4[] __asm__(".L16f4");
extern int L1930[] __asm__(".L1930");
extern int L1938[] __asm__(".L1938");
extern void OvlFunc_944_20080a4(void);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MessageID(int id);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __LoadFieldActors(unsigned char *p);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __Func_8091e9c(int a);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092950(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern char *iwram_3001ebc;
extern unsigned char gOvl_0200976c[];
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void __Actor_WaitMovement(unsigned char *a);
extern void OvlFunc_944_2008a84(int slot);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_944_2008564(void)
{
    char **q;
    int *p;
    unsigned char *a;
    unsigned char *b;
    int v;
    int w;
    int z;
    void (*f)(void);

    z = 0;
    q = iwram_3001e70;
    p = *(int **)q[0];
    __CutsceneStart();
    __LoadFieldActors(L16f4);
    __WaitFrames(1);
    __Func_8092950(0, 0xf);
    a = __MapActor_GetActor(0);
    __Actor_SetSpriteFlags(a, 0);
    __MapActor_SetBehavior(8, gScript_944__0200939c);
    q += 0x13;
    *(int *)(*q + (0xe0 << 1)) = 0x203;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    L1938[0] = *p++;
    L1938[1] = *p;
    { PIN3; q1 = 0xa0; q2 = 0xd2; q2 <<= 16; q1 <<= 15; q0 = 9; __MapActor_SetPos(q0, q1, q2); }
    b = __MapActor_GetActor(9);
    v = 0xa0 << 15;
    b[0x55] = z;
    L1930[0] = v;
    L1930[1] = z;
    __MapActor_SetBehavior(9, (unsigned char *)ActorCmd_ARRAY_944__02009314);
    __CutsceneWait(0x14);
    __PlaySound(0x1d);
    __SetFlag(0x8f << 4);
    __MapActor_SetIdle(8);
    __WaitFrames(1);
    { PIN3; q1 = 0x80; q0 = 8; q1 <<= 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __Func_8092adc(8, 0xb0 << 8, 0);
    __MessageID(0x1e3e);
    __Func_8093040(8, 0, 0xa);
    { PIN3; q2 = 0xd2; q1 = v; q0 = 0xa; q2 <<= 16; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q2 = 0xd2; q1 = v; q0 = 0xb; q2 <<= 16; __MapActor_SetPos(q0, q1, q2); }
    __MapActor_SetPos(0xc, v, 0xd2 << 16);
    __Func_8092b08(0xa, 3);
    __Func_8092b08(0xb, 3);
    __Func_8092b08(0xc, 3);
    __Func_8092950(0xa, 3);
    __Func_8092950(0xb, 3);
    __Func_8092950(0xc, 3);
    a = __MapActor_GetActor(0xa);
    w = 0x80 << 8;
    f = OvlFunc_944_20080a4;
    *(int *)(a + 0x1c) = w;
    *(int *)(a + 0x18) = w;
    *(void (**)(void))(a + 0x6c) = f;
    a = __MapActor_GetActor(0xb);
    *(int *)(a + 0x1c) = w;
    *(int *)(a + 0x18) = w;
    *(void (**)(void))(a + 0x6c) = f;
    a = __MapActor_GetActor(0xc);
    *(int *)(a + 0x1c) = w;
    *(int *)(a + 0x18) = w;
    *(void (**)(void))(a + 0x6c) = f;
    __WaitFrames(1);
    { PIN3; q0 = 0xa; q1 = 0x851e; q2 = 0x428f; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x7333; q2 = 0x3999; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0x9999; q2 = 0x4ccc; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x80; q2 = 0x159; __MapActor_TravelTo(q0, q1, q2); }
    { PIN3; q2 = 0xa5; q0 = 0xb; q1 = 0x88; q2 <<= 1; __MapActor_TravelTo(q0, q1, q2); }
    __MapActor_TravelTo(0xc, 0x9c, 0xaa << 1);
    __CutsceneWait(0x3c);
    __Func_80925cc(8, 2);
    { PIN3; q2 = 0xac; q0 = 8; q1 = 0xa4; q2 <<= 1; __Func_80921c4(q0, q1, q2); }
    __MapActor_Jump(8, 4, 0xa);
    __MapActor_Jump(8, 6, 0x28);
    __Func_809259c(8, 3);
    __Func_8093040(8, 0, 0x14);
    *(int *)(*q + (0xe0 << 1)) = 0x202;
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(0xb);
    __CutsceneEnd();
}

void OvlFunc_944_20087b0(void)
{
    unsigned char *a;
    unsigned char *p;
    unsigned char *s;
    int h;
    int z;
    __CutsceneStart();
    __Func_8092950(0, 0xf);
    p = __MapActor_GetActor(0);
    __Actor_SetSpriteFlags(p, 0);
    __WaitFrames(1);
    __LoadFieldActors(gOvl_0200976c);
    __WaitFrames(1);
    OvlFunc_944_2008a84(0x9);
    OvlFunc_944_2008a84(0xa);
    OvlFunc_944_2008a84(0xb);
    OvlFunc_944_2008a84(0xc);
    OvlFunc_944_2008a84(0xd);
    OvlFunc_944_2008a84(0xe);
    OvlFunc_944_2008a84(0xf);
    s = gScript_944__0200939c;
    __MapActor_SetBehavior(8, s);
    *(int *)(iwram_3001ebc + 0x1c0) = 0x203;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x78);
    a = __MapActor_GetActor(9);
    __MapActor_SetIdle(9);
    h = 0x80 << 24;
    z = 0;
    *(int *)(a + 0x38) = h;
    *(int *)(a + 0x3c) = h;
    *(int *)(a + 0x40) = h;
    *(int *)(a + 0x24) = z;
    *(int *)(a + 0x28) = z;
    *(int *)(a + 0x2c) = z;
    *(int *)(a + 0x4c) = z;
    __CutsceneWait(0x14);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 9; q1 <<= 12; q2 <<= 11; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN4; q1 = 0xa4; q2 = 0x90; q0 = (int)a; q1 <<= 16; q2 <<= 16; q3 = 0x1410000; __Actor_TravelTo((unsigned char *)q0, q1, q2, q3); }
    __Actor_WaitMovement(a);
    { PIN4; q1 = 0xa4; q2 = 0xd0; q0 = (int)a; q1 <<= 16; q2 <<= 15; q3 = 0x1410000; __Actor_TravelTo((unsigned char *)q0, q1, q2, q3); }
    __Actor_WaitMovement(a);
    { PIN4; q1 = 0xcc; q2 = 0xf8; q0 = (int)a; q1 <<= 16; q2 <<= 15; q3 = 0x1410000; __Actor_TravelTo((unsigned char *)q0, q1, q2, q3); }
    __Actor_WaitMovement(a);
    __Actor_TravelTo((unsigned char *)a, 0x90 << 16, 0, 0xa9 << 16);
    __MapActor_SetIdle(8);
    __WaitFrames(1);
    { PIN3; q1 = 0x80; q0 = 8; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0x103; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    __MapActor_SetSpeed(9, 0x80 << 10, 0x80 << 9);
    OvlFunc_944_2008a84(9);
    __CutsceneWait(0x14);
    __MapActor_SetBehavior(8, s);
    __CutsceneWait(0x78);
    __MapActor_SetIdle(9);
    *(int *)(a + 0x38) = h;
    *(int *)(a + 0x3c) = h;
    *(int *)(a + 0x40) = h;
    *(int *)(a + 0x24) = z;
    *(int *)(a + 0x28) = z;
    *(int *)(a + 0x2c) = z;
    *(int *)(a + 0x4c) = z;
    __CutsceneWait(0x14);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 9; q1 <<= 12; q2 <<= 11; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN4; q1 = 0xa4; q2 = 0x90; q3 = 0x1410000; q0 = (int)a; q1 <<= 16; q2 <<= 16; __Actor_TravelTo((unsigned char *)q0, q1, q2, q3); }
    __Actor_WaitMovement(a);
    { PIN3; q1 = 0xa0; q2 = 0xa0; q0 = 9; q1 <<= 11; q2 <<= 10; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN4; q1 = 0xa4; q2 = 0xd0; q0 = (int)a; q1 <<= 16; q2 <<= 15; q3 = 0x1410000; __Actor_TravelTo((unsigned char *)q0, q1, q2, q3); }
    __Actor_WaitMovement(a);
    { PIN4; q1 = 0xa4; q2 = 0xe4; q0 = (int)a; q1 <<= 16; q2 <<= 15; q3 = 0x1410000; __Actor_TravelTo((unsigned char *)q0, q1, q2, q3); }
    __Actor_WaitMovement(a);
    { PIN4; q1 = 0xa4; q2 = 0xd0; q0 = (int)a; q1 <<= 16; q2 <<= 15; q3 = 0x1410000; __Actor_TravelTo((unsigned char *)q0, q1, q2, q3); }
    __Actor_WaitMovement(a);
    { PIN4; q1 = 0xcc; q2 = 0xf8; q0 = (int)a; q1 <<= 16; q2 <<= 15; q3 = 0x1410000; __Actor_TravelTo((unsigned char *)q0, q1, q2, q3); }
    __Actor_WaitMovement(a);
    { PIN4; q1 = 0x90; q1 <<= 16; q3 = 0xa9; q3 <<= 16; q2 = 0; q0 = (int)a; __Actor_TravelTo((unsigned char *)q0, q1, q2, q3); }
    __MapActor_SetIdle(8);
    __WaitFrames(1);
    { PIN3; q1 = 0x80; q0 = 8; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0x103; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x80; q1 <<= 10; q2 <<= 9; q0 = 9; __MapActor_SetSpeed(q0, q1, q2); }
    OvlFunc_944_2008a84(9);
    __MapActor_Jump(8, 4, 0x14);
    __MapActor_Jump(8, 6, 0x28);
    __PlaySound(0x1d);
    __SetFlag(0x8f << 4);
    __MessageID(0x1e49);
    __Func_8093040(0x10, 0, 0x14);
    { PIN3; q1 = 0x80; q0 = 8; q1 <<= 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __Func_80921c4(8, 0xa4, 0xac << 1);
    __CutsceneWait(0x28);
    __Func_80925cc(8, 2);
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(0xc);
    __CutsceneEnd();
}
