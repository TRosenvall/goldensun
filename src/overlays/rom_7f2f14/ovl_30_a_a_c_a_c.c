/* OvlFunc_968_20087d8 -- overlay 968 (rom_7f2f14), ovl_30_a_a_c_a_c.
 *
 * EXACT.  Re-run seven times:
 *
 *   OK OvlFunc_968_20087d8 -- 200 bytes, 76 encodings and 20 relocations identical
 *
 * RETIRES src/non_matching/ovl_7f2f14/20087d8.c, parked at 6 of 76 on
 * "CONSTANT-CSE": the ROM materialises -1 THREE TIMES
 * (`mov r0,#1 / mov r1,#1 / mov r2,#1 / neg r0 / neg r1 / neg r2`) where gcc
 * builds it once and copies.  The park's three solved observations -- the named
 * `p = &a->f55` pointer, the two zero stores spelled as the __GetFlag RESULT,
 * and the +0xa / +0x12 halves reached with the register-offset `ldrsh` -- are
 * all kept here verbatim and were all right.  What closes it is the ordinary
 * PIN4 at __Func_80933f8: pinning r0-r3 leaves no pseudo to common, so each -1
 * is rebuilt.  The park's closing question ("what makes gcc materialise the
 * same small constant twice") has the same answer as the sibling 200af30 in
 * this batch -- nothing does; you remove the pseudo instead.
 *
 * LANDING: WHOLE, no split, no linker edit.
 *   asmfacts.py: WHOLE  convert directly
 *   One function, zero `.section`/`.global`/`incbin`/`.lcomm`, no cross-object
 *   `.L` or data symbol.  The .ld line stays VERBATIM on the asm/ path:
 *     overlays/rom_7f2f14/overlay.ld:33
 *         asm/overlays/rom_7f2f14/ovl_30_a_a_c_a_c.o(.text)
 *   tryc.makefile_flags() = set(); no rom_7f2f14 wildcard reaches this stem
 *   (all of them are `ovl_30_c_a_c_a_c_a%` / `ovl_30_c_a_c_a_c_c%`).
 *   NOTE the same basename exists under overlays/rom_7ed0a0 -- match on the
 *   FULL PATH, which is what separates them.
 *
 * TWO LEVERS, AND THEY ARE INDEPENDENT AND BOTH REQUIRED (7 -> 3 -> 0):
 *
 * 1. THE DERIVED POINTER MUST BE BORN AFTER THE ARGUMENT FILL, NOT BEFORE.
 *    `q = &a->f55;` written before the __Func_80933f8 pin block puts
 *    `adds r7, #0x55` AHEAD of the three `negs`; the ROM emits it as the LAST
 *    insn of the argument setup, immediately before the `bl`.  Moving the one
 *    statement below the pin block is worth 4 encodings.  This is the mirror of
 *    the recorded "a pin constrains which register, not the schedule": the pin
 *    fixes r0-r3 and leaves r7's `add` free to float, and only source position
 *    holds it down.
 *
 * 2. PINNING r0 ALONE IS *NOT* THE CURE AT A POOLED-PAIR SITE; PIN2 IS.
 *    __MapActor_SetSpeed(0, 0x6666, 0x3333) has two POOL LOADS and one `movs
 *    r0,#0`, and the ROM interleaves a store between them:
 *
 *      rom   ldr r3,=OvlFunc_968_20086a0 / movs r0,#0 / str r3,[r5,#0x6c]
 *              / ldr r1,=0x6666 / ldr r2,=0x3333
 *      ours  ldr r3,=... / ldr r1,=0x6666 / str r3,[r5,#0x6c]
 *              / ldr r2,=0x3333 / movs r0,#0
 *
 *    The recorded "PINNING r0 ALONE ORDERS A POOLED r1" lever MISFIRES here:
 *    PIN1 is 4 differing, WORSE than the 3 of no pin at all.  PIN2 and PIN3 are
 *    both exact.  So the recogniser needs its width clause stated: r0-alone
 *    suffices only when the other argument has no bare constant left to
 *    re-home, and TWO pooled constants is not that case -- the second pool load
 *    still floats above `movs r0,#0` unless r1 is nailed as well.  PIN3 ships
 *    for uniformity (exact tie with PIN2).
 *
 * MEASURED WORSE / INERT (ref 76 encodings / 200 bytes):
 *
 *   spelling                                              differing
 *   ---------------------------------------------------  ---------
 *   park's candidate (no PIN4 at __Func_80933f8)              6
 *   `q = &a->f55` before the pin block, no SetSpeed pin        7
 *   `q = &a->f55` before the pin block, PIN1 at SetSpeed       8
 *   `q = &a->f55` before the pin block, PIN2/PIN3              4
 *   `q = &a->f55` after the pin block, no SetSpeed pin         3
 *   `q = &a->f55` after the pin block, PIN1 at SetSpeed        4
 *   THIS FILE (after the pin block, PIN3)                      0  EXACT
 *   PIN2 instead of PIN3 at __MapActor_SetSpeed                0  (tie)
 *
 * WHAT NEEDED NOTHING.  `if (f != 0) return;` as an early guard -- the ROM's
 * `bne .L886` goes straight to the epilogue and the function is `void`, so the
 * recorded "an early `return 0` guard is not a tail `return 0`" does not apply.
 * The two `__Actor_SetSpriteFlags(__MapActor_GetActor(0), n)` calls written
 * uncached, matching the ROM's repeated accessor.  `a->f6c` typed
 * `int (*)()` and stored as `(int (*)())f` for the zero write.
 *
 * -- worked in scratch_elev/b253/f968/87d8
 */
struct Actor {
    unsigned char pad00[0xa];
    short f0a;
    unsigned char pad0c[0x12 - 0xc];
    short f12;
    unsigned char pad14[0x55 - 0x14];
    unsigned char f55;
    unsigned char pad56[0x6c - 0x56];
    int (*f6c)();
};

extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __PlaySound(int id);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8092950(int a, int b);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_809202c(void);
extern int OvlFunc_968_20086a0();

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_968_20087d8(void)
{
    struct Actor *a;
    unsigned char *q;
    int f;

    a = __MapActor_GetActor(0);
    f = __GetFlag(0x109);
    if (f != 0)
        return;
    __CutsceneStart();
    { PIN4; q0 = -1; q1 = -1; q2 = -1; q3 = 0;
      __Func_80933f8(q0, q1, q2, q3); }
    q = &a->f55;
    *q = f;
    __MapActor_SetPos(0, a->f0a << 16, (a->f12 << 16) + 0xfff00000);
    __Func_8092950(0, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __MapTransitionIn();
    __WaitMapTransition();
    __PlaySound(0xe4);
    a->f6c = OvlFunc_968_20086a0;
    { PIN3; q0 = 0; q1 = 0x6666; q2 = 0x3333;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0, 0, 8);
    __Func_8092950(0, 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 1);
    __Func_8092304(0, 0, 8);
    *q = 3;
    a->f6c = (int (*)())f;
    __Func_809202c();
    __CutsceneEnd();
}
