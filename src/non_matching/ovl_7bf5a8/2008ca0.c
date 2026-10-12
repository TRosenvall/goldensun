/* OvlFunc_935_2008ca0 -- NON-MATCHING, 30 differing STREAM encodings of 379
 * at PRODUCTION FLAGS, and 23 of 379 under ALIAS_CFLAGS.  PIN-FREE.
 * Batch 343 brief E.
 *
 * THE FIGURE ABOVE LEADS WITH 30 BECAUSE THAT IS WHAT THE PLAIN `Verify with:`
 * RECIPE BELOW REPRODUCES, and a park whose stated figure its own recipe cannot
 * reproduce is what parkcheck exists to catch.  The 23 is real and is the
 * number worth acting on, but it is FLAG-CONDITIONAL and the flag rule is not
 * installed for a src/non_matching path -- nothing builds this file yet.
 * WHOEVER LANDS THIS MUST ADD THE ALIAS_CFLAGS RULE; see below.
 *
 * RE-DERIVED batch 343: 276 = 276 16-bit instructions (IN PHASE, so a true
 * DISTANCE), size exact.  Relocations differ by FOUR WORDS and that difference
 * is ADMISSIBLE, checked not assumed: the extra symbols are `_AREA_60` (twice),
 * `_AREA_61`, `_AREA_62`, absolute symbols defined BY VALUE at area.sym:99-101
 * (`_AREA_60 = 0x60;`) and already used by landed files, e.g.
 * src/overlays/rom_7b4558/ovl_30_c_c_c_a_c.c.  They link to the literal; the
 * ROM pools the same words.
 *
 * NEEDS ALIAS_CFLAGS.  This body is byte-for-byte the park's, with its two
 * PIN3 blocks folded back into plain calls, and it measures 23 ONLY under
 * -fno-strict-aliasing.  ALIAS_CFLAGS is an existing per-file group
 * (Makefile:235) with precedent at Makefile:252/257/268/275/288/313.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7bf5a8/2008ca0.c \
 *     asm/overlays/rom_7bf5a8/ovl_b8c_c_c.s --func OvlFunc_935_2008ca0
 *   (production flags give 30; the 23 needs ALIAS_CFLAGS -- the installed
 *    Makefile rule supplies it, so `make compare` is the real check.)
 *
 * ========== THE LADDER, ALL MEASURED THIS BATCH ==========
 *   park body, 6 pins, production flags            24 of 379   (installed before)
 *   park body, 6 pins, -fno-strict-aliasing        17 of 379   <- PINNED, not shipped
 *   THIS body, 0 pins, production flags            30 of 379
 *   THIS body, 0 pins, -fno-strict-aliasing      **23 of 379** <- SHIPPED
 * So the six pins are worth 7 encodings, and the flag is worth 7 on EITHER
 * body.  Dropping all six pins and adding the flag is ONE BETTER than the
 * pinned park was, which is why this is the body that gets installed: fewer
 * devices AND a lower figure.  The pinned 17 is recorded as a figure ABOUT THE
 * PINS and is left for pass 3 (reports/pass3-depin.md).
 *
 * THE FLAG RELOCATES THE RESIDUE, which is the useful part.  `first` moves from
 * index 4 to index 105: -fno-strict-aliasing removes the WHOLE early cluster
 * around the opening `*(int *)(iwram_3001ebc + (0xe0 << 1))` store and the
 * `*(short *)(g + ...)` reads that follow it.  Those go through
 * `extern unsigned char *iwram_3001ebc;` and `unsigned char *g`, so with strict
 * aliasing on, gcc takes the int store and the short reads not to alias.  The
 * pass-3 question is therefore whether a HONEST TYPE (a struct, or a union
 * member) reaches the same place from source and retires the flag.
 *
 * AUTOSWEEP, RECORDED NEGATIVE (batch 343, on the PINNED body): flag axis found
 * -fno-strict-aliasing 24 -> 17 and every other group inert at 24 except -O1
 * (362) and -fno-schedule-insns2 (79).  On THIS device-free body the whole flag
 * axis was re-crossed: only -fno-strict-aliasing moves it (30 -> 23); -fno-gcse,
 * -ffixed-r7, -fno-strength-reduce, -fno-rerun-cse-after-loop and
 * -fno-rerun-loop-opt are all inert at 30, -fno-schedule-insns2 is 79, -O1 362.
 * The anchor axis was run to a CAP OF 40 of 97 boundaries and is NOT closed;
 * boundary 1 (after the opening iwram store) was 22 on the pinned body.
 */

/* ===== THE ORIGINAL PARK HEADER, CARRIED FORWARD ===== */
/* OvlFunc_935_2008ca0 -- NON-MATCHING, 24 encodings of 379 against the tree
 * reference; 20 against a symbolised copy, the four-word difference being the
 * already-provisioned _AREA_60/_AREA_61/_AREA_62 pool words.  SIZE EXACT.  345
 * instructions.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7bf5a8/2008ca0.c \
 *     asm/overlays/rom_7bf5a8/ovl_b8c_c_c.s --func OvlFunc_935_2008ca0
 *
 * ================================================================
 * IT NEEDS A DATA DECISION, AND THE CHOICE IS THE MAINTAINER'S
 * ================================================================
 *
 * datacheck.py reports `.section .data` in this stem, exporting
 * gScript_935__02009884.  The blob is FOUR BYTES, `1b 00 00 00`, at
 * overlays/rom_7bf5a8/orig.bin offset 0x1884, and it is consumed by the
 * ALREADY-LANDED src/overlays/rom_7bf5a8/ovl_b8c_a.c:58 as
 * `extern unsigned char gScript_935__02009884[]` passed to __Actor_SetScript.  Its
 * .data line is overlays/rom_7bf5a8/overlay.ld:79.
 *
 *   REHOME (recommended, and the precedent).  asm/overlays/rom_7ac2d8/ovl_314_c_c.s
 *   is exactly this shape: a data-only .s keeping `.section .data`, the `.global` and
 *   the `.incbin`.  Cost is one new data-only .s, its .data line, and repointing the
 *   .text line to src/.  This KEEPS THE ASSERTION that the blob is an opaque ROM
 *   slice, which is what a script blob is.
 *
 *   EMIT FROM C.  `unsigned char gScript_935__02009884[4] = { 0x1b, 0, 0, 0 };` lands
 *   in .data (non-const), so both linker lines repoint to the same object and no new
 *   file is needed.  It is one readable word, inside docs/elevation.md 21174's
 *   allowance -- but it ASSERTS SOURCE-AUTHORED DATA, which for a script blob is a
 *   claim rather than a formatting choice.
 *
 * The function measurement is independent of which is chosen.
 *
 * ================================================================
 * TWO FINDINGS, one of which corrects a rule in the brief
 * ================================================================
 *
 * `blt #5` IS SWITCH LOWERING, NOT AN IF-CHAIN -- AND THE SIGNAL IS THE CONSTANT,
 * NOT THE POLARITY.  gcc's `fold` canonicalises both `v >= 5` and `v < 5` to the
 * `<= 4` form, so AN IF-CHAIN CAN ONLY EVER EMIT `cmp #4 / ble`.  The ROM's
 * `cmp #5 / blt` comes from emit_case_nodes, which tests the real case bounds.  So
 * A RANGE TEST WHOSE COMPARE CONSTANT IS THE LOW BOUND OF THE RANGE IS A `switch`,
 * WHATEVER THE POLARITY.  That refines the recorded branch-polarity rule, which
 * reads on its own as though polarity settles it.  Rewriting the [5,6] guard as
 * `case 5: case 6:` was part of a 90 -> 32 step.
 *
 * `*p |= CONST` WITH A LITERAL BEATS A NAMED CARRIER HERE, which is the INVERSE of
 * the stack-argument rule and therefore genuinely per-site.  `m = 0x80;
 * *p = *p | m;` emits four instructions; `*p |= 0x80;` at both sites emits the ROM's
 * three, because CSE substitutes the shared register INTO the `ior` while leaving the
 * load as operand 0, whereas a user variable is already operand 0 and forces a copy.
 * Worth 58 encodings combined with the switch.  Compare
 * src/overlays/rom_7b4558/ovl_30_c_c_c_a_c.c, where the same `|=` shape needed a
 * QImode carrier -- the discriminator there was the carrier's MODE, not its presence.
 *
 * The HImode int-carrier rule applied again in the tail: `*(short *)(g + 0x242) = 0xa;`
 * pooled as HImode and forced a dump before the epilogue; `ten = 0xa; *hp = ten;`
 * gave `mov r3, #0xa` and took the instruction count from 382 to exactly 379.
 * Dropping that one carrier costs 16 encodings and +8 bytes.
 *
 * `_AREA_60`, `_AREA_61`, `_AREA_62` are already in area.sym -- nothing to add.
 * No per-file Makefile flag override applies to this stem.
 */

extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;
extern int _AREA_60;
extern int _AREA_61;
extern int _AREA_62;

extern int __GetFlag(int id);
extern void __ClearFlag(int id);
extern void __WaitFrames(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);
extern int __StartTask(void (*fn)(void), int n);
extern void OvlFunc_935_2008398(void);
extern void OvlFunc_935_2008410(void);
extern void OvlFunc_935_200850c(void);
extern void OvlFunc_935_2008554(void);
extern void OvlFunc_935_20085a0(void);
extern void OvlFunc_935_20085ec(void);
extern void OvlFunc_935_2008640(void);
extern void OvlFunc_935_2008690(void);
extern void OvlFunc_935_20086e4(void);
extern void OvlFunc_935_2008c08(void);
extern void OvlFunc_935_2008c50(void);

int OvlFunc_935_2008ca0(void)
{
    unsigned char *g;
    unsigned char *p;
    int m;
    int two;
    int one;
    int sa;
    int sb;
    int e5c;
    int e1b;
    int e13;
    int e5b;
    int e19;
    int f5;
    int f49;
    int g6;
    int gc;
    int gb;
    int v;
    int ten;
    short *hp;

    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x81 << 2;
    g = gState;
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_60)) {
        switch (*(short *)(g + (0xe1 << 1))) {
        case 5:
        case 6:
        case 7:
        case 8:
        case 0xd:
            if (__GetFlag(0x9a8) == 0) {
                sa = 0x15;
                sb = 0x1d;
                __Func_8010704(0x16, 0x1d, 1, 1, sa, sb);
            } else {
                e5c = 0x5c;
                e1b = 0x1b;
                __Func_80105d4(0x6c, 0x1b, 1, 1, e5c, e1b);
                __WaitFrames(1);
                e13 = 0x13;
                e5b = 0x5b;
                __Func_80105d4(0x13, 0x53, 0xf, 8, e13, e5b);
                __WaitFrames(1);
                e19 = 0x19;
                __Func_80105d4(2, 0x18, 1, 2, e19, e1b);
            }
            __Func_800fe9c();
            __WaitFrames(1);
            g = gState;
            break;
        case 0xa:
            __ClearFlag(0x9a8);
            break;
        }
    }
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_61)) {
        if (__GetFlag(0x300) == 0)
            *(int *)(__MapActor_GetActor(0x16) + 0x1c) = 0xc0 << 9;
        switch (*(short *)(g + (0xe1 << 1))) {
        case 1:
        case 2:
        case 3:
        case 4:
            if (__GetFlag(0x9a8) == 0) {
                f5 = 5;
                f49 = 0x49;
                __Func_80105d4(5, 0x51, 0xb, 7, f5, f49);
            } else {
                g6 = 6;
                gc = 0xc;
                __Func_8010704(5, 0xc, 1, 1, g6, gc);
                gb = 0xb;
                __Func_8010704(0xc, 0xa, 1, 1, gc, gb);
            }
            break;
        case 8:
        case 9:
        case 0xe:
            OvlFunc_935_2008c08();
            if (__GetFlag(0x200) != 0) {
                __MapActor_SetAnim(0x10, 5);
                OvlFunc_935_200850c();
            }
            if (__GetFlag(0x201) != 0) {
                __MapActor_SetAnim(0x11, 5);
                OvlFunc_935_2008554();
            }
            if (__GetFlag(0x202) != 0) {
                __MapActor_SetAnim(0x12, 5);
                OvlFunc_935_20085a0();
            }
            if (__GetFlag(0x203) != 0) {
                __MapActor_SetAnim(0x13, 5);
                OvlFunc_935_20085ec();
            }
            if (__GetFlag(0x204) != 0) {
                __MapActor_SetAnim(0x14, 5);
                OvlFunc_935_2008640();
            }
            if (__GetFlag(0x205) != 0) {
                __MapActor_SetAnim(0x15, 5);
                OvlFunc_935_2008690();
            }
            __StartTask(OvlFunc_935_20086e4, 0xc8 << 4);
            break;
        case 0xa:
        case 0xb:
            if (__GetFlag(0x9a9) != 0) {
                OvlFunc_935_2008398();
                __MapActor_SetPos((9), ((0xf8) << 16), ((0xdb) << 18));
            }
            __MapActor_GetActor(8)[0x23] = 2;
            break;
        }
        __MapActor_SetAnim(8, 2);
        __MapActor_SetAnim(9, 2);
        __Actor_SetSpriteFlags(__MapActor_GetActor(8), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(9), 0);
        __MapActor_GetActor(9)[0x59] = 1;
        g = gState;
    }
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_62)) {
        __MapActor_SetAnim(8, 2);
        if (__GetFlag(0x207) == 0)
            __MapActor_SetAnim(0xa, 2);
        __Actor_SetSpriteFlags(__MapActor_GetActor(8), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xa), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(9), 0);
        p = __MapActor_GetActor(0xa) + 0x59;
        *p |= 0x80;
        p = __MapActor_GetActor(9) + 0x59;
        *p |= 0x80;
        switch (*(short *)(g + (0xe1 << 1))) {
        case 5:
        case 6:
            OvlFunc_935_2008c50();
            two = 2;
            __MapActor_GetActor(0xb)[0x59] = two;
            __MapActor_GetActor(0xc)[0x59] = two;
            __MapActor_GetActor(0xd)[0x59] = two;
            __MapActor_GetActor(0xe)[0x59] = two;
            one = 1;
            __MapActor_GetActor(8)[0x59] = one;
            __MapActor_GetActor(0xa)[0x59] = one;
            __MapActor_GetActor(9)[0x59] = one;
            if (__GetFlag(0x9aa) != 0) {
                OvlFunc_935_2008410();
                __MapActor_SetPos((0xa), ((0x84) << 17), ((0xcc) << 16));
            }
            break;
        }
    }
tail:
    hp = (short *)(g + 0x242);
    ten = 0xa;
    *hp = ten;
    hp = (short *)(g + (0x90 << 2));
    *hp = (int)(&_AREA_60);
    return 0;
}
