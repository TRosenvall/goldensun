/* asm/overlays/rom_7f2f14/ovl_30_c_c_c_c_a.s, complete: OvlFunc_968_200c610,
 * OvlFunc_968_200c7c0 and OvlFunc_968_200c968.  The .s holds these three and
 * nothing else, and overlay.ld already names
 * `asm/overlays/rom_7f2f14/ovl_30_c_c_c_c_a.o(.text)` and `(.data)`, so this
 * lands with NO split and NO linker edit.
 *
 * OvlFunc_968_200c610 and OvlFunc_968_200c7c0 are the same cutscene written
 * twice with six constants changed and one branch shape changed; c968 is the
 * per-frame particle callback they both hand to OvlFunc_968_2008118.
 *
 * LEVERS, with mechanisms.
 *
 * 1. REAL `do { } while` LOOPS, NOT `goto` LOOPS -- worth 38 differing
 *    positions to 17 in one step, and the single biggest lever here.
 *    gcc-2.96 finds loops from the NOTE_INSN_LOOP_BEG notes the front end
 *    emits for `while`/`for`/`do`, never from a back edge built with `goto`,
 *    so a `goto` loop gets no loop optimisation at all.  That matters twice
 *    over in this function and the recorded entry for it ("`goto` loops
 *    disable loop optimisation ENTIRELY -- a first-class lever") only records
 *    the direction that WANTS the goto.  Here the ROM wants the opposite:
 *      - with `goto`, 0x4ccc and 0x17ffc were commoned into r5/r6 (the ROM
 *        reloads both from the pool at each of the two sites);
 *      - with `goto`, `&t` was hoisted by gcse into a high register and then
 *        cse2 rewrote all three field stores to use it as a base
 *        (`mov r2, r8 / str r3, [r2, #8]` where the ROM has `str r3,
 *        [sp, #0x18]`), which also made `&t` the hottest of the four
 *        call-crossing values and gave it r8 instead of the ROM's r11.
 *    With real loops, loop.c hoists `&t` into the pre-header AFTER cse1 has
 *    already emitted the sp-relative stores, and the two pool constants stay
 *    inside the loop body.  Both defects vanish together.
 *    The one place the ROM genuinely un-rotates is the middle loop, whose
 *    back edge re-enters BELOW the outer loop's random draws: that one is a
 *    `goto mid`, and it is the only goto here.
 *
 * 2. BIRTH ORDER PICKS THE HIGH REGISTER.  The four call-crossing values are
 *    emitted in source-assignment order and take r10, r8, r11, r9 in that
 *    order.  Assigning `tp = &t;` THIRD -- between `i = 0;` and `acc = 0;` --
 *    is what puts the pointer in r11 and the accumulator in r9.  With `acc`
 *    third the two swap and nothing else changes.
 *
 * 3. THE STACK-ARG PAIR, once per call site.  `__CopyMapTiles` takes six
 *    arguments; the ROM materialises both stack values into separate
 *    registers and then stores both.  Two locals per site, assigned adjacent
 *    to the call, and a FRESH pair for the second site (s2/s3, not s0/s1).
 *    Worth 48 -> 38.
 *
 * 4. THE LOOP-TAIL PAIR.  The ROM builds `0x80 << 13` and `1` before doing
 *    either `add`.  Writing `acc += 0x80 << 13;` BEFORE `i++;` reproduces it
 *    even though the ROM's two `add`s come out in the other order -- sched2
 *    reorders the adds, it does not reorder the materialisations.  Naming the
 *    two constants as locals was measured and is inert; only the statement
 *    order moves it.
 *
 * 5. THE `-1` TRIPLE NEEDS CALL-CLOBBERED PINS.  `__Func_80933f8(-1, -1, -1,
 *    0)` is three identical two-instruction constants in ONE basic block, so
 *    cse1 commons them: `mov r2,#1 / neg r2,r2 / mov r0,r2 / mov r1,r2` where
 *    the ROM builds and negates all three.  Measured worse or inert:
 *      three named locals `n0 = n1 = n2 = -1`      inert (still commoned)
 *      `n0 = 1; ...; f(-n0, -n1, -n2, 0)`         180 lines, 45 differing
 *      `-fno-rerun-cse-after-loop`                179 lines, 125 differing
 *      `-fno-gcse`                                173 lines, 182 differing
 *    `register int pN __asm__("rN")` for r0/r1/r2, assigned immediately
 *    before the call, removes the pseudo entirely so there is nothing left to
 *    common.  This is the "bare register pin" half of the recorded remedy for
 *    the batch-242 duplicate-constant refutation, and it is safe here because
 *    no call falls between the assignments and the use.
 *
 * OvlFunc_968_200c968 was PARKED (src/non_matching/ovl_7f2f14/200c968.c) on
 * "scratch-register selection", 22 of 90, with three spellings measured
 * byte-identical.  Two levers close it:
 *
 * 6. AN EARLY `return 0` GUARD IS NOT A TAIL `return 0`.  The park wrote
 *    `if (v != 0) return 0;`, which materialises the zero at the guard.  The
 *    ROM branches over the whole body to one epilogue that sets r0 once, so
 *    the shape is `if (v == 0) { ... } return 0;`.  22 -> 18.
 *
 * 7. `volatile` ON THE RE-READ GLOBAL.  The ROM keeps the ADDRESS of
 *    iwram_3001e40 in r2 and loads through it TWICE, once for the `& 3` test
 *    and again for the `& 7` test inside it.  Naming the address (the lever
 *    the park recorded and measured) cannot reach that: a symbol address is a
 *    link-time constant, so the pointer folds away and gcc still CSEs the
 *    load.  Declaring the global `volatile` emits both loads, and the second
 *    load's live range is what hands r7 to the flag value and r10 to the
 *    struct pointer -- the exact role swap the park was parked on.  With 6
 *    and 7 together the function is exact; either alone is not.
 */

struct P {
    int f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[0x18 - 0x10];
    unsigned short f18;
    unsigned char pad1a[0x1c - 0x1a];
    void *f1c;
    unsigned char pad20[0x22 - 0x20];
    unsigned short f22;
    unsigned char pad24[0x28 - 0x24];
};

struct A { unsigned char pad00[8]; int f8; int fc; int f10; };

extern char *iwram_3001ebc;
extern volatile unsigned int iwram_3001e40;
extern unsigned char L52cc[] __asm__(".L52cc");

extern unsigned int __Random(void);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(void *a, int f);
extern void __Func_8092950(int a, int b);
extern void __Func_8091ff0(int a);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __WaitFrames(int n);
extern int __StartTask(void *f, int n);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8093530(void);
extern void __SetFlag(int id);
extern void __Func_8091e9c(int n);
extern void OvlFunc_968_200c5f0(void);
extern void OvlFunc_968_200c600(void);
extern void OvlFunc_968_2008118(int a, int b, int c, int d,
                                int e, int f, int g, struct P *p);

void OvlFunc_968_200c610(void)
{
    struct P t;
    char *p;
    unsigned int i;
    unsigned int j;
    unsigned int k;
    int acc;
    int base;
    int z;
    int n;
    int s0;
    int s1;
    int s2;
    int s3;
    struct P *tp;
    register int p0 __asm__("r0");
    register int p1 __asm__("r1");
    register int p2 __asm__("r2");

    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x202;
    __CutsceneStart();
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __Func_8092950(0, 0xf);
    __Func_8091ff0(0xaa);
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x28);
    __PlaySound(0xa2);
    __Func_80933d4(0x80 << 8, 0x80 << 5);
    __Func_80933f8(0xdc << 17, -1, 0xb4 << 17, 1);
    j = 0;
    i = 0;
    tp = &t;
    acc = 0;
    do {
        t.f8 = (__Random() * 2 >> 16) * 0x4ccc + 0x17ffc;
        t.fc = (__Random() * 2 >> 16) * 0x4ccc + 0x17ffc;
        t.f22 = (__Random() * 0x1000 >> 16) + (0xf8 << 8);
mid:
        base = 0xc0 << 16;
        k = 0;
        z = 0;
        base += acc;
        do {
            OvlFunc_968_2008118(((__Random() * 7 >> 16) << 19) + (0xd0 << 17),
                                0, base, 0, z, z, 0x88 << 16, tp);
            base += 0x80 << 11;
            k++;
        } while (k <= 3);
        __WaitFrames(3);
        if (i == 3) {
            if (j <= 2) {
                j++;
                goto mid;
            } else if (j == 3) {
                __StartTask(OvlFunc_968_200c5f0, 0xc8 << 4);
            }
        }
        n = i + 0xc;
        s0 = 3;
        s1 = 1;
        __CopyMapTiles(0x35, n, 0x1a, n, s0, s1);
        acc += 0x80 << 13;
        i++;
    } while (i <= 0xc);
    s2 = 9;
    s3 = 2;
    __CopyMapTiles(0x51, 0x29, 0x59, 0xe, s2, s3);
    __Func_8093530();
    p0 = -1;
    p1 = -1;
    p2 = -1;
    __Func_80933f8(p0, p1, p2, 0);
    __CutsceneWait(0x3c);
    __SetFlag(0x306);
    __Func_8091e9c(0x13);
    __CutsceneEnd();
}

void OvlFunc_968_200c7c0(void)
{
    struct P t;
    char *p;
    unsigned int i;
    unsigned int j;
    unsigned int k;
    int acc;
    int base;
    int z;
    int n;
    int s0;
    int s1;
    int s2;
    int s3;
    struct P *tp;
    register int p0 __asm__("r0");
    register int p1 __asm__("r1");
    register int p2 __asm__("r2");

    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x202;
    __CutsceneStart();
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __Func_8092950(0, 0xf);
    __Func_8091ff0(0xaa);
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x28);
    __PlaySound(0xa2);
    __Func_80933d4(0x80 << 8, 0x80 << 5);
    __Func_80933f8(0x8e << 18, -1, 0xb4 << 17, 1);
    j = 0;
    i = 0;
    tp = &t;
    acc = 0;
    do {
        t.f8 = (__Random() * 2 >> 16) * 0x4ccc + 0x17ffc;
        t.fc = (__Random() * 2 >> 16) * 0x4ccc + 0x17ffc;
        t.f22 = (__Random() * 0x1000 >> 16) + (0xf8 << 8);
mid:
        base = 0xc0 << 16;
        k = 0;
        z = 0;
        base += acc;
        do {
            OvlFunc_968_2008118(((__Random() * 7 >> 16) << 19) + (0x88 << 18),
                                0, base, 0, z, z, 0x88 << 16, tp);
            base += 0x80 << 11;
            k++;
        } while (k <= 3);
        __WaitFrames(3);
        if (i == 3) {
            if (j <= 2) {
                j++;
                goto mid;
            }
        }
        __StartTask(OvlFunc_968_200c600, 0xc8 << 4);
        n = i + 0xc;
        s0 = 3;
        s1 = 1;
        __CopyMapTiles(0x3a, n, 0x22, n, s0, s1);
        acc += 0x80 << 13;
        i++;
    } while (i <= 0xc);
    s2 = 5;
    s3 = 2;
    __CopyMapTiles(0x56, 0x29, 0x61, 0xe, s2, s3);
    __Func_8093530();
    p0 = -1;
    p1 = -1;
    p2 = -1;
    __Func_80933f8(p0, p1, p2, 0);
    __CutsceneWait(0x3c);
    __SetFlag(0x307);
    __Func_8091e9c(0x14);
    __CutsceneEnd();
}

int OvlFunc_968_200c968(struct A *src)
{
    struct P t;
    int v;
    int x;
    int y;
    int z;
    int n;

    t.f0 = 1;
    t.f4 = 5;
    t.f18 = 0x8f << 1;
    t.f1c = L52cc;
    v = iwram_3001e40 & 3;
    if (v == 0) {
        if ((iwram_3001e40 & 7) == 0)
            __PlaySound(0xf6);
        x = src->f8 + (((__Random() * 49 >> 16) - 0x18) << 16);
        y = src->fc + (((__Random() * 49 >> 16) - 0x18) << 16);
        z = src->f10 + (((__Random() * 49 >> 16) - 0x18) << 16);
        n = ((__Random() * 4 >> 16) << 15) + (0x80 << 8);
        OvlFunc_968_2008118(x, y, z, 0, n, v, 0xcc << 14, &t);
    }
    return 0;
}
