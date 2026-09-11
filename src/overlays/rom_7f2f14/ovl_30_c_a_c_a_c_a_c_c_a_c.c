// fakematch
/* ovl_30_c_a_c_a_c_a_c_c_a_c.c  --  OvlFunc_968_2009218, the WHOLE of
 *   asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a_c_c_a_c.s.  `tools/asmfacts.py`
 *   says `WHOLE  convert directly`; `split_asm.py` reports `carries data: no`,
 *   `remaining funcs: 0`, `label exports: none needed`, and its BASENAME
 *   WARNING does not apply because the .s is deleted entirely.
 *
 *   OK OvlFunc_968_2009218 -- 732 bytes, 300 encodings and 57 relocations
 *   identical.  Measured six times.
 *
 * >>> THIS OBJECT NEEDS AN EXPLICIT -O2 RULE.  A FOURTH MIS-SCOPED O1 WILDCARD.
 *
 * `asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o` (Makefile:1030 and its
 * duplicates) captures this stem at -O1, and `tryc.makefile_flags()` returns
 * {'O1'}.  -O1 is WRONG here: the finished source is 157 of 300 differing at
 * -O1 and byte-identical at -O2.  Add, next to the sibling rule at Makefile:529
 * (OvlFunc_968_20090cc, same wildcard, same cure):
 *
 *   asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a_c_c_a_c.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a_c_c_a_c.c
 *   	$(GCC296_CC) $(GCC296_CFLAGS) -S -o $(@:.o=.s) $<
 *   	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
 *   	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
 *
 * An explicit rule overrides the wildcard without narrowing it.  WITHOUT that
 * rule this file screens green under `objcmp --func` only if -O2 is forced by
 * hand, and BUILDS RED -- which is the exact failure the Makefile:354 note
 * warns about.  overlay.ld carries ONE line for this object, rom_7f2f14
 * line 57 `asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a_c_c_a_c.o(.text)`; it
 * stays VERBATIM in the `asm/...o(SECTION)` form.  There is no `.data` line,
 * no .section/.word/.byte in the .s, and all four `.L` symbols are branch
 * targets defined in this file.  FAKEMATCH: register-pin idiom.
 *
 * WHAT THE FUNCTION DOES.  Fades out, plays a sting, then picks ONE of two
 * map-tile layouts on save bits 0x982/0x983 (bit 0 of iwram_3001e40 decides
 * the first time), paints twelve tile rectangles for the chosen one, fades
 * back in and runs two camera moves.  It brackets the whole thing by writing
 * 1 and then 0 to the halfword at iwram_3001ebc + 0xcb6.
 *
 * ===================================================================
 * AN ALLOCATION FIX FOR WHAT READS AS A SCHEDULING DEFECT.
 *
 * The ROM's two bracket stores are
 *
 *      ldr r3, =iwram_3001ebc / ldr r7, [r3] / ldr r3, =0xcba
 *      adds r2, r7, r3        / movs r3, #0  / strh r3, [r2]
 *
 * -- r3 is REUSED three times, and the offset's pool load sits AFTER
 * `ldr r7, [r3]`.  Plain C puts the offset in r2 and sched2 then hoists its
 * pool load ABOVE `ldr r7, [r3]` to cover the load-use interlock.  The hoist is
 * not the decision: it is only possible BECAUSE the allocator picked a register
 * other than r3.  When the offset IS r3 the hoist is blocked by the still-live
 * `ldr r3, =iwram_3001ebc`, and the ROM's order falls out.
 *
 *   `{ register int o __asm__("r3"); o = 0xcba; hp = (short *)(p + o);
 *      o = 0; *hp = o; }` is worth 11 -> 2 over the two stores.
 *
 * The SAME pin has to cover the stored zero as well as the offset -- pinning
 * the offset alone is 11 (INERT).  Two separate `register int __asm__("r3")`
 * declarations in one block and one variable reused for both roles are EXACTLY
 * TIED; the single variable is what ships.  r3 is caller-saved, so the recorded
 * callee-saved pin-miscompile hazard does not apply, and reaching ZERO is
 * itself the proof that the pin did not miscompile.
 *
 * > When the residue is an instruction hoisted over a load-use interlock, ask
 * > which register the hoisted value is in before reaching for a barrier. A
 * > post-reload scheduler cannot hoist a value into a register that is still
 * > live.
 * ===================================================================
 *
 * THE POOLED HALFWORD ZERO IS A TYPE QUESTION.  `*(short *)(p + 0xcba) = 0;`
 * emits `ldrh r3, .L8` off a `.word 0` -- a POOLED zero -- where the ROM has
 * `movs r3, #0`.  Storing an `int` local instead gives the `mov`.  Recorded
 * rule, second specimen, and it is what made the mid-function pool go away.
 *
 * -O2 SPENDS A FOURTH CALLEE-SAVED REGISTER ON THE SAVE BITS.  0x982 is tested
 * twice and 0x983 once, all three inside `if` conditions, so gcc commons them
 * (`ldr r6, =0x982`, `ldr r5, =0x983`) and then needs r8 for the iwram pointer:
 * `push {r5, r6, r7, lr} / mov r7, r8 / push {r7}` against the ROM's
 * `push {r5, r6, r7, lr}`.  The statement-expression pin
 * `if (({ PIN1; q0 = 0x982; __GetFlag(q0); }) == 0)` evicts all three and the
 * mask comes right: 300 of 300 (+28 bytes) -> 35 (exact size).  Third instance
 * of "a condition is a pin site"; the three are worth 227, 244 and 251
 * individually when dropped, the largest single-pin values recorded.
 *
 * THE 6-ARGUMENT CALL'S STACK PAIR WANTS TWO NAMED LOCALS.  `__CopyMapTiles`
 * takes four register arguments and two on the stack.  Written as literals the
 * first call of each arm emits `movs r3,#7 / str r3,[sp] / movs r3,#8 /
 * str r3,[sp,#4]`; the ROM materialises BOTH before either store
 * (`movs r3,#7 / movs r2,#8 / str / str`).  `u = 7; w = 8;` gives it.  Worth 3.
 * The other twenty-two calls pass the held `s`/`t` and need nothing.
 *
 * FILL ORDER AT __Func_80933f8, THE SAME SHAPE AS THE rom_7ca63c CUTSCENES.
 * The ROM emits `mov r0,#0xe4 / mov r1,#1 / neg r1,r1 / ldr r2 / mov r3,#1 /
 * lsl r0,#17` -- the `mov`s ascending but the SHIFTS descending.  The source
 * that emits that writes the mov and its shift ADJACENT
 * (`q0 = 0xe4; q0 <<= 17; q1 = 1; q1 = -q1; ...`), i.e. NOT the ROM's own
 * order: sched2 reorders the `mov`s to agree with the shift order.  Worth 2 ->
 * 0, measured at the FIRST of the two sites; six orders were swept there and
 * gave 0, 0, 2, 3, 3, 5.  In the MINIMISED base that first pin then turns out
 * to be removable -- gcc gets the site right unaided once everything else is in
 * place -- while the SECOND site's pin, which transcribes the ROM's own
 * interleaving (`q0 = 0xe4; q1 = 1; q0 <<= 17; q1 = -q1;`), is worth 19.  The
 * two sites emit the same four values in two different orders and each needs
 * its own answer; a lever measured on one is not a lever on the other.
 *
 * `-fno-schedule-insns2` REGRESSES, 0 -> 157.  `-ffixed-r7` is WORSE (+8 bytes)
 * -- the ROM does spend r7 here, so the flag is refuted on the recorded second
 * arm.  Neither is shipped.
 *
 * WHAT CLOSED IT (300 encodings, 732 bytes)              differing
 * ----------------------------------------------------  ---------
 *   plain C at the inherited -O1                              277  (+16 bytes,
 *                                        with a mid-function pool the ROM lacks)
 *   plain C at -O2                                            300  (+28 bytes)
 *   + the three __GetFlag conditions pinned, the stored
 *     halfword zero given an int local                         35  (exact size)
 *   + the second `if` transcribed as `== 0`, the two
 *     stack-argument pairs named, __Func_80933d4 and the
 *     two __Func_80933f8 pinned                                11
 *   + both bracket stores written through a `short *` with
 *     the offset AND the zero pinned to r3                      2
 *   + the __Func_80933f8 mov/shift orders settled                 0
 *
 * MEASURED WORSE / INERT
 *
 *   spelling                                            result
 *   -------------------------------------------------  ----------------
 *   the whole file at -O1 (the wildcard's flag)         157
 *   -fno-schedule-insns2                                157
 *   -ffixed-r7                                          +8 bytes (WORSE)
 *   the r3 pin on the offset ALONE                       11 (INERT)
 *   the stored zero as a bare literal                    pooled `ldrh`
 *   INERT, dropped: __Func_80933d4's pin, the FIRST
 *     __Func_80933f8's pin (the SECOND is worth 19), and all four
 *     __SetFlag/__ClearFlag pins -- seven of sixteen
 *     devices, removed by a per-device fixpoint and
 *     re-checked AS A SET.  `one` at the bracket store is
 *     also inert and is KEPT: the ROM holds one 1 in r5
 *     for both the store and the `& 1`, and writing the
 *     literal there says something the ROM does not.
 */
extern char *iwram_3001ebc;
extern int iwram_3001e40;
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_801776c(int a, int b);
extern void __Func_8091200(int a, int b);
extern void __Func_8091220(int a, int b);
extern void __Func_8091254(int a);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_968_2009218(void)
{
    char *p;
    int one;
    short *hp;
    int s;
    int t;
    int u;
    int w;

    p = iwram_3001ebc;
    { register int o __asm__("r3"); o = 0xcba; hp = (short *)(p + o); o = 0; *hp = o; }
    one = 1;
    *(short *)(p + 0xcb6) = one;
    __CutsceneStart();
    __MapActor_SetAnim(0, 1);
    __Func_801776c(0x2688, 1);
    __Func_8091220(0x80 << 9, 0);
    __Func_8091200(0x10005, 0);
    __Func_8091254(0x78);
    __CutsceneWait(0x64);
    __PlaySound(0x8e);
    __CutsceneWait(0x1e);
    __Func_8091200(0x7fff, 0);
    __Func_8091254(0x3c);
    __CutsceneWait(0x46);
    if (({ PIN1; q0 = 0x982; __GetFlag(q0); }) == 0
        && ({ PIN1; q0 = 0x983; __GetFlag(q0); }) == 0) {
        if ((iwram_3001e40 & one) != 0)
            __SetFlag(0x982);
        else
            __SetFlag(0x983);
    }
    if (({ PIN1; q0 = 0x982; __GetFlag(q0); }) == 0) {
        __SetFlag(0x982);
        __ClearFlag(0x983);
        u = 7;
        w = 8;
        __CopyMapTiles(0x67, 0x1b, 0x59, 0x1b, u, w);
        s = 3;
        t = 2;
        __CopyMapTiles(0x29, 0x5a, 0x1b, 0x5c, s, t);
        __CopyMapTiles(0x29, 0x5a, 0x1d, 0x5d, s, t);
        __CopyMapTiles(0x29, 0x5a, 0x1b, 0x5e, s, t);
        __CopyMapTiles(0x29, 0x5a, 0x1b, 0x60, s, t);
        __CopyMapTiles(0x29, 0x5a, 0x1d, 0x61, s, t);
        __CopyMapTiles(0x29, 0x60, 0x19, 0x5b, s, t);
        __CopyMapTiles(0x29, 0x5c, 0x19, 0x5d, s, t);
        __CopyMapTiles(0x29, 0x60, 0x19, 0x5f, s, t);
        __CopyMapTiles(0x29, 0x60, 0x19, 0x61, s, t);
        __CopyMapTiles(0x29, 0x60, 0x1b, 0x60, s, t);
        __CopyMapTiles(0x29, 0x60, 0x1d, 0x61, s, t);
    } else {
        __SetFlag(0x983);
        __ClearFlag(0x982);
        u = 7;
        w = 8;
        __CopyMapTiles(0x6f, 0x1b, 0x59, 0x1b, u, w);
        s = 3;
        t = 2;
        __CopyMapTiles(0x29, 0x5a, 0x19, 0x5b, s, t);
        __CopyMapTiles(0x29, 0x5a, 0x19, 0x5d, s, t);
        __CopyMapTiles(0x29, 0x5a, 0x19, 0x5f, s, t);
        __CopyMapTiles(0x29, 0x5a, 0x19, 0x61, s, t);
        __CopyMapTiles(0x29, 0x5a, 0x1b, 0x60, s, t);
        __CopyMapTiles(0x29, 0x5a, 0x1d, 0x61, s, t);
        __CopyMapTiles(0x29, 0x5e, 0x1b, 0x5c, s, t);
        __CopyMapTiles(0x29, 0x60, 0x1d, 0x5d, s, t);
        __CopyMapTiles(0x29, 0x5e, 0x1b, 0x5e, s, t);
        __CopyMapTiles(0x29, 0x60, 0x1b, 0x60, s, t);
        __CopyMapTiles(0x29, 0x60, 0x1d, 0x61, s, t);
    }
    __Func_8091200(0x80 << 9, 0);
    __Func_8091254(0x14);
    __CutsceneWait(0x28);
    __Func_80933d4(0x80 << 8, 0x80 << 5);
    __Func_80933f8(0xe4 << 17, -1, 0x21e0000, 1);
    __Func_8093530();
    __CutsceneWait(0x32);
    { PIN4; q0 = 0xe4; q1 = 1; q0 <<= 17; q1 = -q1; q2 = 0x1a70000; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneEnd();
    { register int o __asm__("r3"); o = 0xcb6; hp = (short *)(p + o); o = 0; *hp = o; }
}
