// fakematch
/* ovl_30_c_a_c_c_c_a_a.c  --  OvlFunc_884_2008940 + OvlFunc_884_2008bbc
 *   [the WHOLE of asm/overlays/rom_784360/ovl_30_c_a_c_c_c_a_a.s -- both
 *    `.thumb_func_start`s, so NO SPLIT is required and overlay.ld:46 stays
 *    VERBATIM as `asm/overlays/rom_784360/ovl_30_c_a_c_c_c_a_a.o(.text)`]
 *
 *   OK WHOLE TU -- 1860 bytes, 730 encodings and 175 relocations identical
 *   OK OvlFunc_884_2008940 --  636 bytes, 253 encodings and  55 relocations identical
 *   OK OvlFunc_884_2008bbc -- 1224 bytes, 477 encodings and 120 relocations identical
 *
 * Each line re-measured four times.  objcmp prints no `(built with: ...)`
 * line: adjust = set(), the tree default -O2 -mthumb -mthumb-interwork
 * -fcall-used-r4.  makefile_flags() on this path is the EMPTY set and the only
 * rom_784360 rule in the Makefile names ovl_30_c_a_a_a_c_c_a_c_c_b.o
 * literally, so no wildcard hazard; the rule that fires is `asm/%.o: src/%.c`.
 * The .s carries NO .section, .data, .bss, .lcomm, .word or .byte; all 29 `.L`
 * symbols are branch targets DEFINED IN THIS FILE, so nothing needs exporting.
 * FAKEMATCH: both names go in fakematch.txt (register-pin idiom).
 *
 * This is the sibling of ovl_30_c_a_c_c_c_a_b.c, split out of the same original
 * .s, and it confirms two of that file's findings and adds three.
 *
 * hi = 0, hiv = 0 ON BOTH, AND THE TWO BEHAVE COMPLETELY DIFFERENTLY.  2008940
 * is 19 of 253 differing AT THE EXACT SIZE from the first plain transcription,
 * with RELOCATIONS SILENT -- a pure ordering residue that five pin blocks
 * close.  2008bbc is 455 of 477 with `push {r5, r6, r7, lr}` against the ROM's
 * `push {r5, r6, lr}` -- the recorded "a push mask wider than the ROM's is a
 * COMMONING tell" -- and needs the whole ladder.  Size is not the predictor
 * here; the PUSH MASK is.
 *
 * WHAT CLOSED 2008bbc, in the order the mechanism sizes said to try them:
 *
 *   plain C                                                455  (+2 insns, r7 + sl)
 *   + 96 pins, the 10 OvlFunc_884_200a2e0 sites LEFT BARE  163
 *   + `f = 1` written BEFORE its __MapActor_SetAnim         161
 *   + `v` named and `v + 0x13` at the two a2c8 sites          5  (exact size)
 *   + the __Func_8092a1c fill written r2, r0, r1              2
 *   + the __MapActor_SetBehavior(0, af50) pin DROPPED         0
 *
 * THE 0x4013 IS `v + 0x13` IN THE SOURCE, AND THAT IS THE WHOLE LENGTH FIX.
 * The ROM builds 0x4000 as `mov r5, #0x80 / lsl r5, #7`, passes it to two
 * OvlFunc_884_200a2e0 calls, then turns the SAME register into the constant
 * 0x4013 with a bare `adds r5, #19` and passes THAT to two
 * OvlFunc_884_200a2c8 calls.  Written as four literals gcc pool-loads 0x4013
 * and the function comes out FOUR BYTES SHORT -- two of them the missing
 * `adds r5, #19`, two the alignment halfword that the odd instruction count
 * would have forced before the pool.  Naming the 0x4000 in a plain `int v` and
 * writing the later constant as `v + 0x13` is exact.  NEW, and the discriminator
 * is cheap to check on any function: A CANDIDATE THAT IS EXACTLY TWO HALFWORDS
 * SHORT WITH ONE FEWER INSTRUCTION IS A `use_related_value` SITE -- cse found a
 * live register holding a nearby constant and the C did not offer it one.
 *
 * `int v` MUST BE PLAIN.  `register int v __asm__("r5")` -- the ROM's own
 * register -- is 119 differing with the same `v + 0x13` spelling, and pinning
 * the second constant to r6 as well is 130 AND FOUR BYTES LONG.  The ROM's r5
 * and r6 assignments are the ALLOCATOR, not the source, exactly as the sibling
 * ovl_30_c_a_c_c_c_a_b.c records for its own c1/c2 -- but note the direction is
 * the OPPOSITE of that file, where one hard pin was the fix.  The
 * discriminator between the two cases is whether the value has ONE role (pin
 * it) or TWO (0x4000 then 0x4013 -- leave it plain and let cse relate them).
 *
 * THE TEN OvlFunc_884_200a2e0 SITES MUST NOT BE PINNED -- 244 against 163 with
 * everything else equal.  That is the sibling's recorded width-zero class, on
 * the same callee, at ten sites instead of three: `mov r1 / mov r2 / lsl r1 /
 * mov r0`, the r0 seed LAST.  Diagnosing one names the rest, and it did.
 *
 * TWO SITES WANT A NON-ASCENDING FILL, AND BOTH ARE __Func_8092a1c.  In
 * 2008bbc the last call before __SetFlag is `q2 = ...; q0 = ...; q1 = ...` and
 * in 2008940 the one inside the 0x840 guard is the same shape.  ONE CALLEE, ONE
 * FILL: the pooled script pointer has to be nominated before the two `mov`s or
 * sched2 sinks its `ldr` past them.  Ascending is 3 differing in 2008940 and 2
 * in 2008bbc.  This is the fill-order rule earning its keep as a CALLEE CLASS
 * for a second callee.
 *
 * ONE PIN HAD TO BE DROPPED, AND A BARRIER WAS THE WRONG TOOL.  With everything
 * else right, `__MapActor_SetBehavior(0, gScript_884__0200af50)` sits at 2
 * differing, relocations silent: the ROM emits `ldr r1, =script` then
 * `mov r0, #0` and the pinned form emits them the other way round.  WRITING THE
 * FILL REVERSED DOES NOT HELP -- sched2 normalises it back and perturbs the two
 * preceding sites for 64 differing.  `do { } while (0)` in front of the call is
 * 7, in front of the next call 7, in front of the previous 4: EVERY BARRIER IS
 * WORSE.  Simply removing the pin is exact.  Its neighbour
 * __MapActor_RunScript one line later, the same two-argument symbol-plus-slot
 * shape, KEEPS its pin; the difference is that this one's r0 is `#0`.
 *
 * THE SIX-ARGUMENT CALLS IN 2008940 NEED r3 AND r2 PINNED FOR THE *STACK*
 * ARGUMENTS.  __CopyMapTiles and __Func_8010704 take six; the ROM builds the
 * fifth and sixth in TWO scratch registers and then stores both
 * (`mov r3, #3 / mov r2, #1 / str r3, [sp] / str r2, [sp, #4]`), while gcc
 * recycles r3 for each (`mov r3 / str / mov r3 / str`).  Three sites, three
 * instructions each, nine of the seventeen residue.  `register int s0
 * __asm__("r3"); register int s1 __asm__("r2");` assigned in that order and
 * passed as the last two arguments is exact.  NEW: the pin idiom is not
 * confined to argument registers -- a STACK argument can need a pin too, and
 * the tell is `mov rX / str / mov rX / str` against a two-register ROM.  The
 * two sites whose fifth and sixth arguments are EQUAL (`2, 2`) need nothing:
 * gcc's single register is what the ROM does there.
 *
 * ONE ARGUMENT WAS SIMPLY TRANSCRIBED BACKWARDS and the diff said so plainly:
 * `__MapActor_SetPos(0x15, 0x14b0000, 0xf9 << 16)` reads `mov r2, #0xf9 /
 * ldr r1, =0x14b0000 / mov r0, #0x15 / lsl r2, #16` -- the pooled word is r1
 * and the shifted byte is r2, not the other way round.  Worth 2 differing and
 * worth reading the ROM's register numbers rather than its instruction order.
 *
 * PIN MINIMISATION ran to a fixpoint FROM BOTH ENDS on 2008bbc.  Both
 * directions reach TWENTY-FIVE pins of the 96 pinnable sites and both are
 * exact -- but THE TWO SETS ARE NOT THE SAME.  23 sites are common; forward
 * keeps {69, 70} where reverse keeps {65, 68}, four adjacent
 * __Func_80933d4 / __Func_80933f8 / __MapActor_SetBehavior sites that form one
 * interchangeable ordering cluster.  NEW, and it sharpens the recorded rule
 * "a pin set is minimal only w.r.t. the base it was minimised on": the two
 * directions can agree on CARDINALITY and still disagree on MEMBERSHIP, so
 * agreement in size is not proof the set is canonical.  The forward set ships,
 * then narrowed by width to a fixpoint (13 of the 25 drop to PIN1 or PIN2) and
 * re-confirmed by a strict re-drop.
 *
 * 2008940's FIVE PIN BLOCKS ARE ALL REQUIRED: dropping them costs 3, 3, 6, 3
 * and 2 differing.  Widths narrow to PIN1 at __MapActor_SetPos(0x15, ...) and
 * PIN2 at __MapActor_SetPos(0x10, ...); PIN1 there is 4 differing.
 *
 * MEASURED WORSE / INERT on 2008bbc (against 477 encodings / 1224 bytes):
 *
 *   spelling                                             differing
 *   --------------------------------------------------  ---------
 *   plain C, no pins                                          455  (+20 bytes)
 *   all 96 sites pinned INCLUDING the ten a2e0                 244  (-4 bytes)
 *   96 pins, a2e0 bare, `f = 1` after its call                 163  (-4 bytes)
 *   ... and 0x4013 written as a literal                        161  (-4 bytes)
 *   `int u` naming the 0xe000/0x8000 pair                      199  (-4 bytes)
 *   `register int u __asm__("r6")` on that pair                 49
 *   `register int v __asm__("r5")`, v + 0x13                   119
 *   both v and u hard-pinned, v + 0x13                         130  (+4 bytes)
 *   fill reversed at the af50 __MapActor_SetBehavior             64
 *   do { } while (0) before that call / after it / before        7 / 7 / 4
 *     the previous call
 *   dropping the __MapActor_RunScript pin next to it              4
 *   INERT (tie at the same residue, so the simpler form ships):
 *     __Func_8092a1c filled 1|2|0 and 2|1|0 and 0|2|1; PIN2 at
 *     the af50 site; the 71 pins dropped by minimisation.
 *
 * Harness: scratch_elev/b253/rich -- gen.py (pin/width/perm/knob generator for
 * 2008bbc), minimise.py (drop-to-fixpoint, `fwd`/`rev`), narrow.py (width
 * pass), dis.py + d.sh/d2.sh (side-by-side objdump diff), objcmp_all.py +
 * runall.sh (whole-TU objcmp, since --func filters only the reference).
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __WaitFrames(int n);
extern void __StartRain(void);
extern void __StartThunder(void);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8091e9c(int a);
extern void __Func_8092a1c(int a, int b, void *s);
extern void __Func_8095268(void);
extern void OvlFunc_884_2009084(void);
extern void OvlFunc_884_20095b4(void);
extern void OvlFunc_884_20097c8(void);
extern void OvlFunc_884_200a5b0(void);
extern unsigned char gScript_884__0200ac00[];
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_SetBehavior(int slot, void *s);
extern void __MapActor_RunScript(int slot, void *s);
extern void __MapActor_WaitScript(int slot);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern int __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void OvlFunc_884_200a2c8(int a, int b);
extern void OvlFunc_884_200a2e0(int a, int b, int c);
extern unsigned char gScript_884__0200a874[];
extern unsigned char gScript_884__0200aef0[];
extern unsigned char gScript_884__0200af50[];
extern unsigned char gScript_884__0200af78[];
extern unsigned char *iwram_3001ebc;

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

unsigned int OvlFunc_884_2008940(void)
{
    unsigned char *r;
    unsigned int r3;
    unsigned int r2;
    short *p;
    int w;

    if (__GetFlag(0x90b) != 0)
        __MapActor_SetPos(8, 0, 0);
    if (__GetFlag(0x90c) != 0)
        __MapActor_SetPos(9, 0, 0);
    if (__GetFlag(0x90d) != 0)
        __MapActor_SetPos(0xa, 0, 0);
    r3 = (unsigned int)&gState;
    r2 = 0xe1;
    r2 <<= 1;
    r3 += r2;
    p = (short *)r3;
    switch (*p) {
    case 0x62:
        __SetFlag(0x20);
        __Func_8091e9c(0x32);
        return 0;
    case 0x63:
        OvlFunc_884_200a5b0();
        return 0;
    case 0x61:
        OvlFunc_884_20095b4();
        return 0;
    }
    w = 0xc0 << 9;
    r = __MapActor_GetActor(8);
    *(int *)(r + 0x1c) = w;
    r = __MapActor_GetActor(9);
    *(int *)(r + 0x1c) = w;
    r = __MapActor_GetActor(0xa);
    *(int *)(r + 0x1c) = w;
    if (__GetFlag(0x87a) != 0) {
        __CopyMapTiles(0x61, 2, 0x50, 5, 2, 2);
        { register int s0 __asm__("r3"); register int s1 __asm__("r2");
          s0 = 3; s1 = 1;
          __CopyMapTiles(0x2a, 0x35, 0x2a, 0x36, s0, s1); }
        __Func_800fe9c();
        __WaitFrames(1);
        return 0;
    }
    if (__GetFlag(0x834) != 0) {
        __StartRain();
        __StartThunder();
        { register int s0 __asm__("r3"); register int s1 __asm__("r2");
          s0 = 0x12; s1 = 0x29;
          __Func_8010704(0x15, 0x26, 1, 1, s0, s1); }
        if (__GetFlag(0x84 << 4) != 0) {
            __MapActor_SetPos(0x11, 0, 0);
            __MapActor_SetPos(0x12, 0, 0);
            { PIN3; q2 = (int)gScript_884__0200ac00; q0 = 0x13; q1 = 0x80 << 9;
              __Func_8092a1c(q0, q1, (void *)q2); }
        }
    } else if (__GetFlag(0x815) != 0) {
        { PIN2; q0 = 0x10; q1 = 0xb4 << 16;
          __MapActor_SetPos(q0, q1, 0x8e << 18); }
        __CopyMapTiles(0x5c, 2, 0x50, 5, 2, 2);
        { register int s0 __asm__("r3"); register int s1 __asm__("r2");
          s0 = 3; s1 = 1;
          __CopyMapTiles(0x2a, 0x35, 0x2a, 0x36, s0, s1); }
        __Func_800fe9c();
        __WaitFrames(1);
    }
    r3 = (unsigned int)&gState;
    r2 = 0xe1;
    r2 <<= 1;
    r3 += r2;
    p = (short *)r3;
    if (*p == 0xc) {
        OvlFunc_884_20097c8();
        return 0;
    }
    if (__GetFlag(0x834) != 0) {
        r = __MapActor_GetActor(0x14);
        *(int *)(r + 0x18) = 0x4ccc;
        *(int *)(r + 0x1c) = 0x4ccc;
        __Actor_SetSpriteFlags(__MapActor_GetActor(0x14), 0);
        r = __MapActor_GetActor(0x15);
        *(int *)(r + 0x18) = 0x9999;
        *(int *)(r + 0x1c) = 0x9999;
        __MapActor_SetAnim(0xd, 5);
    } else if (__GetFlag(0x815) != 0) {
        { PIN1; q0 = 0x15;
          __MapActor_SetPos(q0, 0x14b0000, 0xf9 << 16); }
        __Actor_SetSpriteFlags(__MapActor_GetActor(0x15), 0);
    }
    if (__GetFlag(0x84 << 4) != 0) {
        __MapActor_SetPos(0x1a, 0, 0);
        __MapActor_SetPos(0x16, 0, 0);
    }
    r3 = (unsigned int)&gState;
    r2 = 0xe1;
    r2 <<= 1;
    r3 += r2;
    p = (short *)r3;
    if (*p == 0x13) {
        OvlFunc_884_20095b4();
        return 0;
    }
    if (__GetFlag(0x834) != 0 && __GetFlag(0x842) != 0) {
        OvlFunc_884_2009084();
        return 0;
    }
    if (__GetFlag(0x834) != 0) {
        __MapTransitionIn();
        __WaitMapTransition();
        __Func_8095268();
    }
    return 0;
}

void OvlFunc_884_2008bbc(void)
{
    unsigned char *p;
    int v;
    void *s;
    int f = 0;

    if (__GetFlag(0x834) == 0)
        return;
    if (__GetFlag(0x84 << 4) != 0)
        return;
    __CutsceneStart();
    { PIN1; q0 = 0x19999;
      __Func_80933d4(q0, 0x3333); }
    __Func_80933f8(0xc5 << 16, -1, 0xc0 << 18, 1);
    __Func_8093530();
    __MessageID(0xeb6);
    __Func_80925cc(0x13, 2);
    { PIN1; q0 = 0x4013;
      __Func_8093040(q0, 0, 0xa); }
    { PIN3; q0 = 0; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x19; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN2; q0 = 0; q1 = 0xb3;
      __Func_80921c4(q0, q1, 0x315); }
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_SetPos(0x19, *(int *)(p + 8), *(int *)(p + 0x10));
    { PIN2; q0 = 0x19; q1 = 0xb3;
      __Func_80921c4(q0, q1, 0xc9 << 2); }
    __Func_8092848(0, 0x19, 0x28);
    __Func_8092adc(0, 0, 0);
    __Func_8092adc(0x19, 0, 0);
    __MapActor_SetAnim(0x11, 3);
    __MapActor_DoAnim(0x12, 3);
    __Func_8092848(0x11, 0x12, 0);
    __CutsceneWait(0x14);
    __Func_809259c(0x11, 1);
    __Func_8093040(0x4011, 0, 0xa);
    __MapActor_SetAnim(0x12, 3);
    __Func_8093040(0x12, 0, 0xa);
    __Func_8092adc(0x11, 0, 0);
    { PIN1; q0 = 0x12;
      __Func_8092adc(q0, 0xf0 << 8, 0xa); }
    __MapActor_DoAnim(0x13, 3);
    { PIN1; q0 = 0x4013;
      __Func_8093040(q0, 0, 0xa); }
    { PIN3; q0 = 0x11; q1 = 0x19999; q2 = 0xcccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN1; q0 = 0x12;
      __MapActor_SetSpeed(q0, 0x19999, 0xcccc); }
    s = gScript_884__0200aef0;
    __MapActor_SetBehavior(0x11, s);
    __CutsceneWait(0x14);
    __MapActor_SetBehavior(0x12, s);
    { PIN2; q0 = 0; q1 = 0xc0 << 8;
      __Func_8092adc(q0, q1, 0); }
    { PIN1; q0 = 0x19;
      __Func_8092adc(q0, 0xc0 << 8, 0x3c); }
    __MapActor_SetBehavior(0, gScript_884__0200af50);
    __MapActor_RunScript(0x19, gScript_884__0200af78);
    __CutsceneWait(0x14);
    __Func_8092adc(0, 0, 0);
    __Func_8092adc(0x19, 0, 0xa);
    __ActorMessage(0x19, 0);
    { PIN1; q0 = 0x13;
      __Func_8092adc(q0, 0x80 << 8, 0); }
    OvlFunc_884_200a2e0(0x1a, 0xc0 << 7, 0x14);
    __Func_80925cc(0x1a, 2);
    OvlFunc_884_200a2c8(0x1a, 0xa);
    __MapActor_SetAnim(0, 3);
    __MapActor_DoAnim(0x19, 3);
    __CutsceneWait(0x14);
    __Func_80925cc(0x13, 2);
    __Func_8092c40(0x4013, 0);
    if (__Func_8091c7c(0, 0) == 1) {
        f = 1;
        __MapActor_SetAnim(0x13, 4);
    } else {
        __MapActor_SetAnim(0x13, 3);
        ++*(unsigned short *)(iwram_3001ebc + 0x1d8);
    }
    { PIN1; q0 = 0x4013;
      __ActorMessage(q0, 0); }
    if (f != 0)
        ++*(unsigned short *)(iwram_3001ebc + 0x1d8);
    v = 0x80 << 7;
    OvlFunc_884_200a2e0(0x16, v, 0x1e);
    __ActorMessage(0x16, 0);
    { PIN2; q0 = 0x13; q1 = 0x80 << 1;
      __MapActor_Emote(q0, q1, 0); }
    { PIN2; q0 = 0x1a; q1 = 0x80 << 1;
      __MapActor_Emote(q0, q1, 0); }
    { PIN2; q0 = 0; q1 = 0x80 << 1;
      __MapActor_Emote(q0, q1, 0); }
    { PIN1; q0 = 0x19;
      __MapActor_Emote(q0, 0x80 << 1, 0x28); }
    { PIN2; q0 = 0x13; q1 = 0xa0 << 8;
      __Func_8092adc(q0, q1, 0); }
    { PIN1; q0 = 0x1a;
      __Func_8092adc(q0, 0xa0 << 8, 0); }
    { PIN2; q0 = 0; q1 = 0xe0 << 8;
      __Func_8092adc(q0, q1, 0); }
    OvlFunc_884_200a2e0(0x19, 0xe0 << 8, 0xa);
    __Func_80933d4(0x13333, 0x2666);
    { PIN2; q0 = 0xd7 << 16; q1 = -1;
      __Func_80933f8(q0, q1, 0x2f60000, 1); }
    __Func_8093530();
    __Func_80933d4(0xcccc, 0x1999);
    { PIN2; q0 = 0xcd << 16; q1 = -1;
      __Func_80933f8(q0, q1, 0x30a0000, 1); }
    { PIN1; q0 = 0x16;
      __MapActor_SetBehavior(q0, gScript_884__0200a874); }
    __MapActor_WaitScript(0x16);
    OvlFunc_884_200a2e0(0x16, 0x80 << 6, 0x3c);
    __Func_80925cc(0x13, 2);
    OvlFunc_884_200a2c8(0x13, 0xa);
    __MapActor_DoAnim(0x16, 3);
    OvlFunc_884_200a2c8(0x16, 0x14);
    __MapActor_DoAnim(0x13, 3);
    __CutsceneWait(0xa);
    OvlFunc_884_200a2e0(0x13, v, 0x1e);
    OvlFunc_884_200a2c8(v + 0x13, 0xa);
    OvlFunc_884_200a2e0(0x1a, 0xe0 << 8, 0x1e);
    __MapActor_DoAnim(0x1a, 3);
    OvlFunc_884_200a2e0(0x13, 0x80 << 8, 0x1e);
    __Func_80925cc(0x13, 2);
    OvlFunc_884_200a2c8(v + 0x13, 0xa);
    __Func_8092848(0, 0x19, 0x28);
    __Func_8092adc(0, 0, 0);
    OvlFunc_884_200a2e0(0x19, 0, 0x14);
    OvlFunc_884_200a2e0(0x1a, 0x80 << 8, 0x1e);
    __MapActor_DoAnim(0x1a, 3);
    OvlFunc_884_200a2c8(0x1a, 0x1e);
    OvlFunc_884_200a2e0(0x1a, 0xc0 << 8, 0x1e);
    __MapActor_DoAnim(0x1a, 3);
    __MapActor_DoAnim(0x16, 3);
    __MapActor_SetAnim(0x19, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(0x19, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(0x19);
    __MapActor_SetPos(0x19, 0, 0);
    __MapActor_SetAnim(0x1a, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(0x1a, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(0x1a);
    __MapActor_SetPos(0x1a, 0, 0);
    __MapActor_SetAnim(0x16, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(0x16, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(0x16);
    __MapActor_SetPos(0x16, 0, 0);
    { PIN3; q2 = gScript_884__0200ac00; q0 = 0x13; q1 = 0x80 << 9;
      __Func_8092a1c(q0, q1, q2); }
    __SetFlag(0x84 << 4);
    __CutsceneEnd();
}
