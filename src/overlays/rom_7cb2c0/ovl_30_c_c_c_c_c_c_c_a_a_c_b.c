// fakematch
/* ovl_30_c_c_c_c_c_c_c_a_a_c_b.c  --  OvlFunc_945_200d2f4  --  0x0200d2f4
 *
 *   OK OvlFunc_945_200d2f4 -- 912 bytes, 363 encodings and 82 relocations identical
 *
 * Re-measured SIX times (three on the 20-pin base, three on the shipped
 * 17-pin minimum).  objcmp prints no `(built with: ...)` line; tryc's
 * makefile_flags() is the EMPTY SET on the pre-split path and on the post-split
 * src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_a_a_c_b.c alike.  `grep -n
 * rom_7cb2c0 Makefile` returns only EXPLICIT rules for OTHER stems
 * (ovl_30_c_c_c_c_c_c_c_a_c, ovl_30_a_c_c_a_a_b, ...), never a wildcard that
 * reaches this one, so the generic `asm/%.o: src/%.c` applies at the tree
 * default -O2.
 *
 * LANDING NEEDS A SPLIT.  `tools/asmfacts.py` says "2 functions  split first"
 * on asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_a_a_c.s: OvlFunc_945_200d0e4
 * then OvlFunc_945_200d2f4, and only the second is solved here (200d0e4 is
 * PARKED at 4 of 206 -- see NOTES.md).  Run
 *
 *     python3 tools/split_s.py \
 *       asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_a_a_c.s OvlFunc_945_200d2f4
 *
 * which leaves 200d0e4 in ..._a_a_c_a.s and the target in ..._a_a_c_b.s and
 * rewrites overlays/rom_7cb2c0/overlay.ld:87 -- the ONE line naming this .o on
 * its full path -- into the two lines in the same order.  `make compare` must
 * be GREEN after the split and BEFORE this .c lands.  The .s carries NO
 * `.section`, `.data`, `.bss`, `.lcomm`, `.word`, `.byte` or `.global` line;
 * all five `.L` labels are branch targets local to their own function, so the
 * cut exports nothing.  FAKEMATCH: the name goes in fakematch.txt.
 *
 * NEAR-TWIN, AND THE FAMILY PAID.  src/overlays/rom_7d95dc/
 * ovl_30_c_c_c_c_a_c_a_a.c (OvlFunc_953_2009cd4) is the same SHAPE down to the
 * details: one `__Func_8091c7c` diamond, an `iwram_3001ebc[0xec << 1]` counter
 * bump in the taken arm, a SetBehavior/SetBehavior/RunScript trio off one
 * `ldr r5, =gScript`, an r8 holding `0xc0 << 6` and an r10 holding a `0xc0 <<`
 * value.  Its HOLE TEST, its `__Func_8092c40`-wants-descending rule and its
 * store-block spelling ALL TRANSFERRED.  Its hi/hiv reject did not apply:
 * hi = 14, hiv = 3 here, and that is exactly the 14 hole sites.
 *
 * ===================================================================
 * WHAT CLOSED IT, in the order the mechanism sizes said to try them
 * ===================================================================
 *
 *   plain C, no pins                            214 (363 vs 361, 2 insns long,
 *                                               `push {r5, r6, r7, lr}` + r11)
 *   every site with an argument pinned           328 (8 insns SHORT)
 *   the 44 non-hole sites pinned ("cand")        333 (2 insns short)
 *   ... MINUS site 6                              42 (exact insn count)
 *   ... MINUS site 5 as well                      38
 *   + RE-PIN hole 42                              24
 *   + RE-PIN holes 21 and 27                      18
 *   - DROP the pins at 38, 45 and 56              12
 *   + DESCENDING fill at 57 (__Func_8092c40)      10
 *   + the store block as ONE EXPRESSION            0
 *
 * THE HOLE TEST NEEDS A SECOND CLAUSE, AND IT IS WORTH 291 DIFFERING (NEW).
 * The recorded rule carves out every site where the ROM supplies an argument
 * with `mov rLOW, rHIGH` -- gcc has commoned that value and a pin would
 * rematerialise it.  Read only that way the test MISSES THE DEFINITION SIDE.
 * Site 6 is `OvlFunc_945_200c890(0xa, 0xe3 << 1, 0xf8, 0xc0 << 6)` and the ROM
 * reads
 *
 *     mov r3, #0xc0 / lsl r3, #6 / ... / mov r8, r3 / bl OvlFunc_945_200c890
 *
 * -- r3 is BOTH the fourth argument AND the source of the r8 live range that
 * site 30 later reaches with `mov r1, r8`.  Pinning it makes r3 call-clobbered,
 * the r8 tenant disappears, the prologue loses `mov r6, r8 / push {r6}` and the
 * function comes out TWO INSTRUCTIONS SHORT at 333 differing.  Dropping that
 * one pin is 42.  So:
 *
 *   > A SITE IS A HOLE IF THE ROM EITHER READS AN ARGUMENT OUT OF A HIGH /
 *   > CALLEE-SAVED REGISTER (`mov rLOW, rHIGH`) **OR WRITES ONE INTO ONE**
 *   > (`mov rHIGH, rLOW`) IN THE SAME ARGUMENT WINDOW.  The second clause is
 *   > the DEFINITION of the commoned value and it is just as fatal to pin.
 *
 * Mechanically: holes.py must treat `mov rHIGH, rLOW` as marking rLOW, not
 * just `mov rLOW, rHIGH` as marking rLOW.  Here that is site 6 alone, and the
 * neighbour's 36-hole set never exercised it because none of its high-register
 * constants was also an argument at its own definition site.
 *
 * THREE HOLES WANT THEIR PIN BACK, AND THE NEIGHBOUR SAID SO.  21, 27 and 42
 * read r0/r1 or r1 out of r5/r6/r9 and are holes by the first clause, yet
 * pinning them is worth 38 -> 24 -> 18.  That is the neighbour's "a pin asks
 * for the value in r2 at that point and gcc's CSE is free to answer from r6 --
 * what the pin removes is only the FREEDOM TO ORDER", confirmed on a second
 * function and on three sites at once.  The hole test NOMINATES.
 *
 * THREE PINS HAD TO BE DROPPED AND THE TELL IS THE SAME AT ALL THREE.  Sites
 * 38 `OvlFunc_945_200c8e8(1, 0xe0 << 8, 0x3c)`, 45 `OvlFunc_945_200c880(0x1b,
 * 0xd0 << 8)` and 56 `OvlFunc_945_200c880(0x1b, 0xa0 << 7)` each cost 2
 * differing while pinned: the ROM emits `lsl r1, #N` BEFORE `mov r0, #imm` and
 * the pinned form emits them the other way round.  WRITING THE FILL REVERSED
 * DOES NOT HELP -- o10, o102, o120, o210 and the width reductions w1/w2 are ALL
 * inert at 18, sched2 normalises every one of them.  Only removing the pin
 * works, 2 differing each, additively: 18 -> 16 -> 16 -> 16 -> 12 together.
 * The shape is a one-argument shifted operand with a bare-imm8 r0, which is
 * within a hair of the recorded width-zero "leave it bare" class.
 *
 * `__Func_8092c40` WANTS THE DESCENDING FILL -- SECOND INDEPENDENT SAMPLE.
 * Site 57 is `__Func_8092c40(0x1b, 0)`, both arguments bare `mov #imm8`, so no
 * nomination rule reaches it.  `q1 = 0; q0 = 0x1b;` is worth 12 -> 10; the
 * ASCENDING fill and DROPPING the pin both measure 12.  The neighbour recorded
 * exactly this on its only __Func_8092c40 site and called the tell binary; it
 * is binary here too, and the callee is now 2 for 2.  KEEP IT BY NAME.
 *
 * ===================================================================
 * THE LAST TEN WERE A RELOAD SCRATCH, AND THE CURE IS THE STORE BLOCK
 * ===================================================================
 *
 * At 10 the whole residue was:
 *
 *     ROM   movs r2, #216 / lsls r2, #1 / mov r9, r2     (site 4)
 *     ours  movs r3, #216 / lsls r3, #1 / mov r9, r3
 *     ROM   movs r2, #192 / lsls r2, #8 / mov sl, r2     (site 19)
 *     ours  movs r3, #192 / lsls r3, #8 / mov sl, r3
 *
 * plus the two `mov r0` / `mov r2` orderings those drag with them.  Setting a
 * HIGH register from a constant needs a low scratch, and that scratch is a
 * RELOAD register, not an allocated local.  The full mechanism is written up in
 * the sibling ovl_314_c_c_c_a_a_b.c header (reload1.c: spill_cost zeroed per
 * insn, ties broken by inv_reg_alloc_order so r3 always wins, winners sorted
 * ascending into spill_regs[], final pick by ROUND ROBIN in
 * allocate_reload_reg).  Here the function has FOUR reloads -- the r9, r8 and
 * r10 constant builds and the `reg + 0x1d8` address in the counter bump -- and
 * plain C gives `Using reg 3` four times: a singleton set, so every scratch is
 * r3.  The ROM's are r2, r3, r2, which is the round robin over {r2, r3}.
 *
 * TWO INDEPENDENT LEVERS ADD r2 TO THE SET, AND ONLY ONE IS FREE.
 *
 *   (a) A HARD r3 PIN ON A `0` ARGUMENT.  Pinning ONLY q3 at site 4
 *       (`register int q3 __asm__("r3"); q3 = 0;` passed as the fourth
 *       argument) makes r3 live across the r9 build, find_reg answers
 *       `Using reg 2`, and BOTH constant builds come out exactly right.  It
 *       costs 1 differing on its own (the `movs r3, #0` is then hoisted to the
 *       front of the window where the ROM has it last), and q3-pinning sites 3
 *       and 4 together reaches 6.  Instructive, NOT shipped.
 *   (b) THE STORE BLOCK WRITTEN AS ONE EXPRESSION.  The epilogue writes
 *       `*(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x209`.  Spelled through two
 *       named `unsigned int` locals the address arithmetic is ordinary insns
 *       and adds no reload: 10 differing.  Spelled as ONE EXPRESSION the
 *       448-byte offset is out of range for thumb `str`, reload must
 *       materialise the address, that extra reload takes r2, and the function
 *       is EXACT with nothing else changed.  SHIPPED.
 *
 * That the SAME one-line spelling closed both functions in this batch -- two
 * different overlays, two different residues, 500 bytes and 200 bytes away from
 * their defects -- is why it is written up as a class and not as a trick.
 *
 * WHAT THE COUNTER BUMP DID NOT CARE ABOUT.  `(*(unsigned short *)
 * (iwram_3001ebc + (0xec << 1)))++` and the same thing through two named locals
 * measure IDENTICALLY (10 with the named store, 0 with the expression store).
 * The bump's own `reg + 0x1d8` reload is in the set either way; it is the STORE
 * that decides whether r2 joins.  So the lever is not "avoid named locals near
 * iwram" -- it is "make sure ONE out-of-range address is built by reload".
 *
 * THE FLAG DIAMOND NEEDED NO FLAG AT ALL.  The ROM is
 * `bl __Func_8091c7c / cmp r0, #0 / bne .L556e`, so the source is
 * `if (__Func_8091c7c(0, 0) == 0) { ... } else { ... bump ... }` with the bump
 * in the ELSE arm, and that is exact from the first screen.  The neighbour
 * needed a hoisted `v` because its bump was reached from two places; this one
 * is not, so the b240 template's flag does not arise.
 *
 * PIN MINIMISATION: 20 in the closing set, 17 required; forward and reverse
 * drop-to-fixpoint converge on the SAME SEVENTEEN.  The three that fall are
 * 7, 16 and 17 -- all-cheap sites.
 *
 * MEASURED WORSE / INERT (against 363 encodings / 912 bytes):
 *
 *   spelling                                             differing
 *   --------------------------------------------------  ---------
 *   plain C, no pins                                          214  (+2 insns)
 *   every argument-bearing site pinned                        328  (-8 insns)
 *   44 non-hole sites ("cand")                                333  (-2 insns)
 *   cand - {6}                                                 42
 *   cand - {5, 6}                                               38
 *   ... + hole 42                                               24
 *   ... + holes 21, 27                                          18
 *   ... - pins 38, 45, 56                                       12
 *   ... + 57 descending                                         10
 *   ... + store as one expression                                0
 *   re-pin hole 4 / 8 / 19                           327 / 294 / 328
 *   re-pin hole 1 / 2 / 3                              54 / 56 / 40
 *   re-pin hole 30 / 48 / 68 / 69 / 70                 38 each (INERT)
 *   site 6 pinned to width 3 (q0..q2 only)                    333
 *   site 4 partial masks m1010/m1001/m1000/m0010               12  (INERT)
 *   site 4 masks m1011 / m0011 / m0101 / m1101 / m1111  312/313/328/328/328
 *   site 4 q3-only pin (m0001)                                 11
 *   sites 3+4 q3-only pins                                      6
 *   sites 1+2+3+4 q3-only pins                                 12
 *   57 ascending / 57 pin dropped                              12 each
 *   38/45/56 orders o10, o102, o120, o210; widths w1, w2       18  (ALL INERT)
 *   store block through named locals, ptr first                16
 *   store block through named locals, offset first             10
 *   counter bump through named locals                  no change either way
 *
 * Harness: scratch_elev/b257/a2/d2f4 -- gen.py (site table; pin / width /
 * order / per-argument MASK specs; K_ST store-block and K_BU bump knobs),
 * run.sh, climb.py, and the shared cmp2.py / dis.py / minimise.py one level up.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char gScript_945__0200e7f0[];

extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ClearFlag(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int x, int z);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_RunScript(int slot, unsigned char *s);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8091e9c(int a);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void OvlFunc_945_200c86c(int a);
extern void OvlFunc_945_200c880(int a, int b);
extern void OvlFunc_945_200c890(int a, int b, int c, int d);
extern void OvlFunc_945_200c8ac(int a, int b, int c, int d);
extern void OvlFunc_945_200c8e8(int a, int b, int c);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_945_200d2f4(void)
{
    unsigned char *s;

    OvlFunc_945_200c890(0, 0xde << 1, 0x96 << 1, 0);
    OvlFunc_945_200c890(1, 0xe5 << 1, 0x9b << 1, 0);
    OvlFunc_945_200c890(2, 0xde << 1, 0xa5 << 1, 0);
    OvlFunc_945_200c890(3, 0xd8 << 1, 0x9b << 1, 0);
    OvlFunc_945_200c890(0x1b, 0xdc << 1, 0x86, 0x80 << 8);
    OvlFunc_945_200c890(0xa, 0xe3 << 1, 0xf8, 0xc0 << 6);
    __MapActor_SetAnim(0xa, 6);
    OvlFunc_945_200c8ac(0xdc << 17, -1, 0x9a << 17, 0x1000001);
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    { PIN3; q0 = 0; q1 = 0x80 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xa0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xa0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x80 << 6; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    OvlFunc_945_200c8e8(2, 1, 0x14);
    __MessageID(0x1e6e);
    OvlFunc_945_200c86c(0x1b);
    OvlFunc_945_200c8e8(1, 0xc0 << 8, 0);
    __Func_80933d4(0x26666, 0x4ccc);
    { PIN4; q0 = 0xdc << 17; q1 = -1; q2 = 0xb0 << 16; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    { PIN3; q0 = 0x1b; q1 = 0x19999; q2 = 0xcccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x1b; q1 = 0xcc << 1; q2 = 0x86;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x1b; q1 = 0xcc << 1; q2 = 0x98;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x1b; q1 = 0xd4 << 1; q2 = 0xa4;
      __Func_80921c4(q0, q1, q2); }
    __Func_80933d4(0x19999, 0x3333);
    { PIN4; q0 = 0xdc << 17; q1 = -1; q2 = 0x96 << 17; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    { PIN3; q0 = 0x1b; q1 = 0xd4 << 1; q2 = 0xde;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x1b; q1 = 0xd4 << 1; q2 = 0x83 << 1;
      __Func_80921c4(q0, q1, q2); }
    __Func_8092adc(0x1b, 0xc0 << 6, 0x14);
    __Func_80925cc(0x1b, 1);
    OvlFunc_945_200c86c(0x1b);
    OvlFunc_945_200c8e8(2, 1, 0x14);
    __MapActor_DoAnim(0x1b, 3);
    __Func_80925cc(0x1b, 1);
    OvlFunc_945_200c86c(0x1b);
    OvlFunc_945_200c8e8(3, 2, 0x3c);
    OvlFunc_945_200c8e8(1, 0xe0 << 8, 0x3c);
    __Func_8092adc(0x1b, 0, 0x28);
    __Func_80925cc(0x1b, 1);
    __MapActor_SetAnim(0x1b, 2);
    { PIN3; q0 = 0x1b; q1 = 0xd8 << 1; q2 = 0x86 << 1;
      __Func_8092158(q0, q1, q2); }
    { PIN3; q0 = 0x1b; q1 = 0xe2 << 1; q2 = 0x86 << 1;
      __Func_8092158(q0, q1, q2); }
    __MapActor_SetAnim(0x1b, 1);
    OvlFunc_945_200c880(0x1b, 0xd0 << 8);
    __Func_809259c(0x1b, 2);
    __Func_8093040(0x1b, 0, 0x14);
    OvlFunc_945_200c8e8(1, 0xc0 << 8, 0x14);
    __MapActor_DoAnim(0x1b, 4);
    __CutsceneWait(0x28);
    __Func_8093040(0x1b, 0, 0x50);
    __Func_80925cc(0x1b, 1);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x1b, 3);
    __CutsceneWait(0xa);
    OvlFunc_945_200c880(0x1b, 0xa0 << 7);
    { PIN2; q1 = 0; q0 = 0x1b;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0) {
        __MapActor_DoAnim(0x1b, 3);
        OvlFunc_945_200c86c(0x1b);
    } else {
        __MapActor_DoAnim(0x1b, 4);
    (*(unsigned short *)(iwram_3001ebc + (0xec << 1)))++;
        OvlFunc_945_200c86c(0x1b);
        OvlFunc_945_200c8e8(3, 2, 0x28);
        __Func_80925cc(0x1b, 1);
        __MapActor_SetAnim(0x1b, 3);
        OvlFunc_945_200c86c(0x1b);
    }
    OvlFunc_945_200c8e8(2, 1, 0x14);
    s = gScript_945__0200e7f0;
    __MapActor_SetBehavior(1, s);
    __MapActor_SetBehavior(2, s);
    __MapActor_RunScript(3, s);
    __Func_80933d4(0x9999, 0x1333);
    __Func_80933f8(0xdc << 17, -1, 0xb0 << 16, 1);
    { PIN3; q0 = 0; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xd4 << 1; q2 = 0x88 << 1;
      __Func_80921c4(q0, q1, q2); }
    __Func_809218c(0, 0xd4 << 1, 0xa4);
    __CutsceneWait(0x3c);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x209;
    OvlFunc_945_200c8e8(9, 0, 0);
    __ClearFlag(0x301);
    __ClearFlag(0x927);
    __Func_8091e9c(4);
}
