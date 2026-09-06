/* OvlFunc_963_20083c4  --  0x020083c4
 *   [asm/overlays/rom_7ec968/ovl_30_c_c_a_c_d.s, the whole file: ONE
 *    .thumb_func_start, ZERO data directives]
 *
 * 346 instructions of cutscene.  Three actors (0, 8/9, 0xb) are placed, walked
 * and talked through a long call script; one save-flag test near the end picks
 * between a four-call reaction and a counter bump in iwram; a second flag is
 * set on the way out.  Built at the tree default -O2 --
 * `tryc.makefile_flags('src/overlays/rom_7ec968/ovl_30_c_c_a_c_d.c')` returns
 * the EMPTY SET, so no prefix rule in the Makefile captures rom_7ec968 and NO
 * FLAG GROUP is involved.
 *
 * VERDICT
 *   OK OvlFunc_963_20083c4 -- 876 bytes, 349 encodings and 84 relocations identical
 *
 * SELECTION.  tools/solved_twins.py returns ZERO twins across the whole
 * corpus, as it does for most of this class.  The lead that mattered was not a
 * twin but a NEIGHBOUR: src/overlays/rom_7ec968/ovl_30_c_c_a_c_c.c is the
 * elevated front half of the SAME hand-split .s (its own header says "the
 * remainder in _d"), and it closes on the identical
 * `__Func_8010704(6, 0x1b, 1, 1, e, 0x1b)` / `(9, 0x1a, 2, 1, e, 0x1a)` pair
 * this one does.  Read the neighbour before the nominated template.
 *
 * THE PROLOGUE, BY CONTENT.  The ROM spends exactly one callee-saved register:
 * `push {r5, lr}` + `sub sp, #8`.  Plain C gives `push {r5, r6, r7, lr}` with a
 * `mov r7, r8 / push {r7}` spill on top -- FOUR extra bytes and 270 of 349
 * encodings differing -- because gcse commons 0x19999, 0xcccc, 0xd0<<8,
 * 0xc0<<8, 0x80<<1, 0x81<<1 and 0xd2<<1 into pseudos whose ranges straddle a
 * `bl`.  Counting the copies BY DESTINATION in the diff text is what names the
 * commoned values: r5=0x19999, r6=0xcccc, r7=0xd0<<8, r8=0xc0<<8.
 *
 * The one r5 the ROM DOES spend is the HOLE in the pin set.  `e = 7` is passed
 * as the fifth (stack) argument of both __Func_8010704 calls; the ROM itself
 * commons it into r5 and keeps it live across the first call.  That is the ROM
 * commoning, not gcc's, so it must NOT be pinned -- a plain `int e = 7;` is
 * what produces `mov r5, #7 / str r5, [sp]` twice.  Carried verbatim from the
 * neighbour, where the same reading is recorded.
 *
 * TWENTY-SEVEN PINNED SITES, MINIMAL BY MEASUREMENT AND CONFIRMED FROM BOTH
 * ENDS.  All 63 call sites were pinned first; that matched on the first try.
 * A greedy one-at-a-time drop with a re-test after every removal takes 36 out
 * in one round and finds NONE removable in a second -- that is the fixpoint --
 * and sweeping the candidate list in REVERSE order converges on the SAME 27,
 * not merely on the same count.  ("N pins is a size, not a set" warns the two
 * directions can disagree; here they do not, which is worth recording as the
 * negative case.)
 *
 * THE RESIDUE PARTITIONS EXACTLY ON THE RELOCATION LINE, and SIZE is not the
 * discriminator -- 26 of the 27 single-drop residues have size 0.  Dropping any
 * one survivor gives one of two shapes:
 *
 *   ORDERING pins -- 23 sites, 2 or 3 encodings, size 0, RELOCATIONS SILENT.
 *     Example, dropping the pin on __Func_80921c4(0xb, 0x6c, 0x1af):
 *         rom mov r0, #0xb        ours ldr r2, =0x1af
 *         rom mov r1, #0x6c       ours mov r0, #0xb
 *         rom ldr r2, =0x1af      ours mov r1, #0x6c
 *     gcc floats the pool load to the head of the group; the pin puts the three
 *     assignments back in the ROM's order.  Note this site's constants are all
 *     SINGLETONS -- 0x6c and 0x1af appear nowhere else -- so a rule that
 *     nominates pins by REPEATED CONSTANT would never have proposed it.  It is
 *     the singleton member of a uniform CALL-SITE FAMILY (same callee, same
 *     argument shape as the four other __Func_80921c4 sites) and it needs the
 *     same pin they do, for ordering alone.
 *
 *   CSE losses -- 4 sites, 38 to 337 encodings, RELOCATIONS ALWAYS DIFFER,
 *     because the lost rematerialisation moves every following `bl`:
 *         c01  __MapActor_SetSpeed(0, 0x19999, 0xcccc)   337, size +4
 *         c29  __Func_8092adc(0, 0xc0 << 8, 0)           130
 *         c17  __Func_80921c4(0xb, 0x84, 0xd2 << 1)       65
 *         c33  __MapActor_Surprise(9, 0x81 << 1)          38
 *
 * NEW: A TWO-SITE CSE CLASS IS BROKEN FROM EITHER END, AND ORDERING DECIDES
 * WHICH.  Recorded is "PIN THE FIRST USE, NOT THE LATER ONES -- pinning later
 * sites while the first is left open buys nothing" (OvlFunc_895_2008d1c), and
 * its converse "one pin at the first use covers the later ones", refined to
 * hold only when a branch separates the sites.  `0xc0 << 8` here is a clean
 * counter-example to the first half.  It has exactly TWO uses, separated by the
 * `if (p != 0)` join: the FIRST at __Func_8092adc(0, 0xc0<<8, 0) near the top
 * and the SECOND at the identical call 26 sites later.  Measured:
 *
 *     neither pinned              130 differing, relocations differ  (CSE lost)
 *     FIRST pinned only             2 differing, relocations SILENT
 *     SECOND pinned only            EXACT
 *     both pinned                   EXACT (the first is then inert)
 *
 * So the later pin does buy something: a call-clobbered destination that is
 * dead across the next `bl` cannot be fed by a commoned pseudo EITHER, so
 * killing the class's LAST end kills it just as thoroughly as killing its
 * first.  What is asymmetric is not the CSE but the FILL ORDER -- the second
 * site independently wants `q1 = 0xc0; q0 = 0; q1 <<= 8; q2 = 0;` (the ROM puts
 * `mov r0` between the `mov` and the `lsl`), and once that pin exists for
 * ordering it does the CSE job for free, which is why the first-use pin drops
 * out of the minimal set.  Pinning the FIRST use is still the safe default; the
 * point is that a first-use pin can be the redundant one, so a sweep must be
 * allowed to remove it.
 *
 * TEMPLATE LEVERS RE-MEASURED, INCLUDING TWO THAT WOULD HAVE BEEN HARMFUL.  The
 * nominated template is src/overlays/rom_79dd90/ovl_30_c_c_c_c_a_c_a_b_c.c
 * (23 shared symbols).  Sufficient, not necessary, one at a time:
 *
 *   CARRIED  `extern unsigned char *iwram_3001ebc;` as a SCALAR POINTER.  The
 *            reference object's ONLY R_ARM_ABS32 is `iwram_3001ebc` at +0x364,
 *            bare; the array spelling folds the base into `=sym+0x1d8`.
 *   CARRIED  the one-statement-per-argument pinned fill in the ROM's emitted
 *            order, with the `lsl` written where the ROM puts it.  All four
 *            orders in this function -- ascending r0/r1/r2, `mov r1 / mov r0 /
 *            lsl r1 / mov r2`, `mov r1 / mov r2 / mov r0 / lsl r1`, and
 *            seed-r2 / mov r0 / mov r1 / shift-r2 -- fall out of it.
 *   CARRIED  three DESCENDING fills (`q1 = 2; q0 = 0xb;` and the two
 *            __MapActor_Surprise sites that shift before the slot).  These are
 *            properties of the SITE: the other seven __Func_809259c /
 *            __Func_80925cc sites in the same function take the uniform
 *            ascending form, and most need no pin at all.
 *   NOT      the `_MSG_xxxx` message-symbol lever.  The template proves 0x1720
 *            is a message.sym symbol BY RELOCATION.  Here message.sym has no
 *            entry for 0x2654, the reference disassembly writes `ldr r0, =0x2654`
 *            rather than a name, and the reference object carries exactly ONE
 *            R_ARM_ABS32 -- there is no second relocation for a message symbol
 *            to occupy.  Transplanting it would have ADDED a relocation.
 *   NOT      the `unsigned char one = 1` QImode-local ORR lever and the
 *            `&= 0xfe` spelling -- this function has no bitfield write.
 *   NOT      the `__Func_8092c40` descending fill and the bottom-tested `while`
 *            -- neither construct occurs here.
 *
 * MEASURED WORSE
 *     plain C, no pins                     size +4, 270 of 349, relocs differ
 *     all 63 sites pinned                  EXACT, but 36 pins inert -- scaffolding
 *     27 pins minus __MapActor_SetSpeed#1  size +4, 337, relocs differ
 *     27 pins minus __Func_8092adc(0,0xc0<<8,0)#2   130, relocs differ
 *     27 pins minus __Func_80921c4(0xb,0x84,0xd2<<1) 65, relocs differ
 *     27 pins minus __MapActor_Surprise(9,0x81<<1)   38, relocs differ
 *     each of the other 23 dropped singly            2-3, relocs SILENT
 *     first-use pin swapped for the later one         2, relocs SILENT
 *
 * LANDING NEEDS NO SPLIT AND NO LINKER EDIT.  The .s holds ONE function and no
 * data.  Grepping the whole tree on the FULL PATH, exactly one script line
 * names the object, in overlays/rom_7ec968/overlay.ld, inside the
 * `.text : { ... } > overlay` block:
 *
 *     asm/overlays/rom_7ec968/ovl_30_c_c_a_c_d.o(.text)
 *
 * That line STAYS AS WRITTEN: the Makefile's cross-dir rule `asm/%.o: src/%.c`
 * builds asm/overlays/rom_7ec968/ovl_30_c_c_a_c_d.o from
 * src/overlays/rom_7ec968/ovl_30_c_c_a_c_d.c, so the linker keeps referencing
 * the asm/ path, exactly as it already does for the elevated neighbours
 * ovl_30_c_c_a_c_c.o and ovl_30_c_c_a_a_c_b.o.  The script's `.data` block
 * names only `asm/overlays/rom_7ec968/ovl_30_c_c_c.o(.data)` and MUST NOT gain
 * a line for this object: overlay.map records `.data 0x0` and `.bss 0x0`
 * against it, i.e. the object has neither section, and the .s has zero data
 * directives to give it one.  Every callee is already exported by
 * src/overlays/rom_7ec968/imports.s -- all 23 of them, including
 * _Func_8093054, _MapActor_TravelTo, _MapActor_Surprise, _Func_8010704 and
 * _SetFlag -- so imports.s needs no addition either.
 *
 * One housekeeping note for whoever lands this: asm/overlays/rom_7ec968/
 * ovl_30_c_c_a_c_d.s is TRACKED and must leave the index when the .c arrives.
 * The correct end state is the one ovl_30_c_c_a_a_c_b.s is in -- present on
 * disk as a build artifact, untracked.  ovl_30_c_c_a_c_c.s is in neither state:
 * `git show HEAD:` on it returns "Generated by gcc 2.96", so a compiler-
 * generated .s is committed there against the project rule.  Pre-existing, not
 * introduced here, and worth a separate cleanup.
 */
extern unsigned char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8093054(int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_963_20083c4(void)
{
    unsigned char *p;
    int e;

    __CutsceneStart();
    { PIN3; q0 = 0; q1 = 0x19999; q2 = 0xcccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0xdb; q0 = 0; q1 = 0x78; q2 <<= 1;
      __Func_80921c4(q0, q1, q2); }
    __Func_8092adc(0, 0xc0 << 8, 0);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_SetPos(0xb, *(int *)(p + 8), *(int *)(p + 0x10));
    __WaitFrames(1);
    { PIN3; q0 = 0xb; q1 = 0x19999; q2 = 0xcccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x6c; q2 = 0x1af;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xd0; q0 = 0xb; q1 <<= 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0xb; q1 <<= 1; q2 = 0x14;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0xd0; q0 = 0xb; q1 <<= 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0xb, 0, 0x28);
    { PIN3; q1 = 0xd0; q0 = 0xb; q1 <<= 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0xb, 0, 0x14);
    __Func_809259c(0xb, 2);
    __MessageID(0x2654);
    __Func_8093040(0xb, 0, 0x28);
    { PIN3; q1 = 0x80; q2 = 0; q0 = 8; q1 <<= 1;
      __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(8, 2);
    __Func_8093040(8, 0, 0xa);
    { PIN3; q2 = 0xd2; q0 = 0xb; q1 = 0x84; q2 <<= 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xd0; q0 = 0xb; q1 <<= 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xe0; q0 = 0; q1 <<= 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q2 = 0xd0; q0 = 0xb; q1 = 0x8a; q2 <<= 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xb0; q2 = 0xa; q0 = 0xb; q1 <<= 8;
      __Func_8092adc(q0, q1, q2); }
    __Func_809259c(0xb, 2);
    __Func_8093040(0xb, 0, 0x28);
    __Func_80925cc(8, 2);
    __Func_8093040(8, 0, 0x28);
    { PIN3; q1 = 0x80; q2 = 0x14; q0 = 9; q1 <<= 1;
      __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(9, 2);
    __Func_8093040(9, 0, 0x14);
    { PIN3; q1 = 0xc0; q0 = 0; q1 <<= 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_80921c4(0xb, 0x90, 0xd2 << 1);
    __CutsceneWait(0x14);
    __Func_80925cc(9, 2);
    __Func_8093040(9, 0, 0x14);
    { PIN2; q1 = 0x81; q0 = 9; q1 <<= 1;
      __MapActor_Surprise(q0, q1); }
    __Func_809259c(9, 3);
    __Func_8093040(9, 0, 0x14);
    { PIN3; q1 = 0xa0; q0 = 0xb; q1 <<= 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093054(0xb, 0);
    if (__GetFlag(0x9b << 4) != 0) {
        { PIN3; q1 = 0xd0; q2 = 0x28; q0 = 0xb; q1 <<= 8;
          __Func_8092adc(q0, q1, q2); }
        __MapActor_Surprise(0xb, 0x81 << 1);
        __CutsceneWait(0x28);
        __Func_8093040(0xb, 0, 0xa);
    } else {
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    }
    { PIN3; q1 = 0xa0; q0 = 0xb; q1 <<= 7; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0xb, 0, 0x28);
    { PIN3; q1 = 0x80; q0 = 0xb; q1 <<= 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0xb0; q0 = 0xb; q1 <<= 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0xb, 0, 0xa);
    { PIN3; q2 = 0xd0; q0 = 0xb; q1 = 0x8a; q2 <<= 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xb0; q2 = 0x14; q0 = 0xb; q1 <<= 8;
      __Func_8092adc(q0, q1, q2); }
    __Func_809259c(8, 2);
    __Func_8093040(8, 0, 0xa);
    { PIN2; q1 = 0x81; q0 = 0xb; q1 <<= 1;
      __MapActor_Surprise(q0, q1); }
    __Func_80925cc(0xb, 1);
    __CutsceneWait(0x14);
    __Func_8093040(0xb, 0, 0x14);
    __Func_80925cc(9, 2);
    __CutsceneWait(0x14);
    { PIN3; q1 = 0xe0; q2 = 0xa; q0 = 0; q1 <<= 8;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(9, 1);
    __CutsceneWait(0x14);
    __Func_8093040(9, 0, 0xa);
    __Func_80925cc(8, 2);
    __Func_8093040(8, 0, 0x14);
    __Func_80925cc(0xb, 1);
    __CutsceneWait(0x14);
    __Func_8093040(0xb, 0, 0x14);
    { PIN3; q1 = 0xa0; q0 = 0xb; q1 <<= 7; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0xb, 0, 0xa);
    __MapActor_SetAnim(0xb, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(0xb, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(0xb);
    __MapActor_SetPos(0xb, 0, 0);
    e = 7;
    __Func_8010704(6, 0x1b, 1, 1, e, 0x1b);
    __Func_8010704(9, 0x1a, 2, 1, e, 0x1a);
    __SetFlag(0x89f);
    __CutsceneEnd();
}
