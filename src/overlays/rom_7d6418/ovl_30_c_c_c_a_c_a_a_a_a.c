/* WHOLE-FILE CONVERSION of asm/overlays/rom_7d6418/ovl_30_c_c_c_a_c_a_a_a_a.s --
 * OvlFunc_951_20081d8 is its only function and datacheck.py reports no data section,
 * so no split and no linker change.  277 instructions, 740 bytes, 296 encodings and
 * 61 relocations identical.
 *
 * `_AREA_bd` is already in area.sym:130 and nothing needed adding.  Against the tree
 * reference this object reports 1 differing encoding -- that pool word, the
 * relocation placeholder -- and against a symbolised copy it is OK.  CONTROL: with a
 * literal 0xbd instead of the symbol the function collapses to 293 instructions and
 * -8 bytes, so the pool load IS the symbol tell rather than a formatting choice.
 *
 * Was parked at 9 of 296.  Three findings closed it.
 *
 * ================================================================
 * A BARE `__asm__ volatile ("")` PLACED *BEFORE* THE STATEMENT WHOSE TWO CHAINS MUST
 * NOT SWAP IS A CHEAP, REPEATABLE sched2 PHASE LEVER -- AND THE POSITION MATTERS IN
 * ONE DIRECTION ONLY
 * ================================================================
 *
 * Two of them did 8 -> 6 -> 4.  Mechanism read out of haifa-sched.c's
 * rank_for_schedule: with priorities equal, THE CLASS RELATIVE TO
 * `last_scheduled_insn` DECIDES BEFORE `depend_count` AND LONG BEFORE `INSN_LUID`.
 * In the first window the `ldr r3,=0x3f42` reload was anti-dependent on the preceding
 * `str r2,[r3]` (class 2) while `ldr r1,=0x4000050` had dep 0 (class 3) and won; the
 * barrier changed WHICH INSN WAS LAST-SCHEDULED at the sort.
 *
 * A BARRIER PLACED *AFTER* THE STATEMENT FIXES THE REGISTER ORDER BUT KILLS THE
 * ldr/mov INTERLEAVE (8 -> 6, then stuck).  So this is not a symmetric tool -- put it
 * before the statement you are protecting.
 *
 * ================================================================
 * loop.c HOISTS INVARIANTS WITH `emit_insn_before (..., loop_start)`, SO A HOISTED
 * INVARIANT CAN NEVER PRECEDE PREHEADER CODE.  THE CURE IS TO MAKE THE POINTER A
 * STRENGTH-REDUCED giv INSTEAD OF PREHEADER CODE.
 * ================================================================
 *
 * That is why 112 barrier, pin and carrier spellings could not put `mov r8, r5` (the
 * hoisted -1) before `adds r6, r7, #0` (`p = q`): no source construct reorders them,
 * because one is emitted before the loop start and the other is the loop start.
 *
 * Writing the loop over an INDEX (`q[i]`) instead of a walking pointer means
 * `strength_reduce` emits the `p = q` init -- and strength_reduce runs AFTER
 * move_movables, so the invariant lands first, which is the ROM's order.
 * `maybe_eliminate_biv` then rewrites `i == 0` back into the ROM's `cmp r6, r7`.
 * THIS WAS THE SINGLE EDIT FROM 2 TO 0.
 *
 * Negative control worth keeping: naming the -1 in a local (`nm = -1`) at four
 * positions all gave 28, because cse merges it with the earlier -1 pseudo and the copy
 * disappears entirely.
 *
 * RE-RUNNING THE DROP LADDER PAID AGAIN.  The park's
 * `__asm__ volatile ("" : : "r" (beta))` BEFORE `ba = BLDALPHA` became INERT once the
 * new bare barrier landed, and is dropped.  Everything shipped is load-bearing on
 * single drop: barrier 1 -> 6, barrier 2 -> 6, the trailing "r"(beta) barrier -> 2,
 * the bld/bc carriers -> 287, the alpha carrier -> 226, the beta carrier -> 253.
 *
 * No per-file Makefile flag override applies to this stem.
 */
/* OvlFunc_951_20081d8 -- NON-MATCHING, 9 encodings of 296 against the tree
 * reference; 8 against a symbolised copy, the difference being one pool word under
 * an already-provisioned .sym symbol.  SIZE AND RELOCATIONS EXACT.  277 instructions.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7d6418/20081d8.c \
 *     asm/overlays/rom_7d6418/ovl_30_c_c_c_a_c_a_a_a_a.s
 * ONE function, no data section -- it CONVERTS WHOLE when it lands, no split.
 *
 * SHIPS WITH ZERO PINS.  Four PIN blocks were each measured inert SINGLY *and
 * JOINTLY* (8 either way) and all four are dropped.  Only two barriers are
 * load-bearing: `__asm__ volatile ("" : : "r" (beta))` before the address assignment
 * and another AFTER the store -- 12 and 10 respectively if dropped, 14 if both go.
 *
 * A BARRIER *AFTER* A STORE IS NOT IN THE RECORDED REPERTOIRE and is what stops the
 * next call's `mov r0` hoisting above the `strh`.  Worth adding to the barrier
 * vocabulary, which until now has been "before the thing you want kept down".
 *
 * ================================================================
 * THE LEVER THAT GOT IT HERE: A HImode POOL LOAD FORCES A MID-BODY POOL DUMP
 * ================================================================
 *
 * Worth 248 -> 16 IN ONE EDIT.  `*(volatile unsigned short *)p = 0x80c;` compiles to
 * `ldrh r3, .L30` -- a HImode pool load with pool_range 64.  gcc must dump the pool
 * within 64 bytes, so it emits `b .L31` over a mid-body `.word` block at instruction
 * ~50 of 300.  The ROM keeps its whole pool at the end.  Routing the value through
 * an `int` carrier makes it an SImode `ldr` (range 1020) and the dump disappears.
 *
 * THE DIAGNOSTIC IS CHEAP AND EASY TO MISS: generate the .s and grep for
 * `ldrh rN, .L`.  If one exists, every register and ordering difference downstream is
 * noise.  tryc shows it only as a stray `b Lnn / Lnn:` because it strips the pool
 * words -- which reads like a cross-jump artefact and is not.
 *
 * `_AREA_bd` and `_MSG_e30` are already in their tables; `_MSG_e30` is already
 * spelled as a symbol in the reference.  `_MSG_e31`, 0xe13, 0xe14, 0xe2e and 0xe2f
 * all exceed 0xff and reproduce as plain literals -- NO TELL, measured.
 *
 * The _MSG_e43 / _MSG_e49 / _MSG_e4c id space in this overlay was NOT touched, so
 * those three remain UNVERIFIED as src/non_matching/ovl_7d6418/2008ac8.c records.
 *
 * No per-file Makefile flag override applies to this stem.
 *
 * NEXT: 9 encodings with size and relocations exact is a strong position.  The two
 * load-bearing barriers say the residue is scheduling; read .23.sched2's ready list
 * at the remaining windows rather than sweeping spellings.
 */
#define BLDCNT   ((volatile unsigned short *)0x04000050)
#define BLDALPHA ((volatile unsigned short *)0x04000052)

extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;
extern unsigned char iwram_3001d18;
extern unsigned char ewram_2001000[];
extern int _AREA_bd;
extern int _MSG_e30;

extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __PlaySound(int id);
extern void __Func_8019908(int a, int b);
extern void __Func_808f1c0(int a, int b);
extern void __Func_8091a58(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092950(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80b04c4(void);
extern void OvlFunc_951_20088f8(int a);
extern int OvlFunc_951_2008d70(int a);
extern void OvlFunc_951_20096a8(void);

int OvlFunc_951_20081d8(void)
{
    unsigned char *g;
    unsigned char *h;
    unsigned char *r;
    signed char *q;
    int bld;
    int alpha;
    int beta;
    volatile unsigned short *ba;
    volatile unsigned short *bc;
    int d;
    int x;
    int i;

    g = gState;
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_bd)) {
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x100;
        __asm__ volatile ("");
        bld = 0x3f42;
        bc = BLDCNT;
        *bc = bld;
        __asm__ volatile ("");
        beta = 0x80c;
        ba = BLDALPHA;
        *ba = beta;
        __asm__ volatile ("" : : "r" (beta));
        __MapActor_SetAnim(0x18, 2);
        __MapActor_SetAnim(0x19, 2);
        *(int *)(__MapActor_GetActor(0x18) + 0x18) = 0xffff0000;
        *(int *)(__MapActor_GetActor(0x19) + 0x18) = 0xffff0000;
        __MapActor_GetActor(0x18)[0x23] = 2;
        __MapActor_GetActor(0x19)[0x23] = 2;
        __MapTransitionIn();
        if (*(short *)(g + (0xe1 << 1)) != 1)
            goto out;
        OvlFunc_951_20096a8();
        if (__GetFlag(0x200) == 0)
            goto out;
        *bc = bld;
        alpha = 0x1000;
        *ba = alpha;
        goto out;
    }
    if (__GetFlag(0x950) != 0)
        __MapActor_SetPos(0x11, 0, 0);
    iwram_3001d18 = 1;
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x209;
    if (*(short *)(g + (0xe1 << 1)) == 0xa) {
        __Func_8092950(8, 1);
        __Func_8092950(9, 2);
    }
    if (*(short *)(g + (0xe1 << 1)) == 0xd && __GetFlag(0x109) == 0) {
        __CutsceneStart();
        __Func_8092950(8, 1);
        __Func_8092950(9, 2);
        __MapTransitionIn();
        __WaitMapTransition();
        __CutsceneWait(0xa);
        __Func_80921c4(0, 0x78, 0x70);
        __CutsceneWait(0x14);
        d = *(int *)(g + 0x10) - *(int *)ewram_2001000;
        if (d > 0) {
            if (d > 0x4e1f)
                __PlaySound(0x5d);
            else if (d > 0x1387)
                __PlaySound(0x5c);
            else
                __PlaySound(0x5b);
            __CutsceneWait(0x14);
            __MessageID(0xe13);
            __Func_8019908(d, 5);
            __ActorMessage(9, 0);
            __Func_80b04c4();
        } else if (d < 0) {
            __MessageID(0xe14);
            __Func_8019908(-d, 5);
            __ActorMessage(9, 0);
        }
        __CutsceneEnd();
    }
    h = gState;
    if (*(short *)(h + (0xe1 << 1)) != 0xc)
        goto out;
    if (__GetFlag(0x109) != 0)
        goto out;
    q = (signed char *)(h + (0x96 << 1));
    __CutsceneStart();
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0xa);
    if (*q == -1) {
        OvlFunc_951_20088f8(1);
        goto done;
    }
    if (*q == -2)
        goto done;
    __MessageID(0xe2e);
    __ActorMessage(8, 0);
    if (*q == -1)
        goto tail;
    i = 0;
    do {
        if (i == 0)
            __MessageID(0xe2f);
        else
            __MessageID((int)(&_MSG_e30));
        x = OvlFunc_951_2008d70(q[i]);
        __Func_8019908(x, 2);
        __ActorMessage(8, 0);
        __Func_808f1c0(x, 3);
        __Func_8091a58(x, 0);
        __CutsceneWait(0xa);
        __Func_8092adc(0, 0xc0 << 8, 0);
        i++;
        __CutsceneWait(0x1e);
    } while (q[i] != -1);
tail:
    r = gState;
    r[0x96 << 1] = 0xfe;
    __MessageID(0xe31);
    __ActorMessage(8, 0);
done:
    __CutsceneEnd();
out:
    return 0;
}
