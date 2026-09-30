/* OvlFunc_923_200a030 -- NON-MATCHING, 14 of 371 encodings differ.
 * Size equal, count equal (ref 371, ours 371), so the 14 IS a distance.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7aa430/200a030.c \
 *     asm/overlays/rom_7aa430/ovl_1a3c_a_c_c_a.s --func OvlFunc_923_200a030
 * ONE function in the reference -- it CONVERTS WHOLE, no split.
 *
 * ============================================================
 * THIS IS AN EXACT TWIN OF src/non_matching/ovl_7ac2d8/200d5c0.c
 * ============================================================
 * OvlFunc_924_200d5c0 (asm/overlays/rom_7ac2d8/ovl_35b8_a_c_c_a.s) and this
 * function have IDENTICAL instruction streams. Normalising `.L<addr>` labels and
 * the two per-overlay script symbols leaves exactly ONE differing line out of 378:
 *
 *     923   bl OvlFunc_923_2009bc8
 *     924   bl OvlFunc_924_200d158
 *
 * NO CONSTANT DIFFERS. The symbol map is mechanical:
 *     .L5e44 -> .L27f4   (both `.global`'d by the overlay's _c.s file)
 *     gScript_924__0200de2c -> gScript_923__0200a7dc   (the f18 arm)
 *     gScript_924__0200de20 -> gScript_923__0200a7d0   (the spawn arm)
 * dupfuncs.py does NOT pair them -- it only matches byte-identical text, and the
 * three relocated symbols differ. The pairing was found by grepping src/ for the
 * FOUR-HELPER SIGNATURE `__vec3_translate` + `__TestCollision` + `__Func_8092158`
 * + `Func_8000888`, which hits exactly one park. That is a cheap and repeatable
 * way to find a twin the byte-level tool cannot see: a rare COMBINATION of
 * callees is as good a fingerprint as the bytes.
 *
 * This is the SECOND 923/924 twin (ovl_1a3c_a_c_a_b.c records the first,
 * OvlFunc_923_2009ec8 / OvlFunc_924_200d458). Overlay 924 is a near-copy of
 * overlay 923; check for a twin BEFORE writing anything in either.
 *
 * IF YOU EDIT ONE, EDIT BOTH. Everything below applies to 200d5c0.c verbatim.
 *
 * ============================================================
 * WHAT THIS BATCH CLOSED: 16 -> 14
 * ============================================================
 * THE TWO LOOP-SETUP ASSIGNMENTS MUST BE WRITTEN `i = 0;` BEFORE `mask = 0xf;`.
 * The ROM emits
 *     mov r1, #0xf / ldr r6, =gKeyPress / mov r5, #0 / mov r10, r1
 * and the park had
 *     mov r1, #0xf / ldr r6, =gKeyPress / mov r10, r1 / mov r5, #0
 * -- the high-register copy `mov r10, r1` and the `mov r5, #0` swapped. Nothing
 * about the mask expression reaches this; the ONLY lever is the order of the two
 * initialisers in the source. Worth 2 encodings from a one-line move, and it is
 * the same class as batch 278's "s = n->f50 must be first": sched2 emits the
 * high-register copy at the position its source statement occupies relative to
 * its sibling, even though both feed a loop that follows.
 *
 * THE SITE-2 `t` BARRIER IS NOW INERT AND HAS BEEN REMOVED. With the loop-setup
 * order fixed, `__asm__ volatile ("" : : "r" (t))` in the cell2 block measures
 * 14 with and without. THE SITE-1 BARRIER IS STILL LOAD-BEARING: removing it
 * measures 16, and removing both measures 16. One shim fewer than the twin park.
 *
 * ============================================================
 * THE REMAINING 14, by region
 * ============================================================
 * (a) 3 encodings -- the `__vec3_translate` argument fill.
 *         rom   mov r2, r11 / lsl r0, #0xe / mov r1, r6
 *         ours  lsl r0, #0xe / mov r1, r6 / mov r2, r11
 *     The third argument is filled FIRST in the ROM. This is NOT the uniform
 *     ascending pinned-fill shape; it is sched2 having hoisted one `mov` two
 *     slots. MEASURED, all inert at 14: a named `int d = 0x80 << 14` for arg1;
 *     a named `int *pp = p` for arg3; three argument temporaries declared in
 *     reverse order. A `__asm__ volatile ("" : : "r" (p))` before the call is
 *     26 -- far worse, and it is the only probe that moved anything.
 *
 * (b) 8 encodings -- a scratch-register rotation in the SECOND division block.
 *         rom   T1=r2  C1=r0  T2=r1  C2=r0
 *         ours  T1=r1  C1=r2  T2=r0  C2=r1
 *     where Tn is `mov rX, r11` (the low-register copy of `p` the Thumb `ldr`
 *     base requires -- a RELOAD register, not a pseudo) and Cn is the signed-
 *     division rounding constant 0xfffff. THE SHAPE IS EXACT; only the names
 *     differ. The FIRST division block, whose constants fold to 0x17ffff, is
 *     byte-exact in both.
 *
 *     THE ROM GIVES BOTH CONSTANTS r0. Ours gives them r2 and r1, i.e. two qtys
 *     that out-prioritise the reload temps. local-alloc.c's QTY_CMP_PRI is
 *         floor_log2(n_refs) * n_refs * size / (death - birth)
 *     and all four are n_refs=2, size=1, length ~2 -- a four-way TIE, broken by
 *     qty number, i.e. by RTL creation order. So the lever, if there is one, is
 *     the order the two constants are created in, not their live ranges.
 *
 *     AND THE REG_EQUIV LEVER DOES NOT REACH IT. local-alloc.c's
 *     update_equiv_regs doubles REG_LIVE_LENGTH for a constant REG_EQUIV, but
 *     the comment directly above that line says so explicitly:
 *         "Note that the statement below does not affect the priority
 *          in local-alloc!"
 *     That RETIRES the batch-295 "dominating-block local for a repeated
 *     constant" lever for any constant whose live range lies inside ONE basic
 *     block: it is a GLOBAL-alloc lever only. Measured accordingly: hoisting
 *     `int rc = 0xfffff;` to the top of the function and hand-writing both
 *     divisions as `if (x < 0) x += rc; x >>= 20;` reproduces the ROM's
 *     schedule EXACTLY and leaves all four registers unchanged -- 14, inert, in
 *     four placements (function top, before the translate call, inside the
 *     block, declared first among the locals).
 *
 * (c) 2 encodings -- the cell2 destination.
 *         rom   add r1, r3, r1 / str r1, [sp, #0x8]
 *         ours  add r3, r3, r1 / str r3, [sp, #0x8]
 *     `cell2` is spilled the instant it is computed, so r1/r3 is RELOAD's choice
 *     of reload register, not an allocation. `g += t * 4; cell2 = g;` changes the
 *     operand order to `add r3, r1, r3` but not the destination -- still 14.
 *
 * (d) 1 encoding -- `__TestCollision`'s second argument.
 *         rom   add r1, sp, #0x10        ours  mov r1, r11
 *     The ROM re-derives `&v` from sp where r11 already holds it. MEASURED, all
 *     inert at 14: `&v[0]`, `v + 0`, `(int *)(char *)v`, and a one-member
 *     `union { int a[3]; }` wrapping the array (the batch-295 alias escape --
 *     it does not apply, because the conflict here is between two spellings of
 *     one ADDRESS, not between two MEMs).
 *
 * ============================================================
 * Carried forward from the twin park, all still true
 * ============================================================
 * `define_peephole` in arm.md:523 turns `mov rX,#N / add rX,sp` into
 * `add rX,sp,#N` only when the two insns end up ADJACENT, so the ROM's
 * two-instruction form is sched2 output, not a source fact.
 *
 * PINNING A POOLED SYMBOL TO A CALL-CLOBBERED REGISTER reproduces the ROM's
 * rematerialisation where CSE would common it: `register int g __asm__("r1")`
 * at both cell sites gives the ROM's two `ldr r1, =gBuffer` loads, because a
 * hard register cannot survive the intervening `bl`. The assignment must come
 * AFTER the index computation or the load lands in an earlier block. Repinning g
 * to r0 or r2 measures 15; to r3, 21.
 *
 * STACK-SLOT ORDER FOLLOWS C DECLARATION ORDER EXACTLY.
 *
 * The `call_via` clobber list carries "r2" deliberately -- it moves the
 * Func_8000888 pointer from r2 to the ROM's r4.
 *
 * NEXT: (b) is the whole game -- 8 of the 14. It is a four-way qty-number tie
 * in local-alloc, so the probe is a source edit that changes the ORDER in which
 * the two 0xfffff constants are created, with the schedule held fixed (the
 * hand-written-division form above does hold it fixed, so it is the right
 * vehicle). Nothing about live ranges will move it.
 */
extern unsigned int gState;
extern unsigned int gKeyHeld;
extern unsigned int gKeyPress;
extern unsigned char *iwram_3001edc;
extern short L27f4[] __asm__(".L27f4");
extern unsigned char gBuffer[];
extern int gScript_923__0200a7d0;
extern int gScript_923__0200a7dc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __ClearFlag(int f);
extern void __vec3_translate(int dist, int angle, int *v);
extern int __TestCollision(unsigned char *a, int *v);
extern void __Actor_SetScript(unsigned char *a, void *s);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern unsigned char *__CreateActor(int kind, int x, int y, int z);
extern void __DeleteActor(unsigned char *a);
extern void __Sprite_SetAnim(unsigned char *s, int n);
extern void __Func_8092158(int a, int b, int c);
extern void OvlFunc_923_2009bc8(unsigned char *a);
extern int Func_8000888(int a, int b);

static inline int call_via(int (*f)(int, int), int a, int b)
{
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile (
        "\t.align\t2, 0\n"
        "\tmov\tr12, pc\n"
        "\tbx\t%1"
        : "=r" (_a)
        : "r" (f), "0" (_a), "r" (_b)
        : "memory", "lr", "r12", "r2"
    );
    return _a;
}

struct E {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    unsigned char *f14;
    unsigned char *f18;
};

void OvlFunc_923_200a030(void)
{
    int v[3];
    int *p;
    unsigned char *base;
    unsigned char *cell2;
    int saved;
    unsigned char *savep;
    struct E *e;
    unsigned char *actor;
    unsigned char *cell1;
    unsigned char *a;
    unsigned char *spr;
    unsigned int r3;
    unsigned int r4;
    int idx;
    int off;
    int ang;
    int t;
    int n;
    int i;
    int j;
    short *tp;
    int mask;

    r3 = (unsigned int)&iwram_3001edc;
    e = *(struct E **)*(unsigned char **)r3;
    r3 -= 0x20;
    base = *(unsigned char **)r3;
    r4 = (unsigned int)&gState;
    r4 += 0xfa << 1;
    idx = *(int *)r4;
    off = idx * 4;
    off += 0x14;
    actor = *(unsigned char **)(base + off);
    savep = actor + 0x55;
    saved = *savep;
    tp = L27f4;
    i = (gKeyHeld >> 4) & 0xf;
    j = i * 2;
    ang = (unsigned short)*(short *)((char *)tp + j);
    if (*(short *)((char *)tp + j) == -1)
        return;

    p = v;
    p[0] = (*(int *)(actor + 8) & 0xfff00000) + (0x80 << 12);
    p[1] = *(int *)(actor + 0x14);
    p[2] = (*(int *)(actor + 0x10) & 0xfff00000) + (0x80 << 12);
    { register int g __asm__("r1"); int t;
      t = (p[2] / 0x100000) * 128 + p[0] / 0x100000; __asm__ volatile ("" : : "r" (t));
      g = (int)gBuffer;
      cell1 = (unsigned char *)(t * 4 + g); }
    __vec3_translate(0x80 << 14, ang, p);
    { register int g __asm__("r1"); int t;
      t = (p[2] / 0x100000) * 128 + p[0] / 0x100000;
      g = (int)gBuffer;
      cell2 = (unsigned char *)(t * 4 + g); }
    if (cell1[2] != e->f4 && cell2[2] == e->f4 && e->f0 == 0)
        return;
    __CutsceneStart();
    t = __TestCollision(actor, v);
    if (t)
        return;
    a = e->f18;
    if (a != 0) {
        *(unsigned short *)(a + 0x64) = t;
        __Actor_SetScript(a, &gScript_923__0200a7dc);
        __Actor_SetAnim(a, 7);
        e->f18 = (unsigned char *)t;
    }
    if (cell2[2] == e->f4 && e->f0 != 0) {
        unsigned char *m = e->f14;
        a = __CreateActor(0x1a, *(int *)(m + 8), *(int *)(m + 0xc),
                          *(int *)(m + 0x10));
        if (a != 0) {
            spr = *(unsigned char **)(a + 0x50);
            *(unsigned char **)(a + 0x14) = *(unsigned char **)(m + 0x14);
            __Actor_SetScript(a, &gScript_923__0200a7d0);
            a[0x55] = t;
            *(unsigned short *)(a + 0x64) = t;
            a[0x23] = 2;
            *(int *)(a + 0x30) = 0x80 << 11;
            *(int *)(a + 0x34) = 0x80 << 10;
            __Actor_TravelTo(a, p[0], p[1], p[2]);
            if (spr != 0) {
                __Sprite_SetAnim(spr, 6);
                spr[0x26] = 0;
            }
            e->f18 = a;
        }
        n = e->f0 - 1;
        e->f0 = n;
        if (n == 0) {
            __DeleteActor(e->f14);
            e->f14 = (unsigned char *)n;
            __ClearFlag(0x161);
        } else if (e->f14 != 0) {
            __Actor_SetAnim(e->f14, 6 - n);
        }
    }
    __Actor_SetAnim(actor, 6);
    __WaitFrames(3);
    __PlaySound(0x98);
    __Actor_SetAnim(actor, 7);
    *(int *)(actor + 0x30) = 0xc0 << 10;
    *(int *)(actor + 0x34) = 0x80 << 10;
    *(int *)(actor + 0x28) = 0x80 << 11;
    *savep = *savep & 0x7e;
    __Actor_SetSpriteFlags(actor, 0);
    __Func_8092158(0, ((short *)p)[1], ((short *)p)[5]);
    __Actor_SetAnim(actor, 6);
    __WaitFrames(2);
    if (cell2[2] != e->f4)
        __Actor_SetSpriteFlags(actor, 1);
    else
        __PlaySound(0xd7);
    __WaitFrames(1);
    *savep = saved;
    if (cell2[2] == e->f4 && e->f18 == 0) {
        __Actor_SetAnim(actor, 0x12);
        __PlaySound(0xf1);
        i = 0;
        mask = 0xf;
        for (;;) {
            if ((i & mask) == 0)
                OvlFunc_923_2009bc8(actor);
            if (i > 0x1f && gKeyPress != 0)
                break;
            __WaitFrames(1);
            i++;
        }
        __PlaySound(0x90 << 1);
        __WaitFrames(1);
        *(int *)(actor + 8) = e->fc;
        *(int *)(actor + 0x10) = e->f10;
        __Actor_SetSpriteFlags(actor, 1);
    }
    e->f8 = 0;
    __CutsceneEnd();
    *(int *)(base + (0xda << 1)) +=
        call_via(Func_8000888, *(int *)(base + (0xd8 << 1)), 0x80 << 14);
}

/* *** BATCH-305d ADDENDUM for src/non_matching/ovl_7aa430/200a030.c
 *     (AND ITS TWIN src/non_matching/ovl_7ac2d8/200d5c0.c -- EDIT BOTH) ***
 *
 * RE-MEASURED AS INSTALLED: 14 of 371, ref 371 / ours 371, size silent.  The
 * header figure is CONFIRMED and it is a true distance.  STILL 14.
 *
 * ONE HEADER CLAIM DOES NOT REPRODUCE -- CORRECT IT.  The "(b)" section says
 * hand-writing both divisions as `if (x < 0) x += rc; x >>= 20;` "reproduces
 * the ROM's schedule EXACTLY and leaves all four registers unchanged -- 14,
 * inert, in four placements".  IT MEASURES 20, NOT 14.  Every hand-written
 * spelling of the second division block costs 6 encodings on top of the 14:
 *     literal 0xfffff at both divisions                        20
 *     the two divisions written in reverse order               20
 *     `0x100000 - 1` instead of 0xfffff                        20
 *     the rounding constant through a reused existing local
 *       (`j`, `off`, `idx`, `n` -- four separate builds)       20
 * All six keep the count exact at 371, so the 6 extra are register/schedule,
 * not size.  THE HAND-WRITTEN DIVISION IS NOT A NEUTRAL VEHICLE, so the
 * header's "NEXT" plan -- probe the constant creation order with the schedule
 * held fixed by the hand-written form -- rests on a vehicle that itself moves
 * the schedule.  That plan is closed.
 *
 * THE COMPILER'S OWN DIVISION, with the creation order varied instead:
 *     t = 128 * (p[2] / 0x100000) + p[0] / 0x100000            14  (inert)
 *     t = p[0] / 0x100000 + (p[2] / 0x100000) * 128            19
 *     t = (p[2] / 0x100000) * 128; t += p[0] / 0x100000;       16
 *     idx = p[2]/0x100000; off = p[0]/0x100000; t = idx*128+off;   24
 *     the same two assignments in reverse order                24
 *     n = p[2]/0x100000; j = p[0]/0x100000; t = n*128+j;      177
 * So reordering or naming the two dividends cannot move region (b) either:
 * everything is 14 or worse.  Region (b) -- 8 of the 14 -- takes no source
 * handle from the dividend side, the constant side, or the division spelling.
 *
 * ALSO NOTE, for anyone reading tools/tryc.py --full on this park: it prints
 * "rom 375 lines, ours 376 ... 269 differ".  That is an ARTEFACT.  Ours emits
 * one EXTRA LABEL (a redundant `.L7:` beside `.L8:`) which is not an encoding,
 * so the line-level view desynchronises from there and calls everything after
 * it different.  objcmp is right at 14 and the count is exact.  Use objcmp.
 */
