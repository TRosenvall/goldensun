// fakematch
/* ovl_314_c_c_c_a_c_c_a_c_c_c.c  --  OvlFunc_898_200936c
 *
 *   [the WHOLE of asm/overlays/rom_793768/ovl_314_c_c_c_a_c_c_a_c_c_c.s -- ONE
 *    `.thumb_func_start`, so NO SPLIT is required and overlay.ld:75 stays
 *    VERBATIM as `asm/overlays/rom_793768/ovl_314_c_c_c_a_c_c_a_c_c_c.o(.text)`]
 *
 *   OK OvlFunc_898_200936c -- 716 bytes, 281 encodings and 66 relocations identical
 *
 * Re-measured four times.  objcmp prints no `(built with: ...)` line:
 * makefile_flags() on this path is the EMPTY set and rom_793768 has no
 * Makefile rule at all, so the rule that fires is the generic
 * `asm/%.o: src/%.c` and the flags are the tree default -O2 -mthumb
 * -mthumb-interwork -fcall-used-r4.  The .s carries NO .section, .data, .bss,
 * .lcomm, .word or .byte; every `.L` symbol is a branch target defined here,
 * so nothing needs `.global`.  FAKEMATCH: register-pin idiom.
 *
 * hi = 0, hiv = 0.  The push mask is the ROM's `{r5, r6, r7, lr}` from the
 * first plain transcription and stays that way -- no commoning tell, and the
 * whole 27-of-281 residue was ORDERING with the relocation line SILENT except
 * for one call whose address moved by two bytes.  The neighbour that supplied
 * the idiom set is src/non_matching/ovl_797990/2008f30.c, a PARK: it shares
 * this function's entire opening -- the same __Func_8010704(0x37, 0x1a, 4, 2,
 * 0x17, 0x1a), the same OvlFunc_common0_70(0x80 << 16, 0, 0xd2 << 17, 0xdf),
 * the same GetActor(0xe)/GetActor(0xf) pair storing 1 and 0 at +0x64 and a
 * function pointer at +0x6c.  A PARK IS A TEMPLATE: three of its recorded
 * "fixed on the way" spellings transferred unchanged, and two of its four
 * blockers do not exist here (this ROM uses `ldrsh` at both gState reads where
 * the park's uses `ldrh`, and this one's zero lands in r6 without a fight).
 *
 * WHAT CLOSED IT, in the order the mechanism sizes said to try them:
 *
 *   plain C                                            27  (exact size)
 *   + the four SetPos sites pinned r1/r2/r0             18
 *   - the FIFTH SetPos site UNPINNED again              15
 *   + the two SetSpeed sites pinned r0/r1/r2 ascending   9
 *   + `z = 0` FIRST of the three pre-call assignments    5
 *   + a one-member union on ONE of two adjacent stores   3
 *   + the fifth SetPos pinned r0 and r1 ONLY             0
 *
 * THE SAME PIN, THE SAME CALLEE, OPPOSITE SIGNS AT FIVE SITES.  Five
 * __MapActor_SetPos calls have the identical shape `mov r1 / mov r2 / mov r0 /
 * lsl r1 / lsl r2`.  A macro declaring r1, r2, r0 in that order and assigning
 * them in that order is EXACT at four of them and puts `mov r0` at the FRONT at
 * the fifth, for 2 differing there and 2 more at the __Func_8092848 that
 * follows it (18 against 15 with everything else equal).  The fifth site is the
 * first statement after a `bl __CutsceneStart`, and that is the only structural
 * difference.  Three spellings are exact at it: declaring r2 BEFORE r1 with the
 * same assignments, initialising all three AT THE DECLARATION, or pinning ONLY
 * r0 and r1 and leaving the second shifted argument a literal.  The last ships.
 *
 * DECLARATION ORDER OF REGISTER PINS IS A LEVER IN ITS OWN RIGHT, independent
 * of assignment order.  `register int q2("r2"); register int q1("r1");
 * register int q0("r0");` with assignments q1, q2, q0 is exact at the fifth
 * site where the same assignments under `q1, q2, q0` declarations are 2
 * differing.  NEW.  But it does NOT generalise: rewriting the shared macro to
 * declare r2 first and using it at ALL FIVE sites is 6 differing, so the order
 * is a per-site fact, not a house style.
 *
 * A ONE-MEMBER-CLASS ALIAS FIX ON *ONE* OF TWO STORES.  The second
 * GetActor block writes `*(short *)(p + 0x64)` then `*(void **)(p + 0x6c)`; gcc
 * emits them in the other order and no source order moves them, because the two
 * types are in different alias sets and sched2 is free to swap.
 * `-fno-strict-aliasing` fixes it and names the pass in one compile.  The
 * source fix is a union -- but only the 0x6c store needs to become
 * `((Slot *)(p + 0x6c))->fp`; ONE alias-set-0 access is enough to make the pair
 * conflict, and putting the union on both is no better.  The FIRST GetActor
 * block, the same two stores in the same order, needs nothing.
 *
 * MINIMISATION MOVED THE ANSWER, TWICE, AND BOTH TIMES THE BASE WAS THE CAUSE.
 * With a named `void *f` holding OvlFunc_898_2008314, dropping the union costs
 * 2.  Dropping `f` and writing the symbol at both stores is INERT on its own --
 * and once it is gone, the union on the FIRST store drops for free as well.
 * "A pin set is minimal only w.r.t. the base it was minimised on" is not a
 * caution about pin sets alone: a DECLARATION is part of the base.
 *
 * THE THREE PRE-CALL ASSIGNMENTS ARE AN ALLOCATION ORDER, NOT A SCHEDULE.  The
 * ROM keeps the function pointer in r5, the stored zero in r6 and the stored
 * one in r7; gcc's first attempt gives r6, r5, r7.  Writing `z = 0;` before the
 * pointer assignment -- even though z is not used for another two calls -- is
 * what swaps them, and it is worth 2.  Moving the pointer's assignment to AFTER
 * the first __MapActor_GetActor is worth 2 more (the ROM's `ldr r5` follows the
 * `bl`), but ONLY once z is already first: done alone it costs the whole r7
 * slot and the push mask comes out `{r5, r6, lr}` for 29 differing.
 *
 * `-fno-schedule-insns2` is a REGRESSION here, 9 -> 54, and READ THE SIGN: it
 * says sched2 is already producing what the ROM has and the alias axis is the
 * wrong one to push on -- which is exactly right for four of the five levers
 * above and exactly wrong for the fifth, the store pair, where the alias axis
 * is the ONLY one that reaches.  The diagnostic partitions the residue; it does
 * not settle it.
 *
 * MEASURED WORSE / INERT (against 281 encodings / 716 bytes):
 *
 *   spelling                                              differing
 *   ----------------------------------------------------  ---------
 *   plain C, no pins                                             27
 *   + 5 SetPos pins including the CutsceneStart site             18
 *   the CutsceneStart SetPos pinned r1/r2/r0                      6 (of 3)
 *   SetSpeed pinned r0 alone (pool words swap)                   18
 *   the r2-first declaration used at all five SetPos sites         6
 *   `z` written as a literal 0 at the store          size +12 bytes
 *   `one` written as a literal 1 at both uses         size +8 bytes
 *   the two gState offsets inlined                   size -12 bytes
 *   the second gState read reusing the first pointer  size -8 bytes
 *   dropping either stack-argument pin, either SetSpeed pin,
 *     any of the four SetPos pins, or the fifth site's pin      3 each
 *   INERT: `OvlFunc_common0_70` declared `void` instead of `int`;
 *     the union on the first GetActor block; the named `void *f`.
 *
 * Harness: scratch_elev/b255/a2 -- cmp.sh (objcmp), d.sh (side-by-side objdump
 * diff), dx.sh/dx.py (the same with extra flags, for the pass diagnostics),
 * batch.py + b2.sh/b3.sh (many candidates in one container), drops.py
 * (drop-one minimisation).
 */
extern unsigned char gState[];

extern int __GetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_8091e9c(int a);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern int OvlFunc_common0_70(int a, int b, int c, int d);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __Func_80925cc(int slot, int a);
extern void __MessageID(int id);
extern void __Func_8093040(int a, int b, int c);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_80917d0(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void OvlFunc_898_2008314(void);
extern void OvlFunc_898_20083ac(void);

typedef union { short h; void *fp; } Slot;

#define SP(SLOT, A, SA, B, SB) do { \
    register int q1 __asm__("r1"); register int q2 __asm__("r2"); \
    register int q0 __asm__("r0"); \
    q1 = (A); q2 = (B); q0 = (SLOT); q1 <<= (SA); q2 <<= (SB); \
    __MapActor_SetPos(q0, q1, q2); } while (0)

int OvlFunc_898_200936c(void)
{
    unsigned char *p;
    unsigned char *g;
    int o1, o2;
    int s1, s2;
    int one, z;
    int t;
    int e;

    if (__GetFlag(0x87a))
        __Func_8091e9c(0xe);
    if (__GetFlag(0x80 << 2)) {
        s1 = 0x17;
        s2 = 0x1a;
        __Func_8010704(0x37, 0x1a, 4, 2, s1, s2);
    }
    OvlFunc_common0_70(0x80 << 16, 0, 0xd2 << 17, 0xdf);
    z = 0;
    one = 1;
    p = __MapActor_GetActor(0xe);
    *(short *)(p + 0x64) = one;
    *(void **)(p + 0x6c) = OvlFunc_898_2008314;
    p = __MapActor_GetActor(0xf);
    *(short *)(p + 0x64) = z;
    ((Slot *)(p + 0x6c))->fp = OvlFunc_898_2008314;
    if (__GetFlag(0x858))
        SP(0x13, 0xd8, 16, 0xc4, 17);
    t = __GetFlag(0x853);
    if (__GetFlag(0x855) == 0 && (t & one) != 0) {
        p = __MapActor_GetActor(0x15);
        *(void **)(p + 0x6c) = OvlFunc_898_20083ac;
    }
    o1 = 0xe1 << 1;
    g = gState + o1;
    if (*(short *)g <= 2 && __GetFlag(0x109) == 0) {
        __ClearFlag(0x867);
        if (__GetFlag(0x855) == 0 && __GetFlag(0x856) != 0) {
            __CutsceneStart();
            p = __MapActor_GetActor(0);
            if (p != 0)
                __MapActor_SetPos(2, *(int *)(p + 8), *(int *)(p + 0x10));
            if (*(short *)g == 1)
                SP(2, 0xc8, 17, 0xe0, 17);
            else
                SP(2, 0xe0, 16, 0xa2, 16);
            __Func_8092848(2, 0, 0);
            __MapTransitionIn();
            __WaitMapTransition();
            __CutsceneWait(0x1e);
            __Func_80925cc(2, 2);
            __MessageID(0x1328);
            __Func_8093040(2, 0, 0x14);
            __MapActor_DoAnim(0, 3);
            { register int w0 __asm__("r0"); register int w1 __asm__("r1");
              register int w2 __asm__("r2");
              w0 = 2; w1 = 0xcccc; w2 = 0x6666;
              __MapActor_SetSpeed(w0, w1, w2); }
            __MapActor_SetAnim(2, 2);
            p = __MapActor_GetActor(0);
            if (p != 0)
                __MapActor_TravelTo(2, *(short *)(p + 0xa), *(short *)(p + 0x12));
            __MapActor_WaitMovement(2);
            __MapActor_SetPos(2, 0, 0);
            __Func_80917d0(2, 0);
            __CutsceneEnd();
        }
    }
    if (__GetFlag(0x867))
        SP(0x17, 0xcc, 17, 0xf0, 15);
    o2 = 0xe1 << 1;
    e = *(short *)(gState + o2);
    if (e == 0xb) {
        if (__GetFlag(0x855) == 0 && __GetFlag(0x856) != 0
            && __GetFlag(2) == 0) {
            __CutsceneStart();
            { register int y1 __asm__("r1"); register int y0 __asm__("r0");
              y1 = 0xa0; y0 = 2; y1 <<= 14;
              __MapActor_SetPos(y0, y1, 0x9b << 17); }
            __Func_8092848(2, 0, 0);
            __MapTransitionIn();
            __WaitMapTransition();
            __CutsceneWait(0x1e);
            __Func_80925cc(2, 2);
            __MessageID(0x1328);
            __Func_8093040(2, 0, 0x14);
            __MapActor_DoAnim(0, 3);
            { register int w0 __asm__("r0"); register int w1 __asm__("r1");
              register int w2 __asm__("r2");
              w0 = 2; w1 = 0xcccc; w2 = 0x6666;
              __MapActor_SetSpeed(w0, w1, w2); }
            __MapActor_SetAnim(2, 2);
            p = __MapActor_GetActor(0);
            if (p != 0)
                __MapActor_TravelTo(2, *(short *)(p + 0xa), *(short *)(p + 0x12));
            __MapActor_WaitMovement(2);
            __MapActor_SetPos(2, 0, 0);
            __Func_80917d0(2, 0);
            __CutsceneEnd();
        }
        __ClearFlag(0x12f);
    } else if (e == 0xd) {
        if (__GetFlag(0x855))
            __MapActor_SetPos(0x14, 0, 0);
    }
    return 0;
}
