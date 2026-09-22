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
    signed char *p;
    signed char *q;
    int bld;
    int alpha;
    int beta;
    volatile unsigned short *ba;
    volatile unsigned short *bc;
    int d;
    int x;

    g = gState;
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_bd)) {
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x100;
        bld = 0x3f42;
        bc = BLDCNT;
        *bc = bld;
        beta = 0x80c;
        __asm__ volatile ("" : : "r" (beta));
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
    p = q;
    do {
        if (p == q)
            __MessageID(0xe2f);
        else
            __MessageID((int)(&_MSG_e30));
        x = OvlFunc_951_2008d70(*p);
        __Func_8019908(x, 2);
        __ActorMessage(8, 0);
        __Func_808f1c0(x, 3);
        __Func_8091a58(x, 0);
        __CutsceneWait(0xa);
        __Func_8092adc(0, 0xc0 << 8, 0);
        p++;
        __CutsceneWait(0x1e);
    } while (*p != -1);
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
