/* OvlFunc_945_200a7d8  --  0x0200a7d8
 *   [asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_a.s, 3rd of 6]
 *
 * 771 instructions of straight-line cutscene: 203 call sites, three
 * `__Func_8091c7c` tests with two arms each, and six guarded actor fetches.
 * Built at the tree default -O2 -- no Makefile rule matches this stem, so
 * `asm/%.o: src/%.c` applies and objcmp prints no `(built with: ...)` line.
 * NO FLAG GROUP IS INVOLVED; the match is at the tree default.
 *
 * THIS IS A COMMONING FUNCTION, NOT A REBUILDING ONE, AND THE HOLE TEST READS
 * THE PIN SET OFF THE REFERENCE.  The prologue is `push {r5, r6, r7, lr}` plus
 * a hand-rolled save of r10 and r8, and those five registers park script
 * constants that are reached later with `mov r1, r8` / `adds r0, r5, #0` --
 * 40 such copies over r5, r6, r7, r8 and r10.  Every one is a HOLE BY
 * CONSTRUCTION (docs/elevation.md, "THE PIN SET'S HOLES ARE READABLE, NOT
 * SEARCHABLE"): the ROM has already commoned that value, so a pin there could
 * only rematerialise it.  "Every nominated site minus those 40" is 54 pins and
 * lands 21 differing AT THE EXACT SIZE AND THE EXACT INSTRUCTION COUNT
 * (2072 bytes, 803 encodings) from a standing start -- against 700 differing
 * for no pins and 703 for all pins, both of which get the prologue wrong.
 *
 * NEW, AND THE REASON THIS CONVERGED IN ONE SITTING: HERE THE HOLE TEST'S
 * RESIDUE WAS A SINGLE CLASS.  All 21 survivors were argument-window ORDERING
 * inside a call, with no register-allocation or liveness component anywhere --
 * which is what "exact size, exact instruction count" was telling us.  Six
 * ordering-only pins closed 19 of them (sites 19, 29, 124, 133, 135, 199) and
 * one respelling closed the last two.  Site 19 is itself a hole and needed the
 * pin ANYWAY: the pin removes ordering freedom and cse still substitutes r5
 * for the constant, so the commoning survives.  The hole test NOMINATES; it
 * does not DECIDE.
 *
 * REPLAY THE ROM'S ARGUMENT WINDOW, DO NOT FILL ASCENDING.  Each pinned site
 * below is written in the ROM's emitted order INCLUDING THE SHIFTS
 * (`q1 = 0xd0; q1 <<= 8; q0 = 0x8;`), transcribed mechanically by
 * extract.py into windows.txt.  Measured on the final 35-pin set:
 *
 *   ROM window replay ...................... EXACT
 *   descending fill, shifts inline ......... 2 differing
 *   ascending fill, shifts inline .......... 472 differing, 8 bytes SHORT
 *
 * The recorded default ("THE UNIFORM FILL IS A DEFAULT, NOT A LAW" / "uniform
 * whole-value ascending fill") is WRONG on this function and wrong loudly.
 * The mechanism is not cosmetic: with r1 written after r0 at the twelve
 * `0x80 << 8` sites, cse hoists that constant into a NEW held pseudo the ROM
 * does not have (`movs r5,#128 / lsls r5,#8 / adds r1,r5,#0` in place of the
 * ROM's per-site `mov`+`lsl`), which is where the eight missing bytes go and
 * why 470 of the 472 are a shifted literal pool.  Read the shorter output as
 * the recorded tell running BACKWARDS: when a fill makes the output shorter,
 * something is commoned that should not be.  Forcing the three
 * `__Func_8092c40` sites descending by name does not rescue it (470).
 *
 * ONE SITE CANNOT BE TRANSCRIBED, AND THE RECORDED RULE NAMES IT.  Site 199,
 * `__Func_80921c4(8, 0xe4 << 1, 0xa2 << 2)`, emits `mov r1 / mov r2 / lsl r2 /
 * mov r0 / lsl r1` -- the `lsl r2` BEFORE the last argument.  Replaying that
 * verbatim leaves 2 differing; writing it in the form its seven siblings use
 * (`q1; q2; q0; q1 <<= 1; q2 <<= 2;`) is exact.  That is exactly
 * docs/elevation.md, "DO NOT TRANSCRIBE THE ROM'S SHIFT ORDER": sched2
 * produces both emitted orders from one spelling, so a site that looks
 * different from its siblings in the ROM need not be different in the source.
 * The sibling comparison found the right spelling on the first try.
 *
 * THIRTY-FIVE PINS, MINIMAL BY MEASUREMENT AND FROM BOTH ENDS.  The 54
 * candidates were stripped one at a time under objcmp, forward and reverse.
 * Both directions reach 35 and a second forward round over the survivors moves
 * nothing, so 35 is the size.  It is NOT one set: the forward fixpoint holds
 * site 88 and the reverse holds site 78 -- `__Func_8092adc(0, 0x80<<8, 0)` and
 * `__Func_8092adc(2, 0x80<<8, 0)`, the two ends of one CSE class, and either
 * alone is exact.  "N pins is a size, not a set", once more.
 *
 * THE THREE `__Func_8091c7c` TESTS ARE PLAIN TWO-ARMED IFs, NOT THE FLAG-
 * VARIABLE SHAPE.  Neither arm sets a pseudo that a later `cmp` reads, so
 * there is no `v` here as there is in the b240/b243 templates.  Both arms of
 * the first and third tests bump `iwram_3001ebc[0xec << 1]` by the same
 * amount; that duplication is the SOURCE, not a missed CSE, because the arms
 * differ in what they do around it (the third arm-pair runs the same
 * `OvlFunc_945_200c86c(0x1008)` on the other side of the bump).
 *
 * THE ACTOR POINTER IS `unsigned char *`.  `int *` is 135 differing and six
 * instructions long; `short *` is 12 differing.  The six guarded fetches read
 * `*(int *)(p + 8)` / `*(int *)(p + 0x10)` for the SetPos trio and
 * `*(short *)(p + 0xa)` / `*(short *)(p + 0x12)` for the TravelTo trio, and
 * none of the six needs a pin.
 *
 * The `0x1ddb` message id, `0x925` save bit, `0xcccc`/`0x1999`/`0x26666`/
 * `0x13333` speeds and the `0x9009`/`0x900c`/`0x1008`/`0x4008` script ids are
 * all bare literals: objcmp reports 207 relocations identical, and the only
 * symbols this function carries are `iwram_3001ebc` and
 * `gScript_883__0200e6e4`.
 *
 * LANDING NEEDS A SPLIT.  The .s holds six functions and this is the third;
 * overlays/rom_7cb2c0/overlay.ld:58 is the ONLY checked-in line naming the .o
 * (`asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_a.o(.text)`), and
 * the .s has no .data, .rodata or .bss section at all.  The `.word` block at
 * line 2040 is a jump table belonging to OvlFunc_945_200aff0 and must travel
 * with it into the _c part.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char gScript_883__0200e6e4[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetSpeed(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __Func_8078a08(int a);
extern void __Func_808e118(void);
extern int __Func_8091c7c(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void OvlFunc_945_200c86c(int n);
extern void OvlFunc_945_200c880(int slot, int v);
extern void OvlFunc_945_200c8e8(int a, int b, int c);

#define PIN2 register int q0 __asm__("r0"); register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_945_200a7d8(void)
{
    unsigned char *p;

    __CutsceneStart();
    __Func_808e118();
    __Func_809280c(0x8, 0x0, 0x0);
    { PIN3; q1 = 0x80; q2 = 0x28; q0 = 0x8; q1 <<= 1;
      __MapActor_Emote(q0, q1, q2); }
    __Func_809259c(0x8, 0x3);
    __MessageID(0x1ddb);
    OvlFunc_945_200c86c(0x1008);
    __Func_809259c(0x9, 0x1);
    __Func_809259c(0xc, 0x1);
    __Func_809259c(0xb, 0x1);
    __Func_809259c(0xd, 0x1);
    __Func_80925cc(0xa, 0x1);
    { PIN3; q1 = 0xd0; q0 = 0x9; q1 <<= 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xd0; q0 = 0xc; q1 <<= 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xd0; q0 = 0xb; q1 <<= 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xd0; q0 = 0xd; q1 <<= 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xb0; q2 = 0x14; q0 = 0xa; q1 <<= 8;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x8, 0x1);
    { PIN2; q1 = 0x0; q0 = 0x1008;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0) {
        __Func_80925cc(0x9, 0x2);
        OvlFunc_945_200c86c(0x9009);
        { PIN3; q1 = 0x84; q2 = 0x28; q0 = 0x8; q1 <<= 1;
          __MapActor_Emote(q0, q1, q2); }
        OvlFunc_945_200c86c(0x1008);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 2;
    } else {
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 2;
        __Func_80925cc(0x9, 0x1);
        OvlFunc_945_200c86c(0x9009);
        __Func_809259c(0x8, 0x2);
        OvlFunc_945_200c86c(0x9008);
    }
    { PIN3; q2 = 0x28; q0 = 0xd; q1 = 0x105;
      __MapActor_Emote(q0, q1, q2); }
    __Func_80933d4(0xcccc, 0x1999);
    __Func_80933f8(0xec << 17, -0x1, 0x9f << 18, 0x1);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0xd; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xec; q2 = 0x296; q0 = 0xd; q1 <<= 1;
      __Func_80921c4(q0, q1, q2); }
    OvlFunc_945_200c880(0xd, 0xb0 << 8);
    OvlFunc_945_200c86c(0xd);
    OvlFunc_945_200c880(0x8, 0xa0 << 7);
    __MapActor_DoAnim(0x8, 0x3);
    __MapActor_DoAnim(0x9, 0x3);
    __Func_8092adc(0xb, 0x0, 0x0);
    { PIN3; q1 = 0x80; q2 = 0x14; q0 = 0xd; q1 <<= 8;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0xb, 0x3);
    __MapActor_DoAnim(0xd, 0x3);
    __CutsceneWait(0x14);
    __Func_80925cc(0xc, 0x1);
    OvlFunc_945_200c880(0xc, 0xc0 << 6);
    __Func_8093040(0x100c, 0x0, 0x14);
    __Func_8092adc(0xb, 0xb0 << 8, 0x14);
    __MapActor_Emote(0xb, 0x101, 0x28);
    OvlFunc_945_200c86c(0xb);
    OvlFunc_945_200c880(0xc, 0xd0 << 8);
    __MapActor_SetAnim(0xc, 0x4);
    OvlFunc_945_200c86c(0x900c);
    OvlFunc_945_200c880(0xd, 0xb0 << 8);
    __Func_80925cc(0xd, 0x1);
    OvlFunc_945_200c86c(0xd);
    { PIN3; q1 = 0x80; q2 = 0x14; q0 = 0x9; q1 <<= 1;
      __MapActor_Emote(q0, q1, q2); }
    OvlFunc_945_200c880(0x9, 0xc0 << 6);
    __Func_80925cc(0x9, 0x1);
    OvlFunc_945_200c86c(0x9);
    __MapActor_DoAnim(0xc, 0x3);
    OvlFunc_945_200c86c(0x900c);
    __Func_80925cc(0x8, 0x2);
    OvlFunc_945_200c86c(0x8);
    OvlFunc_945_200c880(0xc, 0xd0 << 8);
    __MapActor_DoAnim(0xc, 0x3);
    OvlFunc_945_200c86c(0x900c);
    __Func_80925cc(0xb, 0x2);
    OvlFunc_945_200c880(0xb, 0xb0 << 8);
    OvlFunc_945_200c86c(0xb);
    OvlFunc_945_200c880(0xc, 0x0);
    OvlFunc_945_200c86c(0x900c);
    __Func_8092adc(0x8, 0xc0 << 6, 0x0);
    __Func_8092adc(0x9, 0x0, 0x0);
    __Func_8092adc(0xb, 0xd0 << 8, 0x0);
    __Func_8092adc(0xd, 0xd0 << 8, 0x0);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x0; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xf3; q2 = 0x98; q0 = 0x0; q1 <<= 1; q2 <<= 2;
      __Func_80921c4(q0, q1, q2); }
    __Func_8092adc(0x0, 0x80 << 8, 0x0);
    p = __MapActor_GetActor(0x0);
    if (p != 0)
        __MapActor_SetPos(0x1, *(int *)(p + 8), *(int *)(p + 0x10));
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x1; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xf3; q2 = 0x9c; q0 = 0x1; q1 <<= 1; q2 <<= 2;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0x1; q1 <<= 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    p = __MapActor_GetActor(0x1);
    if (p != 0)
        __MapActor_SetPos(0x2, *(int *)(p + 8), *(int *)(p + 0x10));
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x2; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xf3; q2 = 0xa0; q0 = 0x2; q1 <<= 1; q2 <<= 2;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0x2; q1 <<= 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    p = __MapActor_GetActor(0x2);
    if (p != 0)
        __MapActor_SetPos(0x3, *(int *)(p + 8), *(int *)(p + 0x10));
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x3; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xf3; q2 = 0xa4; q0 = 0x3; q1 <<= 1; q2 <<= 2;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0x3; q1 <<= 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_Emote(0xc, 0x84 << 1, 0x28);
    OvlFunc_945_200c86c(0x900c);
    __Func_80925cc(0x9, 0x1);
    OvlFunc_945_200c86c(0x1009);
    __MapActor_DoAnim(0x8, 0x3);
    OvlFunc_945_200c880(0x8, 0xa0 << 7);
    OvlFunc_945_200c86c(0x8);
    OvlFunc_945_200c880(0x8, 0xc0 << 6);
    { PIN2; q1 = 0x0; q0 = 0x8;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 1) {
        __Func_809259c(0x8, 0x2);
        OvlFunc_945_200c86c(0x8);
        __MapActor_DoAnim(0xc, 0x3);
        OvlFunc_945_200c86c(0x900c);
        __Func_809259c(0x9, 0x1);
        __Func_8093040(0x9009, 0x0, 0x28);
        (*(unsigned short *)(iwram_3001ebc + (0xec << 1)))++;
    } else {
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 3;
        __Func_809259c(0x8, 0x3);
        __Func_8093040(0x8, 0x0, 0x28);
    }
    __Func_80925cc(0xd, 0x1);
    OvlFunc_945_200c86c(0xd);
    __Func_80925cc(0x8, 0x1);
    OvlFunc_945_200c880(0x8, 0xa0 << 7);
    OvlFunc_945_200c86c(0x8);
    __Func_80925cc(0xd, 0x1);
    OvlFunc_945_200c880(0xd, 0xb0 << 8);
    __Func_8093040(0xd, 0x0, 0x14);
    __MapActor_DoAnim(0x8, 0x3);
    { PIN3; q0 = 0x8; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(0x8, 0xec << 1, 0x9e << 2);
    OvlFunc_945_200c86c(0x4008);
    { PIN3; q2 = 0x28; q0 = 0xd; q1 = 0x103;
      __MapActor_Emote(q0, q1, q2); }
    __Func_809259c(0xd, 0x2);
    OvlFunc_945_200c86c(0xd);
    __MapActor_SetAnim(0x8, 0x4);
    __Func_8093040(0x4008, 0x0, 0x28);
    __Func_80925cc(0xb, 0x1);
    OvlFunc_945_200c880(0xb, 0xb0 << 8);
    OvlFunc_945_200c86c(0x100b);
    { PIN3; q1 = 0x81; q0 = 0xa; q1 <<= 1; q2 = 0x14;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x26666; q2 = 0x13333;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_Jump(0xa, 0x2, 0x0);
    { PIN3; q1 = 0xe7; q2 = 0x2a2; q0 = 0xa; q1 <<= 1;
      __Func_80921c4(q0, q1, q2); }
    OvlFunc_945_200c880(0xa, 0xb0 << 8);
    __Func_809259c(0xa, 0x2);
    OvlFunc_945_200c86c(0xa);
    OvlFunc_945_200c880(0x9, 0xa0 << 7);
    __MapActor_DoAnim(0x9, 0x4);
    OvlFunc_945_200c86c(0x9);
    __MapActor_DoAnim(0x8, 0x3);
    OvlFunc_945_200c86c(0x4008);
    { PIN3; q1 = 0x81; q0 = 0xd; q1 <<= 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0xd, 0x0, 0x28);
    OvlFunc_945_200c880(0x9, 0xc0 << 6);
    __Func_809259c(0x9, 0x2);
    OvlFunc_945_200c86c(0x1009);
    OvlFunc_945_200c880(0xc, 0x0);
    { PIN3; q1 = 0x80; q0 = 0x8; q1 <<= 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x9, 0xa0 << 7, 0x0);
    __Func_8092adc(0xb, 0xb0 << 8, 0x0);
    __Func_8092adc(0xd, 0xb0 << 8, 0x0);
    __Func_8092adc(0xa, 0xb0 << 8, 0x14);
    __Func_80925cc(0xc, 0x1);
    __Func_8093040(0x100c, 0x0, 0x14);
    { PIN3; q2 = 0x28; q0 = 0x8; q1 = 0x101;
      __MapActor_Emote(q0, q1, q2); }
    OvlFunc_945_200c880(0x8, 0xd0 << 8);
    { PIN2; q1 = 0x0; q0 = 0x1008;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0) {
        __MapActor_DoAnim(0x8, 0x3);
        OvlFunc_945_200c86c(0x1008);
        (*(unsigned short *)(iwram_3001ebc + (0xec << 1)))++;
    } else {
        (*(unsigned short *)(iwram_3001ebc + (0xec << 1)))++;
        OvlFunc_945_200c86c(0x1008);
    }
    __MapActor_DoAnim(0x0, 0x3);
    __MapActor_DoAnim(0x8, 0x3);
    OvlFunc_945_200c86c(0x1008);
    OvlFunc_945_200c880(0x8, 0x80 << 8);
    OvlFunc_945_200c86c(0x4008);
    OvlFunc_945_200c8e8(0x2, 0x0, 0x0);
    __MapActor_SetAnim(0xc, 0x3);
    __MapActor_SetAnim(0xb, 0x3);
    __MapActor_SetAnim(0x9, 0x3);
    __Func_809259c(0xa, 0x2);
    __Func_80925cc(0xd, 0x2);
    __CutsceneWait(0x14);
    __MapActor_SetBehavior(0xa, gScript_883__0200e6e4);
    __CutsceneWait(0x4);
    __MapActor_SetBehavior(0xb, gScript_883__0200e6e4);
    __CutsceneWait(0x4);
    __MapActor_SetBehavior(0xc, gScript_883__0200e6e4);
    __CutsceneWait(0x4);
    __MapActor_SetBehavior(0x9, gScript_883__0200e6e4);
    __MapActor_SetAnim(0x3, 0x2);
    p = __MapActor_GetActor(0x2);
    if (p != 0)
        __MapActor_TravelTo(0x3, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(0x3);
    __MapActor_SetPos(0x3, 0x0, 0x0);
    __MapActor_SetAnim(0x2, 0x2);
    p = __MapActor_GetActor(0x1);
    if (p != 0)
        __MapActor_TravelTo(0x2, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(0x2);
    __MapActor_SetPos(0x2, 0x0, 0x0);
    __MapActor_SetAnim(0x1, 0x2);
    p = __MapActor_GetActor(0x0);
    if (p != 0)
        __MapActor_TravelTo(0x1, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(0x1);
    __MapActor_SetPos(0x1, 0x0, 0x0);
    __MapActor_SetBehavior(0xd, gScript_883__0200e6e4);
    { PIN3; q1 = 0xe4; q2 = 0xa2; q0 = 0x8; q1 <<= 1; q2 <<= 2;
      __Func_80921c4(q0, q1, q2); }
    OvlFunc_945_200c880(0x8, 0x0);
    __Func_8078a08(0xe8);
    __SetFlag(0x925);
    __CutsceneEnd();
}
