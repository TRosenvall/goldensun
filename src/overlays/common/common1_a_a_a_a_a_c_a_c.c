/* Cluster OvlFunc_common1_148..OvlFunc_common1_2c4, the TAIL of
 * goldensun/asm/overlays/common/common1_a_a_a_a_a_c_a.s.
 *
 * The .s holds FOUR functions: OvlFunc_common1_78 (still parked, see
 * src/non_matching/overlays/common1_78.c) then these three.  Split keeps _78
 * alone in common1_a_a_a_a_a_c_a_a.s (208 bytes) and gives this file
 * common1_a_a_a_a_a_c_a_b (668 bytes); 208 + 668 = 876 = the original object,
 * measured with the assembler, so the split is byte-neutral BEFORE any .c is
 * written.  _78 keeps `.global` from the .thumb_func_start macro, so the two
 * calls to it from here need no linker or .global edit.
 *
 * Retires src/non_matching/overlays/common1_148.c.
 *
 * NEAR-TWINS.  _2c4 was solved first; _190 was transcribed by substituting its
 * constants into the solved tail and came in at 138 lines against 138 with
 * SEVEN instructions in disagreeing regions on the FIRST screen.  One
 * initialiser transposition closed it.  The shared tail is
 * SetFlagByte/SetFlagByte/`n++`/`n > 3`/Func_8091e9c+SetFlag else
 * common1_78+MapTransitionIn+WaitMapTransition+`*p = 0`.
 *
 * LEVERS, in the order they mattered on _2c4 (141 -> 0).  Full measurements in
 * scratch_elev/b258/common/NOTES.md.
 *
 *  1. `register int msg __asm__("r8")` for 0x2086.  The ROM pushes FOUR high
 *     registers and the first transcription pushed three; the missing value is
 *     the MessageID base, which local-alloc rematerialises as three separate
 *     pool loads (0x2086/0x2087/0x2088) because the pseudo is
 *     constant-equivalent.  The pin defeats that so the split into
 *     msg / msg+1 / msg+2 survives to reload.  r8 is the ROM's own register
 *     here and it is the ONLY one that works: r5/r7 come out four bytes short,
 *     r9/r10/r11 eight long, r6 lands at the ROM's length but gives the
 *     one-instruction `add r0, r6, #1` for the ROM's `mov r0, r8 / add r0, #1`.
 *     This is a pin on a register the recorded rule says not to pin; it is
 *     safe here because the candidate reaches ZERO differing, and the emitted
 *     r8 has exactly one definition in the body (`mov r8, r3`) plus the
 *     prologue save and epilogue restore.
 *
 *  2. An `int` local for EVERY constant stored through a halfword.  A bare
 *     literal in a HImode store is pooled -- `ldrh r2, .L7` off a `.word`
 *     entry -- and the pool entry drags the whole pool mid-function behind a
 *     `b .L8`.  `w = 0x2089` and `four = 4` both need it; so does `w = 0x63`
 *     in _148.
 *
 *  3. An ADDRESS LOCAL for the halfword store.  This is the batch's main
 *     finding and it closed TWO functions here.
 *     In _2c4 `i3 = (unsigned short *)(i2 + 0xcc4); *i3 = four;` is worth
 *     9 -> 6 where pinning `four` to every low register and moving its
 *     assignment are not: naming the address forces the address add ahead of
 *     the `mov r3, #4`, which is the ROM's order.
 *     In _148 the same two lines together retire a park that had measured the
 *     int local ALONE at 4 and a hard-register pin ALONE at 4, and concluded
 *     the function had no register to spare.  It does -- the address has to be
 *     named at the same time.  `r = (unsigned short *)(p + 0xc1 * 2); w = 0x63;
 *     *r = w;` is 1 -> 0.  The address local alone is still 1.
 *
 *  4. `extern int __Func_8092c40(...)` -- an int return on the call whose
 *     argument fill order is wrong.  r0 goes LAST for it and FIRST for
 *     __ActorMessage two lines later, in the same function; prototyping
 *     decides each independently.  All twelve combinations over four callees
 *     measured; __MessageID and __Func_8091c7c are inert.
 *
 *  5. `x` and `z` locals for the two `>> 20` reads feeding __SetFlagByte.
 *     THIS IS THE LAST FOUR AND IT IS NOT AT THE DIFFERING SITE.  The residue
 *     was the two `mov rN, r11` actor copies EXACTLY TRANSPOSED -- ROM r0 then
 *     r3, ours r3 then r0 -- a reload round-robin phase error that is inert to
 *     every spelling of those two sites.  Naming the shifted reads adds one
 *     reload upstream and the phase lands.  Measured: `z` alone 6 -> 4, `x`
 *     and `z` together 4 -> 0.  Eleven other upstream perturbations (an int
 *     local for the 0xf, for the 0x200, a second pointer local, dropping `p`,
 *     a local for msg+1, five pins on an actor copy) are all INERT at 4.
 *
 * -fno-schedule-insns2 REGRESSES 28 -> 52 on _2c4, so sched2 was right
 * throughout and alias was never the axis -- the whole residue was allocation.
 *
 * verdict, measured 3x each and 3x on the merged object:
 *   OK OvlFunc_common1_148 --  72 bytes,  32 encodings and  3 relocations identical
 *   OK OvlFunc_common1_190 -- 308 bytes, 134 encodings and 18 relocations identical
 *   OK OvlFunc_common1_2c4 -- 288 bytes, 117 encodings and 23 relocations identical
 *   OK WHOLE OBJECT        -- 668 bytes, 283 encodings and 44 relocations identical
 */

extern int iwram_3001ebc;
extern unsigned char gState[];
extern int __GetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __CutsceneWait(int n);
extern void __MapTransitionOut(void);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __SetFlagByte(int id, int v);
extern void __Func_8091e9c(int n);
extern void __SetFlag(int id);
extern void OvlFunc_common1_78(int n);
extern int __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);

void OvlFunc_common1_148(void)
{
    char *p;
    unsigned char *g;
    unsigned char *q;
    int v;
    int t;
    int off;
    unsigned short *r;
    int w;
    p = (char *)iwram_3001ebc;
    g = gState;
    off = 0xfa * 2;
    q = g + off;
    v = *(int *)q;
    if (v == 0)
        return;
    off -= 0x76;
    q = (unsigned char *)(p + off);
    t = (short)*(unsigned short *)q >> 10;
    if (t != v)
        return;
    if (__GetFlag(0x141) == 0)
        return;
    r = (unsigned short *)(p + 0xc1 * 2);
    w = 0x63;
    *r = w;
}

void OvlFunc_common1_190(void)
{
    unsigned char *iw;
    unsigned char *actor;
    unsigned char *a;
    unsigned char *g;
    unsigned char *q;
    int *p;
    int n;
    int i;
    int besti;
    int best;
    int dx;
    int dz;
    int u;
    int d;
    int x;
    int z;

    iw = (unsigned char *)iwram_3001ebc;
    besti = 8;
    best = 0x80 << 13;
    g = gState;
    q = g + 0xfa * 2;
    n = *(int *)q;
    actor = __MapActor_GetActor(n);
    __CutsceneStart();
    for (i = 8; i <= 0x42; i++) {
        a = __MapActor_GetActor(i);
        if (a == 0)
            continue;
        if (*(a + 0x54) != 1)
            continue;
        if (*(short *)(*(int *)(*(int *)(a + 0x50) + 0x28)) != 0xa5)
            continue;
        dx = (*(int *)(actor + 8) - *(int *)(a + 8)) / 0x10000;
        dz = (*(int *)(actor + 0x10) - *(int *)(a + 0x10)) / 0x10000;
        if (dz > 0)
            continue;
        u = dx;
        if (u < 0)
            u = -u;
        if (dz < 0)
            dz = -dz;
        d = u + dz;
        if (d < best) {
            besti = i;
            best = d;
        }
    }
    __MessageID(0x2085);
    __ActorMessage(besti, 0);
    p = (int *)(iw + 0xe0 * 2);
    *p = 0x80 << 2;
    *(int *)(iw + 0xe4 * 2) = 0xf;
    __CutsceneWait(0x14);
    __MapTransitionOut();
    __WaitMapTransition();
    x = *(int *)(actor + 8) >> 20;
    __SetFlagByte(n * 16 + 0xdc * 4, x);
    z = *(int *)(actor + 0x10) >> 20;
    __SetFlagByte(n * 16 + 0xde * 4, z);
    n++;
    if (n > 3) {
        __Func_8091e9c(0xa);
        __SetFlag(0x8d * 2);
    } else {
        OvlFunc_common1_78(n);
        __MapTransitionIn();
        __WaitMapTransition();
        *p = 0;
    }
    __CutsceneEnd();
}

void OvlFunc_common1_2c4(int slot)
{
    unsigned char *iw;
    unsigned char *actor;
    int *p;
    int n;
    register int msg __asm__("r8");
    int r;
    int four;
    int w;
    unsigned char *g;
    unsigned char *q;
    unsigned char *i2;
    unsigned short *i3;
    int z;
    int x;

    iw = (unsigned char *)iwram_3001ebc;
    __MapActor_GetActor(slot);
    __MapActor_GetActor(slot);
    g = gState;
    q = g + 0xfa * 2;
    n = *(int *)q;
    actor = __MapActor_GetActor(n);
    __CutsceneStart();
    msg = 0x2086;
    __MessageID(msg);
    __Func_8092c40(slot, 0);
    i2 = (unsigned char *)iwram_3001ebc;
    w = 0x2089;
    *(unsigned short *)(i2 + 0xcc2) = w;
    i3 = (unsigned short *)(i2 + 0xcc4);
    four = 4;
    *i3 = four;
    r = __Func_8091c7c(n, 0);
    if (r == 0) {
        __MessageID(msg + 1);
        __ActorMessage(slot, 0);
        p = (int *)(iw + 0xe0 * 2);
        *p = 0x80 << 2;
        *(int *)(iw + 0xe4 * 2) = 0xf;
        __MapTransitionOut();
        __WaitMapTransition();
        x = *(int *)(actor + 8) >> 20;
        __SetFlagByte(n * 16 + 0xdc * 4, x);
        z = *(int *)(actor + 0x10) >> 20;
        __SetFlagByte(n * 16 + 0xde * 4, z);
        n++;
        if (n > 3) {
            __Func_8091e9c(0xa);
            __SetFlag(0x8d * 2);
        } else {
            OvlFunc_common1_78(n);
            __MapTransitionIn();
            __WaitMapTransition();
            *p = 0;
        }
    } else {
        __MessageID(msg + 2);
        __ActorMessage(slot, 0);
    }
    __CutsceneEnd();
}
