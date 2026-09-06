/* OvlFunc_899_2008b48  --  0x02008b48
 *   [asm/overlays/rom_794ac0/ovl_30_a_c_c_c_a_a.s, 1st of 3 -- A SPLIT IS NEEDED]
 *
 * 249 instructions of straight-line cutscene script under one save-bit test,
 * with a second if/else after the join.  ZERO hi-register traffic in the ROM
 * and `push {r14}` alone for a prologue: no callee-saved register is spent
 * anywhere, so every repeated constant is rebuilt at every use and ONLY PINS
 * WORK.  Built at the tree default -O2 -- no Makefile pattern rule matches
 * rom_794ac0/ovl_30_a_c_c_c_a_a (the only explicit rule in that directory is
 * for ovl_30_a_c_a_c_a), so `asm/%.o: src/%.c` applies and objcmp prints no
 * `(built with: ...)` line.  THE MATCH DEPENDS ON NO FLAG GROUP.
 *
 * THE TEMPLATE'S RECIPE WAS SUFFICIENT, NOT NECESSARY, AND RE-MEASURING PAID.
 * src/overlays/rom_7ec19c/ovl_30_c_c_a.c records 35 pins none removable and
 * `__Func_8092c40` taking the DESCENDING fill.  Here 14 pins carry it, one of
 * the first 14 candidates measured inert and was dropped, and BOTH
 * `__Func_8092c40` sites want NO PIN AT ALL -- the ROM emits `mov r0,#2 /
 * mov r1,#0` there, plain ascending, which is what unpinned C already gives.
 * That is the recorded "property of the SITE, not the callee" holding in the
 * negative direction, and it is why the site list was re-derived rather than
 * transcribed.
 *
 * THE LENGTH TELL FIRED IN ITS "EQUAL LENGTH IS NOT INFORMATION" FORM.  Plain
 * C is 254 encodings against the ROM's 254 and 214 of them differ: the widened
 * `push {r5, r6, r7, r14}` costs exactly what the removed rematerialisations
 * save.  The diff TEXT is what carries the signal, and the `mov rN, r5..r7`
 * copies read off it by DESTINATION name six commoned values:
 *
 *   0xcccc  (pool)      -> r1 at all three __MapActor_SetSpeed sites
 *   0x6666  (pool)      -> r2 at all three __MapActor_SetSpeed sites
 *   0xcc << 1           -> r2 at both __Func_80921c4 sites that use it
 *   0xc0 << 8           -> r1 at four __Func_8092adc sites
 *   0xd0 << 8           -> r1 at three __Func_8092adc sites
 *   0x85 << 4           -> r0 at __GetFlag and at __SetFlag
 *
 * UNIFORM ASCENDING FILL, ONE STATEMENT PER ARGUMENT, WHOLE VALUE PER
 * STATEMENT, at every site touching one of those six: 214 differing -> 3.
 * sched2 lands every transposed order the ROM emits from that one spelling --
 * `mov r1 / mov r2 / mov r0 / lsl r1 / lsl r2`, `mov r1 / mov r0 / lsl r1 /
 * mov r2`, `mov r1 / lsl r1 / mov r2 / mov r0` and `mov r1 / mov r2 / lsl r1 /
 * mov r0` all come out of `q0 = ...; q1 = ...; q2 = ...;`.  The ascending fill
 * is CORRECT here, not merely cheaper: taken site by site, the descending fill
 * is 2 to 9 encodings worse at twelve of the thirteen three-argument pins.
 *
 * THE RESIDUE WAS ONE UNPINNED, UNIQUE-CONSTANT SITE.  __MapActor_SetPos(1,
 * 0xc0 << 17, 0xcc << 17) has no repeated constant and so is not a CSE victim,
 * but the ROM issues `mov r0, #1` BETWEEN the two seeds and the two shifts and
 * gcc issues it after both shifts.  That is argument ORDERING, the second job
 * a pin does; the same uniform ascending fill is exact and closes the function.
 *
 * A ONE-ARGUMENT PIN REACHES A CONSTANT CSE'd ACROSS A DOMINATING BRANCH.
 * 0x85 << 4 is built for __GetFlag in the entry block and again for __SetFlag
 * inside the block the flag test guards -- the recorded "first used in the
 * entry block, so it commons" shape.  `{ PIN1; q0 = 0x85 << 4; seen =
 * __GetFlag(q0); }` costs one named `int` for the tested result and rebuilds
 * the constant in both places.  Direction matters and was measured: pinning
 * the SECOND site instead (__SetFlag) is worth NOTHING -- 32 differing and a
 * 4-byte size change, identical to no pin at all -- while pinning only the
 * first is exact.  __SetFlag is therefore left plain here.
 *
 * THE ARGUMENT-LIST CASE OF CONSTANT CSE IS REACHABLE HERE.  __Func_80921c4(2,
 * 0xbc << 1, 0xbc << 1) repeats one `mov`+`lsl` value inside a single argument
 * list; gcc builds it once into r2 and copies (`mov r1, r2`) where the ROM
 * builds it twice.  The uniform ascending pin splits it back into two builds
 * and is byte-exact -- so this presentation of the recorded argument-list CSE
 * blocker is NOT a wall.  (The recorded corpus test -- no generated `.s` holds
 * two consecutive `neg` -- is about the `-1` presentation specifically.)
 *
 * FOURTEEN PINS, EXACTLY ONE WAS REMOVABLE.  Every pin was stripped
 * individually under objcmp.  The THIRD __MapActor_SetSpeed -- the LAST use of
 * both pooled values -- is inert and is gone.  Consistent with "the ones that
 * fall are the last use of their value, never the first": every surviving pin
 * that is a last use (the fourth 0xc0 << 8, the third 0xd0 << 8) still costs 2
 * encodings when dropped, so being a last use is necessary and not sufficient.
 * After that drop the sweep was re-run from the top over the remaining
 * thirteen AND over their spellings; nothing further fell, and only
 * __Func_8092adc(1, 0xd0 << 8, 0) is spelling-indifferent (ascending and
 * descending both byte-exact there).  The ascending form is kept for
 * uniformity.
 *
 * ALL FIVE POOLED VALUES ARE BARE LITERALS.  0xcccc, 0x6666, 0x1256, 0x125d
 * and 0x856 need no symbol -- none is a shifted byte gcc could build with
 * mov+lsl, so each pools unaided.  objcmp reports 75 relocations identical and
 * every one is an R_ARM_THM_CALL: the function has no data relocation at all,
 * so nothing belongs in const.sym or message.sym for it.
 *
 * The far `beq / b` pair at the flag test is gcc's own long-branch expansion;
 * the guarded body is past Thumb's conditional range.  The two
 * `ldrsh rN, [r0, r3]` pairs need no help: Thumb-1 has no immediate-offset
 * `ldrsh`, so `*(short *)(p + 0xa)` is forced into the register-offset form.
 *
 * LAYOUT.  `.func_end` expands to `.pool_aligned`, so each function in the
 * reference carries its own literal pool: this one is 668 bytes of code plus
 * pool, exactly the 0x2008de4 - 0x2008b48 gap, so splitting it out does not
 * move a pool word.  OvlFunc_899_2008de4, which this function calls and which
 * stays behind in the `_b` piece, is already `.global` via `.thumb_func_start`.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetFlag(int id);
extern int __GetFlag(int id);
extern void __PlaySound(int id);
extern void __PlayMapMusic(void);
extern int __Func_8091c7c(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void OvlFunc_899_2008de4(void);
extern void OvlFunc_899_200c5f4(int a, int b);
extern void OvlFunc_899_200c624(int a, int b, int c);
extern void OvlFunc_899_200c63c(int a, int b, int c);
extern void OvlFunc_899_200c658(int a, int b);
extern void OvlFunc_899_200c684(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_899_2008b48(void)
{
    unsigned char *p;
    int seen;

    __CutsceneStart();
    { PIN3; q0 = 0; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetSpeed(2, 0xcccc, 0x6666);
    __PlaySound(0x13);
    { PIN3; q0 = 0; q1 = 0xc0 << 1; q2 = 0xcc << 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 17; q2 = 0xcc << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xb8 << 1; q2 = 0xcc << 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xd0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN1; q0 = 0x85 << 4; seen = __GetFlag(q0); }
    if (seen == 0) {
        __SetFlag(0x85 << 4);
        OvlFunc_899_200c658(2, 0);
        __CutsceneWait(0x28);
        OvlFunc_899_200c684();
        __MessageID(0x1256);
        __PlaySound(0x3c);
        __CutsceneWait(0x1e);
        OvlFunc_899_200c63c(2, 3, 0x1e);
        OvlFunc_899_200c5f4(2, 0x1e);
        __Func_809259c(0, 1);
        __Func_80925cc(1, 1);
        __CutsceneWait(0x14);
        OvlFunc_899_200c658(2, 0);
        __CutsceneWait(0x28);
        OvlFunc_899_200c684();
        OvlFunc_899_200c5f4(2, 0x1e);
        OvlFunc_899_200c624(0, 1, 0x32);
        { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0xd0 << 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0x14);
        OvlFunc_899_200c658(2, 0);
        __CutsceneWait(0x28);
        OvlFunc_899_200c684();
        __Func_80925cc(2, 1);
        OvlFunc_899_200c5f4(2, 0x32);
        __MapActor_SetAnim(0, 3);
        OvlFunc_899_200c63c(1, 3, 0x14);
        OvlFunc_899_200c63c(2, 3, 0x14);
        OvlFunc_899_200c5f4(2, 0x28);
        __Func_80925cc(2, 1);
        __CutsceneWait(0x1e);
        { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0x1e);
        { PIN3; q0 = 2; q1 = 0xbc << 1; q2 = 0xbc << 1;
          __Func_80921c4(q0, q1, q2); }
        __CutsceneWait(0x28);
        OvlFunc_899_200c624(0, 1, 0x32);
        { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0xd0 << 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __Func_80925cc(2, 1);
        __CutsceneWait(0x32);
        OvlFunc_899_200c63c(2, 3, 0x1e);
        __Func_8092adc(2, 0x80 << 7, 0);
        __CutsceneWait(0xa);
        __ActorMessage(2, 0);
        __Func_8092c40(2, 0);
    } else {
        __PlaySound(0x3c);
        __MessageID(0x125d);
        __Func_8092c40(2, 0);
    }
    if (__Func_8091c7c(0, 0) == 0) {
        OvlFunc_899_2008de4();
        __SetFlag(0x856);
        __MapActor_SetAnim(2, 2);
        p = __MapActor_GetActor(0);
        if (p != 0) {
            __MapActor_TravelTo(2, *(short *)(p + 0xa), *(short *)(p + 0x12));
        }
        __MapActor_WaitMovement(2);
        __MapActor_SetPos(2, 0, 0);
    } else {
        __ActorMessage(2, 0);
    }
    __MapActor_SetAnim(1, 2);
    p = __MapActor_GetActor(0);
    if (p != 0) {
        __MapActor_TravelTo(1, *(short *)(p + 0xa), *(short *)(p + 0x12));
    }
    __MapActor_WaitMovement(1);
    __MapActor_SetPos(1, 0, 0);
    __PlayMapMusic();
    __CutsceneEnd();
}
