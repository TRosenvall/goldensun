/* OvlFunc_933_20098a4 -- asm/overlays/rom_7bc690/ovl_18a4_a_a.s
 * NON-MATCHING, 351 encodings of 393.  NOT a distance (ref 888 bytes / 393 encodings against ours 896 / 396, four encodings over).
 * READ `--align`: 175 of 376.  All 33 relocations exact and in the ROM's order, and the pool-word
 * order already matches, so the residue is entirely inside the code.
 *
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M`.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7bc690/20098a4.c \
 *     asm/overlays/rom_7bc690/ovl_18a4_a_a.s --func OvlFunc_933_20098a4
 * Distance while iterating:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/tryc.py src/non_matching/ovl_7bc690/20098a4.c --ref asm/overlays/rom_7bc690/ovl_18a4_a_a.s --align
 * 352 instructions, ONE function, no data section (grep -ci func_start = 1):
 * whole-file conversion.  batch 294, brief E, target 1.
 *
 * PARKED.  --align 175 of 376.  objcmp: 351 of 393 differ (ours 396), SIZE ref
 * 888 / ours 896 -- FOUR ENCODINGS OVER, so the 351 is not a true distance.
 * All 33 RELOCATIONS are present, in the same order, with the same symbols
 * (`_divsi3_RAM` against our `__divsi3` is the alias at
 * overlays/rom_7bc690/overlay.ld:80, not a difference), so the literal pool's
 * contents AND its order -- including the three mid-function pool dumps -- are
 * already right.
 * NO SHIMS from this file: zero `register ... __asm__` declarations, zero
 * `__asm__(".equ ...")` lines, no volatile, no barriers, default flags.  The
 * `register`-pinned locals in the three DMA3_SET expansions come from the
 * project's own include/dma.h and are not mine.
 *
 * BLOCKER: register allocation, in two places -- a shared loop counter that
 * lands in r8 where the ROM has it in the call-clobbered r4 with a caller-save
 * slot, and a pointer whose REG_EQUIV symbol reload rematerialises twice per
 * block instead of keeping once.
 *
 * ================= THE READING =================
 *
 * A per-frame HUD refresh.  A three-way chain drives the short counter at
 * .L26be: forced to 2 while .L3030 is set, decremented while save bit 0x104 is
 * set, otherwise incremented up to 2 with a palette DMA on the way through 1.
 * Zero takes the early exit through Func_8003f78.  Otherwise it fills 144
 * halfwords in the iwram_3001ecc record (644-byte stride, +0x26, every 4th
 * byte), allocates a 0x900 scratch buffer, recolours the six 5-5-5 entries at
 * q[6..11] by gOvl_0200a6bc, hands three words to Func_80038bc, recomputes
 * gOvl_0200a6bc from gState+0x232 over gState+0x22c, runs the .L26c0 fade,
 * blanks a tile region, overlays a second copy of the buffer, uploads it and
 * programs the five 12-byte OAM entries at .L26e0.
 *
 * struct SpriteSlot is src/non_matching/ovl_77dd1c/200c41c.c's
 * `{ u16 size; u16 vramOffset; }` -- `ldrh r3,[r3,#2]` off `gSpriteSlots + sl*4`.
 *
 * ROM SHAPES THAT ARE NOT SOURCE CONSTRUCTS:
 *  - `push {r5,r6,r7,lr}` with r4 unsaved AND r4 spilled around calls: the
 *    project builds with -fcall-used-r4 (Makefile:131).  Not a flag to find.
 *  - the three `.align 2,0 / .word K / .pool` dumps mid-body with a `b` over
 *    them are gcc's own: a *thumb_movhi_insn pool reference has pool_range 64
 *    (arm.md:4353), cannot reach the barrier at the end of a 352-instruction
 *    function, and dump_table manufactures the jump.  Our output reproduces
 *    all three.
 *  - `sub r3, #0xc` after each `stmia r3!, {r0,r1,r2}` is inside DMA3_SET.
 *
 * ============ FIVE LOAD-BEARING CONSTRUCTS, each singly measured ============
 *
 * 1. THE TWO SHORT GLOBALS ARE REACHED THROUGH POINTER LOCALS.
 *    [aligned 218 -> 245, but the LENGTH 366 -> 374, i.e. +8 of the 10 we were
 *    short; it is the length that matters here, per the brief's "a count that
 *    rises while the length becomes right is progress"]
 *    *** NEW MECHANISM, and it sharpens docs/elevation.md's narrowed HImode
 *    rule ("only 0 and >= 0x8000 need an int local", plus a counter-example
 *    section that says "something else can drag a small value into the pool
 *    too").  The something else is WHERE THE DESTINATION ADDRESS COMES FROM.
 *    Probed directly (scratch_elev/b294/E/probe.c, probe2.c):
 *
 *        extern short g[];
 *        void f1(void){ g[0] = 2; }                 ->  mov  r3, #2
 *        void f6(short *p){ p[0] = 2; }             ->  ldrh r3, .L18 / .word 2
 *        void f1b(void){ short *p = g; p[0] = 2; }  ->  ldrh r3, .L3  / .word 2
 *
 *    A HImode store of a small literal through a SYMBOLIC address gives the
 *    immediate; through a POINTER IN A REGISTER it goes to the pool.  The
 *    thumb arm of the movhi expander (arm.md:4269) force_reg's operands[1]
 *    whenever operands[0] is not a REG, and the symbol case is diverted earlier
 *    by the `! memory_address_p` path.  This ROM pools 2, 0x77 AND 0 -- and
 *    0x77 is nowhere near the documented >= 0x8000, so the pointer is the only
 *    reading that explains all three. ***
 *    (And per elevation.md's own warning: gcc prints these as `ldrh rN, .Lx`
 *    while the ROM disassembly writes `ldr rN, =K`; Thumb-1 has no pc-relative
 *    ldrh, so gas emits the identical halfword.  Not a difference.)
 *
 * 2. THE 144-HALFWORD FILL STORES A `short` LOCAL, NOT AN int EXPRESSION.
 *    [218 -> 217 alone; load-bearing in combination]  The ROM hoists
 *    `lsl r3, r0, #0x13 / asr r2, r3, #0x10` -- a shift by 19 then an
 *    ARITHMETIC shift by 16, i.e. the 16-bit sign extension of v * 8.  Writing
 *    `*q = v * 8;` gives a bare `lsl r2, r0, #3`; `short y = v * 8;` outside the
 *    loop restores it.  This is ARM's PROMOTE_MODE holding a HImode local
 *    sign-extended (arm.h:597), the same mechanism as batch 293's finding 5,
 *    used deliberately here.
 *
 * 3. gState IS REACHED THROUGH A POINTER LOCAL, so the base and the offset stay
 *    in separate pool words.  [237 -> 184, the single largest lever]
 *    `*(short *)(gState + 0x232)` folds the addend into ONE relocation
 *    (`ldr r3, =gState+562`); the ROM has `ldr r2, =gState / ldr r0, =0x232 /
 *    add r3, r2, r0` and then reaches +0x22c off the SAME r2 with
 *    `mov r3,#0x8b / lsl r3,#2 / add r2, r3`.  `gs = gState;` immediately before
 *    the statement gives exactly that.  Same family as elevation.md's "a member
 *    array keeps base and offset in separate registers".
 *
 * 4. THE COLOUR-TABLE POINTER'S INITIAL VALUE IS A SEPARATE VARIABLE.
 *    [184 -> 177, and it fixed the frame from 0x8 to the ROM's 0xc]  The ROM
 *    saves p + 0xc to sp+4 before the recolour loop and reloads it for the FIRST
 *    Func_80038bc argument (`ldr r2,[sp,#4] / ldr r1,[r2]`), deriving only the
 *    second and third from p.  `e0 = (unsigned short *)p + 6; e = e0;` and then
 *    `*(int *)e0` for that one argument reproduces the third stack slot, which
 *    in turn puts `t` on the ROM's sp+8 and every other `[sp, #imm]` right.
 *
 * 5. THE POST-CHAIN READ GOES THROUGH THE ARRAY, NOT THE POINTER.
 *    [177 -> 175, length 381 -> 378]  `v = L26be[0];` gives the ROM's single
 *    `ldrsh r0, [r4, r2]`; `v = cp[0];` lets cse share the MEM with the pointer
 *    reads in the chain above and comes out as `ldrh` plus an explicit
 *    `lsl #16 / asr #16`.  Two spellings of the same address are two different
 *    rtx, and that is the whole lever.
 *
 * ============ MEASURED AND WORSE -- do not retry these ============
 *
 *  - A SEPARATE COUNTER PER LOOP (five `unsigned int`s instead of one):
 *    aligned 218 -> 205, and it is the WRONG ANSWER -- the length goes
 *    366 -> 357, nineteen instructions SHORT, because each block-local counter
 *    takes a low register and pays no caller-save.  This is exactly the brief's
 *    "a count too LOW can be a signature".  One shared counter is right.
 *  - `volatile` on the two short globals, with direct array access: 237.
 *    `volatile short *` pointer locals: 204.  `volatile` on gOvl_0200a6bc: 261,
 *    and it moves the first difference to instruction 8.  The ROM's repeated
 *    `ldr rN, =.L26be` + fresh `ldrsh` really does look like elevation.md's
 *    volatile signature and IS NOT: it is reload rematerialising a REG_EQUIV
 *    symbol.  Worth recording, because the tell misfires here.
 *  - assigning `cp = L26be;` inside each arm of the chain (to make it a
 *    block-local allocno that could take r4): 255.
 *  - `cp[0]--` / `cp[0]++` for `cp[0] = cp[0] - 1` / `+ 1`: byte-identical.
 *
 * ============ THE RESIDUE ============
 *
 * (a) THE SHARED LOOP COUNTER IS IN r8, THE ROM'S IS IN r4.  The ROM pays
 *     `str r4,[sp]` / `ldr r4,[sp]` around the calls in the recolour loop and
 *     the OAM loop -- a caller-save slot at sp+0, which is why its frame is 0xc
 *     with only two real spills.  We get a callee-saved high register instead
 *     and pay `mov rN,#1 / add r8,rN / mov rM,r8` at each increment.  For r4 to
 *     be chosen, local-alloc's `find_free_reg` walk must first FAIL over the
 *     callee-saved registers and fall through to the caller-save retry
 *     (`4 * calls_crossed < n_refs`); with r8..r11 free it never gets there.
 *     This is the same fallback-ordering fact batch 293 established for
 *     Func_80191cc -- the retry is unreachable while an earlier register in
 *     REG_ALLOC_ORDER (arm.h:989: 3,2,1,0,12,14,4,5,6,7,8,10,9,11) is free.
 *     Note the direction: adding references cannot help, because the threshold
 *     is not what is failing.
 *
 * (b) TWO EXTRA `ldr rN, =.L26be` PER CHAIN ARM -- and that is exactly the
 *     four-encoding excess.  The ROM keeps the pointer in r4 across an arm
 *     (`ldr r4,=.L26be / ldrsh r3,[r4,r5] / ldrh r2,[r4] / ... / strh r3,[r4]`);
 *     we load the address into r3, then clobber r3 with the loaded value, then
 *     load the address again into r1 for the store.  `cp` carries a REG_EQUIV
 *     symbol, so reload rematerialises it per reference rather than allocating
 *     it, and each rematerialisation picks whatever register is free.  Removing
 *     the REG_EQUIV is the lever and I found no source spelling that does it
 *     without also losing lever 1 -- the pointer is what pools the literals.
 *     THAT COUPLING is the thing for the next attempt to break.
 *
 * (c) `&.L26e0[j]` for the Func_8003dec argument: the ROM re-derives it as
 *     `ldr r0, =.L26e0 / add r0, r7, r0` from a separate j*0xc offset register,
 *     we pass the running store pointer.  And the 0x80 << 23 OAM flag sits in
 *     r8 in the ROM against our r4.  Both follow the allocation in (a).
 *
 * -- worked in scratch_elev/b294/E
 */

#include "dma.h"

extern short L26be[] __asm__(".L26be");
extern short L26c0[] __asm__(".L26c0");
extern short L26d0[] __asm__(".L26d0");
extern short L3030[] __asm__(".L3030");
extern unsigned char L2730[] __asm__(".L2730");
extern unsigned char OvlData_933_2009f80[];
extern unsigned char gState[];
extern short gOvl_0200a6bc[];
extern unsigned char *iwram_3001ecc;
extern int iwram_3001e40;

struct SpriteSlot {
    unsigned short size;
    unsigned short vramOffset;
};
extern struct SpriteSlot gSpriteSlots[];

struct Oam {
    int a;
    int b;
    int c;
};
extern struct Oam L26e0[] __asm__(".L26e0");

extern int __GetFlag(int id);
extern void __Func_8003f78(int slot);
extern void *__Func_8004938(int size);
extern void __Func_80038bc(void *dst, int v);
extern void __UploadSpriteGFX(int slot, int size, void *src);
extern void __Func_8003dec(struct Oam *o, int n);
extern void __free(void *p);

void OvlFunc_933_20098a4(void)
{
    unsigned int t;
    short *cp;
    short *fp;
    int v;
    short y;
    unsigned char *g;
    unsigned char *gs;
    int bi;
    short *q;
    unsigned int j;
    unsigned short *e;
    unsigned short *e0;
    unsigned short c;
    unsigned int r;
    unsigned int gr;
    unsigned int b;
    int n;
    void *p;
    int *d;
    int *w;
    unsigned char *src;
    unsigned char *dst;
    unsigned char bv;
    unsigned int k;
    unsigned int f;

    cp = L26be;
    fp = L26c0;
    t = gSpriteSlots[L26d0[0]].vramOffset >> 5;
    if (L3030[0] != 0) {
        cp[0] = 2;
    } else if (__GetFlag(0x82 << 1) != 0) {
        if (cp[0] > 0)
            cp[0] = cp[0] - 1;
    } else if (cp[0] <= 1) {
        cp[0] = cp[0] + 1;
        if (cp[0] == 1)
            DMA3_SET(OvlData_933_2009f80, (void *)0x50003c0, 0x80000010);
    }
    v = L26be[0];
    if (v == 0) {
        __Func_8003f78(L26d0[0]);
        return;
    }
    g = iwram_3001ecc;
    if (g != 0) {
        bi = g[0x539];
        q = (short *)(g + bi * 644 + 0x26);
        y = v * 8;
        for (j = 0; j < 144; j++) {
            *q = y;
            q += 2;
        }
    }
    p = __Func_8004938(0x90 << 4);
    DMA3_SET(OvlData_933_2009f80, p, 0x80000010);
    e0 = (unsigned short *)p + 6;
    e = e0;
    for (j = 6; j < 12; j++) {
        c = *e;
        r = c & 0x1f;
        gr = (c >> 5) & 0x1f;
        b = (c >> 10) & 0x1f;
        n = gOvl_0200a6bc[0];
        r = r + n / 3;
        b = b - 0x14 - n / 6 + 0x14;
        if (n > 0x3c && (iwram_3001e40 & 1) != 0)
            gr = gr + n * 64 / 120 - 0x20;
        if (r > 0x1f)
            r = 0x1f;
        if (gr > 0x1f)
            gr = 0x1f;
        if (b > 0x1f)
            b = 0x1f;
        *e = r | (gr << 5) | (b << 10);
        e++;
    }
    w = (int *)p;
    __Func_80038bc((void *)0x50003cc, *(int *)e0);
    __Func_80038bc((void *)0x50003d0, w[4]);
    __Func_80038bc((void *)0x50003d4, w[5]);
    gs = gState;
    gOvl_0200a6bc[0] = *(short *)(gs + 0x232) * 15 * 8 / *(short *)(gs + 0x22c);
    if (gOvl_0200a6bc[0] > 0x76)
        fp[0] = 0x77;
    if (fp[0] != 0) {
        gOvl_0200a6bc[0] = fp[0];
        fp[0] = fp[0] - 8;
        if (fp[0] <= 0)
            fp[0] = 0;
    }
    DMA3_SET(L2730, p, 0x84000240);
    if (fp[0] <= 0x76) {
        d = (int *)((unsigned char *)p + 0x50);
        for (j = 0xc; j < 0x80 - gOvl_0200a6bc[0]; j++) {
            d[8] = 0xeeeeeeee;
            *d++ = 0xeeeeeeee;
            if ((j & 7) == 7)
                d += 8;
        }
        d[0] = w[0];
        d[8] = w[8];
    }
    src = (unsigned char *)p + 0x480;
    dst = (unsigned char *)p;
    for (j = 0; j < 0x480; j++) {
        bv = *src++;
        if (bv != 0)
            *dst = bv;
        dst++;
    }
    __UploadSpriteGFX(L26d0[0], 0x90 << 3, p);
    f = 0x80008000;
    k = 8;
    for (j = 0; j < 5; j++) {
        unsigned int x = (cp[0] * 8 - 0x10) & 0x1ff;
        if (j == 4)
            f = 0x80 << 23;
        L26e0[j].a = 0;
        L26e0[j].b = (x << 16) | k | f;
        L26e0[j].c = 0xe4 << 8 | t;
        __Func_8003dec(&L26e0[j], 0xff);
        t += 8;
        k += 0x20;
    }
    __free(p);
}
