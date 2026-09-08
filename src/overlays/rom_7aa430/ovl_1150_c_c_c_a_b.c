// fakematch
/* OvlFunc_923_2009730  --  0x02009730
 *
 * Cut out of goldensun/asm/overlays/rom_7aa430/ovl_1150_c_c_c_a.s, which holds
 * TWO functions.  `tools/split_asm.py ... OvlFunc_923_2009730` reports:
 * lines 436-660, `carries data: no`, `remaining funcs: 1`,
 * `label exports: none needed`, and a BASENAME WARNING -- the .c must not be
 * called ovl_1150_c_c_c_a.  The other function, OvlFunc_923_20092e0, is a
 * 368-instruction switch dispatcher whose jump table is a `.word .L....` list
 * INSIDE .text, so it stays in asm and its table stays with it.
 *
 *   OK OvlFunc_923_2009730 -- 572 bytes, 225 encodings and 57 relocations identical
 *
 * Re-measured four times.  objcmp prints no `(built with: ...)` line;
 * makefile_flags() on this path is the EMPTY set, and the only rom_7aa430
 * Makefile rules name ovl_e90_c_c_a_a_b and the pattern ovl_e90_c_c_a_a_b_%,
 * neither of which matches, so `asm/%.o: src/%.c` fires with the tree default
 * -O2 -mthumb -mthumb-interwork -fcall-used-r4.
 *
 * hi = 8, hiv = 2.  Two beats in a body of two identical iterations: a plain
 * transcription is 186 of 225 with gcc commoning five repeated constants into
 * r5/r6/r8/sl where the ROM rebuilds each one -- the recorded COMMONING tell,
 * seen from the other side, since here it is the CANDIDATE that is greedy.
 *
 * THE DEAD HIGH REGISTER IS `| zero`, AND THAT IS THE HEADLINE.  The ROM writes
 * `mov r3, #0 / mov sl, r3` before the loop and NEVER READS sl again, paying
 * two prologue moves, a wider push, two epilogue moves and a wider pop for a
 * value with no consumer.  That is the DEAD CALLEE-SAVED REGISTER blocker
 * class, parked twice in this corpus (OvlFunc_945_200b7b4,
 * OvlFunc_935_2008704) with the note "a local assigned zero and never read is
 * removed before allocation".  IT IS NOT ALWAYS DEAD IN THE SOURCE.  Writing
 * the loop's two byte-clears as
 *
 *     *p = (*p & mask) | zero;     zero == 0, hoisted with mask
 *
 * reproduces it exactly: loop.c hoists `zero` alongside the other three
 * invariants, the OR-with-zero is folded away afterwards, and the hoisted
 * register survives to the allocator with nothing left to consume it.  Size
 * goes from four instructions short to EXACT and the residue from 207 to 15.
 * THE DISCRIMINATOR is that the dead register is written INSIDE a loop's
 * invariant block next to registers that ARE used: a dead register hoisted with
 * live ones came from an expression, not from a dead local.  Both existing
 * parks should be re-screened against this.
 *
 * WHAT CLOSED IT, in the order the mechanism sizes said to try them:
 *
 *   plain C                                                    186 (-4 insns)
 *   + every argument site pinned (over-evicted: no r8/sl)      212 (-10)
 *   + mask / bit / script named as loop invariants              201 (-2)
 *   + the OR sites written `unsigned char v = *p; *p = v|bit`   202/-4
 *   + `| zero` on the two AND sites                              15 (EXACT SIZE)
 *   + an empty asm before the slot pin at four sites              9
 *   + the two in-loop __Func_80925cc pinned r1 then r0            5
 *   + the third +0x18 store written INSIDE the first pin block    0
 *
 * THE EMPTY asm IS THE LEVER THE PIN ORDER CANNOT REACH.  Four sites want
 * `mov r1 / mov r2 / mov r0 / lsl r1 / lsl r2`; every one of SIXTY pin
 * permutations (three declaration orders x twenty legal assignment orders)
 * gives either `mov r0` FIRST or `mov r0` after the first `lsl`, never third.
 * The `-da` dump says why: reload SPLITS `q1 = 0x88 << 16` into two new insns
 * with fresh UIDs, and the ready list at the relevant clock holds only the
 * slot's insn, so no source order can put it third.  `__asm__ __volatile__("")`
 * between the two shifted assignments and the slot assignment splits the
 * scheduling region in half; each half then comes out in source order and the
 * join is the ROM's.  This is the same device src/overlays/rom_7aa430/
 * ovl_1150_c_c_a.c records two functions earlier in the same overlay -- NEW
 * here in that it is used to place a THIRD argument between two split constant
 * builds rather than to break a two-load tie.  Its position is exact: before
 * the slot assignment is 9, after it 21, on both sides 21, at the end 21.
 *
 * THE FIRST SetPos SITE MUST NOT HAVE THE BARRIER, because the ROM interleaves
 * an unrelated STORE into it: `mov r1 / mov r2 / str r5,[r0,#0x18] / lsl r1 /
 * mov r0,#3 / lsl r2`.  Writing `*(int *)(p + 0x18) = 0xffff0000;` INSIDE the
 * pin block, between the two constant assignments and the shifts, with the slot
 * assignment between the two shifts, is exact -- and adding a barrier there
 * costs 3.  A barrier and an interleave are opposites; the ROM says which one
 * each site wants.
 *
 * THE OR SITES NEED A NAMED LOAD, THE AND SITES DO NOT.  `*p = *p | bit` emits
 * a copy (`add r3, r6, #0 / orr r3, r2`) against the ROM's `ldrb r3 / orr r3,
 * r6`, four bytes long over two sites; `{ unsigned char v = *p; *p = v | bit; }`
 * is exact.  The AND sites need nothing because their invariant is in a HIGH
 * register, so the ROM copies it too (`mov r3, r8 / and r3, r2`) and the shapes
 * coincide.  The asymmetry is the register class, not the operator.
 *
 * TWO PIN CLASSES ARE INTERCHANGEABLE.  With everything else in place, the six
 * __Func_809218c pins and the two __Func_8092adc pins can be dropped -- EITHER
 * set, not both: dropping 809218c alone is exact, dropping 8092adc alone is
 * exact, dropping both is 212 and four instructions long.  Dropping the
 * __MapActor_Emote pin is free from either base.  This is the recorded
 * "minimal only w.r.t. the base" rule at CLASS scale rather than site scale.
 * The shipped set drops the six 809218c pins and keeps the two 8092adc ones.
 *
 * `-fno-schedule-insns2` is a REGRESSION, 27 -> 57 against a reference edited to
 * remove the dead register, so sched2 is right and alias is the wrong axis --
 * consistent with every lever above being a region or an allocation, and none
 * being a type.
 *
 * MEASURED WORSE / INERT (against 225 encodings / 572 bytes):
 *
 *   spelling                                              differing
 *   ----------------------------------------------------  ---------
 *   plain C                                            186 (-4 insns)
 *   all argument sites pinned, no named invariants      212 (-10 insns)
 *   `| zero` dropped                                    213 (-4 insns)
 *   the script pointer not named                        213 (-4 insns)
 *   `mask` written as a literal                              95
 *   `bit` written as a literal                          99 (+2 insns)
 *   the OR sites written `*p = *p | bit`               114 (+4 insns)
 *   the SetSpeed(0, 0xcccc, 0x6666) pin dropped        176 (+2 insns)
 *   `volatile int zero`                                210 (-2 insns)
 *   a second unused counter incremented in the loop     207 (-4 insns)
 *   the four barriers dropped (SetPos / SetSpeed)             6 / 6
 *   the 8092adc barrier dropped                               4
 *   the two in-loop __Func_80925cc pins dropped               4
 *   the __Func_80921c4 pin dropped                            3
 *   the two stack-argument pins dropped                       3
 *   the first SetPos site written as an ordinary pin block    5
 *   INERT: `*p |= bit` / `*p = bit | *p` / `*p = *p | bit` (all the
 *     same); `bit` declared unsigned char or unsigned int; every one
 *     of the sixty SetPos/SetSpeed pin permutations (floor 15, two
 *     values only); every one of the eight 8092adc permutations.
 *
 * Harness: scratch_elev/b255/a2 -- cmp2.sh (objcmp --func), d2.sh (side-by-side
 * objdump diff), d3.sh + ref_nor10.s (the same against a reference edited to
 * delete the dead sl, which isolated the ordering residue from the register),
 * dump.sh (-da, for the sched2 ready lists), batch.py + b2.sh (many candidates
 * in one container).
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __MapActor_SetBehavior(int slot, void *s);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __Func_80925cc(int slot, int a);
extern void __Func_809218c(int slot, int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093500(int a, int b);
extern void __Func_8093530(void);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern unsigned char gScript_923__0200a820[];
extern unsigned char gScript_923__0200a8c8[];
extern unsigned char gScript_884__0200a874[];
extern void OvlFunc_923_2008d58(void);


#define SP(SLOT, A, SA, B, SB) do { \
    register int q1 __asm__("r1"); register int q2 __asm__("r2"); \
    register int q0 __asm__("r0"); \
    q1 = (A); q2 = (B); \
    __asm__ __volatile__(""); \
    q0 = (SLOT); q1 <<= (SA); q2 <<= (SB); \
    __MapActor_SetPos(q0, q1, q2); } while (0)

#define SS(SLOT, A, SA, B, SB) do { \
    register int q1 __asm__("r1"); register int q2 __asm__("r2"); \
    register int q0 __asm__("r0"); \
    q1 = (A); q2 = (B); \
    __asm__ __volatile__(""); \
    q0 = (SLOT); q1 <<= (SA); q2 <<= (SB); \
    __MapActor_SetSpeed(q0, q1, q2); } while (0)

#define F218C(SLOT, A, B) __Func_809218c((SLOT), (A), (B))

#define FADC(SLOT, A, SA, B) do { \
    register int q1 __asm__("r1"); register int q2 __asm__("r2"); \
    register int q0 __asm__("r0"); \
    q1 = (A); \
    __asm__ __volatile__(""); \
    q2 = (B); q0 = (SLOT); q1 <<= (SA); \
    __Func_8092adc(q0, q1, q2); } while (0)

void OvlFunc_923_2009730(void)
{
    unsigned char *p;
    int i;
    int s0, s1;
    int mask, bit, zero;
    void *sc;

    __CutsceneStart();
    p = __MapActor_GetActor(0xc);
    *(int *)(p + 0x18) = 0xffff0000;
    p = __MapActor_GetActor(0xd);
    *(int *)(p + 0x18) = 0xffff0000;
    p = __MapActor_GetActor(0xe);
    {
        register int q1 __asm__("r1"); register int q2 __asm__("r2");
        register int q0 __asm__("r0");
        q1 = 0x88; q2 = 0xb8;
        *(int *)(p + 0x18) = 0xffff0000;
        q1 <<= 16; q0 = 3; q2 <<= 16;
        __MapActor_SetPos(q0, q1, q2);
    }
    SP(0, 0x88, 16, 0x94, 17);
    SP(8, 0x88, 16, 0x98, 16);
    SS(3, 0xc0, 9, 0xc0, 8);
    SS(8, 0xc0, 9, 0xc0, 8);
    { register int q2 __asm__("r2"); register int q0 __asm__("r0");
      register int q1 __asm__("r1");
      q2 = 0x6666; q0 = 0; q1 = 0xcccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80933d4(0xcccc, 0x1999);
    __Func_80933f8(0x88 << 16, -1, 0xb8 << 16, 0);
    __MapTransitionIn();
    __WaitMapTransition();
    __Func_80925cc(3, 1);
    p = __MapActor_GetActor(8);
    p += 0x5a;
    *p = *p & 0xfe;
    __CutsceneWait(0x14);
    mask = 0xfe;
    sc = gScript_923__0200a820;
    zero = 0;
    bit = 1;
    i = 1;
    do {
        F218C(3, 0x98, 0xa8);
        __CutsceneWait(0xa);
        __MapActor_SetBehavior(8, gScript_923__0200a8c8);
        __MapActor_WaitMovement(3);
        FADC(3, 0xc0, 8, 0x1e);
        { register int c1 __asm__("r1"); register int c0 __asm__("r0");
          c1 = 1; c0 = 3; __Func_80925cc(c0, c1); }
        p = __MapActor_GetActor(3);
        p += 0x5a;
        *p = (*p & mask) | zero;
        F218C(3, 0x88, 0xb8);
        __CutsceneWait(0xa);
        __MapActor_SetBehavior(8, sc);
        __MapActor_WaitMovement(3);
        p = __MapActor_GetActor(3);
        p += 0x5a;
        { unsigned char v = *p; *p = v | bit; }
        __CutsceneWait(0x1e);
        F218C(3, 0x78, 0xa8);
        __CutsceneWait(5);
        __MapActor_SetBehavior(8, gScript_884__0200a874);
        __MapActor_WaitMovement(3);
        FADC(3, 0xc0, 8, 0x1e);
        { register int c1 __asm__("r1"); register int c0 __asm__("r0");
          c1 = 1; c0 = 3; __Func_80925cc(c0, c1); }
        __CutsceneWait(0xf);
        p = __MapActor_GetActor(3);
        p += 0x5a;
        *p = (*p & mask) | zero;
        F218C(3, 0x88, 0xb8);
        __CutsceneWait(0xf);
        __MapActor_SetBehavior(8, sc);
        __MapActor_WaitMovement(3);
        __Func_80925cc(3, 1);
        p = __MapActor_GetActor(3);
        p += 0x5a;
        { unsigned char v = *p; *p = v | bit; }
        --i;
    } while (i >= 0);
    __CutsceneWait(0x14);
    __MapActor_Emote(3, 0x81 << 1, 0x3c);
    p = __MapActor_GetActor(3);
    *(void **)(p + 0x6c) = OvlFunc_923_2008d58;
    __Func_8093500(0, 1);
    __CutsceneWait(0x1e);
    { register int q2 __asm__("r2"); register int q0 __asm__("r0");
      register int q1 __asm__("r1");
      q2 = 0x84; q0 = 0; q1 = 0x88; q2 <<= 1;
      __Func_80921c4(q0, q1, q2); }
    __Func_8093530();
    s0 = 7;
    s1 = 9;
    __Func_8010704(0, 0, 3, 3, s0, s1);
    __CutsceneEnd();
}
