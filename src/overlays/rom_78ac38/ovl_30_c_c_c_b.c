/* OvlFunc_889_2008074  --  0x02008074  --  *** BYTE-EXACT.  NOT A PARK. ***
 *
 * MATCHING, 1002 encodings of 1002.  objcmp.py under the production flags:
 *   OK OvlFunc_889_2008074 -- 2628 bytes, 1002 encodings and 294 relocations
 *   identical
 * SIZE EXACT (2628 against 2628).  COUNT EXACT (1002 against 1002).
 * RELOCATIONS EXACT (294 against 294, same symbols at the same offsets).
 * aligncmp.py, reported separately as the discipline requires:
 *   ref 1002 encodings, ours 1002, aligned-equal 1002 (100.0% of ref),
 *   differing/ins/del 0 in 0 hunks.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/overlays/rom_78ac38/ovl_30_c_c_c_b.c \
 *     asm/overlays/rom_78ac38/ovl_30_c_c_c_b.s --func OvlFunc_889_2008074
 * (that is the path this file installs to AFTER the split below; the file as it
 * sits in scratch_elev/b310d is verified against the extracted reference
 * scratch_elev/b310d/ref_OvlFunc_889_2008074.s, which is the same bytes.)
 *
 * LANDING: A .global FIRST, THEN A TEXT/DATA SPLIT.  datacheck.py on
 * asm/overlays/rom_78ac38/ovl_30_c_c_c.s reports one `.data` section, one
 * function, seven already-global exports, and the one thing that matters:
 *     OvlFunc_889_2008074 reads .Lea0
 *     *** SPLIT MUST EXPORT: .global .Lea0
 * split_s.py --dry-run REFUSES for exactly that reason -- the function goes to
 * _b and the eight blobs to _c, and `.Lea0` would then cross files where a `.L`
 * symbol does not survive into the object symbol table.  So the order is:
 *   1. add `.global .Lea0` to asm/overlays/rom_78ac38/ovl_30_c_c_c.s (emits no
 *      bytes) and confirm `make compare` is still green -- keeps the two
 *      changes separable;
 *   2. re-run split_s.py (function -> _b, data -> _c);
 *   3. confirm `make compare` green again BEFORE this .c replaces _b.s;
 *   4. drop this file in as src/overlays/rom_78ac38/ovl_30_c_c_c_b.c.
 * TWO linker rows name the object and BOTH must be updated, not one:
 * overlays/rom_78ac38/overlay.ld:21 `(.text)` and :26 `(.data)`.
 *
 * shimcount.py: 233 register pins across 82 PIN sites, and it flags
 * `has a fakematch-class shim and NO fakematch.txt row`.  A fakematch.txt row
 * is therefore part of landing, as it is for the family's byte-exact sibling
 * src/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c_c_c_c_b.c (2,805 instructions),
 * whose own first line is `// fakematch`.
 *
 * tryc.py is a FALSE NEGATIVE here and must not be used as the verdict: it
 * warns that the reference keeps its literal pool INSIDE the function, so it
 * normalises pool loads.  objcmp is the authority and it says OK.
 *
 * HOW IT WAS BUILT, AND THE THREE EDITS THAT CARRIED IT.
 * The reference is 975 instructions, 287 calls, FOUR branch targets, no `sub sp`
 * frame and -- this is the load-bearing fact -- ZERO references to r8-r11.  Two
 * of the four labels (.L4a8/.L4ec and .L7f0/.L844) are a real if/else pair; the
 * two `.pool_aligned` directives sit inside them.  Both tests are
 * `__Func_8091c7c(0, 0)` with a plain `bne` to the else block, i.e.
 * `if (... == 0)`, the polarity already recorded for that callee.
 *
 * Starting point: every call site transcribed with ASCENDING pin fills
 * (`q0..q3`, one statement per argument) at every site holding an argument
 * outside 0..255.  That is 82 sites, and it measured -4 size / -2 count with
 * 82 of 1000 encodings differing.  Three edits closed it:
 *
 *   1. THE SCRIPT POINTER NEEDS A NAMED LOCAL, AND IT IS THE WHOLE 4 BYTES AND
 *      2 ENCODINGS.  The ROM's `push {r5, lr}` keeps ONE callee-saved register
 *      and it holds `gScript_889__02008cb4`, read by two
 *      __MapActor_SetBehavior calls 3 instructions apart.  Writing the symbol
 *      twice inside the two PIN blocks gives `push {lr}` and two pool loads:
 *      the pin's destination is the call-clobbered hard register r1, so there
 *      is nothing for the allocator to keep.  `scr = gScript_889__02008cb4;`
 *      immediately before the first site, with both sites reading `(int)scr`,
 *      takes 82 differing to 4 and makes size, count AND relocations exact in
 *      one edit.  The lever is the band doc's "change the SET of long-lived
 *      quantities", and the quantity is a SYMBOL, not a constant.
 *
 *   2. THE LAST FOUR ENCODINGS ARE THE `__Func_8092c40` DESCENDING FILL, AND
 *      THE PRECONDITION IS THE ONE ALREADY ON RECORD.  Both of this function's
 *      __Func_8092c40 sites are the ones whose call is followed by the
 *      `__Func_8091c7c` test, and both want `q1 = 0; q0 = N;`.  That is
 *      verbatim the rule in src/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c_c_c_c_b.c
 *      ("six of its eight sites -- every site whose call is followed by the
 *      __Func_8091c7c test"), confirmed here on 2 of 2.  Both sites, flipped,
 *      close the function.
 *
 *   3. DO NOT READ THE ROM'S ARGUMENT ORDER AS THE SOURCE ORDER.  The first
 *      transcription derived each site's fill order from which register the ROM
 *      writes first.  That measured 134 differing against 82 for plain
 *      ascending -- 52 encodings WORSE.  The ROM's order is sched2's output,
 *      not the source's: at `{ PIN3; q0 = 0xb; q1 = 0xa0 << 8; q2 = 0; }` gcc
 *      emits mov r0 / mov r1 / lsl r1 / mov r2 and sched2 hoists `mov r1` over
 *      `mov r0` because the `lsl` depends on it -- `rank_for_schedule`'s
 *      dependent count, the same tiebreak the sibling file documents.  ASCENDING
 *      IS THE SOURCE ORDER; the interleave is free.
 *
 * NOTHING ELSE IS HELD.  Every repeated wide constant in 975 instructions is
 * rebuilt at every use: 112 build sequences (`lsl` of a `mov`, or a pooled
 * `ldr =`) and ZERO `mov rlo, rhigh` reuses.  There is no commoned constant in
 * this function at all, which is why pins alone reach it.
 *
 * THE POOLED-ZERO DEFECT DOES NOT APPLY: the reference has no halfword store of
 * a literal zero.  Its only halfword traffic is the two
 * `*(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;` increments, one in
 * each arm of the second if, written with the family's established idiom.
 *
 * THE OTHER TWO POINTER REGIONS.  `iwram_3001ebc` is read into a named local
 * `p` at both `__MapTransition` sites because the ROM's `add r2, r1, r3` reuses
 * one loaded pointer for two word stores at (0xe0 << 1) and (0xe4 << 1) -- the
 * derived-address lever with its precondition (more than one use) satisfied.
 */
extern void __PlaySound(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __StartThunder2(int a, int b);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetBehavior(int slot, int script);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitScript(int slot);
extern void __ActorMessage(int a, int b);
extern void __Func_800fe9c(void);
extern void __Func_8010560(int a, int b, int c);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8091e9c(int n);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093500(int a, int b);
extern void __Func_8093530(void);
extern void __Func_8095240(void);

extern unsigned char *iwram_3001ebc;
extern unsigned char ActorCmd_ARRAY_889__02008c00[];
extern unsigned char gScript_889__02008c64[];
extern unsigned char gScript_889__02008cb4[];
extern unsigned char Lea0[] __asm__(".Lea0");

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_889_2008074(void)
{
    unsigned char *p;
    unsigned char *scr;
    __CutsceneStart();
    __MapActor_SetAnim(0xe, 0);
    __MapActor_SetAnim(0xf, 0);
    __MapActor_SetAnim(0x10, 0);
    __MapActor_SetAnim(0x11, 0);
    __MapActor_SetAnim(0x12, 0);
    __MapActor_SetAnim(0x13, 0);
    { PIN3; q0 = 0xb; q1 = 0x109; q2 = 0x1e7;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xa0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0x80 << 1; q2 = 0xfa << 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0xa0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN2; q0 = 0x10003; q1 = 0x10006;
      __StartThunder2(q0, q1); }
    __Func_8095240();
    __WaitFrames(0x3c);
    { PIN4; q0 = 0x80 << 17; q1 = -1; q2 = 0x99 << 18; q3 = 0;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __Func_800fe9c();
    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0;
    *(int *)(p + (0xe4 << 1)) = 0x20;
    __MapTransitionIn();
    { PIN2; q0 = 0xcccc; q1 = 0x1999;
      __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0x80 << 17; q1 = -1; q2 = 0xfa << 17; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x80 << 9; q1 = 0x80 << 10; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __Func_8095240();
    __PlaySound(0x91);
    __CutsceneWait(0x1e);
    __Func_8095240();
    __PlaySound(0x91);
    __Func_8093530();
    { PIN3; q0 = 0x80 << 10; q1 = 0xc0 << 10; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __Func_8095240();
    __PlaySound(0x91);
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    __Func_8012350();
    __CutsceneWait(0x3c);
    { PIN1; q0 = 0x1122;
      __MessageID(q0); }
    { PIN3; q0 = 8; q1 = 0x81 << 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __ActorMessage(8, 0);
    { PIN3; q0 = 9; q1 = 0xa0 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __ActorMessage(9, 0);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(0xb, 4);
    __ActorMessage(0xb, 0);
    { PIN3; q0 = 9; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_809280c(0xc, 0xb, 0);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(0xc, 4);
    __ActorMessage(0xc, 0);
    __Func_80925cc(0xd, 1);
    __ActorMessage(0xd, 0);
    __Func_809280c(0xa, 0xd, 0);
    __CutsceneWait(0x1e);
    __Func_80925cc(0xa, 1);
    __ActorMessage(0xa, 0);
    __Func_809280c(9, 0xa, 0);
    __CutsceneWait(0x1e);
    __Func_80925cc(9, 1);
    __ActorMessage(9, 0);
    __Func_809280c(0xa, 9, 0);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(0xa, 4);
    __ActorMessage(0xa, 0);
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0x80 << 10; q1 = 0xc0 << 10; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __Func_8095240();
    __PlaySound(0x91);
    __CutsceneWait(0x3c);
    __Func_8092848(8, 9, 0);
    __Func_8092848(0xa, 0xb, 0);
    __Func_8092848(0xc, 0xd, 0);
    __Func_809259c(8, 2);
    __Func_809259c(9, 2);
    __Func_809259c(0xa, 2);
    __Func_809259c(0xb, 2);
    __Func_809259c(0xc, 2);
    __Func_809259c(0xd, 2);
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    __Func_8012350();
    { PIN3; q0 = 0; q1 = 0x80 << 8; q2 = 0x80 << 7;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x80 << 8; q2 = 0x80 << 7;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xf0 << 15; q2 = 0x81 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN2; q0 = 0xc0 << 9; q1 = 0xc0 << 6;
      __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0xe0 << 15; q1 = -1; q2 = 0xa0 << 17; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __CutsceneWait(0x28);
    __MapActor_SetAnim(0, 2);
    __MapActor_SetAnim(1, 2);
    { PIN3; q0 = 0; q1 = 0x78; q2 = 0xa0 << 1;
      __MapActor_TravelTo(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x68; q2 = 0xa0 << 1;
      __Func_8092158(q0, q1, q2); }
    __MapActor_SetAnim(0, 1);
    __MapActor_SetAnim(1, 1);
    __Func_8093530();
    __CutsceneWait(0x1e);
    { PIN3; q0 = 1; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x80 << 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x32);
    { PIN3; q0 = 1; q1 = 0xc0 << 9; q2 = 0xc0 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(1, 2);
    { PIN3; q0 = 1; q1 = 0x69; q2 = 0xab << 1;
      __Func_8092158(q0, q1, q2); }
    __MapActor_SetAnim(1, 1);
    __Func_80925cc(1, 2);
    __ActorMessage(1, 0);
    __CutsceneWait(0xa);
    __Func_809259c(0, 1);
    __Func_8092848(0, 1, 0);
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 1;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0) {
    __CutsceneWait(0x3c);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __CutsceneWait(0x32);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(1, 2);
    { PIN3; q0 = 1; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x67; q2 = 0xa0 << 1;
      __Func_8092158(q0, q1, q2); }
    __MapActor_SetAnim(1, 1);
    } else {
    __CutsceneWait(0x3c);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __CutsceneWait(0x32);
    { PIN3; q0 = 1; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0, 2);
    { PIN3; q0 = 0; q1 = 0x78; q2 = 0xaa << 1;
      __Func_8092158(q0, q1, q2); }
    __MapActor_SetAnim(0, 1);
    }
    __ActorMessage(0xc, 0);
    { PIN2; q0 = 1; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __Func_80925cc(1, 2);
    __CutsceneWait(0x28);
    { PIN3; q0 = 9; q1 = 0xa0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xa0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xa0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0xa0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0xa0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN2; q0 = 0xc0 << 10; q1 = 0xc0 << 7;
      __Func_80933d4(q0, q1); }
    __Func_8093500(0xa, 1);
    __Func_8093530();
    __CutsceneWait(0x32);
    __Func_80925cc(0xa, 2);
    __ActorMessage(0xa, 0);
    __CutsceneWait(0x1e);
    __Func_80925cc(8, 1);
    __ActorMessage(8, 0);
    __CutsceneWait(0x28);
    __Func_80925cc(9, 1);
    __ActorMessage(9, 0);
    __CutsceneWait(0x28);
    { PIN3; q0 = 0; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN4; q0 = 0xe0 << 15; q1 = -1; q2 = 0xa0 << 17; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __Func_809259c(0, 2);
    __Func_809259c(1, 2);
    __MapActor_WaitScript(1);
    __CutsceneWait(0x32);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_WaitScript(1);
    __CutsceneWait(0x3c);
    { PIN2; q0 = 0x80 << 9; q1 = 0x80 << 6;
      __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0xd6 << 16; q1 = -1; q2 = 0xec << 17; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    { PIN3; q0 = 0; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN2; q0 = 0; q1 = (int)ActorCmd_ARRAY_889__02008c00;
      __MapActor_SetBehavior(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN2; q0 = 1; q1 = (int)gScript_889__02008c64;
      __MapActor_SetBehavior(q0, q1); }
    __MapActor_WaitScript(1);
    __Func_8092adc(0, 0, 0);
    __Func_8092adc(1, 0, 0);
    __Func_8093530();
    { PIN3; q0 = 9; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(8, 2);
    { PIN3; q0 = 8; q1 = 0x109; q2 = 0x1c7;
      __Func_8092158(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0xf6; q2 = 0x1c7;
      __Func_8092158(q0, q1, q2); }
    __MapActor_SetAnim(8, 1);
    __CutsceneWait(0x1e);
    __Func_80925cc(9, 1);
    __ActorMessage(9, 0);
    { PIN3; q0 = 0; q1 = 0x81 << 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x32);
    { PIN3; q0 = 1; q1 = 0x81 << 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x32);
    __Func_80925cc(8, 1);
    __ActorMessage(8, 0);
    __CutsceneWait(0x28);
    __Func_8092848(0, 1, 0);
    __CutsceneWait(0x32);
    __Func_8092adc(0, 0, 0);
    __Func_8092adc(1, 0, 0);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0, 4);
    __MapActor_DoAnim(1, 4);
    __CutsceneWait(0x28);
    { PIN3; q0 = 0xa; q1 = 0x81 << 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x32);
    { PIN3; q0 = 1; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 0; q0 = 0xa;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0) {
    __CutsceneWait(0x28);
    __Func_8092848(8, 9, 0);
    __CutsceneWait(0x32);
    { PIN3; q0 = 8; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x28);
    __Func_80925cc(9, 1);
    __ActorMessage(9, 0);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    } else {
    __CutsceneWait(0x28);
    __Func_8092848(8, 9, 0);
    __CutsceneWait(0x32);
    { PIN3; q0 = 8; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x28);
    __Func_80925cc(9, 1);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    __ActorMessage(9, 0);
    }
    __CutsceneWait(0x1e);
    __Func_8092adc(1, 0, 0);
    __CutsceneWait(0x1e);
    __Func_809259c(0, 2);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x28);
    __MapActor_SetAnim(0, 4);
    __MapActor_DoAnim(1, 4);
    __CutsceneWait(0x3c);
    __Func_80925cc(8, 1);
    __ActorMessage(8, 0);
    __Func_809280c(8, 9, 0);
    __CutsceneWait(0x1e);
    __Func_80925cc(8, 1);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(8, 0);
    __CutsceneWait(0x14);
    __Func_80925cc(9, 1);
    { PIN3; q0 = 9; q1 = 0xb0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(9, 3);
    __CutsceneWait(0x32);
    { PIN3; q0 = 8; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __Func_80925cc(8, 1);
    __ActorMessage(8, 0);
    __CutsceneWait(0x28);
    __Func_8092848(8, 9, 0);
    __CutsceneWait(0x28);
    __MapActor_SetAnim(8, 3);
    __MapActor_DoAnim(9, 3);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 8; q1 = 0xff; q2 = 0x1bd;
      __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(0x28);
    { PIN3; q0 = (int)Lea0; q1 = 0x2d; q2 = 0xb;
      __Func_8010560(q0, q1, q2); }
    __PlaySound(0xbc);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 8; q1 = 0xff; q2 = 0xc3 << 1;
      __Func_809218c(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN3; q0 = 9; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0xff; q2 = 0xc3 << 1;
      __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xff; q2 = 0xe6 << 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x28);
    __MapActor_DoAnim(0xa, 3);
    __CutsceneWait(0x1e);
    __Func_809259c(0, 1);
    __Func_80925cc(1, 1);
    __CutsceneWait(0x28);
    { PIN3; q0 = 0xa; q1 = 0xff; q2 = 0xc3 << 1;
      __Func_809218c(q0, q1, q2); }
    scr = gScript_889__02008cb4;
    { PIN2; q0 = 0; q1 = (int)scr;
      __MapActor_SetBehavior(q0, q1); }
    __CutsceneWait(0x28);
    { PIN2; q0 = 1; q1 = (int)scr;
      __MapActor_SetBehavior(q0, q1); }
    __MapActor_WaitScript(1);
    { PIN3; q0 = 0xb; q1 = 0x81 << 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0x81 << 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x28);
    { PIN3; q0 = 0x80 << 10; q1 = 0xc0 << 10; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __Func_8095240();
    __PlaySound(0x91);
    __CutsceneWait(0x1e);
    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0;
    *(int *)(p + (0xe4 << 1)) = 0x40;
    __MapTransitionOut();
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    __Func_8012350();
    { PIN1; q0 = 0x12f;
      __ClearFlag(q0); }
    { PIN1; q0 = 0x879;
      __SetFlag(q0); }
    __Func_8091e9c(1);
    __CutsceneEnd();
}
