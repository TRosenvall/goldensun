/* OvlFunc_943_200bf30  --  0x0200bf30      [1st of 2 in this file]
 * OvlFunc_943_200c218  --  0x0200c218      [2nd of 2 in this file]
 *   [asm/overlays/rom_7c7b9c/ovl_30_c_c_c.s]
 *
 * BOTH EXACT, so the .s has NO ASSEMBLY LEFT IN IT and the whole text section
 * becomes this one TU.
 *
 *   OK OvlFunc_943_200bf30 -- 744 bytes, 286 encodings and 75 relocations identical
 *   OK OvlFunc_943_200c218 --  60 bytes,  25 encodings and  5 relocations identical
 *
 * both re-run four times, and the merged object verified WHOLE against a
 * text-only copy of the reference.  makefile_flags() on the target path is the
 * EMPTY set: tree default -O2 -fcall-used-r4, no flag group, no wildcard.
 *
 * 271 + 25 instructions of straight-line cutscene script, one save-counter fork.
 *
 * ================= OvlFunc_943_200bf30 =================
 *
 * READ THE PROLOGUE BY CONTENT.  `push {r5, r6, lr}` plus `mov r6, r8 /
 * push {r6}`, and every saved register is a real value:
 *
 *     r5   0xa0 << 7, then gScript_943__0200c918   TWO disjoint roles
 *     r6   0x2014                                  a repeated script constant
 *     r8   &iwram_3001ebc                          gcse commons the symbol
 *
 * `hi = 4, hiv = 1` says the high-register traffic is prologue boilerplate.  It
 * does NOT say there is no eviction work: unaided gcc spends r7 AND r8-r11 on
 * five commoned constants (0x80<<8, 0x80<<9, 0x80<<1, 0xe0<<8, 0x2016), pushes
 * four extra registers and lands 279 of 286 differing at 764 bytes against 744.
 * The ROM holding ONE high-register value is what gives gcc four MORE
 * candidates to shed, not fewer.
 *
 * TWENTY-FOUR PINNED SITES OF 46, MINIMAL BY MEASUREMENT AND MINIMAL IN WIDTH.
 * All 46 pinnable sites were pinned first (6 differing); a single-drop sweep
 * found 22 individually inert, all 22 drop together at zero cost, and a
 * fixpoint pass over the 24 survivors removes NONE -- dropping one costs
 * between 2 and 273.  A second pass then narrowed 19 of the 24: four sites need
 * r0 only and fifteen need r0+r1, so 43 registers are named where the template
 * form would ship 68.
 *
 * UNIFORM ASCENDING FILL AT 22 OF THE 24 SITES.  One statement per argument,
 * whole value per statement, reproduces `mov r1 / mov r2 / mov r0 / lsl r1`,
 * seeds-then-shifts, and the pooled-pair orders alike.
 *
 * THE TWO 0x2016 SITES BOTH WANT THE DESCENDING FILL, AND THEY ARE COUPLED
 * (NEW).  `__Func_8092c40(0x2016, 0)` and `__Func_8093054(0x2016, 0)` each emit
 * `mov r1, #0 / ldr r0, =0x2016`; ascending is 4 differing, descending is exact.
 * __Func_8092c40 wanting descending is the seventh instance in the corpus and
 * is recorded in the sibling ovl_30_c_c_c_a_c_c_c_c_c_b.c as "binary:
 * descending or nothing".  There IS a third option: at EITHER site a width-1
 * pin (r0 alone, second argument left as a literal) is an exact TIE with the
 * descending PIN2 -- the descending fill and the r0-only pin buy the same
 * thing, which is the slot `mov` scheduled ahead of the pool load.  But NOT AT
 * BOTH: narrowing both to PIN1 together is 4 differing, while each alone is 0.
 * The uniform descending PIN2 ships at both, because a pin set is chosen as a
 * SET and the coupled form is the one that does not depend on which site you
 * narrow.
 *
 * TWO NAMED CONSTANTS AND ONE NAMED POINTER, ALL THREE REQUIRED.  `a1 =
 * 0xa0 << 7` is born INSIDE the __MapActor_Emote argument setup (`mov r5,#0xa0`
 * between the movs, `lsl r5,#7` after the `bl`) and read at two later sites;
 * `a2 = 0x2014` is born mid-function and read three times; `s` carries the
 * script address to three __MapActor_SetBehavior calls.  Writing the statement
 * for a1 immediately after the Emote call is what puts the split build in the
 * ROM's place -- no barrier and no launder is needed for it.
 *
 * `iwram_3001ebc` IS READ TWICE AND BOTH READS ARE PLAIN.  The `+= 1` on the
 * 0xec<<1 halfword and the `= 0x201` on the 0xe0<<1 word both fall out written
 * bare, including the ROM's `add r2, #0x41` deriving 0x201 from the 0x1c0
 * index -- the same idiom the sibling ovl_30_c_c_c_a_c_c_c_c_c_b.c records.
 *
 * `.L51d8` IS REACHED WITH THE ASM-LABEL EXTENSION, AND IT IS NOT YET EXPORTED.
 * `extern unsigned char L51d8[] __asm__(".L51d8");` is the recorded least
 * invasive spelling and there is precedent for it in
 * src/overlays/rom_7b2078/ovl_314_c_c_c_c_c_c_c_a.c.  But CHECK THIS BEFORE
 * SPLITTING: `.L51d8` is defined at ovl_30_c_c_c.s:407 and is **NOT** in that
 * file's `.global` block -- grepped, zero hits.  It needs no export today
 * because the `ldr r0, =.L51d8` at line 12 is in the SAME object.  The moment
 * the split puts the function in a .c and the label in _b.s the reference
 * crosses an object boundary, so `.global .L51d8` MUST BE ADDED to the data
 * half.  A label emits no bytes, so the export is symbol-table metadata only
 * and the link stays byte-identical.  (gScript_943__0200c918, the other symbol
 * this file names, IS already `.global` at line 383.)
 *
 * The identically-named `.L51d8` in asm/overlays/rom_7b2078/ is a DIFFERENT
 * overlay's label and does not collide: overlays link separately, each with its
 * own overlay.ld.
 *
 * ================= OvlFunc_943_200c218 =================
 *
 * THIS FUNCTION WAS PARKED AS UNREACHABLE AND IT IS NOT.  The park
 * src/non_matching/ovl_7c7b9c/200c218.c settled in batch 42, by reading
 * local-alloc.c rather than probing, that gcc-2.96 rematerialises a constant
 * only when the pseudo spans more than one basic block -- so in a STRAIGHT-LINE
 * function the duplicated `0xe8 << 16` / `0xa9 << 18` pair cannot be rebuilt
 * from plain C, at 14 of 25 differing.  That reading is correct and it is also
 * no longer the whole story: r0-r3 are CALL-CLOBBERED, so a constant pinned
 * there cannot survive the intervening `bl` and gcc must rebuild it with no
 * branch involved.  docs/elevation.md records exactly this correction ("no
 * dominating branch means the ordinary spellings are hopeless -- go straight
 * to a call-clobbered pin"); the park predates it.  PIN4 at __Func_80933f8 and
 * PIN3 at __MapActor_SetPos, uniform ascending, are exact on the FIRST screen.
 *
 * The park's own `v = 0x80 << 7` before the halfword store is kept -- it is the
 * recorded HImode-literal lever and it was already right.
 *
 * MEASURED WORSE / INERT (200bf30 unless noted):
 *
 *   spelling                                              differing
 *   ---------------------------------------------------  ---------
 *   no pins at all                                       279 (+20 bytes)
 *   all 46 pins, ascending everywhere                      6
 *   all 46 pins, descending at __Func_8092c40 only         4
 *   drop the first __MapActor_SetSpeed pin               175 (-4 bytes)
 *   drop the first __Func_8092adc(0,0xe0<<8,0) pin       106 (-4 bytes)
 *   drop the __MapActor_SetSpeed(0x16,0x19999,..) pin    273 (+8 bytes)
 *   drop the __Func_8092c40 pin                          244
 *   both 0x2016 sites narrowed to PIN1 together            4
 *   200c218: plain C, no pins                             14 of 25
 *
 * LANDING: HAND SPLIT, BECAUSE THE .s IS WHOLE-PLUS-DATA.  Both functions are
 * closed, so no assembly is left -- but ovl_30_c_c_c.s also carries a trailing
 * `.section .data` (33 `.global` blobs, including `.L51d8` and
 * gScript_943__0200c918 which THIS file references) and a `.section .bss`
 * (12 `.lcomm` slots).  tools/split_s.py cannot help: there is no function
 * boundary to cut at that separates the data.  Split BY HAND into
 *
 *     asm/overlays/rom_7c7b9c/ovl_30_c_c_c_a.s   the two functions, no data
 *     asm/overlays/rom_7c7b9c/ovl_30_c_c_c_b.s   .include + .data + .bss
 *
 * verify `make compare` byte-neutral WITH BOTH FUNCTIONS STILL IN ASSEMBLY,
 * and only then replace _a.s with src/overlays/rom_7c7b9c/ovl_30_c_c_c_a.c.
 * Three linker lines name the object and each becomes TWO lines, _a then _b,
 * every one of them keeping the `asm/` prefix VERBATIM -- the build rule is
 * `asm/%.o: src/%.c`, so a line rewritten to `src/...o` matches nothing and is
 * SILENTLY IGNORED:
 *
 *     overlays/rom_7c7b9c/overlay.ld:64   asm/.../ovl_30_c_c_c.o(.text)
 *     overlays/rom_7c7b9c/overlay.ld:71   asm/.../ovl_30_c_c_c.o(.data)
 *     overlays/rom_7c7b9c/overlay.ld:77   asm/.../ovl_30_c_c_c.o(.bss)
 *
 * The suffixes _a/_b are free under asm/overlays/rom_7c7b9c/.  Landing this
 * file also RETIRES src/non_matching/ovl_7c7b9c/200c218.c.
 *
 * -- worked in scratch_elev/b251/lowp; gen2.py rebuilds any pin set from a
 *    Python set literal, drop/ fix/ wid/ fix2/ hold the four sweeps.
 */
extern unsigned char L51d8[] __asm__(".L51d8");
extern unsigned char gScript_943__0200c918[];
extern unsigned char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __LoadFieldActors(unsigned char *p);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093054(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8091e9c(int n);
extern void OvlFunc_943_2008bb8(void);
extern void OvlFunc_943_200b9ec(int a);
extern void OvlFunc_943_200ba00(int a, int b);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_800fe9c(void);
extern unsigned char *__MapActor_GetActor(int slot);

#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_943_200bf30(void)
{
    int a1;
    int a2;
    unsigned char *s;

    __CutsceneStart();
    __LoadFieldActors(L51d8);
    __WaitFrames(1);
    __MapTransitionIn();
    { PIN3; q0 = 0; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN2; q0 = 0; q1 = 0x94;
      __Func_80921c4(q0, q1, 0xa4 << 2); }
    { PIN2; q0 = 0x16; q1 = 0x80 << 1;
      __MapActor_Emote(q0, q1, 0); }
    a1 = 0xa0 << 7;
    __Func_80925cc(0x16, 1);
    OvlFunc_943_200ba00(0x16, a1);
    __MessageID(0x1f69);
    { PIN2; q1 = 0; q0 = 0x2016;
      __Func_8092c40(q0, q1); }
    { PIN2; q0 = 0; q1 = 0xe0 << 8;
      __Func_8092adc(q0, q1, 0); }
    if (__Func_8091c7c(0, 0) == 1) {
        OvlFunc_943_200b9ec(0x2016);
        __CutsceneEnd();
        return;
    }
    *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    { PIN2; q1 = 0; q0 = 0x2016;
      __Func_8093054(q0, q1); }
    OvlFunc_943_2008bb8();
    { PIN2; q0 = 0x1a; q1 = 0xd8 << 16;
      __MapActor_SetPos(q0, q1, 0x93 << 18); }
    { PIN2; q0 = 0x1a; q1 = 0x13333;
      __MapActor_SetSpeed(q0, q1, 0x9999); }
    { PIN2; q0 = 0x1a; q1 = 0xd8;
      __Func_80921c4(q0, q1, 0x95 << 2); }
    { PIN2; q0 = 0x1a; q1 = 0xbc;
      __Func_80921c4(q0, q1, 0x9a << 2); }
    { PIN2; q0 = 0; q1 = 0xe0 << 8;
      __Func_8092adc(q0, q1, 0); }
    { PIN2; q0 = 0x15; q1 = 0xd0 << 8;
      __Func_8092adc(q0, q1, 0); }
    { PIN2; q0 = 0x16; q1 = 0xd0 << 8;
      __Func_8092adc(q0, q1, 0); }
    OvlFunc_943_200ba00(0x1a, a1);
    __MapActor_Jump(0x1a, 2, 0);
    __MapActor_SetAnim(0x1a, 4);
    __ActorMessage(0x1a, 0);
    { PIN2; q0 = 0x14; q1 = 0xb4 << 16;
      __MapActor_SetPos(q0, q1, 0x3090000); }
    { PIN3; q0 = 0x14; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN2; q0 = 0x14; q1 = 0xb4;
      __Func_80921c4(q0, q1, 0xa6 << 2); }
    a2 = 0x2014;
    { PIN2; q0 = 0x14; q1 = 0xd0 << 8;
      __Func_8092adc(q0, q1, 0); }
    OvlFunc_943_200b9ec(a2);
    { PIN1; q0 = 0;
      __Func_8092adc(q0, 0x80 << 6, 0); }
    { PIN1; q0 = 0x16;
      __Func_8092adc(q0, 0xc0 << 6, 0); }
    { PIN1; q0 = 0x1a;
      __MapActor_Emote(q0, 0x101, 0x3c); }
    __Func_80925cc(0x14, 1);
    OvlFunc_943_200b9ec(a2);
    __Func_809259c(0x15, 2);
    OvlFunc_943_200b9ec(0x15);
    __Func_8092adc(0x14, a1, 0x14);
    __MapActor_SetAnim(0x14, 3);
    OvlFunc_943_200b9ec(0x6014);
    __MapActor_Jump(0x1a, 2, 0x14);
    __MapActor_SetAnim(0x1a, 4);
    OvlFunc_943_200b9ec(0x1a);
    { PIN2; q0 = 0x14; q1 = 0xb6;
      __Func_80921c4(q0, q1, 0xa0 << 2); }
    __Func_8092adc(0x14, 0xd0 << 8, 0);
    OvlFunc_943_200b9ec(0x8014);
    { PIN1; q0 = 0x1a;
      __MapActor_Emote(q0, 0x80 << 1, 0x14); }
    __Func_809259c(0x1a, 2);
    OvlFunc_943_200b9ec(0x1a);
    __MapActor_DoAnim(0x14, 3);
    OvlFunc_943_200ba00(0x16, 0);
    __Func_80925cc(0x16, 1);
    OvlFunc_943_200b9ec(0x16);
    { PIN3; q0 = 0x16; q1 = 0x19999; q2 = 0xcccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    s = gScript_943__0200c918;
    __MapActor_SetBehavior(0x16, s);
    { PIN2; q0 = 0x15; q1 = 0x19999;
      __MapActor_SetSpeed(q0, q1, 0xcccc); }
    __Func_80921c4(0x15, 0xa8, 0x9e << 2);
    __MapActor_SetBehavior(0x15, s);
    __CutsceneWait(0x50);
    __MapActor_SetBehavior(0x1a, s);
    __CutsceneWait(0x28);
    OvlFunc_943_200ba00(0x14, 0x80 << 8);
    OvlFunc_943_200b9ec(a2);
    OvlFunc_943_200ba00(0, 0xe0 << 8);
    __MapActor_DoAnim(0, 3);
    __MapActor_DoAnim(0x14, 3);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x201;
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(0x11);
}

void OvlFunc_943_200c218(void)
{
    unsigned char *a;
    int v;

    { PIN4; q0 = 0xe8 << 16; q1 = -1; q2 = 0xa9 << 18; q3 = 0;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_800fe9c();
    { PIN3; q0 = 0; q1 = 0xe8 << 16; q2 = 0xa9 << 18;
      __MapActor_SetPos(q0, q1, q2); }
    a = __MapActor_GetActor(0);
    v = 0x80 << 7;
    *(unsigned short *)(a + 6) = v;
    __WaitFrames(1);
}
