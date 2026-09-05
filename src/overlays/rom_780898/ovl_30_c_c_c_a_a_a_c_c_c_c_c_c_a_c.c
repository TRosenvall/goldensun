/* OvlFunc_883_200b2b0  --  0x0200b2b0, and OvlFunc_883_200b380  --  0x0200b380.
 *
 * The WHOLE of asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_a_c.s:
 * the .s holds exactly two functions and nothing else, and both are here, so
 * this TU replaces the file outright.  NO SPLIT IS NEEDED.  overlay.ld names
 * the object on ONE line only,
 *
 *     asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_a_c.o(.text)
 *
 * (overlays/rom_780898/overlay.ld:67); there is no .data/.data1 line to remap.
 * Landing is: add this .c under the same basename, delete the hand-written
 * asm/.../ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_a_c.s, leave overlay.ld alone.
 * Byte-exact as a TU: 428 bytes of .text (0x1ac), identical bytes and 25
 * identical relocations against the assembled reference; each function also
 * verified alone under objcmp --func.
 *
 * THE TWO FUNCTIONS ARE NEAR-SIBLINGS, and solving the first made the second
 * nearly free.  Same skeleton -- fetch the actor, take its +0x50 sub-object,
 * raise speed, two path calls, clear two flag bytes, play anim1, walk, wait,
 * play anim2, walk, set the flag bytes, optionally set +0x55 to 3, play anim 1.
 * They differ in five places only: the path constants (0x376/0x36b/0x35b
 * against 0x35b/0x36b/0x37a), __Func_8092adc's first argument (a literal 0 in
 * _2b0, `slot` in _380 -- read off `mov r0, #0` vs `mov r0, r7`), _2b0 waits
 * twice and _380 once, _380 adds `a->f28 = 0x80 << 10`, and _380's call order
 * puts __MapActor_SetSpeed(0x4ccc, 0x2666) after the wait.  _380 needed no new
 * reading at all: the same struct spelling and the same pin rule, transcribed,
 * screened OK on the FIRST try and minimised to the same seven sites.
 *
 * THE PROLOGUE IS WIDE AND IT IS STILL A PIN FUNCTION.  `push {r5, r6, r7, lr}`
 * plus r8/r9/r10 (r8-r11 in _380) looks like a function that keeps things
 * across calls, and it is -- but every one of the six saved registers holds a
 * POINTER OR AN ARGUMENT: r5 = the +0x50 sub-object, r6 = the actor, r7 = slot,
 * r8/r9/r10 = the three parameters.  Not one holds a constant, so the ROM
 * rebuilds `0x80 << 9`, `0x80 << 8` and `0xc4 << 1` at every use and only PINS
 * reach it.  This is the recorded "read the prologue by CONTENT, not width".
 *
 * SEVEN PINS AT A GREEDY FIXPOINT OVER TWELVE SITES (eleven in _380).  Exactly
 * the three-argument sites that carry a rebuilt constant are load-bearing:
 * __MapActor_SetSpeed, __Func_80921c4, __Func_8092adc, __Func_8092158.  All
 * five __MapActor_SetAnim and __CutsceneWait sites measure EXACTLY INERT and
 * are not shipped.  Two rounds to the fixpoint, verified as a set under objcmp
 * after every drop.
 *
 * THE PIN SET IS A SIZE, NOT A SET -- AND IT MOVES.  Pinning only the first
 * __MapActor_SetSpeed took 84 differing to 81; the constant-CSE then reappeared
 * one site LOWER, hoisting `0xc4 << 1` into r11 across the three __Func_8092158
 * / __Func_80921c4 sites.  "Pin the first use" does not mean "one pin": each
 * INDEPENDENT repeated constant needs its own first-use pin, and you only see
 * the second one after the first is in place.
 *
 * THE UNIFORM FILL IS RIGHT HERE.  One statement per argument, ascending
 * q0..q3, whole value per statement, at every pinned site -- byte-identical to
 * the ROM even though the ROM's emitted order is transposed at nine of the
 * fourteen sites (`mov r1 / mov r2 / mov r6 / lsl r1 / mov r0 / lsl r2`,
 * `mov r1 / mov r0 / lsl r1 / ldr r2`, and so on).  sched2 produces every one
 * of those from the uniform spelling.  The shifted-byte spellings `0x80 << 9`,
 * `0x80 << 8`, `0xc0 << 8`, `0xc4 << 1` and `0x80 << 10` ARE load-bearing --
 * the ROM builds them with mov/lsl -- while 0x376, 0x36b, 0x35b, 0x37a, 0x4ccc
 * and 0x2666 are written whole and pool.
 *
 * ===================================================================
 * NEW: TWO BYTE STORES THAT MAY ALIAS COST ONE SCHEDULING SLOT, AND
 * DIFFERENT STRUCT TAGS BUY IT BACK.
 * ===================================================================
 *
 * This is what the whole function turned on, and it is a mechanism the notebook
 * did not have.
 *
 * The residue was TWO instructions.  The ROM has
 *
 *     strb r3, [r6]      @ actor + 0x55
 *     strb r3, [r5]      @ sub    + 0x26
 *     mov  r0, r7
 *
 * and every `unsigned char *` spelling gives
 *
 *     strb r3, [r6]
 *     mov  r0, r7
 *     strb r3, [r5]
 *
 * READ FROM haifa-sched.c, NOT INFERRED.  All four candidates in the ready list
 * tie at INSN_PRIORITY 13, so `rank_for_schedule` falls past the priority test.
 * The notebook already records its LAST tie-break (the dependent count).  It has
 * one MORE test in between, and that is the one that decides here:
 *
 *     /-- Classify the instructions into three classes:
 *         1) Data dependent on last schedule insn.
 *         2) Anti/Output dependent on last scheduled insn.
 *         3) Independent of last scheduled insn, or has latency of one.
 *         Choose the insn from the highest numbered class if different. --/
 *
 * With both stores reached through `unsigned char *` -- alias set 0, which
 * aliases everything -- the second store carries an OUTPUT DEPENDENCE on the
 * first.  The first store is the last-scheduled insn and its cost is 2, so the
 * `latency of one` escape does not apply: the second store is class 2.
 * `mov r0, r7` is independent of it, so it is class 3, and class 3 wins
 * OUTRIGHT -- before the dependent count is ever consulted.  That count would
 * have gone the other way: the store has NINE dependents to the mov's three.
 *
 * The cure is to make the dependence not exist.  gcc-2.96 gives a DISTINCT
 * ALIAS SET PER STRUCT TAG (already on the books for gcse hashing); reaching
 * the two bytes as `a->f55` and `c->f26` through two different tags puts them
 * in non-conflicting sets, `write_dependence_p` reports no dependence, both
 * insns are class 3, and the dependent count then picks the store -- the ROM's
 * order.  Confirmed first with a deliberately wrong probe (`*(int *)a = 0` and
 * `*(short *)(c + 0x26) = 0`), which made the two stores adjacent, before any
 * legal spelling was written.
 *
 * > Two byte stores through char pointers can never be disambiguated, because
 * > character types are alias set 0.  When the residue is ONE instruction
 * > wedged between two narrow stores, give each store its own STRUCT TAG.
 *
 * THE THREE REGISTER PINS THIS REPLACED MEASURED EXACTLY ZERO AFTERWARDS.
 * Before the alias reading, the three parameters came out of local-alloc in the
 * wrong order -- anim1/anim2/flag in r9/r8/r10 against the ROM's r8/r10/r9 (and
 * note reg_alloc_order in arm.h is `... 6, 7, 8, 10, 9, 11 ...`, so r10 really
 * is allocated BEFORE r9).  Six of the eight differing lines were that rotation.
 * `register int anim1 __asm__("r8")` plus `register int anim2 __asm__("r10")`
 * fixed it and took the screen 8 -> 2; ANY TWO of the three pins did, and all
 * three together were much worse (16).  Every one of them measures EXACTLY
 * INERT once the struct tags are in, and none ships.  Textbook "a pin can be a
 * symptom of a different defect" -- re-test every drop after any unrelated edit.
 *
 * ONE MORE READING, small but load-bearing before the structs went in: the
 * actor pointer must be UPDATED IN PLACE, not copied.  A separate
 * `p = a + 0x55` local costs a seventh callee-saved value and a `mov r6, r8`;
 * `a += 0x55` coalesces onto the ROM's r6 (84 -> 79 differing).  The struct
 * spelling makes the point moot -- `a->f55` needs no pointer at all -- but the
 * measurement is the recorded in-place-update rule confirmed again.  _380 is
 * the counter-case in the same file: there the actor is still needed for
 * `a->f28` after the flag byte, so the ROM really does keep TWO pointers
 * (`mov r2, #0x55 / add r2, r6 / mov r11, r2`) and spends r11 for it.  Written
 * as struct members that falls out with no help.
 *
 * MEASURED-WORSE TABLE (tryc differing, reference is 81 lines / 88 for _380)
 *
 *   plain C, no pins, `p = a + 0x55` local ............... 84  (88 lines)
 *   + pin site 1 only (__MapActor_SetSpeed) .............. 81  (85 lines)
 *   + `a += 0x55` in place of the extra pointer .......... 79  (84 lines)
 *   + pin site 2 (__Func_80921c4, the 0xc4<<1 CSE) ....... 53  (82 lines)
 *   all 12 sites pinned, char-pointer stores ............. 8
 *     `if (flag)` instead of `if (flag != 0)` ............ 8   (invariant)
 *     `if (flag == 0) goto` inverted ..................... 8   (invariant)
 *     `unsigned int flag` ................................ 8   (invariant)
 *     drop the __MapActor_SetAnim pins ................... 8   (invariant)
 *     copy params to locals, all 3 orders ................ 8   (invariant)
 *     -fno-rerun-cse-after-loop .......................... 8   (invariant)
 *   `c->f26 = 1` moved BELOW the `if` .................... 17
 *   one high-register pin (any single one of three) ...... 6
 *   two high-register pins (any pair) .................... 2
 *   all three high-register pins ......................... 16
 *   (next four measured on the 2-differing register-pin base)
 *   reverse the two zero stores .......................... 3
 *   descending fill at the SetAnim site .................. 3
 *   pin only q1 at the SetAnim site ...................... 3
 *   drop, or reorder, the pin at __Func_8092adc .......... 4
 *   -O1 .................................................. 31
 *   -fno-schedule-insns2 ................................. 31
 *   DIFFERENT STRUCT TAGS, no register pins .............. 0   EXACT
 *
 * Tree default -O2; no Makefile rule or wildcard captures this object, and none
 * is wanted -- all three flag probes are worse or neutral.  This TU must never
 * fall under an -fno-strict-aliasing rule: the struct-tag separation is exactly
 * what that flag destroys.
 */
struct Sub {
    unsigned char pad00[0x26];
    unsigned char f26;
};

struct Actor {
    unsigned char pad00[0x28];
    int f28;
    unsigned char pad2c[0x50 - 0x2c];
    struct Sub *f50;
    unsigned char pad54[1];
    unsigned char f55;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __CutsceneWait(int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_883_200b2b0(int slot, int anim1, int anim2, int flag)
{
    struct Actor *a;
    struct Sub *c;

    a = __MapActor_GetActor(slot);
    c = a->f50;
    { PIN3; q0 = slot; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = slot; q1 = 0xc4 << 1; q2 = 0x376;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    a->f55 = 0;
    c->f26 = 0;
    __MapActor_SetAnim(slot, anim1);
    { PIN3; q0 = slot; q1 = 0x4ccc; q2 = 0x2666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = slot; q1 = 0xc4 << 1; q2 = 0x36b;
      __Func_8092158(q0, q1, q2); }
    __CutsceneWait(0xa);
    __MapActor_SetAnim(slot, anim2);
    { PIN3; q0 = slot; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = slot; q1 = 0xc4 << 1; q2 = 0x35b;
      __Func_8092158(q0, q1, q2); }
    c->f26 = 1;
    if (flag != 0)
        a->f55 = 3;
    __CutsceneWait(0xa);
    __MapActor_SetAnim(slot, 1);
}

void OvlFunc_883_200b380(int slot, int anim1, int anim2, int flag)
{
    struct Actor *a;
    struct Sub *c;

    a = __MapActor_GetActor(slot);
    c = a->f50;
    { PIN3; q0 = slot; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = slot; q1 = 0xc4 << 1; q2 = 0x35b;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = slot; q1 = 0xc0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    a->f55 = 0;
    c->f26 = 0;
    __MapActor_SetAnim(slot, anim1);
    { PIN3; q0 = slot; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = slot; q1 = 0xc4 << 1; q2 = 0x36b;
      __Func_8092158(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q0 = slot; q1 = 0x4ccc; q2 = 0x2666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(slot, anim2);
    { PIN3; q0 = slot; q1 = 0xc4 << 1; q2 = 0x37a;
      __Func_8092158(q0, q1, q2); }
    a->f28 = 0x80 << 10;
    c->f26 = 1;
    if (flag != 0)
        a->f55 = 3;
    __MapActor_SetAnim(slot, 1);
}
