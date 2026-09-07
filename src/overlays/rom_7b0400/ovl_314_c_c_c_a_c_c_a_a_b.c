// fakematch
/* OvlFunc_925_2008b24  --  0x02008b24   [FIRST of two in
 *   asm/overlays/rom_7b0400/ovl_314_c_c_c_a_c_c_a_a.s, lines 7-1592 -- 1563
 *   instructions of straight-line cutscene script, 409 call sites over 26
 *   distinct callees, two __Func_8091c7c diamonds and three guarded
 *   __MapActor_GetActor blocks]
 *
 *   XX ENCODINGS differ in 1 place(s) (ref 1590, ours 1590)
 *      first at index 1587: ref 0000003a  ours 00000000
 *   XX RELOCATIONS differ            <- ours-only: 00000fc0 R_ARM_ABS32 _AREA_3a
 *
 *   ... and that ONE word is the SAME VALUE.  Linking both objects with every
 *   undefined symbol defsym'd and `_AREA_3a = 0x3a` (which is what
 *   stage1.ld:17's `INCLUDE "area.sym"` supplies) makes the two .text sections
 *   BYTE-IDENTICAL, 4588 bytes.  Re-run three times, with two negative
 *   controls: `_AREA_3a = 0x3b` differs in exactly one byte, and the
 *   5-differing candidate below differs in eight.  Harness:
 *   scratch_elev/b253/rich/t2/linkchk.py.  SIZE and instruction COUNT already
 *   agree without linking (4044 bytes, 1590 encodings).
 *
 * WHY objcmp CANNOT SAY OK.  The ROM's disassembly writes `ldr r0, =0x3a` --
 * an unsymbolised pool word -- where the original source passed an area id.
 * 0x3a is BELOW 256, so a plain literal would be `mov r0, #58` and no pool word
 * at all; the ROM pooling it is the recorded textbook pooled-small-constant
 * tell, and `_AREA_3a` already exists in area.sym.  Two landed files in the
 * tree pass `(int) (&_AREA_51)` / `(int) (&_AREA_4f)` to this same
 * __Func_8091f90, and src/non_matching/ovl_7aa430/20091b4.c records the same
 * reading for _AREA_35.  Ours therefore carries a relocation the reference .s
 * does not, and objcmp compares relocations by name.  The literal spelling is
 * measurably WORSE, not better: `0x3a` inline is 5 differing, ONE INSTRUCTION
 * SHORT and FOUR BYTES SHORT.
 *
 * THE BARRIER IS THE LEVER FOR TWO POOLED LOADS IN SWAPPED REGISTERS -- NEW.
 * The tail is `ldr r3, =gState / ldr r2, =0x22b / add r3, r2 / mov r2, #3 /
 * strb r2, [r3]`, and gcc emits the two loads in the opposite registers, which
 * also flips the two pool words.  This is exactly the blocker
 * src/non_matching/ovl_7aa430/20091b4.c is parked on, with the SAME two values
 * and the SAME store, where a four-rung ladder (naming the offset, naming the
 * address, folding the addition, splitting the base) tied or lost.  Every one
 * of those rungs ties here too, at 5 differing.  ONE `do { } while (0);` in
 * front of the block is 1 -- the whole instruction stream becomes exact.  An
 * `__asm__ volatile ("" : "=r"(gp) : "0"(gp))` on the base is equally exact,
 * and the wall placed BETWEEN the two assignments works as well as in front of
 * them.  Barrier budget: ONE.  The recorded ladder for this class was missing
 * its cheapest rung; the pooled-load-order park is worth re-running with a wall.
 *
 * A BARRIER IS NOT ALWAYS RIGHT FOR AN ORDERING RESIDUE, and this batch has the
 * counter-example beside it: in the sibling ovl_30_c_a_c_c_c_a_a.c a 2-differing
 * two-pooled-operand residue got WORSE with a wall in all three placements and
 * needed a pin DROPPED instead.  The discriminator is what the two operands
 * are: TWO POOL WORDS feeding one `add` want the wall; a POOL WORD AND AN
 * IMMEDIATE feeding one call want the pin removed.
 *
 * THE LADDER, in the order the mechanism sizes said to try it:
 *
 *   plain C, no pins                                       1407  (r7 commoned;
 *                                                                `push {r5, r6,
 *                                                                r7, lr}`)
 *   + all 373 pinnable sites pinned                          17
 *   + the six calls INSIDE the two diamonds pinned too       11
 *   + `(int) (&_AREA_3a)` for the pooled area id              5
 *   + `do { } while (0);` before the gState store             1  <- the word
 *
 * hi = 0, hiv = 0, and the push mask is again the predictor: unpinned gcc
 * commons a shifted constant into r7 and the prologue reads `push {r5, r6, r7,
 * lr}` against the ROM's `push {r5, r6, lr}`.
 *
 * THE DIAMOND ARMS ARE ORDINARY PIN SITES.  Six of the seventeen differing at
 * the all-pinned stage were three statements repeated in both arms of the first
 * __Func_8091c7c diamond, each `mov r1 / mov r0 / lsl r1` in the ROM against
 * `mov r1 / lsl r1 / mov r0` in ours.  They were differing only because the
 * first harness emitted both arms as raw text and so could not pin them.  A
 * conditional arm is not a special region: give it the same treatment as
 * straight-line code.
 *
 * WHICH CALLEES WANT PINS, MEASURED BY DROPPING A WHOLE CLASS AT ONCE.  This is
 * the cheapest minimisation there is on a 373-pin function -- 26 builds -- and
 * it removed 219 pins before a single per-site drop:
 *
 *   callee                    pinned  of    callee                  pinned of
 *   ------------------------  ------------  ----------------------- ---------
 *   __Func_8092adc                56  73    __CutsceneWait               0  74
 *   __MapActor_Emote              18  19    __Func_8093040               0  45
 *   __MapActor_SetSpeed           12  14    __MapActor_DoAnim            0  30
 *   __Func_80921c4                11  16    __Func_80925cc               0  22
 *   __MapActor_SetPos              9  10    __ActorMessage               0   9
 *   __Func_809218c                 6   8    __MapActor_WaitMovement      0   8
 *   __Func_80933d4                 4   4    __Func_809259c               0   8
 *   __Func_80933f8                 3   7    __MapActor_SetAnim           0   6
 *   __MapActor_Surprise            1   3    __Func_8092b08               0   4
 *                                           __Func_8092848               0   3
 *                                           __PlaySound                  0   3
 *                                           __Func_8092c40               0   2
 *                                           the five singleton callees   0   5
 *
 * The split is not by argument COUNT, it is by whether any argument is a
 * SHIFTED or POOLED constant: every zero-pin callee here is called with bare
 * small immediates only.  Sixteen of the twenty-six classes need nothing.
 * Greedy per-site dropping then ran to a fixpoint (two rounds, 154 -> 120 ->
 * 120).  MINIMISED FROM BOTH ENDS: a reverse pass run from the full 373 with NO
 * class pass reaches the IDENTICAL 120 sites, so this set is canonical -- which
 * the sibling ovl_30_c_a_c_c_c_a_a.c's is not (it agrees on 25 from both ends
 * but not on which 25).
 *
 * THE MASKS 0xfe AND 1 ARE NOT NAMED, AND THE STRUCT IS.  The ROM keeps 0xfe in
 * r5 and 1 in r6 across the middle third for eight `->f5a &= 0xfe` / `|= 1`
 * pairs, and reaches the byte as an aggregate member, exactly the recorded
 * batch-240 discriminator that the sibling ovl_30_c_c_c_c_a_a_a_c_c_c_a.c
 * settled: `&=` matches through a pointer, `|=` does not, and the struct member
 * gets both.  Writing the masks as literals at every site is what produces the
 * ROM's commoning -- naming them is not needed and was not used.  f28 and f55
 * go through the SAME struct here, unlike the sibling where widening the struct
 * beyond f5a cost 642 differing; the difference is that f55's three sites are
 * `&=`, `= 4` and `= 3` byte operations rather than word stores.
 *
 * Harness: scratch_elev/b253/rich/t2 -- conv.py (ref.s -> body.py, folding the
 * bit-op / guarded-GetActor / iwram-counter idioms automatically; 0 unparsed
 * argument builds out of 409 sites), gen2.py + gen3.sh (pin/width/perm
 * generator), drop.py (in-container class pass then greedy fixpoint, judged on
 * LINKED bytes), drop_rev.py (the same from the other end), linkchk.py,
 * relchk.py, d.sh.
 */
struct Actor {
    unsigned char pad00[0x28];
    int f28;
    unsigned char pad2c[0x29];
    unsigned char f55;
    unsigned char pad56[4];
    unsigned char f5a;
};

typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char *iwram_3001ebc;
extern int _AREA_3a;
extern void OvlFunc_925_200b208(void);
extern void __ActorMessage(int a0, int a1);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __CutsceneEnd(void);
extern void __CutsceneStart(void);
extern void __CutsceneWait(int a0);
extern void __Func_8091eb0(int a0, int a1);
extern void __Func_8091f90(int a0, int a1);
extern void __Func_8092158(int a0, int a1, int a2);
extern void __Func_809218c(int a0, int a1, int a2);
extern void __Func_80921c4(int a0, int a1, int a2);
extern void __Func_809259c(int a0, int a1);
extern void __Func_80925cc(int a0, int a1);
extern void __Func_8092848(int a0, int a1, int a2);
extern void __Func_8092adc(int a0, int a1, int a2);
extern void __Func_8092b08(int a0, int a1);
extern int __Func_8092c40(int a, int b);
extern void __Func_8093040(int a0, int a1, int a2);
extern void __Func_80933d4(int a0, int a1);
extern void __Func_80933f8(int a0, int a1, int a2, int a3);
extern void __Func_8093530(void);
extern void __MapActor_DoAnim(int a0, int a1);
extern void __MapActor_Emote(int a0, int a1, int a2);
extern void __MapActor_SetAnim(int a0, int a1);
extern void __MapActor_SetPos(int a0, int a1, int a2);
extern void __MapActor_SetSpeed(int a0, int a1, int a2);
extern void __MapActor_Surprise(int a0, int a1);
extern void __MapActor_WaitMovement(int a0);
extern void __MessageID(int a0);
extern void __PlaySound(int a0);
extern void __WaitFrames(int a0);
extern int __Func_8091c7c(int a, int b);
extern unsigned char *__Func_8093554(void);
extern unsigned char *__MapActor_GetActor(int a);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_925_2008b24(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned int b3;
    unsigned int b2;

    __CutsceneStart();
    ((struct Actor *)__MapActor_GetActor(0x11))->f55 &= 0xfa;
    { PIN3; q0 = 0x0; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    p = __MapActor_GetActor(0x0);
    if (p != 0)
        __MapActor_SetPos(0x1, *(int *)(p + 8), *(int *)(p + 0x10));
    p = __MapActor_GetActor(0x0);
    if (p != 0)
        __MapActor_SetPos(0x2, *(int *)(p + 8), *(int *)(p + 0x10));
    p = __MapActor_GetActor(0x0);
    if (p != 0)
        __MapActor_SetPos(0x3, *(int *)(p + 8), *(int *)(p + 0x10));
    __WaitFrames(0x1);
    { PIN3; q0 = 0x0; q1 = 0xac << 1; q2 = 0xe8;
      __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xa4 << 1; q2 = 0xe8;
      __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xac << 1; q2 = 0xf8;
      __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0xa4 << 1; q2 = 0xf8;
      __Func_809218c(q0, q1, q2); }
    __MapActor_WaitMovement(0x0);
    { PIN3; q0 = 0x0; q1 = 0xa0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_WaitMovement(0x1);
    { PIN3; q0 = 0x1; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_WaitMovement(0x2);
    { PIN3; q0 = 0x2; q1 = 0xa0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_WaitMovement(0x3);
    { PIN3; q0 = 0x3; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x32);
    __Func_80925cc(0x1, 0x2);
    __CutsceneWait(0x14);
    { PIN2; q0 = 0xc0 << 9; q1 = 0xc0 << 6;
      __Func_80933d4(q0, q1); }
    __Func_80933f8(0xa4 << 17, 0xa0 << 14, 0xb0 << 16, 0x1);
    q = __Func_8093554();
    q[0x55] = 0;
    { PIN3; q0 = 0x1; q1 = 0xc0 << 9; q2 = 0xc0 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xa4 << 1; q2 = 0xd8;
      __Func_80921c4(q0, q1, q2); }
    __Func_8093530();
    __MapActor_SetSpeed(0x1, 0xcccc, 0x6666);
    __CutsceneWait(0x3c);
    __Func_80933f8(0xac << 17, 0xc0 << 13, 0xe8 << 16, 0x1);
    { PIN3; q0 = 0x1; q1 = 0x80 << 6; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093530();
    __MapActor_DoAnim(0x1, 0x4);
    __MessageID(0x159c);
    __Func_8093040(0x1, 0x0, 0x14);
    __MapActor_Surprise(0x3, 0x81 << 1);
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0x3; q1 = 0xc0 << 9; q2 = 0xc0 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(0x3, 0xa4 << 1, 0xe8);
    __CutsceneWait(0xa);
    __Func_8093040(0x3, 0x0, 0x14);
    __MapActor_DoAnim(0x3, 0x4);
    __CutsceneWait(0x14);
    __Func_8093040(0x3, 0x0, 0x1e);
    __Func_8092848(0x2, 0x0, 0x0);
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0x2; q1 = 0xa8 << 1; q2 = 0xf8;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xa0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x0, 0x80 << 8, 0x0);
    __CutsceneWait(0x1e);
    __Func_8093040(0x2, 0x0, 0x14);
    __Func_8092adc(0x3, 0x0, 0x1e);
    __Func_8092c40(0x3, 0x0);
    if (__Func_8091c7c(0, 0) == 0) {
            __CutsceneWait(0x14);
            { PIN3; q0 = 0x3; q1 = 0x80 << 1; q2 = 0x3c;
              __MapActor_Emote(q0, q1, q2); }
            { PIN3; q0 = 0x1; q1 = 0x80 << 7; q2 = 0xa;
              __Func_8092adc(q0, q1, q2); }
            __MapActor_DoAnim(0x1, 0x3);
            __Func_8093040(0x1, 0x0, 0x14);
            { PIN3; q0 = 0x3; q1 = 0xc0 << 8; q2 = 0x0;
              __Func_8092adc(q0, q1, q2); }
            __MapActor_DoAnim(0x1, 0x4);
            __CutsceneWait(0x14);
            __Func_8093040(0x1, 0x0, 0x14);
        *(unsigned short *)(iwram_3001ebc + 0x1d8) += 2;
    } else {
        *(unsigned short *)(iwram_3001ebc + 0x1d8) += 2;
            __CutsceneWait(0x14);
            { PIN3; q0 = 0x3; q1 = 0x101; q2 = 0x3c;
              __MapActor_Emote(q0, q1, q2); }
            { PIN3; q0 = 0x1; q1 = 0x80 << 7; q2 = 0xa;
              __Func_8092adc(q0, q1, q2); }
            __MapActor_DoAnim(0x1, 0x4);
            __Func_8093040(0x1, 0x0, 0x14);
            { PIN3; q0 = 0x3; q1 = 0xc0 << 8; q2 = 0x0;
              __Func_8092adc(q0, q1, q2); }
            __Func_80925cc(0x1, 0x2);
            __CutsceneWait(0x14);
            __Func_8093040(0x1, 0x0, 0x14);
    }
    { PIN3; q0 = 0x3; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x3, 0x0, 0x14);
    __Func_8092848(0x0, 0x1, 0x0);
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0x0; q1 = 0x80 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x80 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0x0, 0x3);
    __MapActor_DoAnim(0x1, 0x3);
    __CutsceneWait(0x14);
    __Func_8093040(0x1, 0x0, 0x14);
    __Func_80925cc(0x3, 0x2);
    __Func_8093040(0x3, 0x0, 0x14);
    __MapActor_SetAnim(0x0, 0x3);
    __MapActor_SetAnim(0x1, 0x3);
    __MapActor_DoAnim(0x2, 0x3);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x3, 0x4);
    __CutsceneWait(0xa);
    __Func_8092adc(0x3, 0x0, 0xa);
    __MapActor_SetAnim(0x3, 0x10);
    __Func_8093040(0x3, 0x0, 0x3c);
    __PlaySound(0x11);
    { PIN3; q0 = 0x5; q1 = 0xd8 << 16; q2 = 0xc8 << 16;
      __MapActor_SetPos(q0, q1, q2); }
    __Func_8093040(0x5, 0x0, 0x14);
    { PIN3; q0 = 0x5; q1 = 0xf0 << 15; q2 = 0xa0 << 16;
      __MapActor_SetPos(q0, q1, q2); }
    __Func_8092adc(0x5, 0x0, 0x0);
    __MapActor_SetAnim(0x3, 0x1);
    __Func_809259c(0x0, 0x1);
    __Func_80925cc(0x1, 0x1);
    __Func_8092adc(0x0, 0x0, 0xa);
    { PIN3; q0 = 0x1; q1 = 0xe0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xa0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0x80 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x0; q1 = 0xa0 << 8; q2 = 0x5;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xc0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x80 << 8; q2 = 0x5;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x0; q1 = 0x80 << 8; q2 = 0x5;
      __Func_8092adc(q0, q1, q2); }
    __Func_809259c(0x0, 0x2);
    { PIN3; q0 = 0x1; q1 = 0x80 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x1, 0x2);
    __CutsceneWait(0x14);
    { PIN2; q0 = 0xc0 << 10; q1 = 0xc0 << 7;
      __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0xf0 << 15; q1 = 0xffe80000; q2 = 0xa8 << 16; q3 = 0x1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x28);
    __Func_80925cc(0x15, 0x1);
    __CutsceneWait(0x14);
    __PlaySound(0x3d);
    __Func_8093040(0x15, 0x0, 0x14);
    __Func_80925cc(0x17, 0x1);
    __CutsceneWait(0x14);
    __Func_8093040(0x17, 0x0, 0x14);
    { PIN3; q0 = 0x17; q1 = 0xc0 << 8; q2 = 0x3c;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x17, 0x0, 0x14);
    __Func_8093040(0x17, 0x0, 0x14);
    { PIN3; q0 = 0x1; q1 = 0x84 << 17; q2 = 0x90 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    __Func_8093040(0x1, 0x0, 0x14);
    { PIN3; q0 = 0x1; q1 = 0xa4 << 17; q2 = 0xd8 << 16;
      __MapActor_SetPos(q0, q1, q2); }
    __MapActor_DoAnim(0x17, 0x4);
    __CutsceneWait(0x14);
    __Func_8093040(0x17, 0x0, 0x14);
    __MapActor_DoAnim(0x17, 0x3);
    __CutsceneWait(0x14);
    __Func_8093040(0x17, 0x0, 0x14);
    { PIN3; q0 = 0x17; q1 = 0xa0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x15; q1 = 0x80 << 6; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x15, 0x1);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x5; q1 = 0x80 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x14; q1 = 0x80 << 8; q2 = 0x1e;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x15, 0x0, 0x1e);
    __Func_80925cc(0x5, 0x2);
    __MapActor_DoAnim(0x15, 0x4);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x15; q1 = 0x80 << 6; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x15, 0x3);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x17, 0x3);
    { PIN3; q0 = 0x15; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(0x15, 0x68, 0xa8);
    __CutsceneWait(0x14);
    __Func_8092adc(0x17, 0x0, 0x14);
    __Func_8092adc(0x14, 0x0, 0xa);
    { PIN3; q0 = 0x5; q1 = 0x81 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x14; q1 = 0x81 << 1; q2 = 0x46;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x16; q1 = 0x84 << 17; q2 = 0x90 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    __Func_8093040(0x16, 0x0, 0x14);
    { PIN3; q0 = 0x16; q1 = 0x94 << 17; q2 = 0xf0 << 15;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x0; q1 = 0xa0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x5, 0x0, 0x0);
    __Func_8092adc(0x14, 0x0, 0x0);
    { PIN2; q0 = 0xc0 << 9; q1 = 0xc0 << 6;
      __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0xe8 << 16; q1 = 0xa0 << 14; q2 = 0x98 << 16; q3 = 0x1;
      __Func_80933f8(q0, q1, q2, q3); }
    { PIN3; q0 = 0x16; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x16; q1 = 0x88 << 1; q2 = 0x80;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x16; q1 = 0x84 << 1; q2 = 0x98;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x16; q1 = 0x8c << 1; q2 = 0xa8;
      __Func_80921c4(q0, q1, q2); }
    __Func_8093530();
    { PIN3; q0 = 0x16; q1 = 0xa0 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __ActorMessage(0x17, 0x0);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x16, 0x3);
    __CutsceneWait(0x14);
    __ActorMessage(0x16, 0x0);
    { PIN4; q0 = 0x94 << 17; q1 = 0xa0 << 14; q2 = 0xd8 << 16; q3 = 0x1;
      __Func_80933f8(q0, q1, q2, q3); }
    { PIN3; q0 = 0x16; q1 = 0x98 << 1; q2 = 0xb0;
      __Func_80921c4(q0, q1, q2); }
    __Func_8092adc(0x16, 0x80 << 6, 0x0);
    __Func_8093530();
    __CutsceneWait(0x14);
    __Func_809259c(0x0, 0x1);
    __Func_809259c(0x1, 0x1);
    __Func_809259c(0x2, 0x1);
    __Func_80925cc(0x3, 0x1);
    __CutsceneWait(0x14);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x16, 0x3);
    __CutsceneWait(0x14);
    __ActorMessage(0x16, 0x0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x0; q1 = 0x107; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x107; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0x107; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x107; q2 = 0x46;
      __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(0x16, 0x1);
    __CutsceneWait(0x1e);
    __ActorMessage(0x16, 0x0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x0; q1 = 0x105; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x105; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x105; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0x105; q2 = 0x46;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_DoAnim(0x16, 0x3);
    __CutsceneWait(0x14);
    __ActorMessage(0x16, 0x0);
    __CutsceneWait(0x14);
    __ActorMessage(0x17, 0x0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x16; q1 = 0xa0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x28);
    __ActorMessage(0x16, 0x0);
    __CutsceneWait(0x1e);
    __ActorMessage(0x17, 0x0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x16; q1 = 0xc0 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __ActorMessage(0x16, 0x0);
    __CutsceneWait(0x14);
    ((struct Actor *)__MapActor_GetActor(0x1))->f5a &= 0xfe;
    { PIN3; q0 = 0x1; q1 = 0xa4 << 1; q2 = 0xe0;
      __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 0x0; q1 = 0xac << 1; q2 = 0xe0;
      __Func_809218c(q0, q1, q2); }
    __Func_809218c(0x2, 0xac << 1, 0xe8);
    __MapActor_WaitMovement(0x1);
    ((struct Actor *)__MapActor_GetActor(0x1))->f5a |= 0x1;
    { PIN3; q0 = 0x1; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_WaitMovement(0x0);
    { PIN3; q0 = 0x0; q1 = 0xa0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_WaitMovement(0x2);
    { PIN3; q0 = 0x2; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0x17; q1 = 0xa8 << 16; q2 = 0xc8 << 16;
      __MapActor_SetPos(q0, q1, q2); }
    __Func_8093040(0x17, 0x0, 0x14);
    { PIN3; q0 = 0x17; q1 = 0xd0 << 15; q2 = 0xc8 << 16;
      __MapActor_SetPos(q0, q1, q2); }
    __Func_80925cc(0x16, 0x1);
    __CutsceneWait(0x14);
    __Func_8093040(0x16, 0x0, 0x14);
    { PIN2; q0 = 0xc0 << 10; q1 = 0xc0 << 7;
      __Func_80933d4(q0, q1); }
    __Func_80933f8(0xf0 << 15, 0xffe80000, 0xa8 << 16, 0x1);
    __Func_8093530();
    __MapActor_DoAnim(0x17, 0x3);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x17; q1 = 0xd0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x17, 0x3);
    __CutsceneWait(0x14);
    __Func_8093040(0x17, 0x0, 0x14);
    { PIN3; q0 = 0x5; q1 = 0xa0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x14; q1 = 0xc0 << 6; q2 = 0x46;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092848(0x5, 0x14, 0x0);
    __CutsceneWait(0x32);
    { PIN3; q0 = 0x5; q1 = 0xa0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x14; q1 = 0xc0 << 6; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x5, 0x4);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x17; q1 = 0x80 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x17, 0x0, 0x14);
    { PIN3; q0 = 0x5; q1 = 0x105; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x15; q1 = 0xd0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x15, 0x1);
    __MapActor_DoAnim(0x17, 0x3);
    __CutsceneWait(0x14);
    __Func_8093040(0x17, 0x0, 0x14);
    __MapActor_DoAnim(0x17, 0x4);
    __CutsceneWait(0x14);
    __Func_8093040(0x17, 0x0, 0x14);
    { PIN2; q0 = 0x15; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x3c);
    __MapActor_DoAnim(0x15, 0x4);
    __CutsceneWait(0x14);
    __Func_8093040(0x15, 0x0, 0x14);
    __MapActor_Surprise(0x5, 0x81 << 1);
    __CutsceneWait(0x3c);
    __Func_8093040(0x5, 0x0, 0x28);
    __MapActor_DoAnim(0x15, 0x3);
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0x5; q1 = 0xa0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x14, 0x3);
    __CutsceneWait(0x14);
    __Func_8093040(0x14, 0x0, 0x1e);
    { PIN3; q0 = 0x5; q1 = 0xb333; q2 = 0x5999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x14; q1 = 0xb333; q2 = 0x5999;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_809218c(0x5, 0x80, 0x90);
    __Func_80921c4(0x14, 0x78, 0x88);
    __Func_8092adc(0x14, 0xa0 << 7, 0x0);
    __MapActor_WaitMovement(0x5);
    __Func_8092adc(0x5, 0x0, 0x14);
    { PIN3; q0 = 0x15; q1 = 0xc0 << 6; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    ((struct Actor *)__MapActor_GetActor(0x15))->f5a &= 0xfe;
    __Func_80921c4(0x15, 0x58, 0x98);
    ((struct Actor *)__MapActor_GetActor(0x15))->f5a |= 0x1;
    { PIN3; q0 = 0x17; q1 = 0xb0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0x17, 0x3);
    __MapActor_DoAnim(0x15, 0x3);
    __CutsceneWait(0x28);
    { PIN3; q0 = 0x17; q1 = 0xc0 << 10; q2 = 0x80 << 10;
      __MapActor_SetSpeed(q0, q1, q2); }
    ((struct Actor *)__MapActor_GetActor(0x17))->f28 = 0x80 << 11;
    __PlaySound(0x98);
    ((struct Actor *)__MapActor_GetActor(0x17))->f55 &= 0x7e;
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x17), 0x0);
    ((struct Actor *)__MapActor_GetActor(0x11))->f55 = 0x4;
    __Func_8092158(0x17, 0x68, 0xa8);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x17), 0x1);
    ((struct Actor *)__MapActor_GetActor(0x17))->f55 = 0x3;
    __Func_8092adc(0x17, 0x0, 0x1e);
    __Func_8092adc(0x15, 0x0, 0xa);
    __Func_8093040(0x15, 0x0, 0x14);
    __Func_80925cc(0x5, 0x2);
    __MapActor_DoAnim(0x5, 0x4);
    __CutsceneWait(0x14);
    __Func_8093040(0x5, 0x0, 0x14);
    __Func_8093040(0x17, 0x0, 0x14);
    __Func_8092b08(0x11, 0x0);
    __Func_8092b08(0x12, 0x0);
    OvlFunc_925_200b208();
    __Func_8092b08(0x11, 0x1);
    __Func_8092b08(0x12, 0x1);
    { PIN3; q0 = 0x0; q1 = 0xc0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xc0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xc0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0xc0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_80933f8(0x98 << 17, 0x80 << 14, 0xd8 << 16, 0x1);
    __Func_8093530();
    { PIN3; q0 = 0x14; q1 = 0x88 << 17; q2 = 0x8c << 17;
      __MapActor_SetPos(q0, q1, q2); }
    __Func_8093040(0x14, 0x0, 0x14);
    __MapActor_SetPos(0x14, 0x0, 0x0);
    __Func_80925cc(0x1, 0x1);
    __CutsceneWait(0x14);
    __Func_8093040(0x1, 0x0, 0x14);
    { PIN3; q0 = 0x1; q1 = 0xc0 << 9; q2 = 0xc0 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x9c << 1; q2 = 0xd8;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xc0 << 7; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x16, 0x0, 0x14);
    { PIN3; q0 = 0x0; q1 = 0xa0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x16; q1 = 0x9c << 1; q2 = 0xb8;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x16; q1 = 0xc0 << 6; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x3, 0x0, 0x14);
    { PIN3; q0 = 0x1; q1 = 0x80 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_SetSpeed(0x1, 0x80 << 10, 0x80 << 9);
    ((struct Actor *)__MapActor_GetActor(0x1))->f5a &= 0xfe;
    __Func_80921c4(0x1, 0xa4 << 1, 0xe0);
    __CutsceneWait(0x1);
    ((struct Actor *)__MapActor_GetActor(0x1))->f5a |= 0x1;
    __Func_809259c(0x0, 0x1);
    __Func_809259c(0x1, 0x1);
    __Func_809259c(0x2, 0x1);
    __Func_80925cc(0x3, 0x1);
    __CutsceneWait(0x14);
    __Func_80925cc(0x16, 0x1);
    __CutsceneWait(0xa);
    __Func_8093040(0x16, 0x0, 0x14);
    __Func_80925cc(0x1, 0x2);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x1, 0x4);
    __CutsceneWait(0xa);
    __Func_8093040(0x1, 0x0, 0x14);
    __Func_80925cc(0x2, 0x1);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x2, 0x3);
    __CutsceneWait(0x14);
    __Func_8093040(0x2, 0x0, 0x14);
    { PIN3; q0 = 0x16; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8092c40(0x16, 0x0);
    if (__Func_8091c7c(0, 0) == 0) {
            __CutsceneWait(0x14);
            __MapActor_DoAnim(0x16, 0x4);
            __CutsceneWait(0x14);
            __Func_8093040(0x16, 0x0, 0x14);
        *(unsigned short *)(iwram_3001ebc + 0x1d8) += 1;
    } else {
            __CutsceneWait(0x14);
            __MapActor_DoAnim(0x16, 0x4);
            __CutsceneWait(0x14);
        *(unsigned short *)(iwram_3001ebc + 0x1d8) += 1;
            __Func_8093040(0x16, 0x0, 0x14);
    }
    __Func_80925cc(0x16, 0x2);
    __CutsceneWait(0x14);
    __Func_8093040(0x16, 0x0, 0x14);
    { PIN3; q0 = 0x16; q1 = 0xa4 << 1; q2 = 0xc8;
      __Func_80921c4(q0, q1, q2); }
    __Func_80925cc(0x16, 0x2);
    __Func_8092adc(0x16, 0xb0 << 8, 0x14);
    ((struct Actor *)__MapActor_GetActor(0x16))->f5a &= 0xfe;
    { PIN3; q0 = 0x16; q1 = 0xa8 << 1; q2 = 0xd0;
      __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(0x1);
    ((struct Actor *)__MapActor_GetActor(0x16))->f5a |= 0x1;
    __MapActor_Emote(0x16, 0x81 << 1, 0x3c);
    { PIN3; q0 = 0x3; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x3, 0x0, 0x14);
    __Func_8093040(0x16, 0x0, 0x14);
    { PIN3; q0 = 0x16; q1 = 0xa0 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x16, 0x2);
    __CutsceneWait(0x14);
    __Func_8093040(0x16, 0x0, 0x14);
    { PIN3; q0 = 0x16; q1 = 0xa8 << 1; q2 = 0xd8;
      __Func_80921c4(q0, q1, q2); }
    __Func_8091f90((int)(&_AREA_3a), 0x2);
    do { } while (0);
    b3 = (unsigned int)&gState;
    b2 = 0x22b;
    b3 += b2;
    *(unsigned char *)b3 = 3;
    __Func_8091eb0(0x24, 0x2);
    __CutsceneEnd();
}
