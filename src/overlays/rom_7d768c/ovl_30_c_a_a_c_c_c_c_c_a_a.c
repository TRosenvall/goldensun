// fakematch
/* OvlFunc_952_2008af8  --  0x02008af8
 *   [asm/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c_c_c_a_a.s, 1st of 1 -- WHOLE FILE]
 *
 * 496 instructions of cutscene script around a quadrant facing test, an
 * already-seen-it early return and four `__GetFlag(0x96a)` diamonds.  Sibling
 * of OvlFunc_952_2008ff8 (src/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c_c_c_a_b.c,
 * batch 228): the two were cut out of one `.s`, and that one is the 2nd piece.
 * VERDICT:
 *
 *   OK OvlFunc_952_2008af8 -- 1280 bytes, 504 encodings and 130 relocations identical
 *
 * THE ONE THING THAT MATTERS HERE IS THAT THE TWO LEVERS ARE ONE LEVER.  The
 * prologue is `push {r5, r6, lr}` -- TWO callee-saved registers, and the ROM
 * says what each is for.  r5 is the facing local.  r6 is 0x8000, commoned by
 * cse_main and reloaded with `mov rN, r6` at six argument sites.  Plain C
 * spends FOUR: r5, r6, r7 and r8, i.e. `push {r5,r6,r7,lr}` plus a `mov r6,r8`
 * frame push -- 512 encodings against 504.  Pinning the repeated expensive
 * constants takes that to 504 with 17 differing, and the whole 17 is ONE
 * cause: the flag id 0x96a, pooled and used SIX times with the first use
 * dominating the other five, hoisted into r7.  That is the textbook
 * commoned-constant tell, and the recorded remedies are `CSE_CFLAGS` and
 * separate named locals.
 *
 *   named locals for 0x96a (one each, or one shared)  -- 17, unchanged
 *   CSE_CFLAGS alone                                  -- 510 encodings, 428 differ
 *   `register int e __asm__("r6")` alone              -- 17, unchanged
 *   BOTH                                              -- BYTE-IDENTICAL
 *
 * A ONE-LEVER-AT-A-TIME SWEEP REPORTS BOTH HALVES AS "NO BETTER" OR "MUCH
 * WORSE" AND IS WRONG.  `-fno-rerun-cse-after-loop` does defeat the 0x96a
 * hoist -- and it also defeats the 0x8000 commoning the ROM HAS, so r6
 * disappears (`push {r5, lr}`) and every one of the six uses rebuilds
 * `mov #0x80 / lsl #8`.  Naming 0x8000 in a plain `int e` does not restore it,
 * because cprop folds a never-modified constant local straight back into its
 * uses; the local has to be a REGISTER local for gcc to carry it.  So the flag
 * removes a CSE the ROM does not have, the r6 pin re-supplies the one it does,
 * and neither statement is true without the other.  This is the second half of
 * "the constant-CSE rule needs BOTH halves" arriving from a new direction: the
 * flag is not free, and what it costs has to be bought back by hand.
 *
 * NEW, and grepped first ("named constant", "callee-saved", "cprop",
 * "commoned-constant tell"): the recorded remedy list for the commoned-constant
 * tell is `CSE_CFLAGS` OR named locals, tried in that order, park if neither.
 * There is a THIRD outcome -- the flag plus a REGISTER local naming the
 * constant the flag over-kills.  Look for it whenever the ROM's push list is
 * NON-EMPTY: the flag is a blunt instrument that clears every commoned
 * constant, so on a function whose push list the ROM genuinely uses, the flag
 * must be paired with an explicit re-statement of the CSE that should survive.
 * `docs/elevation.md` says "A value in a callee-saved register is NOT evidence
 * the source named it" -- true at -O2, but UNDER THIS FLAG the implication
 * reverses, and the ROM's push list becomes the specification for which
 * register locals to write.
 *
 * THE FACING TEST is the batch-91 quadrant spelling verbatim -- rotate by half
 * a quadrant, mask to the top two bits, compare -- with the ROM's `lsl #16 /
 * asr #16` narrowing making the local a SIGNED short and a later `lsl #16 /
 * lsr #16` re-reading it unsigned.  Written as a `(short)` store followed by
 * `d = (unsigned short)d;` where the ROM re-narrows, both shifts fall out and
 * both comparisons (`== 0`, `== 0x8000`) read the u16.  The mask spelling is
 * the recorded lever and it was RE-MEASURED, not transplanted:
 *
 *   `& ~0x3fff`        -- exact
 *   `& -0x4000`        -- exact (a second spelling, not previously recorded)
 *   `& 0xffffc000`     -- 1 encoding: gcc narrows the mask and the POOL WORD
 *                         becomes 0xc000
 *
 * The literal form is the one that narrows; both complement-shaped forms are
 * opaque to it.  Splitting the expression across two statements costs 4.
 *
 * TWENTY-TWO PINS from 39 candidate sites.  All 39 expensive-constant and
 * ordering-only sites were pinned first and that also matches byte for byte.
 * Greedy removal re-tested under objcmp after every drop and run to a fixpoint
 * from BOTH ends of the list agrees on the same 22, and a one-at-a-time
 * confirmation round over the survivors finds every one load-bearing.  The
 * seventeen inert ones are sites 7, 16, 26, 28, 30, 40, 44, 54, 59, 66, 70, 81,
 * 88, 92, 97, 104 and 128 (numbering is the ordinal of the `bl` in the
 * reference).  NINE of those are 0x96a sites (7, 66, 70, 88, 92, 128) or
 * sites whose constant IS the r6 local (16, 28, 44) -- work the flag and the
 * register local already do; INERT SCAFFOLDING MUST NOT SHIP and they are gone.
 *
 * THE FILLS ARE UNIFORM ASCENDING, whole value per statement, at all 21 sites
 * that take one -- descending at any single one of them costs 2 to 9 encodings
 * and descending everywhere costs 49, even though the ROM emits several of
 * these sites in a descending or interleaved order (`mov r2 / mov r0 / mov r1 /
 * neg r2` at __Func_8092304(1, 0, -0x20), `mov r1 / mov r2 / lsl r1 / mov r0`
 * at __Func_8092adc(2, 0xe0 << 8, 0)).  sched2 reproduces them from the one
 * spelling.
 *
 * __Func_8092c40 IS THE EXCEPTION AGAIN, at its single site: `q1 = 0; q0 = -1;`
 * is exact and ascending costs 2.  That is one more for the corpus tally, and
 * it is a site whose ROM order (`mov r0, #1 / mov r1, #0 / neg r0, r0`) LOOKS
 * ascending -- the `neg` is scheduled last, so reading the emitted order is not
 * how this call is diagnosed.
 *
 * THE SHIFTED-BYTE SPELLING IS COSMETIC.  Rewriting every `0x80 << 9` style
 * constant as its flat literal (0x10000, 0x6000, 0xe000, ...) is
 * byte-identical.  The `<<` form is kept because it reads as the ROM's
 * `mov`/`lsl` pair.
 *
 * ALL FOUR POOLED VALUES ARE BARE LITERALS.  objcmp reports 130 relocations
 * identical; the reference has 129 `bl` instructions, so the only non-call
 * relocation is the single R_ARM_ABS32 iwram_3001ebc and 0x1ffb, 0x96a,
 * 0x13333 and 0x9999 carry none.  Nothing belongs in const.sym or message.sym
 * for this function.
 *
 * LANDING.  The `.s` holds ONE function, so this is a whole-file replacement
 * and NO linker edit is needed: the single line naming the .o is already
 * `asm/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c_c_c_a_a.o(.text)` in
 * overlays/rom_7d768c/overlay.ld, inside the `.text` output section, and the
 * `.data` section there names only ovl_30_c_c.o.  (The zero-length `.data` and
 * `.bss` rows for this .o in overlay.map are the empty sections every object
 * has and are NOT evidence of data.)  What IS required is a Makefile rule --
 * no rule or wildcard currently names rom_7d768c, so the TU would build at the
 * tree default -O2 and the screen would be green while the build is red:
 *
 *     asm/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c_c_c_a_a.o: \
 *         src/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c_c_c_a_a.c
 *             $(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
 *             ...
 *
 * the same CSE_CFLAGS group and the same guard/set shape as the rom_7b9cb4
 * rule already in the Makefile.
 *
 * Reproduce: scratch_elev/b236/f2008af8/gen.py emits this file from a pin set
 * given as reference call-site numbers; minimise.py runs the greedy sweep from
 * either end and sweep.py the measured-worse table, each inside ONE container
 * invocation.  runcmp.py drives tools/objcmp.py with the TU's flag group, which
 * objcmp cannot yet read from the Makefile because the rule above is the thing
 * being proposed.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __InnHeal(int a);
extern int __GetFlag(int f);
extern void __SetFlag(int f);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_WaitScript(int slot);
extern void __Func_808e118(void);
extern int __Func_8091c7c(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80922c4(int a, int b, int c);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_809233c(int a, int b, int c, int d);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);

extern unsigned char *iwram_3001ebc;

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_952_2008af8(void)
{
    unsigned char *p;
    int d;
    register int e __asm__("r6");

    d = (short)((*(unsigned short *)(__MapActor_GetActor(0) + 6) + (0x80 << 6)) & ~0x3fff);
    __CutsceneStart();
    __Func_808e118();
    __MessageID(0x1ffb);
    { PIN2; q1 = 0; q0 = -1;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0) {
        if (__GetFlag(0x96a) != 0) {
            __CutsceneWait(0x14);
            __InnHeal(0);
            return;
        }
        __CutsceneWait(0x14);
        d = (unsigned short)d;
        if (d == 0)
            __Func_80921c4(0, 0x80, 0x78);
        e = 0x80 << 8;
        if (d == e)
            __Func_80921c4(0, 0xf0, 0x78);
        __Func_80921c4(0, 0xb8, 0x78);
        __Func_8092adc(0, 0, 0);
        __CutsceneWait(0xa);
        __Func_809233c(1, 0x10, 0, e);
        __MapActor_WaitScript(1);
        __CutsceneWait(0xa);
        __ActorMessage(1, 0);
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0, 3);
        __CutsceneWait(0x14);
        { PIN3; q0 = 1; q1 = 0x80 << 9; q2 = e;
          __MapActor_SetSpeed(q0, q1, q2); }
        __Func_8092304(1, 0x28, 0);
        { PIN3; q0 = 1; q1 = 0; q2 = -0x20;
          __Func_8092304(q0, q1, q2); }
        __Func_8092adc(1, 0xc0 << 7, 0);
        __CutsceneWait(0x14);
        __Func_8092adc(0, e, 0);
        __CutsceneWait(0x14);
        __Func_809233c(2, -0x10, 0, 0);
        __MapActor_WaitScript(2);
        __CutsceneWait(0xa);
        __ActorMessage(2, 0);
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0, 3);
        __CutsceneWait(0x14);
        { PIN3; q0 = 2; q1 = 0x80 << 9; q2 = e;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 2; q1 = -0x28; q2 = 0;
          __Func_8092304(q0, q1, q2); }
        __Func_8092304(2, 0, 0x28);
        __Func_8092adc(2, 0xe0 << 8, 0);
        __CutsceneWait(0x14);
        __Func_8092adc(0, 0, 0);
        __CutsceneWait(0x14);
        __Func_809233c(3, 0x10, 0, e);
        __MapActor_WaitScript(3);
        __CutsceneWait(0xa);
        __ActorMessage(3, 0);
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0, 3);
        __CutsceneWait(0x14);
        { PIN3; q0 = 3; q1 = 0x80 << 9; q2 = e;
          __MapActor_SetSpeed(q0, q1, q2); }
        __Func_8092304(3, 0x28, 0);
        __Func_8092304(3, 0, 0x28);
        __Func_8092adc(3, 0xa0 << 8, 0);
        __CutsceneWait(0x14);
        { PIN4; q0 = -1; q1 = -1; q2 = -1; q3 = 0;
          __Func_80933f8(q0, q1, q2, q3); }
        { PIN3; q0 = 0; q1 = -0x38; q2 = 0;
          __Func_8092304(q0, q1, q2); }
        { PIN3; q0 = 0; q1 = 0; q2 = -0x20;
          __Func_8092304(q0, q1, q2); }
        __Func_8092adc(0, 0x80 << 6, 0);
        __CutsceneWait(0x1e);
        __MapActor_DoAnim(3, 3);
        __CutsceneWait(0xa);
        __ActorMessage(3, 0);
        __MapActor_DoAnim(1, 3);
        __CutsceneWait(0xa);
        if (__GetFlag(0x96a) == 0)
            __ActorMessage(1, 0);
        else
            *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __MapActor_DoAnim(2, 3);
        __CutsceneWait(0xa);
        if (__GetFlag(0x96a) == 0)
            __ActorMessage(2, 0);
        else
            *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __CutsceneWait(0x14);
        __Func_8092adc(0, 0, 0);
        { PIN3; q0 = 1; q1 = 0x80 << 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 3; q1 = 0x80 << 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __Func_8092adc(2, 0, 0);
        __InnHeal(0);
        { PIN3; q0 = 0; q1 = 0x80 << 6; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0xc0 << 7; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 3; q1 = 0xa0 << 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __Func_8092adc(2, 0xe0 << 8, 0);
        __CutsceneWait(0x14);
        __MapActor_DoAnim(1, 3);
        __CutsceneWait(0xa);
        __ActorMessage(1, 0);
        __MapActor_DoAnim(2, 3);
        __CutsceneWait(0xa);
        if (__GetFlag(0x96a) == 0)
            __ActorMessage(2, 0);
        else
            *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __MapActor_DoAnim(3, 3);
        __CutsceneWait(0xa);
        if (__GetFlag(0x96a) == 0)
            __ActorMessage(3, 0);
        else
            *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __Func_8092304(1, 0, 0x20);
        { PIN3; q0 = 1; q1 = -0x70; q2 = 0;
          __Func_80922c4(q0, q1, q2); }
        { PIN3; q0 = 3; q1 = 0; q2 = -0x28;
          __Func_8092304(q0, q1, q2); }
        __Func_80922c4(3, -0x70, 0);
        __CutsceneWait(0x32);
        { PIN3; q0 = 2; q1 = 0; q2 = -0x18;
          __Func_8092304(q0, q1, q2); }
        __MapActor_WaitMovement(1);
        { PIN3; q0 = 0; q1 = 0x80 << 7; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0; q2 = -0x10;
          __Func_8092304(q0, q1, q2); }
        __MapActor_WaitMovement(3);
        __Func_8092adc(3, 0xc0 << 8, 0);
        __CutsceneWait(0x14);
        __ActorMessage(1, 0);
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0, 3);
        __CutsceneWait(0x14);
        { PIN3; q0 = 1; q1 = 0x13333; q2 = 0x9999;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 2; q1 = 0x13333; q2 = 0x9999;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 3; q1 = 0x13333; q2 = 0x9999;
          __MapActor_SetSpeed(q0, q1, q2); }
        __MapActor_SetAnim(1, 2);
        p = __MapActor_GetActor(0);
        if (p != 0)
            __MapActor_TravelTo(1, *(short *)(p + 0xa), *(short *)(p + 0x12));
        __MapActor_WaitMovement(1);
        __MapActor_SetPos(1, 0, 0);
        __MapActor_SetAnim(3, 2);
        p = __MapActor_GetActor(0);
        if (p != 0)
            __MapActor_TravelTo(3, *(short *)(p + 0xa), *(short *)(p + 0x12));
        __MapActor_WaitMovement(3);
        __MapActor_SetPos(3, 0, 0);
        __MapActor_SetAnim(2, 2);
        p = __MapActor_GetActor(0);
        if (p != 0)
            __MapActor_TravelTo(2, *(short *)(p + 0xa), *(short *)(p + 0x12));
        __MapActor_WaitMovement(2);
        __MapActor_SetPos(2, 0, 0);
        __SetFlag(0x96a);
    }
    __CutsceneEnd();
}
