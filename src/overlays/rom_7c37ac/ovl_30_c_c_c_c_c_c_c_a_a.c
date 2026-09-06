// fakematch
/* ovl_30_c_c_c_c_c_c_c_a_a.c  --  OvlFunc_938_2008360  --  0x02008360
 *   [asm/overlays/rom_7c37ac/ovl_30_c_c_c_c_c_c_c_a_a.s, the ONLY function in
 *    that .s -- one `.thumb_func_start`, no `.data`/`.rodata`/`.bss` section,
 *    no split needed and none proposed]
 *
 * 1649 instructions of straight-line cutscene script: 426 calls, FOUR
 * `__Func_8091c7c` diamonds (one of them a plain `== 1` guard paired with a
 * later re-test of the flag it clears), eight `iwram_3001ebc[0xec<<1]` counter
 * bumps, two whole-word store blocks and FOUR mid-function `.pool_aligned`
 * dumps.  No `__MapActor_GetActor` block anywhere.  Selected on two independent
 * signals -- 36 shared symbols with
 * src/overlays/rom_7c37ac/ovl_30_c_c_c_c_c_c_c_c_b.c, the highest on the list,
 * and ZERO r8-r11 traffic in the reference.  VERDICT:
 *
 *   OK OvlFunc_938_2008360 -- 4268 bytes, 1680 encodings and 431 relocations identical
 *
 * ...against BOTH the scratch reference and the REAL asm/ path.
 *
 * NO FLAG GROUP IS INVOLVED, AND THAT IS A POSITIVE FINDING.  `grep -n
 * rom_7c37ac Makefile` returns NOTHING: the directory has no explicit rule and
 * no wildcard reaches it, so the generic cross-dir `asm/%.o: src/%.c` at
 * Makefile:146 applies and the TU takes the tree default -O2.  objcmp printed
 * no "(built with: ...)" line when screened through the real asm/ path, which
 * is the check that caught the rom_7ef4f4 O1-wildcard trap.  Nothing to add to
 * the Makefile.
 *
 * A PIN FUNCTION, AND THE PROLOGUE SAYS SO BY CONTENT, NOT BY WIDTH.  The ROM
 * pushes {r5, r6, lr} -- a WIDE push -- and still wants pins, because of WHAT
 * it keeps: r6 is the ADDRESS OF `iwram_3001ebc` held across the first three
 * quarters of the function, r5 is the `int v` flag of the first diamond and
 * then the address of the ActorCmd array at the very end.  Counting
 * `mov rN, r5..r11` copies by DESTINATION finds exactly three, all
 * `adds r1, r5, #0` for that array.  NO CALLEE-SAVED REGISTER HOLDS A SCRIPT
 * CONSTANT, so named locals cannot be what the source had.  Plain C opens
 * `push {r5,r6,r7,lr}` (b5e0) against the ROM's b560, 1397 of 1680 differing.
 *
 * THE LENGTH TELL IS SILENT ON THE THING THAT MATTERED.  Plain C is 4256 bytes
 * against 4268 -- twelve SHORT -- but the whole 12 bytes are recovered by the
 * pins, and of the 138 required pins SIZE is silent on 99 of them when dropped
 * (all 52 ordering pins and 47 of the 86 CSE pins).  Read the diff text.
 *
 * 149 PIN CANDIDATES, 138 REQUIRED.  The candidate set is 144 sites nominated
 * by the recorded CSE rule (an argument list carrying a constant the ROM builds
 * more than once and that is not a bare `mov #imm8`; `gen.py --info` prints
 * it), PLUS 4 nominated only by CALL-SITE FAMILY, PLUS the 4 `__Func_8092c40`
 * sites, which no nomination rule reaches.  With all 149 the function is
 * already exact, so the sweep is pure minimisation: every site stripped
 * individually under objcmp, greedily, re-testing after each drop, run to a
 * fixpoint FROM BOTH ENDS.  Both directions survive the SAME 138.  The eleven
 * that fall are sites 5, 15, 60, 112, 165, 176, 225, 236, 242, 329 and 365 --
 * every one a LATER use of a value whose earlier use is pinned, the recorded
 * "one pin at the first use covers the later ones".  They are INERT SCAFFOLDING
 * and DO NOT SHIP.
 *
 * THE FAMILY HEURISTIC PAID, AND IT IS NOT SUFFICIENT ON ITS OWN (NEW).  The
 * recorded rule "same callee AND same argument shape" nominated exactly four
 * extras here -- 104 (`__Func_8092adc(8, 0x80 << 5, 0x28)`), 177 and 287 (two
 * `__MapActor_Emote` sites whose ids 0x107 and 0x84 << 1 are used once) and 204
 * (`__Func_80921c4(2, 0xd9 << 2, 0xec << 1)`).  ALL FOUR SURVIVE MINIMISATION,
 * each costing exactly 2-3 encodings when dropped, and all four are ordering
 * jobs.  The recorded claim on the 630-instruction sibling is that the family
 * rule "makes the set exact WITH NO RESIDUE READ AT ALL".  AT THIS SIZE IT DOES
 * NOT: the family-nominated set alone leaves 39 differing, and 29 after the
 * descending fills, because two things no nomination rule can see remain --
 * a callee that wants the descending fill, and a HOLE.  Both are below.
 *
 * `__Func_8092c40` WANTS THE DESCENDING FILL -- FOUR TIMES IN ONE FUNCTION, AND
 * THAT IS A NEW MAXIMUM.  The recorded entry has this callee as the LONE
 * descending site on each of six functions.  Here sites 160, 180, 227 and 369
 * ALL take `q1 = 0; q0 = N;`, and none of them is nominated by any rule --
 * both arguments are bare `mov #imm8`, so neither the CSE rule nor the family
 * rule can see them.  They have to be added as ordering-only extras.  The
 * recorded "the tell is binary, not graded" holds exactly: the ASCENDING fill
 * at all four measures 8 differing, and so does DROPPING ALL FOUR PINS
 * ALTOGETHER.  Ascending here is a non-pin.
 *
 * NEW: `__Func_8093054` IS A SECOND DESCENDING CALLEE BY NAME.  Site 90,
 * `__Func_8093054(0x6002, 0)`, is the only call to it and the ROM emits
 * `mov r1, #0` before `ldr r0, =0x6002`.  It is nominated by the CSE rule
 * (0x6002 is pooled and used three times), so it is easy to pin and get wrong:
 * pinned ASCENDING it measures 2 differing, EXACTLY what dropping the pin
 * measures, the same binary tell as `__Func_8092c40`.  Its own CSE job is
 * already done by the pins at sites 71 and 87, so the pin at 90 is purely an
 * ordering pin.  The recorded alternative cure for this shape -- the
 * no-prototype lever -- is not available here, because the same reversal is
 * needed at four `__Func_8092c40` sites AND at this one and removing five
 * prototypes costs the file its type checking for no gain.
 *
 * THE PIN SET NEEDS A HOLE WHERE THE ROM ITSELF COMMONS.  The closing trio
 *
 *     __MapActor_SetBehavior(1, ActorCmd_ARRAY_938__02009b94);
 *     __MapActor_SetBehavior(2, ActorCmd_ARRAY_938__02009b94);
 *     __MapActor_RunScript(3, ActorCmd_ARRAY_938__02009b94);
 *
 * carries a symbol address used three times, so the CSE rule nominates all
 * three sites -- and pinning them is WRONG.  The ROM holds the address in r5
 * (`ldr r5, =...` once, then `adds r1, r5, #0` three times); pinning
 * `q1 = ActorCmd_...` rematerialises the `ldr` at each site, costs one
 * instruction, and shifts the entire epilogue: 29 differing with RELOCATIONS
 * DIFFERING.  gcc commons it unaided.  This is the only hole in the set and it
 * was the last residue standing -- the whole rest of the function was already
 * byte-exact.
 *
 * THE RELOCATION LINE SORTS THE 138 PINS PERFECTLY, AND SHARPENS THE RECORDED
 * BOUNDS (NEW).  Dropping each required pin one at a time:
 *
 *   52 ORDERING pins   RELOCATIONS silent, SIZE silent on 52 of 52,
 *                      encoding count EXACTLY 2 or 3, never more
 *   86 CSE pins        RELOCATIONS differ on 86 of 86,
 *                      encoding counts 10 to 1633, SIZE silent on 47 of 86
 *
 * Zero overlap and no middle, on the largest sample yet taken (138 against the
 * recorded 48).  Two refinements to the recorded entry: the CSE band's floor is
 * 10, not the recorded 23 -- site 8 costs exactly 10 with relocations moving --
 * and the ORDERING band is TIGHTER than "small": every one of the 52 costs 2 or
 * 3, so a residue of 4-9 encodings belongs to neither band and did not occur.
 * SIZE is silent on 55% of the genuine CSE losses here against the recorded
 * 42%, confirming it is not the discriminator.
 *
 * THE `int v` FLAG IS STRUCTURAL, NOT SCAFFOLDING, and the first diamond is NOT
 * the template's if/else.  The ROM sets `mov r5, #1` BEFORE the `bl`, tests
 * `cmp r0, #1`, clears r5 inside the arm, and re-tests `cmp r5, #0` AFTER an
 * intervening call -- a plain `== 1` guard plus a later `if (v != 0)`, not an
 * if/else.  Writing it without the flag (the bump duplicated into both arms) is
 * how this candidate first came out and it cost 39 differing with the whole
 * tail displaced; the flag alone took that to 39 -> 29.  Binding v to r5 by
 * hand is unnecessary: gcc picks r5 itself.
 *
 * MEASURED-WORSE TABLE (against the scratch ref, one container run each):
 *
 *   final 138 pins, scratch ref                    OK
 *   final 138 pins, REAL asm/ path                 OK
 *   all 149 pin candidates (11 inert)              OK
 *   whole-value consts (no mov/lsl)                OK  -- both spellings tie
 *   no pins at all                                 1397 differing, SIZE, RELOC
 *   ROM's emitted arg order, not ascending          210 differing
 *   descending fill EVERYWHERE                      546 differing, RELOC
 *   ASCENDING at the four 8092c40 sites               8 differing
 *   drop the four 8092c40 pins entirely               8 differing
 *   ASCENDING at 90 (8093054)                         2 differing
 *   drop pin 90 (8093054) entirely                    2 differing
 *   ASCENDING at every descending site               10 differing
 *   RE-PIN the ActorCmd trio 421,422,423             29 differing, RELOC
 *   drop the 4 FAMILY-only pins                       9 differing
 *   drop 104 / 177 / 287 individually                 2 differing each
 *   drop 204 individually                             3 differing
 *   0x81<<1: pin only the 1st use (39)              1085 differing, SIZE, RELOC
 *   0xc0<<8: pin only the 1st use (76)              1208 differing, SIZE, RELOC
 *
 * The whole-value row is worth keeping: with the pins in place `0x80 << 8` and
 * `0x8000` produce the SAME OBJECT, so the recorded whole-value spelling is a
 * free choice here rather than a lever.  The mov/lsl spelling is kept because
 * it is what the reference reads as.
 *
 * LANDING.  Two linker lines name this .o, both in overlays/rom_7c37ac/
 * overlay.ld and both already pointing at the asm/ path the generic cross-dir
 * rule produces from src/:
 *
 *   overlay.ld:28   asm/overlays/rom_7c37ac/ovl_30_c_c_c_c_c_c_c_a_a.o(.text)
 *   overlay.ld:40   asm/overlays/rom_7c37ac/ovl_30_c_c_c_c_c_c_c_a_a.o(.data)
 *
 * NEITHER NEEDS TO CHANGE.  Line 40 names a `.data` section THIS .s DOES NOT
 * HAVE and never had -- it is an empty input section today and stays empty
 * after elevation, exactly as for the six already-elevated stems in the same
 * two lists.  Landing is: add this file at
 * src/overlays/rom_7c37ac/ovl_30_c_c_c_c_c_c_c_a_a.c and `git rm`
 * asm/overlays/rom_7c37ac/ovl_30_c_c_c_c_c_c_c_a_a.s, which is how every
 * elevated sibling in this directory already sits (`git ls-files asm/overlays/
 * rom_7c37ac` lists 14 .s files; the two with a .c in src/ are absent).
 *
 * Harness: scratch_elev/b240/f2008360/ -- extract.py (ref.s -> calls.txt, and
 * note that a `b .LX / .pool_aligned / .LX:` triple is a POOL DUMP, not control
 * flow; TWO of the four here sit BETWEEN a call's argument movs and its `bl`,
 * so treating them as basic-block ends silently drops arguments), mkbody.py,
 * gen.py, sweep.py, minimise.py, classify.py, variants.py, diffcand.py.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char ActorCmd_ARRAY_938__02009b94[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_SetBehavior(int slot, void *script);
extern void __MapActor_RunScript(int slot, void *script);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_800fe9c(void);
extern void __Func_8091200(int a, int b);
extern void __Func_8091220(int a, int b);
extern void __Func_8091254(int a);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8093054(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern int __Func_8091c7c(int a, int b);
extern void OvlFunc_938_200940c(int n);
extern void OvlFunc_938_2009450(int n);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_938_2008360(void)
{
    int v;

    __CutsceneStart();
    { PIN4; q0 = -0x1; q1 = -0x1; q2 = -0x1; q3 = 0x0;
      __Func_80933f8(q0, q1, q2, q3); }
    __WaitFrames(0x1);
    { PIN2; q0 = 0x10002; q1 = 0x0;
      __Func_8091220(q0, q1); }
    __Func_8091200(0x10002, 0x0);
    __Func_8091254(0x1);
    __WaitFrames(0x1);
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x18;
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x201;
    { PIN3; q0 = 0x8; q1 = 0xd6 << 18; q2 = 0xdc << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x0; q1 = 0xd6 << 18; q2 = 0xf3 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xd4 << 18; q2 = 0xfb << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xda << 18; q2 = 0xf3 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0xdc << 18; q2 = 0xfb << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xd2 << 18; q2 = 0x2060000;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xde << 18; q2 = 0x2060000;
      __MapActor_SetPos(q0, q1, q2); }
    __Func_80933f8(0xd8 << 18, -0x1, 0xec << 17, 0x0);
    __Func_800fe9c();
    __WaitFrames(0x1);
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x28);
    __Func_80925cc(0x8, 0x1);
    __MapActor_DoAnim(0x8, 0x3);
    __CutsceneWait(0xa);
    __Func_80925cc(0x2, 0x1);
    __MapActor_DoAnim(0x2, 0x4);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x2; q1 = 0x80 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x0, 0x2);
    __Func_8092adc(0x0, 0x0, 0xa);
    __MapActor_DoAnim(0x0, 0x3);
    __Func_809259c(0x1, 0x1);
    __Func_80925cc(0x3, 0x1);
    { PIN3; q0 = 0x1; q1 = 0xe0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0xa0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0x1, 0x3);
    __MapActor_DoAnim(0x3, 0x3);
    __CutsceneWait(0x14);
    OvlFunc_938_2009450(0x14);
    { PIN2; q0 = 0x8; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __Func_809259c(0x8, 0x2);
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0x8; q1 = 0xa0 << 7; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x8, 0x3);
    { PIN3; q0 = 0xa; q1 = 0x80 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    __Func_809259c(0xa, 0x2);
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0xa; q1 = 0xf0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0xb, 0x2);
    { PIN3; q0 = 0xb; q1 = 0x90 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xd0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xb0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0xb, 0x3);
    __CutsceneWait(0xa);
    __Func_80925cc(0x8, 0x2);
    { PIN3; q0 = 0x8; q1 = 0xc0 << 6; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0x105; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(0xb, 0x2);
    __MapActor_DoAnim(0xb, 0x4);
    __MapActor_DoAnim(0xb, 0x4);
    __Func_8091200(0x80 << 9, 0x0);
    __Func_8091254(0x28);
    __WaitFrames(0x3c);
    { PIN3; q0 = 0x8; q1 = 0x105; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __MessageID(0x1b21);
    __Func_8093040(0x8, 0x0, 0xa);
    __Func_80925cc(0xa, 0x2);
    __Func_8093040(0xa, 0x0, 0xa);
    __MapActor_DoAnim(0xb, 0x4);
    __Func_8093040(0xb, 0x0, 0xa);
    { PIN3; q0 = 0x2; q1 = 0xc0 << 7; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x6002; q1 = 0x0; q2 = 0xa;
      __Func_8093040(q0, q1, q2); }
    __Func_80925cc(0xb, 0x2);
    __Func_8093040(0xb, 0x0, 0xa);
    __MapActor_SetAnim(0xa, 0x4);
    __Func_8093040(0xa, 0x0, 0xa);
    { PIN3; q0 = 0x2; q1 = 0xc0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2002; q1 = 0x0; q2 = 0xa;
      __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0xa);
    __Func_80925cc(0xb, 0x2);
    __Func_8093040(0xb, 0x0, 0xa);
    { PIN3; q0 = 0x2; q1 = 0x80 << 6; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0xa, 0x3);
    __Func_8093040(0xa, 0x0, 0xa);
    { PIN3; q0 = 0x2; q1 = 0xc0 << 7; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_809259c(0x2, 0x1);
    { PIN3; q0 = 0x6002; q1 = 0x0; q2 = 0xa;
      __Func_8093040(q0, q1, q2); }
    __MapActor_DoAnim(0xa, 0x3);
    { PIN3; q0 = 0x2; q1 = 0x80 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    { PIN2; q1 = 0x0; q0 = 0x6002;
      __Func_8093054(q0, q1); }
    { PIN2; q0 = 0x8; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x3c);
    __Func_8093040(0x8, 0x0, 0xa);
    __Func_80925cc(0x2, 0x2);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x2; q1 = 0xc0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x2, 0x3);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x1, 0x3);
    __Func_8093040(0x1, 0x0, 0xa);
    { PIN3; q0 = 0x8; q1 = 0x105; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_DoAnim(0x3, 0x4);
    __Func_8093040(0x3, 0x0, 0x28);
    { PIN3; q0 = 0x8; q1 = 0x80 << 5; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0x6666; q2 = 0x3333;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0xdf << 2; q2 = 0xdc << 1;
      __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(0x28);
    { PIN3; q0 = 0x8; q1 = 0xd0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0x105; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    OvlFunc_938_200940c(0x3c);
    OvlFunc_938_2009450(0x28);
    __Func_80921c4(0x8, 0xd6 << 2, 0xdc << 1);
    __CutsceneWait(0x28);
    { PIN3; q0 = 0x8; q1 = 0x90 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0x105; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xf0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x90 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xd0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xb0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x8, 0x1);
    __Func_8093040(0x8, 0x0, 0x14);
    __MapActor_DoAnim(0x8, 0x4);
    __Func_8093040(0x8, 0x0, 0x14);
    __Func_80925cc(0xa, 0x2);
    __Func_8093040(0xa, 0x0, 0xa);
    __Func_80925cc(0x8, 0x2);
    { PIN3; q0 = 0x8; q1 = 0xa0 << 7; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x0; q1 = 0x80 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x80 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xc0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0xc0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x90 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xf0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0xa, 0x0, 0x14);
    __Func_80925cc(0xb, 0x2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xb; q1 = 0xb0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0xb, 0x3);
    __Func_8093040(0xb, 0x0, 0xa);
    { PIN3; q0 = 0xb; q1 = 0x90 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0xa, 0x3);
    __MapActor_DoAnim(0xb, 0x3);
    { PIN3; q0 = 0xa; q1 = 0xd0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xb0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0xa, 0x2);
    __Func_8093040(0xa, 0x0, 0x14);
    OvlFunc_938_2009450(0x14);
    __Func_80925cc(0x8, 0x2);
    __CutsceneWait(0x28);
    __Func_8093040(0x8, 0x0, 0xa);
    { PIN2; q0 = 0x8; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x3c);
    __Func_8093040(0x8, 0x0, 0x14);
    __MapActor_DoAnim(0x8, 0x4);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0x8, 0x4);
    __Func_8093040(0x8, 0x0, 0x14);
    __Func_80925cc(0x1, 0x2);
    { PIN3; q0 = 0x1; q1 = 0xe0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN2; q1 = 0x0; q0 = 0x1;
      __Func_8092c40(q0, q1); }
    { PIN3; q0 = 0x0; q1 = 0xc0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    v = 1;
    if (__Func_8091c7c(0, 0) == 1) {
        (*(unsigned short *)(iwram_3001ebc + (0xec << 1)))++;
        v = 0;
    }
    __Func_8093040(0x1, 0x0, 0xa);
    if (v != 0) {
        (*(unsigned short *)(iwram_3001ebc + (0xec << 1)))++;
    }
    { PIN3; q0 = 0x2; q1 = 0x80 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_Surprise(0x2, 0x81 << 1);
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0x8; q1 = 0x101; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0xc0 << 6; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0xa0 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0xc0 << 6; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0xa);
    __Func_80925cc(0x2, 0x1);
    { PIN3; q0 = 0x2; q1 = 0xc0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x0; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xc0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x2002, 0x0, 0xa);
    { PIN3; q0 = 0x8; q1 = 0x107; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0xa);
    __MapActor_DoAnim(0x8, 0x4);
    { PIN2; q1 = 0x0; q0 = 0x8;
      __Func_8092c40(q0, q1); }
    { PIN3; q0 = 0x2; q1 = 0x80 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xe0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0xa0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x14);
        __MapActor_DoAnim(0x8, 0x3);
        __Func_8093040(0x8, 0x0, 0xa);
        (*(unsigned short *)(iwram_3001ebc + (0xec << 1)))++;
    } else {
        __CutsceneWait(0x14);
        __MapActor_DoAnim(0x8, 0x4);
        (*(unsigned short *)(iwram_3001ebc + (0xec << 1)))++;
        __Func_8093040(0x8, 0x0, 0xa);
    }
    OvlFunc_938_2009450(0x14);
    { PIN3; q0 = 0x2; q1 = 0x80 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0xa0 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0xc0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x3, 0x0, 0x14);
    { PIN3; q0 = 0x8; q1 = 0x105; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0x28);
    __Func_80925cc(0x8, 0x2);
    __Func_8093040(0x8, 0x0, 0x28);
    __Func_80925cc(0x2, 0x1);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x2; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xd9 << 2; q2 = 0xec << 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x2002; q1 = 0x0; q2 = 0xa;
      __Func_8093040(q0, q1, q2); }
    __MapActor_DoAnim(0x8, 0x4);
    __Func_8093040(0x8, 0x0, 0xa);
    OvlFunc_938_200940c(0x28);
    OvlFunc_938_2009450(0x14);
    { PIN3; q0 = 0x8; q1 = 0x105; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0xa);
    __Func_80925cc(0x1, 0x2);
    __Func_8093040(0x1, 0x0, 0xa);
    __Func_80925cc(0x8, 0x1);
    { PIN3; q0 = 0x8; q1 = 0xa0 << 7; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0xa);
    __MapActor_DoAnim(0x8, 0x3);
    { PIN3; q0 = 0x8; q1 = 0xc0 << 6; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0xa);
    { PIN2; q0 = 0x0; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 0x1; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 0x2; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 0x3; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x28);
    __Func_8093040(0x2002, 0x0, 0xa);
    __MapActor_DoAnim(0x8, 0x3);
    { PIN2; q1 = 0x0; q0 = 0x8;
      __Func_8092c40(q0, q1); }
    __Func_809259c(0x1, 0x1);
    __Func_809259c(0x2, 0x1);
    __Func_80925cc(0x3, 0x1);
    { PIN3; q0 = 0x1; q1 = 0xe0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xc0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0xa0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x14);
        __MapActor_Surprise(0x8, 0x81 << 1);
        __CutsceneWait(0x28);
        __Func_8093040(0x8, 0x0, 0xa);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 2;
    } else {
        __CutsceneWait(0x14);
        __Func_80925cc(0x1, 0x2);
        (*(unsigned short *)(iwram_3001ebc + (0xec << 1)))++;
        __Func_8093040(0x1, 0x0, 0xa);
        __MapActor_Surprise(0x8, 0x81 << 1);
        __CutsceneWait(0x28);
        __Func_8093040(0x8, 0x0, 0xa);
    }
    { PIN3; q0 = 0x3; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0xc0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __ActorMessage(0x3, 0x0);
    { PIN3; q0 = 0x1; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xc0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x8, 0x3);
    __Func_8093040(0x8, 0x0, 0xa);
    { PIN2; q0 = 0x0; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 0x1; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 0x2; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 0x3; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x28);
    __Func_8093040(0x8, 0x0, 0xa);
    { PIN3; q0 = 0x0; q1 = 0xe0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xe0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0xa0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x2, 0x4);
    { PIN3; q0 = 0x2002; q1 = 0x0; q2 = 0xa;
      __Func_8093040(q0, q1, q2); }
    __MapActor_DoAnim(0x8, 0x3);
    __Func_8093040(0x8, 0x0, 0xa);
    __Func_80925cc(0x8, 0x1);
    __Func_8093040(0x8, 0x0, 0xa);
    OvlFunc_938_2009450(0xa);
    __MapActor_DoAnim(0x8, 0x4);
    __Func_8093040(0x8, 0x0, 0x14);
    __MapActor_SetAnim(0x0, 0x4);
    __MapActor_SetAnim(0x1, 0x4);
    __MapActor_SetAnim(0x2, 0x4);
    __MapActor_DoAnim(0x3, 0x4);
    { PIN2; q0 = 0x8; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x3c);
    __Func_8093040(0x8, 0x0, 0xa);
    OvlFunc_938_200940c(0x28);
    __MapActor_DoAnim(0x8, 0x4);
    __Func_8093040(0x8, 0x0, 0xa);
    OvlFunc_938_2009450(0x14);
    __Func_80925cc(0x2, 0x2);
    { PIN3; q0 = 0x2002; q1 = 0x0; q2 = 0xa;
      __Func_8093040(q0, q1, q2); }
    __MapActor_DoAnim(0x8, 0x3);
    __Func_8093040(0x8, 0x0, 0xa);
    { PIN3; q0 = 0x3; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x3, 0x0, 0xa);
    { PIN3; q0 = 0x8; q1 = 0x84 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0xa);
    __MapActor_DoAnim(0x3, 0x3);
    __Func_8093040(0x3, 0x0, 0xa);
    __MapActor_DoAnim(0x8, 0x3);
    __Func_8093040(0x8, 0x0, 0xa);
    { PIN3; q0 = 0x1; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x1, 0x0, 0xa);
    { PIN3; q0 = 0x8; q1 = 0xa0 << 7; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x8, 0x3);
    __Func_8093040(0x8, 0x0, 0xa);
    __Func_80925cc(0xa, 0x2);
    __MapActor_DoAnim(0xa, 0x4);
    __Func_8093040(0xa, 0x0, 0xa);
    { PIN3; q0 = 0x8; q1 = 0x105; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0xa);
    { PIN3; q0 = 0x8; q1 = 0xc0 << 6; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0x8, 0x4);
    __Func_8093040(0x8, 0x0, 0xa);
    __Func_80925cc(0x8, 0x1);
    { PIN3; q0 = 0x8; q1 = 0xa0 << 7; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0xa);
    __Func_80925cc(0x2, 0x1);
    { PIN3; q0 = 0x2002; q1 = 0x0; q2 = 0xa;
      __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0xc0 << 6; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0xa);
    OvlFunc_938_200940c(0x28);
    OvlFunc_938_2009450(0x14);
    __Func_80925cc(0x1, 0x1);
    __Func_8093040(0x1, 0x0, 0xa);
    __MapActor_DoAnim(0x8, 0x3);
    __Func_8093040(0x8, 0x0, 0xa);
    __MapActor_SetAnim(0x8, 0x4);
    __Func_8093040(0x8, 0x0, 0xa);
    __MapActor_SetAnim(0x0, 0x3);
    __MapActor_SetAnim(0x1, 0x3);
    __MapActor_SetAnim(0x2, 0x3);
    __MapActor_DoAnim(0x3, 0x3);
    __Func_80925cc(0x8, 0x1);
    __Func_8093040(0x8, 0x0, 0xa);
    { PIN2; q0 = 0x0; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __MapActor_Surprise(0x1, 0x81 << 1);
    __CutsceneWait(0x3c);
    __MapActor_DoAnim(0x8, 0x4);
    __Func_8093040(0x8, 0x0, 0xa);
    __Func_809259c(0x2, 0x1);
    __Func_80925cc(0x3, 0x1);
    __CutsceneWait(0x14);
    __Func_80925cc(0x8, 0x1);
    __Func_8093040(0x8, 0x0, 0xa);
    __MapActor_SetAnim(0x8, 0x3);
    __Func_8093040(0x8, 0x0, 0xa);
    { PIN3; q0 = 0x2; q1 = 0x105; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x2002; q1 = 0x0; q2 = 0xa;
      __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0xa0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x3, 0x0, 0xa);
    __MapActor_DoAnim(0x2, 0x4);
    __MapActor_DoAnim(0x2, 0x4);
    { PIN3; q0 = 0x2; q1 = 0x80 << 6; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0x2, 0x4);
    { PIN3; q0 = 0x2002; q1 = 0x0; q2 = 0xa;
      __Func_8093040(q0, q1, q2); }
    __MapActor_DoAnim(0x8, 0x4);
    __Func_8093040(0x8, 0x0, 0xa);
    { PIN3; q0 = 0x2; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x3, 0x2);
    { PIN3; q0 = 0x3; q1 = 0xc0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x3, 0x0, 0xa);
    __MapActor_DoAnim(0x8, 0x4);
    __Func_8093040(0x8, 0x0, 0xa);
    __Func_809259c(0xa, 0x2);
    __Func_8093040(0xa, 0x0, 0xa);
    __MapActor_DoAnim(0xb, 0x3);
    __Func_8093040(0xb, 0x0, 0x14);
    __MapActor_DoAnim(0x8, 0x3);
    __Func_8093040(0x8, 0x0, 0xa);
    __MapActor_DoAnim(0x2, 0x4);
    __MapActor_SetAnim(0x2, 0x4);
    __Func_8093040(0x2002, 0x0, 0xa);
    __MapActor_DoAnim(0x1, 0x3);
    __Func_8093040(0x1, 0x0, 0xa);
    __Func_80925cc(0x8, 0x1);
    { PIN2; q1 = 0x0; q0 = 0x8;
      __Func_8092c40(q0, q1); }
    { PIN3; q0 = 0x1; q1 = 0xe0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xc0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0xa0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x14);
        OvlFunc_938_2009450(0xa);
        __MapActor_SetAnim(0x1, 0x3);
        __MapActor_SetAnim(0x2, 0x3);
        __MapActor_DoAnim(0x3, 0x3);
        __CutsceneWait(0xa);
        __Func_8093040(0x1, 0x0, 0xa);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 2;
    } else {
        __CutsceneWait(0xa);
        __Func_80925cc(0x1, 0x2);
        (*(unsigned short *)(iwram_3001ebc + (0xec << 1)))++;
        __Func_8093040(0x1, 0x0, 0xa);
        OvlFunc_938_2009450(0xa);
        __MapActor_SetAnim(0x1, 0x3);
        __MapActor_SetAnim(0x2, 0x3);
        __MapActor_DoAnim(0x3, 0x3);
        __CutsceneWait(0xa);
        __Func_8093040(0x1, 0x0, 0xa);
    }
    __MapActor_DoAnim(0x8, 0x3);
    __Func_8093040(0x8, 0x0, 0x28);
    { PIN3; q0 = 0x8; q1 = 0xa0 << 7; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x8, 0x1);
    __CutsceneWait(0xa);
    __Func_80925cc(0xa, 0x1);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x8; q1 = 0xc0 << 6; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x8, 0x1);
    __CutsceneWait(0xa);
    __Func_80925cc(0xb, 0x1);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x8, 0x3);
    __Func_8093040(0x8, 0x0, 0xa);
    __MapActor_SetAnim(0xa, 0x3);
    __MapActor_DoAnim(0xb, 0x3);
    { PIN3; q0 = 0xa; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xd4 << 2; q2 = 0x87 << 2;
      __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xdc << 2; q2 = 0x87 << 2;
      __Func_80921c4(q0, q1, q2); }
    __MapActor_SetPos(0xa, 0x0, 0x0);
    __MapActor_SetPos(0xb, 0x0, 0x0);
    __MapActor_DoAnim(0x8, 0x3);
    __MapActor_SetAnim(0x0, 0x3);
    __MapActor_SetAnim(0x1, 0x3);
    __MapActor_SetAnim(0x2, 0x3);
    __MapActor_DoAnim(0x3, 0x3);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x1; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetBehavior(0x1, ActorCmd_ARRAY_938__02009b94);
    __MapActor_SetBehavior(0x2, ActorCmd_ARRAY_938__02009b94);
    __MapActor_RunScript(0x3, ActorCmd_ARRAY_938__02009b94);
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x10;
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x209;
    __ClearFlag(0x12f);
    __SetFlag(0x912);
    __CutsceneEnd();
}
