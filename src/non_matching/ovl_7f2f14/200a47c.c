/* OvlFunc_968_200a47c -- PARKED, **PIN-FREE, 26 DIFFERING ENCODINGS OF 273**.
 *
 * ref 273 / ours 273 stream encodings; **238 / 238** 16-bit instructions -- IN
 * PHASE, a TRUE DISTANCE.  dsize 0.  Relocations differ in ONE entry and it is
 * a PURE OFFSET SHIFT: `bl __Func_8092b08` at 0xae in the reference against
 * 0xb0 here.  Same call, same symbol, two bytes later.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7f2f14/200a47c.c \
 *     asm/overlays/rom_7f2f14/ovl_30_c_c_a_a_a_c.s --func OvlFunc_968_200a47c
 *
 * ================ BATCH 341 D: THE FIVE PINS ARE WORTH 15 ================
 *
 * PINNED FIGURE 11 of 273 with FIVE register pins.  The old header's per-pin
 * drop costs are CONFIRMED and they are EXACTLY ADDITIVE:
 *     r2 = 2 at the first `|= 2` site              4
 *     r5 = 0 for `a->f44 = 0`                      7
 *     r1 = 0 at the first OvlFunc_968_2008058      2
 *     r0, r1 at the second OvlFunc_968_2008058     2
 *                                            sum  15   and 11 + 15 = 26 = this
 * figure, measured directly with all five removed at once.  No pair interacts.
 * To rebuild the pinned alternate, restore the four blocks named above.
 * NOT INSTALLED: pin policy prefers pin-free and a pinned landing needs a
 * fakematch.txt row.
 *
 * ***THE OLD HEADER'S "narrow-local is 2 WORSE THAN THE PIN" DOES NOT HOLD
 * PIN-FREE, and the difference matters.***  It reads as though
 * `unsigned char m = 2` at the first orr costs 2 over the pinned 11, i.e. 13.
 * Measured on the PIN-FREE base it is 28 -- 2 worse than 26, not 2 worse than
 * 11.  The old number was taken with the OTHER FOUR PINS STILL IN.  A per-pin
 * figure quoted against a multi-pin base is not a figure about that pin.
 *
 * TEN DEVICE-FREE SPELLINGS, ALL EXACTLY INERT AT 26 -- a hard floor on this
 * axis.  Do not re-screen:
 *     `{ int m2 = 2; *bp = m2 | *bp; }`                       26
 *     `*bp |= 2;`                                             26
 *     `*bp = *bp | 2;`                                        26
 *     `m2` declared at the top, assigned, then used           26
 *     the load named first (`u2 = *bp; *bp = 2 | u2;`)        26
 *     `a->f44` through a named `int` zero, block or top-level 26
 *     `a->f8` through a named temp at either 2008058 site     26
 *   and WORSE:
 *     `{ unsigned char m2 = 2; ... }`                         28
 *     anchor before or after `a->f44 = 0;`                    28
 *     anchor after the first 2008058                          29
 *     both 2008058 args through shared temps                  198 (+4 bytes)
 *
 * MEASURED AND CLOSED THIS BATCH (do not re-screen): autosweep BOTH AXES on the
 * pinned body.  Flags -- `-fno-gcse`, `-fno-strict-aliasing`,
 * `-fno-strength-reduce`, `-fno-rerun-cse-after-loop`, `-fno-rerun-loop-opt`
 * all EXACTLY INERT at 11; `-fno-schedule-insns2` 64, `-O1` 66,
 * `-ffixed-r7` 247.  Anchors -- all 40 boundaries inert or worse.
 *
 * The `= b->f55` readback at __Func_8093554 is SCAFFOLDING, not a pin, and it
 * stays: the old header measures the honest `__Func_8093554()[0x55] = 0;` at
 * 121 and +8 bytes, because gcc commons that zero with the earlier HALFWORD
 * `b->f64 = 0` and rematerialises it from a mid-function pool.
 *
 * ref: asm/overlays/rom_7f2f14/ovl_30_c_c_a_a_a_c.s -- the file's ONLY function,
 * no data; whole-TU conversion, no split.  overlay.ld:78 already names the
 * asm/ path and MUST STAY VERBATIM.
 */
/* OvlFunc_968_200a47c -- NON-MATCHING, 11 differing STREAM encodings of 273.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch_elev/b248/f200a47c/final.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 273 / ours 273 stream encodings; 238/238 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7f2f14/200a47c.c \
 *     asm/overlays/rom_7f2f14/ovl_30_c_c_a_a_a_c.s --func OvlFunc_968_200a47c
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
/* OvlFunc_968_200a47c -- PARKED at 11 differing encodings.
 * ref: asm/overlays/rom_7f2f14/ovl_30_c_c_a_a_a_c.s (the file's ONLY function)
 *
 *   XX ENCODINGS differ in 11 place(s) (ref 273, ours 273)
 *      first at index 179: ref 1c03  ours 1c06
 *   SIZE, INSTRUCTION COUNT and RELOCATIONS all exact (616 bytes, 273 encodings).
 *
 * MODEL: src/overlays/rom_7ed0a0/ovl_30_c_a_a.c :: OvlFunc_964_2009d04 is a
 * near-twin (same helper set, same swap-through-a-stack-struct shape, base slot
 * 0xc/bound 2 instead of 0xa/3).  Its C supplied the whole skeleton.
 *
 * LEVERS THAT SURVIVE, with the measurement each removal costs (from 273/273):
 *   spilled-local declaration order (e1, e2, tp)         6
 *   separate `flag`/`flag2` and `t`/`t3` variables       9
 *   the inner loop's body hoisted OUT OF LINE with a
 *     `goto swap` (see the finding below)               44
 *   `tgt` reusing the loop variable `j`                  7
 *   `i = 0;` before `tp = &tmp;`                         6
 *   pin r1=0 at the FIRST OvlFunc_968_2008058 site       2
 *   pin r0,r1 at the SECOND OvlFunc_968_2008058 site     2
 *   pin r5=0 for `a->f44 = 0`                            7
 *   pin r2=2 at the first `|= 2` site                    4
 *   the `= b->f55` readback at __Func_8093554 (SCAFFOLD) 121 (and +8 bytes)
 *
 * MEASURED WORSE / INERT (all from the same base):
 *   `while (j < i)`, `continue`-style loop, unsigned j     inert / worse
 *   `k = i` moved before the field stores                  inert
 *   literal 0 instead of `t3` in the a-block stores        inert
 *   b-block written with an explicit pointer chain         inert
 *   b-block zero pinned to r2 / r1 / r3                    16-20
 *   __Func_8093554 zero pinned to r0 / r1 / r2 / r3        13 / 92 / 92 / 92
 *   `__Func_8093554()[0x55] = t3`                          78
 *   `... = b->f14` / `= a->f55` / `= (uchar)e1`            126 / 135 / 99
 *   pinning __Func_80933d4's arguments                     inert
 *   narrow-local (`unsigned char m = 2`) at the first orr  2 worse than the pin
 *
 * THE REMAINING 11 ARE ONE BLOCKER, and it is a compiler behaviour, not an
 * allocation guess.  The honest source for the store after __Func_8093554 is
 * `__Func_8093554()[0x55] = 0;`.  Written that way gcc-2.96 commons that zero
 * with the earlier `b->f64 = 0` (a HALFWORD constant store) and then
 * rematerialises it after the two calls as `ldrh r5, .Ln` out of a MID-FUNCTION
 * pool -- `.word 0` plus a `b` around it, +8 bytes and +3 encodings, where the
 * ROM has a plain `mov r3, #0`.  Minimal repro in scratch_elev/b248/probe/.
 * The `= b->f55` readback in this file is scaffolding that dodges it at the
 * cost of holding b+0x55 in r6 across both calls, which is the whole 11.
 */
struct Sub {
    unsigned char pad0[9];
    unsigned char f9;
};

struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    int f14;
    unsigned char pad18[0x23 - 0x18];
    unsigned char f23;
    unsigned char pad24[0x28 - 0x24];
    int f28;
    unsigned char pad2c[0x3c - 0x2c];
    int f3c;
    unsigned char pad40[0x44 - 0x40];
    int f44;
    unsigned char pad48[0x50 - 0x48];
    struct Sub *f50;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[0x59 - 0x56];
    unsigned char f59;
    unsigned char pad5a[0x64 - 0x5a];
    unsigned short f64;
    unsigned char pad66[0x6c - 0x66];
    void *f6c;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __DeleteActor(void *e);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8092b08(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern unsigned char *__Func_8093554(void);
extern void *OvlFunc_968_2008058(int a, int b, int c, int d);
extern void OvlFunc_968_200894c(struct Actor *a);
extern void OvlFunc_968_200a2a4(void);
extern void OvlFunc_968_200a2c8(int n);
extern void OvlFunc_968_200a3d4(int slot);

void OvlFunc_968_200a47c(void)
{
    struct Actor tmp;
    void *e1;
    void *e2;
    struct Actor *tp;
    struct Actor *a;
    struct Actor *o;
    struct Actor *b;
    unsigned char *bp;
    unsigned char *bp2;
    unsigned short *p;
    int i;
    int j;
    int k;
    int who;
    int x;
    int z;
    int t;
    int t3;
    int n;
    int flag;
    int flag2;
    int flag3;

    e1 = 0;
    e2 = 0;
    __CutsceneStart();
    i = 0;
    tp = &tmp;
    who = 0xa;
loop:
    a = __MapActor_GetActor(who);
    x = a->f8 >> 20;
    if (x == 0xd) {
        z = a->f10 >> 20;
        if (z == 7) {
            flag = (0x80 << 2) + i;
            t = __GetFlag(flag);
            if (t == 0) {
                OvlFunc_968_200894c(a);
                __SetFlag(flag);
                bp = &a->f23;
                *bp = 2 | *bp;
                a->f59 = t;
                a->f55 = t;
                __Func_8010704(4, 0x13, 1, 1, x, z);
                goto done;
            }
        }
    }
    if ((a->f50->f9 & 0xc) == 0xc) {
        flag2 = (0x80 << 2) + i;
        if (__GetFlag(flag2) == 0) {
            __Func_8092b08(who, 1);
            a->f44 = 0;
            if ((a->f10 >> 20) <= 0xc) {
                e1 = OvlFunc_968_2008058(a->f8, 0, 0xe0 << 16, 0xfd);
                e2 = OvlFunc_968_2008058(a->f8, 0, 0xf0 << 16, 0xfd);
            }
            OvlFunc_968_200894c(a);
            __MapActor_SetPos(who, 0, 0);
            __DeleteActor(e1);
            __DeleteActor(e2);
            __SetFlag(flag2);
            goto done;
        }
    }
    goto b3;
swap:
    o = __MapActor_GetActor(j + 0xa);
    tp->f8 = a->f8;
    tp->fc = a->fc;
    tp->f10 = a->f10;
    a->f8 = o->f8;
    a->fc = o->fc;
    a->f10 = o->f10;
    o->f8 = tp->f8;
    o->fc = tp->fc;
    o->f10 = tp->f10;
    k = j;
    goto after;
b3:
    if ((a->f10 >> 20) != 0x13)
        goto next;
    t3 = __GetFlag((0x80 << 2) + i);
    if (t3 != 0)
        goto next;
    a->f3c = 0x80 << 24;
    a->f14 = t3;
    a->f28 = t3;
    a->f55 = t3;
    a->f64 = t3;
    k = i;
    for (j = 0; j < i; j++)
        if (__GetFlag((0x80 << 2) + j) == 0)
            goto swap;
after:
    j = k + 0xa;
    b = __MapActor_GetActor(j);
    b->f3c = 0x80 << 24;
    b->f14 = 0;
    b->f28 = 0;
    b->f55 = 0;
    b->f64 = 0;
    __Func_80933d4(0xc0 << 10, 0xc0 << 7);
    __Func_8093554()[0x55] = b->f55;
    __Func_80933f8(0x88 << 16, 0x80 << 12, 0xac << 17, 1);
    __Func_8093530();
    OvlFunc_968_200a3d4(j);
    if ((__MapActor_GetActor(j)->f8 >> 20) == 6) {
        p = &__MapActor_GetActor(8)->f64;
        n = *p;
        *p = n + 1;
        p = &__MapActor_GetActor(9)->f64;
        n = *p;
        *p = n - 1;
    } else {
        p = &__MapActor_GetActor(8)->f64;
        n = *p;
        *p = n - 1;
        p = &__MapActor_GetActor(9)->f64;
        n = *p;
        *p = n + 1;
    }
    __MapActor_GetActor(j)->f6c = (void *)OvlFunc_968_200a2a4;
    OvlFunc_968_200a2c8(0x28);
    bp2 = &__MapActor_GetActor(j)->f23;
    { unsigned char m = 2; *bp2 = m | *bp2; }
    __SetFlag((0x80 << 2) + k);
    goto done;
next:
    i++;
    who++;
    if (i <= 3)
        goto loop;
done:
    __CutsceneEnd();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_968_200a47c -- 0x0200a47c,
 * asm/overlays/rom_7f2f14/ovl_30_c_c_a_a_a_c.s
 *
 * ELEVEN differing encodings out of 273, at EXACT SIZE (616 bytes), exact
 * instruction count and EXACT RELOCATIONS.
 *      first at index 179: ref 1c03  ours 1c06
 * Candidate at scratch_elev/b248/f200a47c/final.c; d.sh / sweep.sh / pool.sh /
 * shape.sh in that directory rebuild any measurement below.
 *
 * LANDING SHAPE IF CLOSED: src/overlays/rom_7f2f14/ovl_30_c_c_a_a_a_c.c.
 * The .s holds ONE function and emits no .section/.data/.word/.byte, so the
 * whole TU is replaced -- no split, no linker edit. overlay.ld:78 already names
 * asm/overlays/rom_7f2f14/ovl_30_c_c_a_a_a_c.o(.text) and MUST STAY THAT WAY.
 * makefile_flags() is empty, WILDCARD_HITS empty, so default -O2.
 *
 * THE NEIGHBOUR HEURISTIC HELD DECISIVELY. neighbour.py ranked
 * src/overlays/rom_7ed0a0/ovl_30_c_a_a.c first at 11/18 shared symbols, and its
 * OvlFunc_964_2009d04 is a near-twin -- same swap through a stack `struct
 * Actor`, same __Func_80933d4/__Func_8093554/__Func_80933f8/__Func_8093530 run,
 * same f6c callback and SetFlag, differing only in base slot (0xc against 0xa),
 * loop bound (2 against 3) and one extra leading branch. Its struct and idiom
 * carried over verbatim and took the FIRST candidate to 180 differing.
 *
 * BLOCKER: gcc-2.96 THUMB NARROW-CONSTANT REMATERIALISATION, and the mechanism
 * is a HALFWORD STORE POISONING A LATER BYTE CONSTANT ACROSS A CALL.
 *
 * The honest source line is `__Func_8093554()[0x55] = 0;`. gcc commons that zero
 * with the earlier `b->f64 = 0` -- a HALFWORD store -- and, after the two
 * intervening calls, rematerialises it as `ldrh r5, .Ln` out of a MID-FUNCTION
 * POOL (`.word 0` plus a `b` around it, +8 bytes and +3 encodings) where the ROM
 * has a plain `mov r3, #0`.
 *
 * This is NOT the recorded halfword-pooling entry, which is about constants
 * `mov #imm8` cannot build. THE CONSTANT HERE IS ZERO. See the general entry in
 * docs/elevation.md ("A HALFWORD STORE POISONS A LATER NARROW CONSTANT ACROSS A
 * CALL") for the four-way discriminator and the minimal repro.
 *
 * Nothing at the C level breaks the commoning: int locals, unsigned char locals,
 * casts, reordering the stores, bitfields and separate pointer chains were all
 * tried and all still pool. The shipped candidate DODGES it with a `= b->f55`
 * readback, which costs a different thing -- it pins b+0x55 in r6 across both
 * calls -- and that readback is the entire remaining 11.
 *
 * LEVERS THAT ARE LOAD-BEARING (figure = what REMOVING it costs from 11/273):
 *      spilled-local declaration order (e1, e2, tp)             6
 *      separate flag/flag2 and t/t3 variables                   9
 *      inner-loop body hoisted out of line via `goto swap`      44
 *      tgt reusing the loop variable j                          7
 *      `i = 0;` placed before `tp = &tmp;`                      6
 *      pin r1 = 0 at the first OvlFunc_968_2008058              2
 *      pin r0, r1 at the second OvlFunc_968_2008058             2
 *      pin r5 = 0 for `a->f44 = 0`                              7
 *      pin r2 = 2 at the first `|= 2` site                      4
 *      `= b->f55` readback at __Func_8093554 (SCAFFOLD)       121, +8 bytes
 *
 * MEASURED WORSE OR INERT: `while (j<i)` and continue-style loops inert;
 * hand-rolled goto loop 199; `k = i` moved before the field stores inert;
 * literals instead of t3 in the a-block inert; explicit pointer chain in the
 * b-block inert; b-block zero pinned to r2/r1/r3 16-20; the __Func_8093554 zero
 * pinned to r0/r1/r2/r3 13/92/92/92; `= t3` 78; `= b->f14` 126; `= a->f55` 135;
 * `= (unsigned char)e1` 99; pinning __Func_80933d4's arguments inert.
 *
 * NOTE FOR THE RECORDED `orr` NARROW-LOCAL LEVER: it was BEATEN HERE by an
 * eviction pin. `unsigned char m = 2` at the first orr is 2 WORSE than the
 * r2 pin. The lever is not wrong, but it is not automatically the best
 * spelling at an orr site.
 *)

*/
#endif
