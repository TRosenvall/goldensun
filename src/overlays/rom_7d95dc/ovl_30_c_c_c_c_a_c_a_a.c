/* ovl_30_c_c_c_c_a_c_a_a.c  --  OvlFunc_953_2009cd4  --  0x02009cd4
 *   [asm/overlays/rom_7d95dc/ovl_30_c_c_c_c_a_c_a_a.s, the ONLY function in
 *    that .s -- one `.thumb_func_start`, no `.section`/`.data`/`.rodata`/
 *    `.bss` line anywhere in the file, no split needed and none proposed]
 *
 * 697 instructions of straight-line cutscene script: 180 calls across 33 shared
 * symbols, ONE `__Func_8091c7c` diamond with a flag re-tested after it, two
 * `iwram_3001ebc[0xec << 1]` counter bumps, two whole-word store blocks and ONE
 * mid-function `.pool_aligned` dump.  VERDICT:
 *
 *   OK OvlFunc_953_2009cd4 -- 1804 bytes, 710 encodings and 183 relocations identical
 *
 * ...against BOTH the scratch reference and the REAL asm/ path.
 *
 * NO FLAG GROUP IS INVOLVED, AND THAT IS A POSITIVE FINDING.  `grep -n
 * rom_7d95dc Makefile` returns exactly TWO lines, 837 and 4654, and both are
 * explicit `CSE_CFLAGS` rules for OTHER stems (`ovl_30_c_c_c_a_a_a_c_a_c_c`
 * and `ovl_30_c_c_c_a_a_a_c_a_a_a`).  No wildcard reaches this directory, so
 * the generic cross-dir `asm/%.o: src/%.c` at Makefile:146 applies and the TU
 * takes the tree default -O2.  objcmp printed no "(built with: ...)" line when
 * screened through the real asm/ path, which is the check that catches an
 * O1-wildcard trap.  NOTHING TO ADD TO THE MAKEFILE.
 *
 * THE HIGH-REGISTER SCREEN READ 15 REFERENCES OVER THREE REGISTERS AND THAT WAS
 * NOT A REJECT.  r8 holds 0xc0 << 6, r10 holds 0xa0 << 8 and r9 holds the
 * ADDRESS of `iwram_3001ebc`; r5, r6 and r7 hold six more script constants
 * between them.  Every one of those is gcc's own constant CSE, reached with no
 * named local at all -- the recorded "a value in a callee-saved register is NOT
 * evidence the source named it" holds at six registers at once here.
 *
 * THE PROLOGUE SAYS "PIN", BY CONTENT AND BY WIDTH BOTH.  The ROM pushes
 * {r5, r6, r7, lr} plus {r5, r6, r7} for r8/r9/r10 -- SIX callee-saved
 * registers.  Plain C opens the same b5e0 and then wants a SEVENTH: it commons
 * 0x88 << 16 and the -1 of `__Func_80933f8` as well, spends r11, and emits an
 * extra `mov r7, r8 / push {r7}`.  Pinning EVERY site goes the other way and
 * commons nothing at all, opening b560 -- push {r5, r6, lr}.  Both ends are
 * about equally far off (632 and 641 of 710), and the answer is neither.
 *
 * THE PIN SET IS DEFINED BY ITS HOLES, AND THE HOLES ARE READABLE STRAIGHT OFF
 * THE REFERENCE (NEW).  Recorded: "a pin set can need a hole, and the hole is
 * where the ROM itself commons", found there by trial over a REGION.  It does
 * not have to be a search.  Wherever the ROM supplies an argument with
 * `mov rLOW, rHIGH` out of a callee-saved register, gcc has commoned that value
 * and a pin would rematerialise it; every other site is fair game.  `holes.py`
 * walks ref.s with a symbolic register file and prints the set -- here 36 sites
 * over six registers (r5 x11, r6 x8, r7 x10, r8 x3, r10 x2, plus the gScript
 * trio).  "every site with an argument, MINUS those 36" is 138 pins and lands
 * at 130 differing with the first 472 of 710 encodings already exact, from a
 * standing start.  The four measured re-pins of hole runs:
 *
 *   RE-PIN all 36 holes            641 differing   (identical to pinning all)
 *   RE-PIN the r8 run 21,39,83     439 differing
 *   RE-PIN the r7 run 78,80,81,82  397 differing
 *   RE-PIN the gScript trio        113 differing
 *
 * The gScript trio 159/160/161 is the b240 template's ActorCmd hole exactly --
 * `SetBehavior(1, s); SetBehavior(2, s); RunScript(3, s)` off one `ldr r5, =`
 * -- and it needs no special-casing here because the mechanical rule already
 * carves it out.
 *
 * A PIN CONSTRAINS ORDER; IT DOES NOT BY ITSELF DEFEAT CSE (NEW, and it is the
 * step that closed this function).  The last residue was five
 * `__MapActor_SetSpeed(slot, 0x80 << 9, 0x80 << 8)` sites, 156/157/158/166/167.
 * The ROM commons 0x80 << 8 into r6 and REBUILDS 0x80 << 9 fresh at each site;
 * unpinned, gcc commons 0x80 << 9 into r6 and puts 0x80 << 8 in r8 -- the two
 * values swap roles and the tail loses two instructions.  These are hole sites
 * by the r6 test, so the obvious move is to leave them bare, and that is what
 * stalls at 130.  PINNING THEM ANYWAY IS THE FIX: q2 = 0x80 << 8 is still
 * served by `adds r2, r6, #0`, because the pin asks for the value in r2 at that
 * point and gcc's CSE is free to answer from r6.  What the pin removes is only
 * the FREEDOM TO ORDER, and that is enough to put 0x80 << 9 back in front.  A
 * `GEN_PARTIAL=156:2 ...` build that pins q0/q1 and leaves q2 a plain
 * expression ALSO matches, which is the control: the q2 pin is inert, the
 * q0/q1 pins are the load-bearing half.  So the HOLE TEST NOMINATES, IT DOES
 * NOT DECIDE -- a hole site whose OTHER arguments are rebuilt still wants a
 * pin.
 *
 * 143 PIN CANDIDATES, 53 REQUIRED.  Greedy stripping under objcmp, re-testing
 * after every drop, run to a fixpoint from BOTH ends: forward and reverse
 * converge on the SAME 53, so the answer is unique here.  The 90 that fall are
 * inert scaffolding and DO NOT SHIP.  Compare the b240 template's 138 of 149:
 * this function keeps a far smaller fraction, because 36 of its sites are holes
 * and a further large block is all-cheap.
 *
 * THE FLAG'S POLARITY IS THE OPPOSITE OF THE TEMPLATE'S, AND IT IS WORTH 20.
 * b240 hoists `v = 1` above the `bl` and CLEARS it in the arm.  This reference
 * does the reverse -- `mov r5, #0` sits in the argument window of
 * `bl __Func_8091c7c` and `mov r5, #1` is the last instruction of the taken arm
 * -- so the source is `v = 0;` hoisted, `v = 1;` inside.  Spelling it the
 * template's way costs 20 differing with relocations moving.  Two neighbouring
 * spellings are much worse: writing the bump into both arms and dropping the
 * flag costs 253, and `int v = 0;` at the DECLARATION costs 633, because the
 * initialiser sinks to function entry and rearranges the whole prologue.
 * Binding v to r5 by hand is unnecessary; gcc picks r5 itself.
 *
 * `__Func_8092c40` WANTS THE DESCENDING FILL, ONCE, AND THE TELL IS BINARY.
 * Site 117, `__Func_8092c40(2, 0)`, is the only call to it and both arguments
 * are bare `mov #imm8`, so no nomination rule reaches it -- it is in the set
 * only because the recorded by-name list puts it there.  `q1 = 0; q0 = 2;`
 * matches; the ASCENDING fill measures 2 differing and DROPPING THE PIN
 * ALTOGETHER measures the same 2, exactly the recorded binary tell.  Ascending
 * here is a non-pin.  No prototype had to be withheld: one descending site does
 * not justify losing the type check, and the pin does the same job.
 *
 * THE TWO STORE BLOCKS ARE SOURCE ORDER, AND IT IS 0xe0 FIRST.  Both blocks
 * write `*(int *)(iwram + (0xe0 << 1))` before `*(int *)(iwram + (0xe4 << 1))`;
 * the ROM chains the second offset off the first VALUE (`add r3, #0x43` to make
 * 0x203, then `sub r3, #0x3b` to get back to 0x1c8).  The b240 template's
 * blocks read 0xe4-then-0xe0, so this is not transferable -- writing them that
 * way costs 648 differing and TWO EXTRA INSTRUCTIONS, since the offset chain
 * breaks and both offsets have to be built.
 *
 * WHERE THE LENGTH TELL POINTED, AND WHERE IT LIED.  Unpinned the function is
 * 24 bytes SHORT, over-pinned it is 8 bytes LONG, and the 138-pin base is 4
 * bytes short -- two instructions missing at the very end of the tail, with
 * every `ldr [pc, #K]` in the function displaced by 4 as a consequence.  That
 * displacement is what makes the raw first-difference index useless as a
 * search score: it stops 90 indices EARLY, on a pool load that is not the
 * defect.  `grow.py` normalises pool loads to their destination register for
 * SCORING ONLY -- objcmp still gates the verdict on true encodings -- and the
 * first difference then lands on the real one.
 *
 * THE RELOCATION LINE SORTS THE 53 PINS PERFECTLY, AND THE RECORDED GAP HOLDS.
 * Dropping each required pin one at a time:
 *
 *   16 ORDERING pins   RELOCATIONS silent, SIZE silent on 16 of 16,
 *                      encoding count EXACTLY 2 or 3, never more
 *   37 CSE pins        RELOCATIONS differ on 37 of 37,
 *                      encoding counts 12 to 658, SIZE silent on 14 of 37
 *
 * Zero overlap, and the 4-to-9 band is empty again -- the recorded refinement
 * that a residue of 4-9 encodings belongs to neither band now has a second
 * independent sample.  The CSE floor here is 12 against the recorded 10.
 *
 * MEASURED-WORSE TABLE (one container run each, scratch ref unless noted):
 *
 *   final 53 pins, scratch ref                     OK
 *   final 53 pins, REAL asm/ path                  OK
 *   all 143 pin candidates (90 inert)              OK
 *   whole-value consts (no mov/lsl)                OK  -- both spellings tie
 *   pin q0/q1 only at the five SetSpeed sites      OK  -- the q2 pin is inert
 *   no pins at all                             632 differing, SIZE -24, RELOC
 *   every site with an argument pinned          641 differing, SIZE +8, RELOC
 *   RE-PIN every one of the 36 holes            641 differing, SIZE, RELOC
 *   RE-PIN the r8 hole run 21,39,83             439 differing, SIZE, RELOC
 *   RE-PIN the r7 hole run 78,80,81,82          397 differing, SIZE, RELOC
 *   RE-PIN the gScript trio 159,160,161         113 differing, SIZE, RELOC
 *   store blocks in the other order             648 differing, SIZE, RELOC
 *   `int v = 0;` at the declaration             633 differing, SIZE, RELOC
 *   0x105: pin only the 1st use (40)            583 differing, SIZE, RELOC
 *   no flag v: bump duplicated into both arms   253 differing, SIZE, RELOC
 *   0xc0<<8: pin only the 1st use (78)          184 differing, SIZE, RELOC
 *   descending fill EVERYWHERE                  110 differing
 *   ROM's emitted arg order, not ascending       72 differing
 *   template polarity: hoist v=1, clear in else  20 differing, RELOC
 *   ASCENDING at 117 (__Func_8092c40)             2 differing
 *   drop pin 117 entirely                         2 differing
 *
 * The whole-value row is worth keeping: with the pins in place `0xc0 << 8` and
 * `0xc000` produce the SAME OBJECT.  The mov/lsl spelling is kept because it is
 * what the reference reads as.
 *
 * LANDING.  EXACTLY ONE linker line names this .o on its FULL PATH:
 *
 *   overlays/rom_7d95dc/overlay.ld:42
 *       asm/overlays/rom_7d95dc/ovl_30_c_c_c_c_a_c_a_a.o(.text)
 *
 * IT DOES NOT CHANGE.  Makefile:146's generic `asm/%.o: src/%.c` produces that
 * exact .o from the new src/ .c, which is how 30 of the 31 elevated stems in
 * this directory already sit.  (The one exception, `ovl_30_c_c_c_a_a_c_c`, is
 * listed as `src/...o(.text)` at overlay.ld:38 and builds in place through
 * Makefile:135; do not copy it.)
 *
 * NO `.data`, `.rodata` OR `.bss` LINE NAMES THIS .o ANYWHERE.  The .ld's
 * `.data` block at overlay.ld:47 lists only `ovl_30_c_c_c_c_c.o(.data)`.  The
 * zero-length `.data`/`.bss`/`.ARM.attributes` records at overlay.map:108-111
 * are LINKER MAP output for input sections swept by `/DISCARD/ : { *(*) }`;
 * every .o in the directory has them, elevated or not, and `*.map` is
 * gitignored.  Nothing to remap.
 *
 * WATCH THE BASENAME.  `overlays/rom_79dd90/overlay.ld:30` names
 * `asm/overlays/rom_79dd90/ovl_30_c_c_c_c_a_c_a_a.o(.text)` -- the SAME
 * BASENAME in a DIFFERENT overlay.  Matched on full path it is not ours and
 * must not be touched; this is the recorded "match linker-script references on
 * the full path, never the basename".
 *
 * The generated `.s` at the asm/ path is STAGED, NOT DELETED, per the recorded
 * "the generated `.s` beside the `.c` IS tracked -- commit it": 3799 of the
 * 4339 tracked `.c` files under src/ in this tree have a tracked sibling `.s` bearing
 * gcc's banner, and all 30 elevated stems in this directory do.
 *
 * Harness: scratch_elev/b243/f2009cd4/ -- extract.py (GLOBAL symbolic register
 * file plus a per-callee arity table; a per-call reset loses every high-register
 * constant, and counting written registers makes a 2-arg call look 4-wide when
 * r3 was the scratch used to build the r8 value), holes.py, mkbody.py, gen.py,
 * grow.py, look.py, tail.py, sweep.py, minimise.py, classify.py, variants.py,
 * diffcand.py, verify.py.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char gScript_953__0200adac[];

extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __MessageID(int id);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetSpeed(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_RunScript(int slot, unsigned char *s);
extern void __Func_8091200(int a, int b);
extern void __Func_8091220(int a, int b);
extern void __Func_8091254(int a);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8091e9c(int a);
extern void __Func_8092158(int slot, int x, int z);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void OvlFunc_953_2009c48(int slot);
extern void OvlFunc_953_2009c5c(int slot, int v);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_953_2009cd4(void)
{
    int v;

    __CutsceneStart();
    { PIN3; q0 = 0x1; q1 = 0xc6 << 18; q2 = 0x88 << 16;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xce << 18; q2 = 0x88 << 16;
      __MapActor_SetPos(q0, q1, q2); }
    __MapActor_SetPos(0x3, 0xca << 18, 0x98 << 16);
    __WaitFrames(0x1);
    { PIN4; q0 = -0x1; q1 = -0x1; q2 = -0x1; q3 = 0x0;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8091220(0x0, 0x0);
    __Func_8091200(0x0, 0x0);
    __Func_8091254(0x1);
    __WaitFrames(0x1);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x203;
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x1;
    __MapTransitionIn();
    __WaitMapTransition();
    __Func_8091220(0x0, 0x0);
    __Func_8091200(0x10002, 0x0);
    __Func_8091254(0x28);
    __CutsceneWait(0x50);
    __Func_80925cc(0x8, 0x1);
    __CutsceneWait(0x14);
    __Func_80925cc(0x2, 0x2);
    __CutsceneWait(0x28);
    OvlFunc_953_2009c5c(0x8, 0xc0 << 6);
    __Func_80925cc(0x8, 0x2);
    __CutsceneWait(0x14);
    __Func_8091200(0x80 << 9, 0x0);
    __Func_8091254(0x28);
    __CutsceneWait(0x50);
    { PIN3; q0 = 0x2; q1 = 0x80 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(0x2, 0x1);
    __CutsceneWait(0x14);
    __MessageID(0x20f8);
    OvlFunc_953_2009c48(0x2);
    __MapActor_DoAnim(0x8, 0x3);
    OvlFunc_953_2009c48(0x8);
    { PIN3; q0 = 0x3; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    OvlFunc_953_2009c48(0x3);
    { PIN3; q0 = 0x8; q1 = 0x80 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x9, 0x0, 0x28);
    { PIN3; q0 = 0x8; q1 = 0xa0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    OvlFunc_953_2009c5c(0x9, 0xc0 << 6);
    { PIN3; q0 = 0x9; q1 = 0x105; q2 = 0x14;
      __MapActor_Emote(q0, q1, q2); }
    OvlFunc_953_2009c48(0x9);
    { PIN3; q0 = 0x1; q1 = 0x103; q2 = 0x14;
      __MapActor_Emote(q0, q1, q2); }
    __Func_809259c(0x1, 0x2);
    OvlFunc_953_2009c48(0x1);
    { PIN3; q0 = 0xa; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    OvlFunc_953_2009c48(0xa);
    __MapActor_DoAnim(0xb, 0x3);
    OvlFunc_953_2009c48(0xb);
    OvlFunc_953_2009c5c(0x2, 0xa0 << 8);
    __MapActor_DoAnim(0x2, 0x4);
    OvlFunc_953_2009c48(0x2);
    __Func_80925cc(0x3, 0x1);
    OvlFunc_953_2009c48(0x3);
    { PIN3; q0 = 0x1; q1 = 0x103; q2 = 0x14;
      __MapActor_Emote(q0, q1, q2); }
    OvlFunc_953_2009c5c(0x1, 0x0);
    __Func_809259c(0x1, 0x2);
    OvlFunc_953_2009c48(0x1);
    OvlFunc_953_2009c5c(0x0, 0xc0 << 7);
    { PIN3; q0 = 0x0; q1 = 0x81 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(0x2, 0x1);
    OvlFunc_953_2009c5c(0x2, 0x80 << 8);
    OvlFunc_953_2009c48(0x2);
    __MapActor_SetAnim(0x3, 0x4);
    __CutsceneWait(0x14);
    OvlFunc_953_2009c48(0x3);
    __Func_80925cc(0x1, 0x1);
    OvlFunc_953_2009c5c(0x1, 0x80 << 6);
    OvlFunc_953_2009c48(0x1);
    { PIN3; q0 = 0x2; q1 = 0x105; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8092adc(0x2, 0x80 << 6, 0x0);
    __Func_8092adc(0x1, 0x0, 0x0);
    { PIN3; q0 = 0x3; q1 = 0xe0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x0; q1 = 0x81 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x81 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0x81 << 1; q2 = 0x50;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x83 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(0x2, 0x1);
    OvlFunc_953_2009c5c(0x2, 0xc0 << 8);
    OvlFunc_953_2009c48(0x2);
    __Func_8092adc(0x0, 0xc0 << 8, 0x0);
    __Func_8092adc(0x1, 0xc0 << 8, 0x0);
    __Func_8092adc(0x3, 0xc0 << 8, 0x14);
    OvlFunc_953_2009c5c(0x8, 0xc0 << 6);
    __MapActor_DoAnim(0x8, 0x3);
    OvlFunc_953_2009c48(0x8);
    { PIN3; q0 = 0x0; q1 = 0x80 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x80 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(0x9, 0x1);
    OvlFunc_953_2009c48(0x9);
    __Func_8092adc(0x0, 0xc0 << 7, 0x0);
    { PIN3; q0 = 0x1; q1 = 0xe0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x2, 0xc0 << 7, 0x0);
    { PIN3; q0 = 0x3; q1 = 0xe0 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0xa, 0x1);
    OvlFunc_953_2009c48(0xa);
    __Func_8092adc(0x0, 0xc0 << 8, 0x0);
    __Func_8092adc(0x1, 0xc0 << 8, 0x0);
    __Func_8092adc(0x2, 0xc0 << 8, 0x0);
    OvlFunc_953_2009c5c(0x3, 0xc0 << 8);
    __MapActor_DoAnim(0xb, 0x3);
    OvlFunc_953_2009c48(0xb);
    __Func_8092adc(0x0, 0xc0 << 7, 0x0);
    { PIN3; q0 = 0x1; q1 = 0xe0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x2, 0xc0 << 7, 0x0);
    { PIN3; q0 = 0x3; q1 = 0xe0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x105; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x105; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0x105; q2 = 0x50;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x83 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(0x2, 0x1);
    OvlFunc_953_2009c5c(0x2, 0xe0 << 8);
    OvlFunc_953_2009c48(0x2);
    __Func_8092adc(0x0, 0x0, 0x0);
    __Func_8092adc(0x1, 0x0, 0x14);
    { PIN3; q0 = 0xb; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0x0; q0 = 0x2;
      __Func_8092c40(q0, q1); }
    __Func_8092adc(0x0, 0x80 << 6, 0x0);
    __Func_8092adc(0x1, 0xe0 << 8, 0x0);
    __Func_8092adc(0x2, 0xa0 << 8, 0x0);
    __Func_8092adc(0x3, 0xc0 << 8, 0x0);
    v = 0;
    if (__Func_8091c7c(0, 0) == 1) {
        OvlFunc_953_2009c48(0x2);
        v = 1;
    } else {
        (*(unsigned short *)(iwram_3001ebc + (0xec << 1)))++;
        __MapActor_DoAnim(0x2, 0x3);
        OvlFunc_953_2009c5c(0x2, 0xc0 << 8);
        OvlFunc_953_2009c48(0x2);
    }
    if (v != 0) {
        (*(unsigned short *)(iwram_3001ebc + (0xec << 1)))++;
    }
    { PIN3; q0 = 0x8; q1 = 0x80 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x9, 0x0, 0x0);
    { PIN3; q0 = 0xa; q1 = 0xc0 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xb0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0x105; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x9; q1 = 0x105; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x105; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x105; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(0x8, 0x1);
    OvlFunc_953_2009c5c(0x8, 0xc0 << 6);
    OvlFunc_953_2009c48(0x8);
    { PIN3; q0 = 0x0; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0xc0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x9, 0x1);
    OvlFunc_953_2009c5c(0x9, 0xc0 << 6);
    OvlFunc_953_2009c48(0x9);
    __Func_80925cc(0xa, 0x1);
    OvlFunc_953_2009c5c(0xa, 0xa0 << 7);
    OvlFunc_953_2009c48(0xa);
    OvlFunc_953_2009c5c(0xb, 0x80 << 8);
    __MapActor_DoAnim(0xb, 0x3);
    OvlFunc_953_2009c48(0xb);
    { PIN3; q0 = 0x1; q1 = 0xe0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xa0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0x1, 0x3);
    __MapActor_SetAnim(0x2, 0x3);
    __MapActor_DoAnim(0x3, 0x3);
    { PIN3; q0 = 0x1; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetBehavior(0x1, gScript_953__0200adac);
    __MapActor_SetBehavior(0x2, gScript_953__0200adac);
    __MapActor_RunScript(0x3, gScript_953__0200adac);
    __CutsceneWait(0x14);
    OvlFunc_953_2009c5c(0x0, 0x0);
    __MapActor_DoAnim(0x0, 0x3);
    __MapActor_DoAnim(0xb, 0x3);
    { PIN3; q0 = 0xb; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x0; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(0xb, 0x2);
    { PIN3; q0 = 0xb; q1 = 0x33e; q2 = 0x98;
      __Func_8092158(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xca << 2; q2 = 0xa4;
      __Func_8092158(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xca << 2; q2 = 0x9c << 1;
      __MapActor_TravelTo(q0, q1, q2); }
    __CutsceneWait(0x14);
    __Func_80933d4(0x6666, 0xccc);
    __Func_80933f8(0xca << 18, -0x1, 0x9c << 17, 0x1);
    { PIN3; q0 = 0x0; q1 = 0xca << 2; q2 = 0xa4;
      __Func_80921c4(q0, q1, q2); }
    __Func_809218c(0x0, 0xca << 2, 0x9c << 1);
    __CutsceneWait(0x3c);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x100;
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x28;
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(0x40);
}
