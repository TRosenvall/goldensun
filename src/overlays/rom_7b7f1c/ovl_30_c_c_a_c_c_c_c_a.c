/* OvlFunc_930_2008b2c  --  0x02008b2c
 * OvlFunc_930_2008c30  --  0x02008c30
 *   [asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_a.s -- BOTH functions of the
 *    file, lines 8-103 and 110-455 of 456.  Nothing else is in it.]
 *
 * EXACT, both, under tools/objcmp.py against single-function extracts of this
 * exact source:
 *
 *   OK OvlFunc_930_2008b2c -- 260 bytes, 96 encodings and 32 relocations identical
 *   OK OvlFunc_930_2008c30 -- 924 bytes, 351 encodings and 105 relocations identical
 *
 * and the WHOLE translation unit, assembled the way the Makefile assembles it
 * and compared against asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_a.s as one
 * object, is 1184 bytes, 447 encodings and 137 relocations identical.  (objcmp's
 * --func trims the REFERENCE only, so a two-function candidate has to be checked
 * both ways.)  Measured against the asm/ path and against a scratch copy of the
 * reference alike, so no Makefile pattern rule is in play; objcmp printed no
 * "(built with: ...)" line, i.e. adjust=set().
 *
 * THE FLAG GROUP MATTERS HERE AND IT IS THE ABSENCE OF ONE.  This TU must build
 * at the tree default -O2 -mthumb -mthumb-interwork -fcall-used-r4.  At -O1 the
 * combined object is 309 of 447 differing.  That is worth saying loudly because
 * the IMMEDIATE NEIGHBOURS in this overlay are -O1: Makefile carries
 * `asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o` and
 * `..._c_c_a_c_c_c_c_b_%.o` at $(O1_CFLAGS), and its own comment records that
 * the rule USED to read `ovl_30_c_c_a_c_c_c_c%`, which would have captured this
 * stem.  Re-broadening that pattern silently moves this file to -O1.
 *
 * Two straight-line cutscene scripts, 94 and 344 instructions, no control flow
 * of any kind -- no branch, no loop, no null guard.  Roughly 25 and 105 calls,
 * all of them constant argument scripts.  The whole problem is argument
 * scheduling and constant CSE.
 *
 * READ THE PROLOGUE BY CONTENT.  2008c30's is NARROW -- `push {r5, lr}`, one
 * callee-saved register -- and that narrowness is not the good news it looks
 * like.  r5 holds only 0xfe, for the two `[0x5a] &= 0xfe` sites, and it falls
 * out of gcse unaided (see WHAT NEEDED NOTHING).  Plain C widens it to
 * `push {r5, r6, lr} / mov r6, r8 / push {r6}`: the extra pressure is entirely
 * gcc commoning repeated SCRIPT CONSTANTS across `bl`s -- 0x81<<1 into r5,
 * 0xd0<<8 into r8, the 0x3333/0x1999 pool pair into r5/r6 -- which the ROM
 * rebuilds at every site.  319 of 351 differing, +12 bytes.  2008b2c's
 * `push {lr}` stays right in plain C and is still 39 of 96 differing, so the
 * prologue is a symptom of this class, not a test for it.
 *
 * SEVENTEEN PINNED SITES OF 82 PINNABLE, MINIMAL BY MEASUREMENT.  Thirteen of
 * 64 in 2008c30 and four of 18 in 2008b2c.  The recorded routine: pin
 * everything, then strip greedily, re-measuring under objcmp after EVERY drop,
 * to a fixpoint.  Round 1 removed 51 and 14; round 2 removed none.  Each
 * shipped set was measured AS A SET, which is what the OK lines above are.
 *
 * THE DROP-COST PARTITION IS EXACT, ON ALL SEVENTEEN.  The new rule -- an
 * ORDERING pin costs a small count with SIZE and RELOCATIONS both SILENT, a
 * genuine CSE loss costs much more with RELOCATIONS ALWAYS differing, and SIZE
 * is not the discriminator -- holds on every one, with no intermediate case:
 *
 *   site                                          drop costs   verdict
 *   --------------------------------------------  -----------  -------
 *   c30  1  __MapActor_Surprise(8, 0x81<<1)          19 RELOC    CSE
 *   c30  6  __MapActor_Surprise(0xa, 0x81<<1)         2          order
 *   c30 13  __Func_8092adc(0xa, 0xd0<<8, 0x14)      274 RELOC    CSE
 *   c30 16  __Func_809218c(8, 0xb2, 0x8a<<1)          3          order
 *   c30 17  __Func_80921c4(0xa, 0xac, 0x8e<<1)        2          order
 *   c30 19  __Func_8092adc(8, 0xa0<<7, 0)             2          order
 *   c30 27  __MapActor_SetSpeed(8, 0x3333, 0x1999)  221 RELOC +4 CSE
 *   c30 28  __MapActor_SetSpeed(0xa, 0x3333, 0x1999)  2          order
 *   c30 55  __Func_8093054(8, 0)                      2          order
 *   c30 79  __Func_8092adc(0xa, 0xd0<<8, 0x14)       47 RELOC    CSE
 *   c30 86  __MapActor_SetSpeed(0xa, 0xcccc, 0x6666)  5          order
 *   c30 87  __Func_80921c4(0xa, 0xa8, 0x94<<1)        3          order
 *   c30 88  __Func_8092adc(0xa, 0xd0<<8, 0x14)        2          order
 *   b2c  4  __Func_8092adc(0, 0xc0<<7, 0)             2          order
 *   b2c 17  __Func_8092adc(0xa, 0xd0<<8, 0x14)        2          order
 *   b2c 21  __MapActor_Emote(8, 0x81<<1, 0x3c)       33 RELOC +4 CSE
 *   b2c 28  __Func_8092adc(0xa, 0xb0<<8, 0x14)        2          order
 *
 * Four CSE pins in c30, one in b2c, thirteen ordering jobs.  Only the CSE
 * drops move a byte count, and only two of the five do; RELOCATIONS fire on
 * all five and on none of the twelve.  So the relocation line is the test and
 * the size line is not.
 *
 * THE CSE PINS ARE FIRST-USE-PER-REGION, AND THE REGIONS ARE MADE BY THE PINS
 * THEMSELVES.  0xd0<<8 has three sites in c30 -- 13, 79, 88.  Site 13 is the
 * first use anywhere (274); site 79 is the first use after 13's pin has killed
 * the carried copy (47); site 88 then needs ORDERING ONLY (2), because 79's
 * pinned r1 is call-clobbered and dead before it.  The same pattern in b2c:
 * 0x81<<1 is used at sites 21 and 26 and only 21 is pinned.  That is the
 * recorded "pin the first use in each REGION" with the sharpening that the
 * region boundary is created by the previous pin, not read off the source.
 *
 * THERE IS NO HOLE HERE, AND THE FULL SET IS ALSO EXACT.  Unlike the
 * file-sibling OvlFunc_954_2008db8, where five __MapActor_SetPos sites had to
 * be carved OUT because the ROM commons there itself, every pinnable site in
 * both functions here can carry a pin: all 64 in c30 and all 18 in b2c measure
 * OK.  The 65 non-shipped pins are therefore inert scaffolding, and the
 * recorded discipline applies -- they are a false claim about what the ROM
 * required, so they were stripped.  A pin set that measures exact is not
 * finished; it has to be minimised.
 *
 * THE PINS ARE REGISTER PINS, NOT NAMES.  The seventeen blocks written as plain
 * `int q0, q1, q2;` locals measure 319 and 39 -- byte-for-byte the same numbers
 * as NO SCAFFOLDING AT ALL, same SIZE delta, same relocation failure.  What
 * destroys the CSE is the hard call-clobbered destination.
 *
 * UNIFORM WHOLE-VALUE ASCENDING FILL AT SIXTEEN OF SEVENTEEN.  One statement
 * per argument, ascending q0..q2, whole value per statement, `0x81 << 1`
 * written whole.  It reproduces every emitted order the ROM has here even where
 * the ROM emits r1/r2/r0 (`mov r1 / mov r2 / mov r0 / lsl r1` at __Func_8092adc)
 * or r2/r0/r1 (`mov r2 / mov r0 / mov r1 / lsl r2` at __Func_809218c) -- the
 * scheduler puts the `lsl` and the pool load back where the ROM has them.  Read
 * the source order off what the pins have to ESTABLISH, not off the emission
 * order.  Descending was measured at each of the seventeen separately and is
 * worse at every one, 2 to 7 differing.
 *
 * THE ONE EXCEPTION IS __Func_8093054(8, 0), WHICH WANTS DESCENDING.  The ROM
 * emits `mov r1, #0 / mov r0, #8`; ascending is 2 differing.  This is the same
 * binary, per-callee behaviour the template records for __Func_8092c40 -- the
 * two-argument script calls whose second argument is a zero are the family to
 * check, and it stays binary: descending or nothing.
 *
 * THE `|= 1` WANTS THE TYPED FIELD, AND THE NAMED DESTINATION POINTER IS NOT A
 * SUBSTITUTE (NEW -- grepped first against "orr rd, rs -- which operand becomes
 * the destination", "The ORR-destination lever needs an unsigned char local",
 * "The constant-as-destination lever", "Name the store's DESTINATION pointer"
 * and "typed field").  The ROM makes the CONSTANT the `orr` destination:
 *
 *     ref   bl GetActor / add r0,#0x5a / ldrb r2,[r0] / mov r3,#1 / orr r3,r2
 *     ours  bl GetActor / add r0,#0x5a / ldrb r3,[r0] / mov r2,#1 / orr r3,r2
 *
 * Section "## `orr rd, rs`" carries a batch-237 blockquote asserting that the
 * named-destination-pointer remedy and the operand-order remedies "are one
 * lever".  On this site they are NOT.  Nine spellings, all with the same single
 * call and the same address computation:
 *
 *     ((struct Actor *)__MapActor_GetActor(0xa))->f5a |= 1;               OK
 *     struct Actor *e = ...; e->f5a |= 1;                                 OK
 *     unsigned char *bp = &e->f5a; *bp |= 1;         (the batch-237 form)  2
 *     p = GetActor(0xa) + 0x5a; *p |= 1;                                   2
 *     p = ...; *p = 1 | *p;                                                2
 *     unsigned short one = 1; *p = one | *p;                               2
 *     int one = 1; unsigned char v = *p; *p = one | v;                     2
 *     unsigned char one = 1; *p = one | *p;   (the recorded narrow local)  4
 *     unsigned char one = 1; *p = *p | one;                                4
 *
 * The distinction is not the address computation and not the width of the
 * value -- `bp` and `->f5a` compute the same byte address and both are QImode.
 * It is whether the destination is an aggregate MEMBER REFERENCE or a
 * dereferenced pointer.  So batch 237's identification holds for its own case
 * (where the plain form cost a SECOND call) and does not generalise: when the
 * plain form already emits one call, the typed field is the lever and the named
 * pointer is not.  Two spellings are much worse and are recorded so nobody
 * re-tries them: a register pin on the constant (r3, either width) is 67
 * differing WITH RELOCATIONS, and writing the field on both sides
 * (`e->f5a = 1 | e->f5a` with the cast repeated) is 83, +12 bytes -- it emits
 * the call twice.
 *
 * `struct Actor` with a byte at 0x5a is not a new name in this tree; the
 * template's own `struct Actor` is the 0x6 halfword variant, and
 * docs/structs.md carries both shapes.
 *
 * THE ORR SPELLING IS INVISIBLE UNTIL THE PINS ARE IN.  Against the no-pin
 * baseline the typed field and the plain `|= 1` both measure 319 -- exactly
 * equal, same size delta, same relocation failure.  The 2-differing cost only
 * appears once the thirteen pins are in place.  A one-lever-at-a-time sweep
 * started from plain C would have recorded this lever as INERT and dropped it.
 * That is the recorded "nothing in this function is diagnosable one lever at a
 * time" in its masking direction rather than its cancelling one.
 *
 * PROTOTYPES ARE LOAD-BEARING, 11 OF THE 23 TESTED.  Each was dropped on its
 * own and the function re-measured.  Dropping __Func_809228c costs 24,
 * __Func_8093040 22 in c30 and 11 in b2c, __MapActor_SetAnim 16 and 2,
 * __Func_8092adc 10 and 7, __MapActor_SetSpeed 6, __MapActor_Surprise 4,
 * __Func_809218c 3, __Func_80921c4 3, __Func_809259c 2, __Func_8093500 2,
 * __MapActor_Emote 2.  __MapActor_GetActor is required to compile at all.  The
 * twelve inert ones -- __CutsceneWait, __MessageID, __PlaySound, __SetFlag,
 * __MapActor_DoAnim, __MapActor_Jump, __MapActor_SetIdle,
 * __MapActor_WaitMovement, __Func_80925cc, __Func_8093054, __Func_8091e9c --
 * stay declared because a complete prototype list is this tree's convention and
 * costs nothing; the five `void (void)` callees were not tested because they
 * have no argument order to get wrong.  Note that __MapActor_Surprise and
 * __Func_809259c are load-bearing in ONE function of this TU and inert in the
 * OTHER: a prototype's effect is per-call-site argument shape, not per-symbol.
 *
 * WHAT NEEDED NOTHING.  The two `&= 0xfe` sites are exact as bare
 * `__MapActor_GetActor(8)[0x5a] &= 0xfe;` -- no local, no named pointer, no
 * struct.  The ROM's `mov r5,#0xfe / mov r3,r5 / and r3,r2 / strb` then
 * `and r5,r3 / strb r5` -- one materialisation carried in the pushed r5 across
 * `bl __MapActor_GetActor`, with the CONSTANT as the `and` destination at the
 * first site and the VALUE at the second -- falls out of gcse unaided.  This is
 * the internal control for the ORR paragraph above: same function, same actor
 * field, same shape, and the recorded `int m = 0xfe` remedy for `and` is not
 * needed because gcc already agrees.  Also free: the end-of-function store
 * `*(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x201`, whose `add r2, #0x41` off the
 * 0xe0<<1 index is gcc's own arithmetic; `iwram_3001ebc` as a scalar
 * `unsigned char *`; and 0x19cf / 0x19da / 0x8b1 / 0x8b2 as plain literals --
 * the reference object carries exactly ONE R_ARM_ABS32 record and it is
 * `iwram_3001ebc`, so none of the four is a symbol.
 *
 * MEASURED WORSE (2008c30, against 924 bytes / 351 encodings):
 *
 *   spelling                                             differing
 *   ---------------------------------------------------  ---------
 *   no pins at all                                         319 (+12 bytes, RELOC)
 *   the 13 pins as plain `int` locals, not register pins   319 (+12 bytes, RELOC)
 *   all 64 pinnable sites pinned                             0  (inert, stripped)
 *   ascending fill at __Func_8093054(8, 0)                    2
 *   descending fill at any single other pinned site        2 to 7
 *   `|= 1` plain, or via any named `unsigned char *`          2
 *   `|= 1` via `unsigned char one = 1; *p = one | *p;`        4
 *   `|= 1` with the constant register-pinned to r3           67 (RELOC)
 *   `|= 1` written with the cast on both sides               83 (+12 bytes)
 *   no prototype for __Func_809228c                          24
 *   no prototype for __Func_8093040                          22
 *   no prototype for __MapActor_SetAnim                      16
 *   the whole TU at -O1                                     309 (of 447)
 *
 * MEASURED WORSE (2008b2c, against 260 bytes / 96 encodings):
 *
 *   no pins at all                                          39 (+4 bytes, RELOC)
 *   the 4 pins as plain `int` locals                        39 (+4 bytes, RELOC)
 *   all 18 pinnable sites pinned                             0  (inert, stripped)
 *   descending fill at any single pinned site                 2
 *   no prototype for __Func_8093040                          11
 *   no prototype for __Func_8092adc                           7
 *
 * LANDING NEEDS NO SPLIT AND NO LINKER EDIT.  The .s holds exactly TWO
 * functions and no data -- no `.section`, no `.data`, no `.rodata`, no
 * `.align`/`.word` outside the two bodies; every constant reaches an
 * assembler-built end-of-function pool through `ldr rN, =value`.  Both are
 * solved here, so the whole file lands as one .c.  overlays/rom_7b7f1c/
 * overlay.ld names the .o on exactly ONE line, matched on the full path:
 *
 *     overlays/rom_7b7f1c/overlay.ld:34
 *         asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_a.o(.text)
 *
 * in the `.text : {` block that opens at line 15.  The file's `.data : {` block
 * at line 47 holds two entries and NEITHER is this .o (they are
 * ovl_30_c_c_c_c_b.o and ovl_30_c_c_c_c_c.o), and the only other section is
 * `/DISCARD/ : { *(*) }` -- so the "a split that says no data is a claim to
 * CHECK / remap EVERY section" trap has been checked and there is nothing to
 * remap.  No other .ld in the tree names this full path; three other overlays
 * carry a same-STEM `ovl_30_c_c_a_c_c_c_c_a.o` (rom_7b4558:55, rom_7c5974:25)
 * and they are different files.  The landing is therefore: delete
 * asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_a.s, add this file at
 * src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_a.c, touch no .ld and no
 * Makefile.  It builds through the generic cross-dir `asm/%.o: src/%.c` rule
 * at Makefile:146 under the default GCC296_CFLAGS -- and see the flag warning
 * at the top: DO NOT add a rule for it, and do not re-broaden the neighbouring
 * -O1 pattern.
 */
extern unsigned char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __MessageID(int id);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __SetFlag(int id);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8093500(int a, int b);
extern void __Func_8093530(void);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_8093054(int a, int b);
extern void __Func_8091e9c(int n);

struct Actor { unsigned char pad00[0x5a]; unsigned char f5a; };

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")


void OvlFunc_930_2008b2c(void)
{
    __CutsceneStart();
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0; q1 = 0xc0 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093500(9, 1);
    __Func_8093530();
    __Func_80925cc(8, 2);
    __CutsceneWait(0x14);
    __MessageID(0x19cf);
    __Func_8093040(8, 0, 0x14);
    __Func_80925cc(9, 1);
    __CutsceneWait(0x14);
    __Func_8093040(9, 0, 0x14);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x14);
    __Func_8093040(8, 0, 0x14);
    { PIN3; q0 = 0xa; q1 = 0xd0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0xa, 0, 0x14);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x3c);
    { PIN3; q0 = 8; q1 = 0x81 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_DoAnim(8, 4);
    __CutsceneWait(0x14);
    __Func_8093040(8, 0, 0x14);
    __Func_809259c(0xa, 2);
    __MapActor_Surprise(0xa, 0x81 << 1);
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0xa; q1 = 0xb0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(9, 5);
    __CutsceneEnd();
    __SetFlag(0x8b1);
}

void OvlFunc_930_2008c30(void)
{
    __CutsceneStart();
    { PIN2; q0 = 8; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __Func_809259c(8, 2);
    __CutsceneWait(0x3c);
    __MessageID(0x19da);
    __Func_8093040(8, 0, 0x14);
    { PIN2; q0 = 0xa; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __MapActor_Jump(0xa, 4, 0);
    __CutsceneWait(0x3c);
    __Func_8093040(0xa, 0, 0x14);
    __Func_80925cc(8, 1);
    __CutsceneWait(0x14);
    __Func_8093040(8, 0, 0x14);
    { PIN3; q0 = 0xa; q1 = 0xd0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0xa, 3);
    __CutsceneWait(0x14);
    { PIN3; q0 = 8; q1 = 0xb2; q2 = 0x8a << 1;
      __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xac; q2 = 0x8e << 1;
      __Func_80921c4(q0, q1, q2); }
    __MapActor_WaitMovement(8);
    { PIN3; q0 = 8; q1 = 0xa0 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0xa, 0xb0 << 8, 0);
    __CutsceneWait(0x14);
    __Func_80925cc(8, 2);
    __CutsceneWait(0x14);
    __Func_8093040(8, 0, 0x14);
    __MapActor_GetActor(8)[0x5a] &= 0xfe;
    __MapActor_GetActor(0xa)[0x5a] &= 0xfe;
    { PIN3; q0 = 8; q1 = 0x3333; q2 = 0x1999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x3333; q2 = 0x1999;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(8, 5);
    __MapActor_SetAnim(0xa, 6);
    __CutsceneWait(0x14);
    __PlaySound(0x7d);
    __Func_809228c(8, 2, 0);
    __Func_809228c(9, 2, 0);
    __Func_809228c(0xa, 2, 0);
    __MapActor_WaitMovement(0xa);
    __CutsceneWait(0x1e);
    __MapActor_SetAnim(8, 5);
    __MapActor_SetAnim(0xa, 6);
    __CutsceneWait(0x14);
    __PlaySound(0x7d);
    __Func_809228c(8, 4, 0);
    __Func_809228c(9, 4, 0);
    __Func_809228c(0xa, 4, 0);
    __MapActor_WaitMovement(0xa);
    __MapActor_SetIdle(9);
    __MapActor_SetAnim(8, 1);
    __MapActor_SetAnim(0xa, 1);
    __CutsceneWait(0x32);
    __MapActor_Jump(0xa, 2, 0);
    __CutsceneWait(0x14);
    __Func_8093040(0xa, 0, 0x14);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 8;
      __Func_8093054(q0, q1); }
    __Func_8093040(8, 0, 0x14);
    __MapActor_SetAnim(8, 5);
    __MapActor_SetAnim(0xa, 6);
    __CutsceneWait(0x14);
    __PlaySound(0x7d);
    __Func_809228c(8, 2, 0);
    __Func_809228c(9, 2, 0);
    __Func_809228c(0xa, 2, 0);
    __MapActor_WaitMovement(0xa);
    __CutsceneWait(0x1e);
    __MapActor_SetAnim(8, 5);
    __MapActor_SetAnim(0xa, 6);
    __CutsceneWait(0x14);
    __PlaySound(0x7d);
    __Func_809228c(8, 4, 0);
    __Func_809228c(9, 4, 0);
    __Func_809228c(0xa, 4, 0);
    __MapActor_WaitMovement(0xa);
    __CutsceneWait(0x28);
    __MapActor_SetAnim(8, 1);
    __MapActor_SetAnim(0xa, 1);
    __MapActor_Jump(0xa, 2, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xa; q1 = 0xd0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0xa, 0, 0x14);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x14);
    __Func_8093040(8, 0, 0x1e);
    __Func_8093040(8, 0, 0x14);
    ((struct Actor *)__MapActor_GetActor(0xa))->f5a |= 1;
    { PIN3; q0 = 0xa; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xa8; q2 = 0x94 << 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xd0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0xa, 5);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x14);
    __Func_8093040(8, 0, 0x14);
    __MapActor_DoAnim(0xa, 3);
    __CutsceneWait(0x14);
    __Func_8093040(0xa, 0, 0x14);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x14);
    __Func_8093040(8, 0, 0x14);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0x14);
    __CutsceneEnd();
    __SetFlag(0x8b2);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x201;
    __Func_8091e9c(6);
}
