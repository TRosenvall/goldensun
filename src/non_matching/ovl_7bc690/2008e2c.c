/* PARKED -- OvlFunc_933_2008e2c, --align 77 of 237 / objcmp 181 of 239
 * NON-MATCHING, 181 encodings of 239.  NOT a distance (ref 552 bytes / 239 encodings against ours 568 / 242).  READ `--align`: 77 of 237.
 * Opened from nothing this batch.  No .sym needed -- the two pooled ids are already _AREA_59 and
 * _AREA_5a (area.sym:168-169) and the three labels are already .global in the file-mate.
 *
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M`.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7bc690/2008e2c.c \
 *     asm/overlays/rom_7bc690/ovl_4e4_c_a_a.s --func OvlFunc_933_2008e2c
 *   [asm/overlays/rom_7bc690/ovl_4e4_c_a_a.s -- the file's ONLY function,
 *   confirmed with `grep -ci func_start` = 1.  228 instructions.  datacheck
 *   reports no data section, so NO SPLIT; every data symbol it reads is defined
 *   and already `.global` elsewhere (see below).]
 *
 * VERDICT -- this target was UNOPENED at the start of the batch.
 *   this draft (t5_park.c)  --align 77 of 237, objcmp 181 of 239 (ours 242,
 *                           568 bytes against 552) -- 3 instructions LONG
 *   t5_v3.c                 --align 86 of 237, objcmp 222 of 239 (ours 246,
 *                           576 bytes) -- the LINE count matches exactly
 *   t5_v1.c                 --align 91 of 237, objcmp 206 of 239 (ours 236,
 *                           556 bytes) -- forward `for` loop, 3 SHORT
 * NONE of the three is a true distance: the encoding count is wrong in all of
 * them, so objcmp's figures are positional and `--align` is the number to rank
 * by.  All three are kept because they are wrong in different directions and
 * the brief's own warning applies -- the count that is lowest (77) is not the
 * one whose length is right (t5_v3), and the closest length (t5_v1) has the
 * worst distance.
 *
 * Verify with:
 *   python3 tools/tryc.py src/non_matching/ovl_7bc690/2008e2c.c \
 *       --ref asm/overlays/rom_7bc690/ovl_4e4_c_a_a.s --align
 *   python3 tools/objcmp.py src/non_matching/ovl_7bc690/2008e2c.c \
 *       asm/overlays/rom_7bc690/ovl_4e4_c_a_a.s --whole
 *
 * =========================== WHAT IS SETTLED ================================
 *
 * 1. NO NEW `.sym` IS NEEDED -- BOTH POOLED IDS ALREADY EXIST.  The ROM does
 *    `ldr r3, =0x59 / cmp r2, r3` where `cmp r2, #0x59` would have done, which
 *    is verbatim area.sym's own criterion ("gcc-2.96 never pools a constant it
 *    can build with an eight-bit `mov`").  `_AREA_59 = 0x59` and
 *    `_AREA_5a = 0x5a` are ALREADY in area.sym at lines 168-169, so this needs
 *    no proposal at all -- only `extern int _AREA_59;` and
 *    `area == (int)&_AREA_59`, the idiom of
 *    src/overlays/rom_7b4558/ovl_30_c_c_c_a_c.c.  In-function control is
 *    complete: the same chain also compares nothing else, and the two pooled
 *    words are the whole tell.
 *
 * 2. THE THREE TABLES BIND WITH PLAIN EXTERNS.  `.L1f48` and `.L1f70` are
 *    already `.global` in the file-mate asm/overlays/rom_7bc690/ovl_4e4_c_c_c.s
 *    at :292 and :296, whose comments there read "data table read from
 *    OvlFunc_933_2008e2c; exported so ..." -- somebody already did this work.
 *    `Events_TolbiSpring` is `.global` at :289 of the same file.  Names five hex
 *    characters long, far outside the `.L1`..`.L30` range gcc emits here, so no
 *    capture and no linker alias.
 *
 * 3. THE 8-ARGUMENT CALL IS TARGET 1 OF THIS BRIEF.  Both
 *    `OvlFunc_common0_10c` sites confirm the signature independently derived
 *    from asm/overlays/common/common0_c.s: r0-r3 = x, y, z, 0 and four stack
 *    words = 0, 0, flags, &block, with flags 0x1c0000 selecting exactly the
 *    three bits (0x40000 script, 0x80000 f18/f1c, 0x100000 id) whose reads that
 *    function performs, and the block being a 0x28-byte record -- which is the
 *    size of the two locals at sp+0x18 and sp+0x40 here.  rom_7bc690 is one of
 *    the nineteen overlays that pull common0_c.o in.
 *
 * 4. THE LOOP IS A COUNTDOWN IN THE SOURCE, NOT A REVERSED FORWARD LOOP.
 *    The ROM has a pre-loop guard `cmp r7,#0 / beq` plus a bottom test
 *    `cmp r7,#0 / bne`, r7 falling from cnt to 0, a byte offset r6 rising by 8,
 *    and the index recovered as `mov r2,r9 / sub r2,r7` (= cnt - r7).  That
 *    reads as check_dbra_loop having reversed a forward `for`, and IT CANNOT BE:
 *    the loop contains two calls, and the inc-to-dec rewrite needs either
 *    `no_use_except_counting` (false -- the index is stored) or
 *    `! loop_info->has_call` (false).  Written forward the loop comes out
 *    forward, `cmp r8,r7 / bge`, and is 3 instructions SHORT (t5_v1, 91).
 *    Written as the countdown with `bi = cnt - k` it is the ROM's shape and the
 *    length becomes right (t5_v3, 86 at an exact line count).  **A "reversed"
 *    loop around a call is a source countdown, because the pass that would have
 *    reversed it is gated off by the call.**
 *
 * 5. TWO SMALL ORDERING FACTS, each measured as a single drop on t5_v2:
 *    - the stack slot order is the DECLARATION order: `unsigned char *w;` then
 *      `int n;` puts w at [sp,#0x14] and n at [sp,#0x10] as the ROM has them;
 *      the other order swaps them.  93 -> 86.
 *    - `p = tab;` must sit ABOVE the `if (k != 0)` guard and `off = 0;` inside
 *      it, because the ROM's `mov r5, r8` precedes the guard and its
 *      `mov r6, #0` follows it.
 *    - making the if/else chain assign the LOOP variable and copying to the
 *      saved count afterwards (`k = 3; ... cnt = k;`) rather than the reverse
 *      is 86 -> 77 but costs 3 instructions, so it is the lowest-distance and
 *      not the right-length draft.  Both are kept.
 *
 * ============================= WHAT IS OPEN =================================
 *
 * THE RESIDUE IS REGISTER ALLOCATION IN THE SELECTION CHAIN AND THE LOOP.  The
 * ROM keeps the countdown in LOW callee-saved r7 and the table base in r8, so
 * each arm of the chain is `ldr r2,=tab / mov r7,#N / mov r8,r2` -- three
 * instructions.  We put the countdown in r5/r8 and the base in r10 and need a
 * fourth `mov` per arm.  Downstream of that, `best` sits in r9 where the ROM
 * has r11, and the index arithmetic (`lsl #1` then `lsl #2`, i.e. the source
 * indexes an `int[]` by `bi * 2` rather than an 8-byte record by `bi`) lands in
 * different registers.  The next thing to try is the file-mate discipline from
 * src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_a_c_a_a.c -- sweep the
 * declaration order of {cnt, k, tab, best, bi, off} AFTER the loop shape is
 * fixed, not before, since those two searches are one search there too.
 *
 * NOT YET CHECKED, and cheap: the `*(unsigned short *)(w + 0xcba) = n` tail
 * reads `add r1, sp, #0x10 / ldrh r1, [r1]` in the ROM -- a HALFWORD read of an
 * `int` stack slot, which is what a plain `= n` gives when only the low half is
 * needed; writing a literal `0` there instead has not been measured, and the
 * same question applies to the second OvlFunc_common0_10c call's 5th and 6th
 * arguments (the ROM passes `ldr r3,[sp,#0x10]`, i.e. n, where the value is
 * provably 0 on that path).
 *
 * SHIMS: NONE (both classes checked separately, on the code with this comment
 * stripped).
 */
struct Actor {
    unsigned char pad0[8];
    int f08;
    int f0c;
    int f10;
    unsigned char pad14[0x28 - 0x14];
    int f28;
};

struct Param {
    unsigned char pad00[8];
    int f08;
    int f0c;
    int f10;
    int f14;
    short f18;
    unsigned char pad1a[2];
    int f1c;
    unsigned short f20;
    unsigned short f22;
    int f24;
};

extern int _AREA_59;
extern int _AREA_5a;
extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;
extern short ewram_2000472;
extern int Events_TolbiSpring[];
extern int L1f48[] __asm__(".L1f48");
extern int L1f70[] __asm__(".L1f70");

extern void __SetFlag(int id);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern void __CutsceneWait(int n);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Actor_TravelTo(struct Actor *a, int x, int b, int y);
extern void OvlFunc_933_2009c78(int n);
extern int OvlFunc_933_2008e00(void *p, int *e);
extern void OvlFunc_933_2008324(struct Actor *a, int v);
extern void OvlFunc_common0_10c(int x, int y, int z, int dx, int dy, int dz,
                                int flags, struct Param *p);

void OvlFunc_933_2008e2c(void)
{
    unsigned char *w;
    int n;
    struct Param b2;
    struct Param b1;
    struct Actor *a;
    unsigned char *g;
    int *tab;
    unsigned short *q;
    int cnt, i, best, bi, j, k, off;
    int *p;

    n = 0x3c;
    w = iwram_3001ebc;
    best = 0xf0 << 16;
    __SetFlag(0x80 << 2);
    OvlFunc_933_2009c78(1);
    g = gState;
    if (*(short *)(g + (0xe0 << 1)) == (int)&_AREA_59) {
        k = 3;
        tab = Events_TolbiSpring;
    } else if (*(short *)(g + (0xe0 << 1)) == (int)&_AREA_5a) {
        k = 5;
        tab = L1f48;
    } else {
        k = 2;
        tab = L1f70;
    }
    bi = 0;
    cnt = k;
    p = tab;
    if (k != 0) {
        off = 0;
        do {
            int d = OvlFunc_933_2008e00((char *)__MapActor_GetActor(0) + 8, p);
            if (d <= best) {
                best = d;
                bi = cnt - k;
            }
            off += 8;
            k--;
            p = (int *)((char *)tab + off);
        } while (k != 0);
    }
    j = bi * 2;
    __MapActor_SetSpeed(0, 0x80 << 10, 0x80 << 9);
    a = __MapActor_GetActor(0);
    __Actor_TravelTo(a, tab[j], 0, tab[j + 1]);
    __MapActor_GetActor(0)->f28 = 0xc0 << 11;
    __PlaySound(0x98);
    OvlFunc_933_2008324(__MapActor_GetActor(0), __MapActor_GetActor(0)->f0c);
    __PlaySound(0xf1);
    a = __MapActor_GetActor(0);
    b1.f18 = 0xd6;
    b1.f08 = 0x80 << 8;
    b1.f0c = 0xcccc;
    b1.f10 = 0x80 << 9;
    b1.f14 = 0x13333;
    OvlFunc_common0_10c(a->f08, a->f0c, a->f10, 0, 0, 0, 0xe0 << 13, &b1);
    __MapActor_Emote(0, 0x82 << 1, 0);
    __MapActor_SetAnim(0, 0x12);
    q = (unsigned short *)(w + 0xcba);
    do {
        *q = 0x96 << 2;
        n--;
        if (ewram_2000472 != 0) {
            ewram_2000472 -= 5;
            if (ewram_2000472 <= 0)
                ewram_2000472 = 0;
            else if (n == 0)
                n = 1;
        }
        __WaitFrames(1);
    } while (n != 0);
    a = __MapActor_GetActor(0);
    b2.f18 = 0xd6;
    b2.f0c = 0xcccc;
    b2.f08 = 0x80 << 8;
    b2.f10 = 0x80 << 8;
    b2.f14 = 0x13333;
    OvlFunc_common0_10c(a->f08, a->f0c, a->f10, 0, n, n, 0xe0 << 13, &b2);
    __PlaySound(0x90 << 1);
    __PlaySound(0x98);
    __MapActor_GetActor(0)->f28 = 0xc0 << 11;
    __MapActor_SetAnim(0, 1);
    __CutsceneWait(0xa);
    *(unsigned short *)(w + 0xcba) = n;
    OvlFunc_933_2009c78(0);
}
