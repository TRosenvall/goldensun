/* OvlFunc_943_200ba0c  --  0x0200ba0c   [the ONLY function in
 *   asm/overlays/rom_7c7b9c/ovl_30_c_c_a.s -- 224 instructions of straight-line
 *   cutscene script, no branches, no data section]
 *
 *   OK OvlFunc_943_200ba0c -- 636 bytes, 241 encodings and 64 relocations identical
 *
 * re-measured five times (all-pinned form) and four times again after narrowing;
 * objcmp prints no `(built with: ...)` line, so adjust=set() -- the tree default
 * -O2 -mthumb -mthumb-interwork -fcall-used-r4.  makefile_flags() on
 * src/overlays/rom_7c7b9c/ovl_30_c_c_a.c is the EMPTY set and no `%` pattern
 * rule fires, so there is no wildcard hazard.
 *
 * LANDING: WHOLE.  tools/asmfacts.py says "WHOLE  convert directly".  The .s
 * holds one function and NOTHING else -- no .section .data, no .bss, no .lcomm,
 * no .word, no file-local label.  Three linker lines name the object:
 *
 *     overlays/rom_7c7b9c/overlay.ld:62   asm/.../ovl_30_c_c_a.o(.text)
 *     overlays/rom_7c7b9c/overlay.ld:69   asm/.../ovl_30_c_c_a.o(.data)
 *     overlays/rom_7c7b9c/overlay.ld:75   asm/.../ovl_30_c_c_a.o(.bss)
 *
 * and ALL THREE STAY VERBATIM: the build rule is `asm/%.o: src/%.c`, so the
 * object keeps living at the asm/ path.  There is NO linker edit at all --
 * delete the .s, add the .c, rebuild.
 *
 * THE THREE `.L` SYMBOLS ARE ALREADY EXPORTED.  `.L5160`, `.L5b50` and `.L5b60`
 * are referenced here and DEFINED IN ANOTHER FILE,
 * asm/overlays/rom_7c7b9c/ovl_30_c_c_c_b.s -- and all three carry `.global`
 * there (lines 27, 122, 124), grepped.  So the cross-object reference already
 * works today and the split adds no export work.  `.L5b50` and `.L5b60` are
 * `.lcomm ..., 8` slots written as plain `int`s here, which is what the ROM's
 * `str r3, [r2]` says; `.L5160` is a field-actor table address.
 *
 * EIGHTEEN PINNED SITES OF FIFTY, MINIMAL BY MEASUREMENT AND NARROWED IN WIDTH.
 * All 50 pinnable sites were pinned first (that is 0 too); a single-drop sweep
 * found 32 individually inert and all 32 drop together at zero cost; a fixpoint
 * pass over the 18 survivors removes NONE (dropping one costs between 2 and 239 differing); a
 * width pass then narrows 13 of the 18, so 33 registers are named where the
 * uniform form would ship 50.  Uniform ASCENDING fill everywhere; no site wants
 * descending.
 *
 * WHY PINS AT ALL, WITH hi=0 hiv=0.  The ROM spends r5 and r6 only.  Unaided
 * gcc commons five repeated constants (0xcccc, 0x6666, 0xd0 << 8, 0x101,
 * 0x6014) into r8-r11, pushes four extra registers and lands 240 of 241
 * differing at 668 bytes against 636.  This is the recorded reading that a LOW
 * `hiv` means MORE candidates for gcc to shed, not fewer -- read from the zero
 * end this time.
 *
 * TWO NAMED CONSTANTS, AND THEY MUST BE TWO.  `v = 0xb0 << 8` is born beside
 * the second actor's halfword store and read at five later argument sites;
 * `a2 = 0x6014` is born just before __Func_80925cc(0x14, 2) -- the ROM's
 * `ldr r6, =0x6014` sits exactly there -- and read twice.  Neither needs a hard
 * register: the plain `int` locals land in r5 and r6 by themselves.
 *
 * THE 0xc000 HALFWORD STORE NEEDS ITS OWN `int` LOCAL, AND IT IS WORTH 216.
 * `*(unsigned short *)(a + 6) = 0xc0 << 8;` written bare is the recorded HImode
 * literal case (0xc000 >= 0x8000): gcc emits `ldrh r3, .L3` against the ROM's
 * `mov r3, #0xc0 / lsl r3, #8`, and the halfword pool word it needs forces a
 * MID-FUNCTION pool with a `b` over it -- 216 differing and +4 bytes from one
 * literal.  `w = 0xc0 << 8; *(unsigned short *)(a + 6) = w;` is exact.
 * Recycling `v` for both stores instead of a second local `w` is 15 differing,
 * which is the recorded one-local-per-value rule from the other side.
 *
 * WHAT NEEDED NOTHING.  `*(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x202;` --
 * gcc's own `add r2, #0x42` derivation of 0x202 off the 0x1c0 index falls out,
 * the same idiom the file-mate ovl_30_c_c_c_a.c records with 0x201;
 * `__Actor_SetSpriteFlags(__MapActor_GetActor(0), 0)` as one nested expression;
 * the three `a = __MapActor_GetActor(n)` fetches recycling ONE pointer local;
 * `__LoadFieldActors(L5160)` bare, with gcc scheduling the `ldr r0` up between
 * the two symbol stores by itself.
 *
 * MEASURED WORSE / INERT (against 241 encodings / 636 bytes):
 *
 *   spelling                                              differing
 *   ---------------------------------------------------  ---------
 *   no pins at all                                        240 (+32 bytes)
 *   final 18 pins, bare 0xc000 literal                    216 (+4 bytes)
 *   drop the first __MapActor_SetSpeed pin                239 (+20 bytes)
 *   drop the __Func_8092adc(0x15,0xd0<<8,0) pin           219 (+8 bytes)
 *   drop the __Func_8092adc(0x16,0xd0<<8,0) pin           213 (+12 bytes)
 *   drop the __MapActor_Emote(0x15,0x101,0) pin           210 (+12 bytes)
 *   drop the first __MapActor_Surprise pin                 19
 *   drop the __MapActor_SetSpeed(0x15,0x19999,..) pin     137 (+4 bytes)
 *   drop the __MapActor_SetSpeed(0x14,0x80<<9,..) pin      35
 *   drop any other survivor                             2 to 3
 *   one recycled local for the 0xc000 and 0xb000 stores    15
 *   narrowing the SetSpeed / Surprise / 8092adc pins
 *     to PIN1                                          19 to 239
 *   narrowing any of the 80921c4 / 809218c pins to PIN1  2
 *
 *   INERT (tie at 0, so the narrower form ships):
 *     all 50 sites pinned instead of the minimal 18
 *     the 32 inert pins, dropped individually or all together
 *
 * -- worked in scratch_elev/b252/zhi; gen.py rebuilds any pin set from a Python
 *    dict, sweep.py / sweep2.py are the drop, fixpoint and width passes.
 */
extern unsigned char L5160[] __asm__(".L5160");
extern int L5b50 __asm__(".L5b50");
extern int L5b60 __asm__(".L5b60");
extern unsigned char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __LoadFieldActors(unsigned char *p);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __MessageID(int id);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092950(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8091e9c(int n);
extern void OvlFunc_943_200b9ec(int a);
extern void OvlFunc_943_200ba00(int a, int b);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_943_200ba0c(void)
{
    unsigned char *a;
    int v;
    int w;
    int a2;

    __CutsceneStart();
    L5b50 = 0x80 << 11;
    L5b60 = -0x8000;
    __LoadFieldActors(L5160);
    __WaitFrames(1);
    __MapActor_SetPos(0x15, 0xb6 << 16, 0x26a0000);
    a = __MapActor_GetActor(0x15);
    w = 0xc0 << 8;
    *(unsigned short *)(a + 6) = w;
    __MapActor_SetPos(0x14, 0xda << 16, 0x81 << 18);
    a = __MapActor_GetActor(0x14);
    v = 0xb0 << 8;
    *(unsigned short *)(a + 6) = v;
    __MapActor_SetPos(0x16, 0xcc << 16, 0x20e0000);
    a = __MapActor_GetActor(0x16);
    *(unsigned short *)(a + 6) = v;
    __MapActor_SetPos(0x17, 0, 0);
    __Func_8092950(0, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __WaitFrames(1);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x202;
    __MapTransitionIn();
    __WaitMapTransition();
    { PIN3; q0 = 0x15; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN2; q0 = 0x15; q1 = 0xb6;
      __Func_80921c4(q0, q1, 0x85 << 2); }
    __Func_8092adc(0x15, v, 0x28);
    __MessageID(0x1f23);
    OvlFunc_943_200b9ec(0x15);
    a2 = 0x6014;
    __Func_80925cc(0x14, 2);
    __MapActor_SetAnim(0x14, 4);
    OvlFunc_943_200b9ec(a2);
    { PIN2; q0 = 0x15; q1 = 0xd0 << 8;
      __Func_8092adc(q0, q1, 0); }
    { PIN2; q0 = 0x16; q1 = 0xd0 << 8;
      __Func_8092adc(q0, q1, 0); }
    { PIN2; q0 = 0x15; q1 = 0x101;
      __MapActor_Emote(q0, q1, 0); }
    { PIN1; q0 = 0x16;
      __MapActor_Emote(q0, 0x101, 0x3c); }
    __MapActor_SetAnim(0x14, 3);
    OvlFunc_943_200b9ec(a2);
    { PIN2; q0 = 0x15; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __MapActor_Surprise(0x16, 0x81 << 1);
    __CutsceneWait(0x50);
    { PIN1; q0 = 0x15;
      __MapActor_Emote(q0, 0x80 << 1, 0x14); }
    { PIN3; q0 = 0x15; q1 = 0x19999; q2 = 0xcccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN2; q0 = 0x15; q1 = 0xc2;
      __Func_80921c4(q0, q1, 0xfa << 1); }
    __Func_8092adc(0x15, v, 0x14);
    { PIN2; q0 = 0x16; q1 = 0xcccc;
      __MapActor_SetSpeed(q0, q1, 0x6666); }
    { PIN2; q0 = 0x16; q1 = 0xc0;
      __Func_80921c4(q0, q1, 0x206); }
    __Func_8092adc(0x16, v, 0);
    { PIN3; q0 = 0x14; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(0x14, 0xd2, 0xfe << 1);
    OvlFunc_943_200ba00(0x14, v);
    __Func_80925cc(0x15, 1);
    OvlFunc_943_200b9ec(0x5015);
    __MapActor_DoAnim(0x14, 3);
    OvlFunc_943_200ba00(0x16, 0xd0 << 8);
    OvlFunc_943_200b9ec(0x9016);
    { PIN1; q0 = 0x14;
      __Func_8092adc(q0, 0x80 << 8, 0x14); }
    __MapActor_SetAnim(0x14, 4);
    OvlFunc_943_200b9ec(0xa014);
    { PIN2; q0 = 0x14; q1 = 0xcc;
      __Func_80921c4(q0, q1, 0x86 << 2); }
    __Func_8092adc(0x16, v, 0);
    { PIN2; q0 = 0x14; q1 = 0xb6;
      __Func_80921c4(q0, q1, 0x89 << 2); }
    { PIN2; q0 = 0x14; q1 = 0xb6;
      __Func_80921c4(q0, q1, 0x94 << 2); }
    { PIN2; q0 = 0x14; q1 = 0xb6;
      __Func_809218c(q0, q1, 0xa6 << 2); }
    __CutsceneWait(0x28);
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(0x10);
}
