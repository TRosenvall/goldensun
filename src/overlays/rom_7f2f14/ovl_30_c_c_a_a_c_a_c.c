/* OvlFunc_968_200af30 -- overlay 968 (rom_7f2f14), ovl_30_c_c_a_a_c_a_c.
 *
 * EXACT.  Re-run seven times, three of them in a consolidated pass over all
 * six functions of this batch:
 *
 *   OK OvlFunc_968_200af30 -- 92 bytes, 37 encodings and 9 relocations identical
 *
 * RETIRES src/non_matching/ovl_7f2f14/200af30.c, which parked this at 21 of 37
 * on "SHARED-BASE CONSTANT CSE": the park's candidate hoisted 0x80<<7 into r5
 * (`push {r5, lr}` against the ROM's `push {lr}`) and kept it across three
 * calls, where the ROM rebuilds `mov rN,#0x80 / lsl rN,#7` at each site.  The
 * park's own note that "writing 0x4000 instead of the shift does not hide the
 * shared base" is correct and beside the point -- THE CONSTANT IS NOT THE
 * PROBLEM, THE PSEUDO IS.  A `register int qN __asm__("rN")` argument pin
 * leaves no pseudo for gcse to common, so nothing survives the call and each
 * site rebuilds.  With PIN3/PIN4 at the five constant-argument sites the
 * prologue collapses to `push {lr}` and 21 differing becomes 2.
 *
 * LANDING: WHOLE, no split, no linker edit.
 *   asmfacts.py: WHOLE  convert directly
 *   The .s holds exactly one function and ZERO `.section`/`.global`/`incbin`/
 *   `.lcomm` lines.  One .ld line names the object and it STAYS VERBATIM on the
 *   asm/ path -- the build rule is `asm/%.o: src/%.c`:
 *     overlays/rom_7f2f14/overlay.ld:82
 *         asm/overlays/rom_7f2f14/ovl_30_c_c_a_a_c_a_c.o(.text)
 *   tryc.makefile_flags() = set() -- tree default -O2 -fcall-used-r4, and no
 *   wildcard can reach this stem: every rom_7f2f14 pattern rule in the Makefile
 *   is scoped to `ovl_30_c_a_c_a_c_a%` or `ovl_30_c_a_c_a_c_c%`, which diverge
 *   from `ovl_30_c_c_...` at the first component after `ovl_30_`.
 *   No `.L` or data symbol is referenced from another object.
 *
 * ------------------------------------------------------------------- NEW ----
 * A CALL-ORDER RESIDUE CAN NEED THE CALLEE'S *RETURN TYPE* AND A FILL ORDER
 * TOGETHER, AND NEITHER ALONE IS WORTH ANYTHING.
 *
 * With all five sites pinned ascending the function is 2 differing, and the two
 * are one swapped pair inside OvlFunc_968_2008058's argument setup:
 *
 *     rom    mov r0,#0x82 / mov r2,#0xc4 / mov r3,#0xdf / lsl r2,#18
 *              / mov r1,#0 / lsl r0,#18
 *     ours   ... same four movs ...        / lsl r0,#18 / mov r1,#0 / lsl r2,#18
 *
 * Twenty-two spellings of that one site -- all 24 permutations of the four
 * assignments, both split-shift forms, PIN1/PIN2/PIN3/PIN4 and no pin at all --
 * are ALL 2, 3 or 4 differing while the callee is declared `void`.  The site is
 * not reachable from its own fill.
 *
 * The lever is the CALLEE'S RETURN TYPE, and it is the tree's own declaration:
 * src/overlays/rom_7f2f14/ovl_30_a_a_a_c_a_b.c defines
 * `void *OvlFunc_968_2008058(...)` and the sibling ovl_30_c_a_c_c_c_a_c_b.c
 * declares it `void *` too.  Declared `void`, the call is a bare CLOBBER of r0;
 * declared `void *` (or `int`), it is a `call_value` with an explicit SET, and
 * the output dependence on r0 that the SET creates is what pins `lsl r0,#18`
 * last -- the same mechanism src/overlays/rom_7f2f14/ovl_30_a_c_c_a_c_a.c
 * records for `void f(void)` prototypes reaching BACKWARD, seen here on a call
 * with arguments.
 *
 * BUT THE RETURN TYPE ALONE IS ALSO 2.  It fixes the `lsl` pair and breaks
 * `mov r1,#0` one slot early.  Only the SET plus a fill that seeds q1 LAST is
 * exact.  Four of the 24 permutations pass -- acbd, acdb, adcb, dacb -- and
 * every one of them has `q1 = 0` after `q2`.  `acdb` ships because it is also
 * the ROM's own emission order (r0, r2, r3, r1).
 *
 * MEASURED WORSE / INERT (ref 37 encodings / 92 bytes, relocations SILENT
 * throughout, so this was an ordering problem end to end -- the recorded
 * "2-3 differing with relocations silent is ordering" recogniser fired exactly):
 *
 *   spelling                                                  differing
 *   -------------------------------------------------------  ---------
 *   park's candidate (no pins, named constants)                21 (+ wider push)
 *   `void` return, ascending fill                               2
 *   `void` return, any of the other 23 permutations           2-4
 *   `void` return, split shifts (q0 = 0x82; q0 <<= 18)        2-4
 *   `void` return, PIN1 / PIN2 / PIN3 / no pin at the site     2-3
 *   `void *` return, ascending (abcd) fill                      2
 *   `void *` return, bacd / badc / bdac / dabc / dbac           2
 *   `void *` return, split-shift forms                        2-4
 *   `int` return instead of `void *`                            0  (tie; the
 *       tree's own `void *` ships)
 *   retyping __Func_8092708 or __Func_8092adc to `int`         2, 5
 *
 * WHAT NEEDED NOTHING.  The `__Func_8092708(0, 6, 0)` site emits the ROM's
 * `mov r1,#6 / mov r2,#0 / mov r0,#0` -- r0 seeded LAST -- from the ORDINARY
 * ASCENDING PIN3.  The documented "where the ROM seeds r0 last, no r0 pin can
 * express it" is about sites where the pin cannot be written at all; here sched2
 * reconstructs the descending emission from the canonical ascending source, so
 * the recogniser must not be read as "ROM seeds r0 last => drop the pin".
 * Likewise the three `mov / mov / mov / lsl / lsl` seeds-then-shifts sites all
 * fall out of one statement per argument, whole value per statement.
 *
 * -- worked in scratch_elev/b253/f968/af30
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092708(int a, int b, int c);
extern void __Func_8091e9c(int n);
extern void *OvlFunc_968_2008058(int a, int b, int c, int d);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_968_200af30(void)
{
    __CutsceneStart();
    { PIN3; q0 = 0; q1 = 0x80 << 8; q2 = 0x80 << 7;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x82 << 2; q2 = 0xb2 << 2;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x80 << 7; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    { PIN4; q0 = 0x82 << 18; q2 = 0xc4 << 18; q3 = 0xdf; q1 = 0;
      OvlFunc_968_2008058(q0, q1, q2, q3); }
    { PIN3; q0 = 0; q1 = 6; q2 = 0;
      __Func_8092708(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __Func_8091e9c(0x14);
    __CutsceneEnd();
}
