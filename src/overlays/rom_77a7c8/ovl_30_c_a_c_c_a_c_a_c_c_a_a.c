/* OvlFunc_881_200a8e8 (0x0200a8e8) -- MATCHES.  972 bytes, 381 encodings and
 * 90 relocations identical to
 * asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_c_c_a_a.s.  363 instructions.
 *
 * Needs ONE message.sym line -- `_MSG_2644 = 0x2644;` -- and nothing else.  With
 * it the object is byte-identical; without it the single difference is the pool
 * word at index 375 and its relocation.
 *
 * Verify with (the .equ stands in for the message.sym line, per _MSG_1299's note
 * -- objcmp assembles the candidate directly and cannot see a linker script):
 *   python3 tools/objcmp.py <cand.c> \
 *     asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_c_c_a_a.s \
 *     --func OvlFunc_881_200a8e8
 * The reference holds this function ALONE (its file-mate OvlFunc_881_200acb4
 * landed in batch 280 and the file was split) and `tools/datacheck.py` reports no
 * data section, so no split and no export list.
 *
 * ================================================================
 * WHAT CLOSED IT: ONE EMPTY `__asm__ volatile ("")` AFTER THE strh
 * ================================================================
 *
 *     *(short *)(x + 6) = h;
 *     __asm__ volatile ("");
 *     __MapActor_Emote(L679c, 0x80 << 1, 0);
 *
 * 5 encodings -> 0.  The park had this window derived correctly and declared it
 * "a proven two-way priority tie ... genuinely unreachable from C", with 19
 * spellings and one flag on file.  THE DERIVATION WAS RIGHT AND THE CONCLUSION
 * WAS WRONG, because the list it was drawn from contained
 * `do { } while (0);` after the store (66 differing) and NOT the empty volatile
 * asm.  Those are not the same tool: batch 282 established that
 * `do { } while (0)` plants TWO barriers via NOTE_INSN_LOOP_BEG/LOOP_END, while
 * an empty asm plants ONE.  Here one is right and two are catastrophic.
 *
 * WHY IT WORKS, and it does not contradict the park's arithmetic -- it steps
 * around it.  The park showed that at t=40 the ready list is {375 (the strh),
 * 1096 (`movs r1,#0x80`)} and 1096 wins on priority 9 against 8: the strh's
 * dependents are all ANTI-dependences of zero latency onto single-instruction
 * argument fills (`movs r2,#0`, `ldr r0,[r6]`, both priority 8), so
 * prio(strh) = 8, while 1096 -> 1097 (`lsls`) -> the call is a two-deep TRUE
 * chain worth 9.  That gap cannot be closed by any spelling, because 0x100
 * exceeds Thumb's 8-bit immediate and so the r1 argument is the ONLY one of the
 * three that needs two instructions.  The barrier does not close the gap: IT
 * REMOVES THE CONTEST.  A traditional asm (ASM_INPUT, no operands) is analysed
 * as using and clobbering every hard register (haifa-sched.c, "Traditional and
 * volatile asm instructions must be considered to use and clobber all hard
 * registers, all pseudo-registers and all of memory"), so it is an absolute
 * ordering point: the store is before it, every argument fill after it, and the
 * ready list they would have competed in never forms.  The park verified by hand
 * that from strh@40 the ROM's remaining order (1096, 388, 380, 1097) follows
 * from priority -> dependent count -> INSN_LUID, and that is what happens.
 *
 * THE LESSON, and it is the general one: WHEN A sched2 RESIDUE IS A CONTEST
 * BETWEEN TWO INSNS THAT COME FROM DIFFERENT STATEMENTS, AN EMPTY
 * `__asm__ volatile ("")` BETWEEN THE STATEMENTS IS A ROUTE EVEN WHEN THE
 * PRIORITY ARITHMETIC IS PROVABLY UNREACHABLE.  The arithmetic only governs
 * insns that are ready in the same cycle.  Check whether the two insns are
 * separated by a statement boundary BEFORE writing a window off.  (Where they
 * are not -- two argument fills of ONE call -- the arithmetic really is the end
 * of the road; see the sibling park src/non_matching/rom_a1000/80a96d8.c.)
 *
 * CONTROL kept from the park: -fno-schedule-insns2 gives 74 differing, so sched2
 * is producing everything else in this 363-instruction function correctly.
 *
 * ================================================================
 * THE LEVERS THAT CLOSED THE OTHER 358 INSTRUCTIONS
 * ================================================================
 *
 * 1. DEFEATING CSE OF REPEATED POOL CONSTANTS WAS THE WHOLE GAME, AND NO FLAG
 *    DOES IT.  169 -> 99 by pinning 0x17710000 / 0xd580000 at their two
 *    __Actor_TravelTo sites and 0x80 << 9 at its two.  Measured:
 *    -fno-rerun-cse-after-loop 183 (worse), -fno-gcse 180,
 *    -fno-cse-follow-jumps 169 (inert), -fno-expensive-optimizations 179.
 *    This is the documented "long straight-line region has no branch to dominate
 *    from" case: calls do not end a basic block, so the two sites are in ONE
 *    block, local-alloc has to give the commoned constants callee-saved
 *    registers, and THAT is what pushed the actor pointers out of r5/r6/r7 into
 *    r8 and forced eight `mov rN, r8` copies.
 * 2. ONE LOCAL SERVING THREE ROLES.  The ROM's r7 holds the zero, then the
 *    created actor, then actor 0x37.  Writing all three as one
 *    `unsigned char *q` (q = 0; -> *(p+0x5b) = (int)q; -> *(int *)(q + 0xc),
 *    which is how the ROM's genuine null-deref `ldr r2,[r7,#0xc]` reproduces)
 *    was worth 82 -> 58 on its own.
 * 3. `neg` MEANS THE MASK IS 32 BITS WIDE, AND ONLY A NAMED MASK KEEPS IT THERE.
 *    `s[5] & ~0x20` is narrowed by combine to `movs r3,#0xdf`;
 *    `mask = ~0x20; s[5] = s[5] & mask;` gives the ROM's
 *    `movs r3,#33 / negs r3,r3`.  Worth 52.
 * 4. THE REGISTER PINS.  ELEVEN pinned blocks, and EVERY ONE IS LOAD-BEARING --
 *    each was dropped on its own against the exact file:
 *      __Func_80933f8 PIN4                       45
 *      __MapActor_SetSpeed(0,0xcccc,0x6666) PIN3  2
 *      __CreateActor PIN4                         2
 *      z1/a0 pair (the three byte stores)       297, +4 bytes
 *      __Func_8092adc(0,0xe0<<8,0) PIN1            2
 *      __Actor_TravelTo #1 PIN4                   2
 *      __Actor_TravelTo #2 PIN4                 274, +52 bytes
 *      __MapActor_SetSpeed(L679c,...) PIN3       94
 *      __Actor_TravelTo 0x176d (r2 only)          2
 *      __Actor_TravelTo #4 PIN4                   2
 *      __MapActor_TravelTo PIN3                  87, +4 bytes
 *    ONE REDUCTION WAS FOUND: the 0x176d0000 __Actor_TravelTo block declared
 *    PIN3 but only ever assigned q2, and the two unused q0/q1 declarations are
 *    NOT needed -- it is exact with `register int q2 __asm__("r2")` alone.  That
 *    took the pin count from 35 to 33.  Worth checking on every PIN block in the
 *    corpus: an unused `register ... __asm__` declaration still reserves the
 *    register, so it is a shim that may be paying for nothing.
 *
 * THE SYMPTOM-vs-CAUSE LESSON IS THE ONE TO CARRY.  Every one of the nine wrong
 * register roles here cleared the moment the constant CSEs went.  A full
 * .17.lreg/.18.greg derivation showed allocno 101 (&.L679c) could not take r6
 * because two long-lived LOCAL pseudos holding 0x17710000 and 0xd580000 had
 * claimed r6 and r8 -- correct, and completely beside the point once the cause
 * was removed.  READ THE HARD-REGISTER CONFLICT LIST TO IDENTIFY WHICH VALUE IS
 * SQUATTING, THEN GO FIX WHY IT EXISTS, NOT WHERE IT SITS.
 *
 * ================================================================
 * _MSG_2644 = 0x2644 -- THE SYMBOL, NOW ADMISSIBLE
 * ================================================================
 *
 * The ROM does `ldr r3, =0x2644 / mov sl, r3 / mov r0, sl / bl __MessageID`
 * ... `mov r0, sl / add r0, #6 / bl __Func_801776c`.  That is message.sym's own
 * criterion in its established form (compare _MSG_242e, _MSG_261c, _MSG_2399,
 * _MSG_f76): a plain `int m = 0x2644` is constant-propagated, m + 6 folds to a
 * pool 0x264a, gcc never spends r10, and the ROM's `add r0, #6` cannot appear.
 * With `extern int _MSG_2644; m = (int)&_MSG_2644;` the callee-saved r10 and the
 * add both appear.  The .s comment above the function independently says
 * "Message base 0x2644".
 *
 * message.sym:527 records this candidate as withheld in batch 280 because it did
 * not COMPLETE its function.  IT NOW COMPLETES THE FUNCTION, which is the
 * position batch 282 resolved in favour of adding the line for _MSG_820 ("Batch
 * 282 closed those 8, so it DOES complete the function, and on this tree's own
 * stated criterion -- evidence AND completion -- it qualifies").
 *
 * ================================================================
 * SHIMS -- fakematch.txt rows required
 * ================================================================
 *
 * CLASS 1, `register ... __asm__` declarations: 33.
 *   PIN4 x 5 uses = 20, PIN3 x 3 uses = 9, PIN1 x 1 use = 1, plus the standalone
 *   `register int z1 __asm__("r1")`, `register unsigned char *a0 __asm__("r0")`
 *   and `register int q2 __asm__("r2")`.
 * CLASS 2, `__asm__(".equ ...")`: NONE in the landed file.  The `.equ _MSG_2644`
 *   used for objcmp is replaced by the message.sym line.
 * NEITHER CLASS but worth naming: `extern int L679c __asm__(".L679c");` is an
 *   asm-NAME declaration on an extern, not a register pin and not an `.equ`.
 *
 * No per-file Makefile override applies to this stem; every figure above is on
 * the production flag group.
 */
extern unsigned char iwram_3001ebc[];
extern int L679c __asm__(".L679c");
extern int _MSG_2644;

extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_808c4c0(void);
extern void __Func_808c44c(void);
extern void __Func_80936a0(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int n);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_DoAnim(int slot, int n);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __Actor_Stop(unsigned char *a);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void __Actor_WaitMovement(unsigned char *a);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern unsigned char *__CreateActor(int id, int x, int y, int z);
extern void __DeleteActor(unsigned char *a);
extern void __DeleteFieldActor(int slot);
extern unsigned char *__galloc_iwram(int tag, int n);
extern void __gfree(int tag);
extern void __LoadItemIcon(int id);
extern int __UploadSpriteGFX(int a, int b, void *p);
extern void __MessageID(int id);
extern void __Func_801776c(int a, int b);
extern void __Func_8078a08(int a);
extern void OvlFunc_881_200813c(void);
extern void OvlFunc_881_200a7dc(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_881_200a8e8(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned char *s;
    unsigned char *t;
    unsigned char *r;
    unsigned char *u;
    unsigned char *e;
    unsigned char *w;
    int zero;
    int h;
    int k;
    int m;
    int mask;
    int z;
    unsigned char *x;

    p = __MapActor_GetActor(0);
    __CutsceneStart();
    __Func_808c4c0();
    __Func_80936a0(0x16666, 6);
    __Func_80933d4(0xc0 << 10, 0xc0 << 7);
    { PIN4; q0 = 0x17880000; q1 = -1; q3 = 1; q2 = 0xd680000;
      __Func_80933f8(q0, q1, q2, q3); }
    { PIN3; q2 = 0x6666; q0 = 0; q1 = 0xcccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(0, 2);
    q = 0;
    *(p + 0x5b) = (int)q;
    __Actor_Stop(p);
    if (*(int *)(p + 0x10) > 0xd680000) {
        if (*(int *)(p + 8) > 0x176e0000) {
            __Actor_TravelTo(p, 0x176e0000, *(int *)(p + 0xc), 0xd7d0000);
            __Actor_WaitMovement(p);
        }
    } else {
        if (*(int *)(p + 8) > 0x177a0000) {
            __Actor_TravelTo(p, 0x177a0000, *(int *)(q + 0xc), 0xd480000);
            __Actor_WaitMovement(p);
        }
    }
    __Actor_TravelTo(p, 0x17690000, 0, 0xd680000);
    __Actor_WaitMovement(p);
    __MapActor_SetAnim(0, 1);
    __Func_8092adc(0, 0, 0x28);
    __Func_808c44c();
    __Func_80925cc(0, 2);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0, 0x1c);
    { PIN4; q1 = *(int *)(p + 8) + (0x80 << 10); q0 = 0x16; q3 = *(int *)(p + 0x10);
      q2 = 0x98 << 14;
      q = __CreateActor(q0, q1, q2, q3); }
    if (q != 0) {
        { register int z1 __asm__("r1");
          z1 = 0;
          { register unsigned char *a0 __asm__("r0"); a0 = q + 0x55; *a0 = z1; }
          s = *(unsigned char **)(q + 0x50);
          s[0x26] = z1;
          s[0x27] = z1; }
        mask = ~0x20;
        s[5] = s[5] & mask;
        s[9] = s[9] & 0xf;
        t = __galloc_iwram(0x11, 0xc1 << 3);
        __LoadItemIcon(0xf2);
        __UploadSpriteGFX(s[0x1c], 0x80, t + (0x80 << 3));
        __gfree(0x11);
        __CutsceneWait(0x14);
        *(int *)(q + 0x6c) = (int)OvlFunc_881_200813c;
        __CutsceneWait(0x50);
    }
    x = __MapActor_GetActor(L679c);
    h = 0xc0 << 6;
    *(short *)(x + 6) = h;
    __asm__ volatile ("");
    __MapActor_Emote(L679c, 0x80 << 1, 0);
    __Func_80925cc(L679c, 2);
    m = (int)&_MSG_2644;
    __MessageID(m);
    __Func_8093040(L679c, 0, 0x50);
    if (q != 0)
        __DeleteActor(q);
    __MapActor_SetAnim(0, 1);
    __CutsceneWait(0x28);
    __MapActor_Jump(L679c, 6, 0x28);
    __Func_8093040(L679c, 0, 0x14);
    { PIN1; q0 = 0;
      __Func_8092adc(q0, 0xe0 << 8, 0); }
    __Func_8092adc(L679c, 0xd0 << 8, 0x14);
    k = 0x90 << 8;
    __Func_8093040(L679c | k, 0, 0x28);
    __MapActor_DoAnim(L679c, 4);
    __Func_8093040(L679c | k, 0, 0x14);
    __Func_8092adc(L679c, h, 0x14);
    __Func_8093040(L679c, 0, 0xa);
    __MapActor_SetSpeed(L679c, 0xcccc, 0x6666);
    __MapActor_SetAnim(L679c, 2);
    q = __MapActor_GetActor(0x37);
    { PIN4; q2 = *(int *)(q + 0xc); q1 = 0x177a0000; q3 = 0xd480000; q0 = (int)q;
      __Actor_TravelTo((unsigned char *)q0, q1, q2, q3); }
    __Actor_WaitMovement(q);
    { PIN4; q3 = 0xd580000; q2 = 0; q1 = 0x17710000; q0 = (int)q;
      __Actor_TravelTo((unsigned char *)q0, q1, q2, q3); }
    __Actor_WaitMovement(q);
    __MapActor_SetAnim(0x37, 1);
    __Func_8092adc(L679c, 0xa0 << 7, 0xa);
    __Func_80925cc(L679c, 1);
    __Func_8093040(L679c | (0x80 << 5), 0, 0x14);
    { PIN3; q1 = 0x80 << 9; q2 = 0x80 << 8; q0 = L679c;
      __MapActor_SetSpeed(q0, q1, q2); }
    u = __MapActor_GetActor(0x37) + 0x5a;
    *u = *u & 0xfe;
    __MapActor_SetAnim(0x37, 2);
    { register int q2 __asm__("r2"); q2 = 0;
      __Actor_TravelTo(q, 0x176d0000, q2, 0xd6 << 20); }
    __Actor_WaitMovement(q);
    __MapActor_SetAnim(0x37, 1);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0x37, 2);
    { PIN4; q2 = 0; q3 = 0xd580000; q1 = 0x17710000; q0 = (int)q;
      __Actor_TravelTo((unsigned char *)q0, q1, q2, q3); }
    __Actor_WaitMovement(q);
    __MapActor_SetAnim(0x37, 1);
    __Func_801776c(m + 6, 1);
    e = *(unsigned char **)iwram_3001ebc + (0xec << 1);
    *(short *)e = *(short *)e + 1;
    __Func_8078a08(0xf2);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(L679c, 4);
    __Func_8093040(L679c, 0, 0xa);
    __MapActor_DoAnim(0, 3);
    __MapActor_DoAnim(L679c, 3);
    __MapActor_SetAnim(L679c, 2);
    w = __MapActor_GetActor(0);
    if (w != 0)
        { PIN3; q1 = *(short *)(w + 0xa); q2 = *(short *)(w + 0x12); q0 = L679c;
          __MapActor_TravelTo(q0, q1, q2); }
    __MapActor_WaitMovement(L679c);
    __MapActor_SetPos(L679c, 0, 0);
    __Func_808c4c0();
    __Func_80936a0(0x80 << 9, 6);
    __CutsceneWait(0x14);
    OvlFunc_881_200a7dc();
    __DeleteFieldActor(L679c);
    __ClearFlag(0x8d << 2);
    __SetFlag(0x85d);
    __CutsceneEnd();
}

