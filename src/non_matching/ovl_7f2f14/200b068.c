/* OvlFunc_968_200b068 -- NON-MATCHING, 998 of 1586 encodings differ.
 * Unattempted before batch 313.  Reference asm/overlays/rom_7f2f14/ovl_30_c_c_a_c_a_a.s.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7f2f14/200b068.c \
 *     asm/overlays/rom_7f2f14/ovl_30_c_c_a_c_a_a.s --func OvlFunc_968_200b068
 *
 * READ THE FIGURE CORRECTLY.  998 IS NOT A DISTANCE.  objcmp's count is
 * index-by-index with no alignment and our encoding count is 1590 against the
 * reference's 1586, so every index after the first insertion differs.  The
 * ranking instrument here is aligncmp: ALIGNED-EQUAL 1355 of 1586 = 85.4%,
 * 287 differing/ins/del in 155 hunks.  objcmp: SIZE ref 3964 / ours 3972,
 * ENCODINGS ref 1586 / ours 1590, RELOCATIONS differ.
 *
 * WHAT IS ALREADY EXACT, and it is the part that says the program is right:
 *   * THE CALL MULTISET IS EXACT -- 224 named calls against 224, every target
 *     at the identical count.  That was the last structural defect fixed (see
 *     lever 9) and it is the strongest evidence the reconstruction is the ROM's
 *     program and not merely the ROM's shape.
 *   * RUNG 8 PER-OPCODE HISTOGRAM, ref 1431 instructions against ours 1435:
 *         b +3   bl -2   ldrsh +2   mov +1   add +1   sub -1
 *     The b/bl pair is NOT a defect: 24 of the reference's `bl` are LONG
 *     BRANCHES to local labels (`bl .L3fa6`), and two of ours fell inside the
 *     +/-256-byte conditional range and came out as `b`.  That is downstream of
 *     layout, not of the source.
 *   * THE DISPATCH IS EXACT: 5 dispatch sites (`grep -cE '^\t(mov|ldr|add)\tpc'`)
 *     and 5 jump tables of 26 / 21 / 11 / 17 / 20 entries in both.
 *
 * INSTALL SHAPE.  `split_s.py --dry-run` says "holds only OvlFunc_968_200b068
 * and no data; convert it directly, NO SPLIT NEEDED".  `datacheck.py` prints
 * nothing and exits 0; the .s has zero `.section` / `.incbin` / `.lcomm`.  So a
 * landing is a WHOLE-FILE conversion to
 * src/overlays/rom_7f2f14/ovl_30_c_c_a_c_a_a.c with NO new exports and no
 * linker-script change.  `shimcount.py` reports 2 register pins (the PINXZ
 * macro, used at 11 sites) and flags the missing fakematch.txt row -- note the
 * near-twin src/overlays/rom_7f2f14/ovl_30_c_c_a_a_c_a_a.c is LANDED EXACT with
 * 22 pins and DOES carry three fakematch.txt rows, so the precedent is settled.
 *
 * FRAME, by the four greps.  `sub sp,#0xc` -- 12 bytes, and all of it is
 * accounted for: sp+0 and sp+4 are OUTGOING ARGUMENT SPACE (59 `str rX,[sp]`
 * with no matching load, the most of any function in this brief, all of them
 * the 5th argument of a six-argument call), and sp+8 is THE FUNCTION'S ONLY
 * SPILL SLOT -- one `str r4,[sp,#8]` / `ldr r4,[sp,#8]` pair bracketing the
 * __GetFlag call inside the 0xba case-1/2 search loop.  ZERO stack aggregates:
 * `mov rX,sp` = 0 and `add rX,sp,#K` = 0.  The 59 sp0 stores made this look
 * like the heaviest frame in the brief and it is in fact the lightest.
 *
 * WHAT IT DOES.  The cutscene/state entry point for map areas 0xb5..0xba.  It
 * waits one frame, runs OvlFunc_968_2008558 if save bit 0x109 is set, sets save
 * bit 0x110, writes 0x204 to the state word at [iwram_3001ebc]+0x1C0, then
 * dispatches on the AREA ID (the signed halfword at gState+0x1C0) through SIX
 * `if`/`else if` arms, and inside five of those on the SUB-AREA ID at
 * gState+0x1C2 through a jump table.  The arms re-place and re-dress map actors
 * (`__MapActor_SetPos` / `__MapActor_SetAnim` / `__Func_8092b08`), stamp tiles
 * (`__CopyMapTiles`, `__Func_8010704`, `__Func_80105d4`), start two tasks, and
 * hand off to one of ten sibling cutscene routines.  It always returns 0.
 *
 * THE OUTER SIX-WAY IS AN `if`/`else if` CHAIN, NOT A SWITCH, and the evidence
 * is the pooled comparison constant: the ROM does `ldr r3,=0xb5 / cmp r2,r3` at
 * every one of the six, where a literal would assemble as `cmp r2,#0xb5` (181
 * fits the 8-bit immediate -- our first draft emitted exactly that).  That is
 * the tree's solved AREA-ID idiom: `st == (int)(&_AREA_b5)`, and all six
 * symbols _AREA_b5 .. _AREA_ba are ALREADY in area.sym (lines 170-174 and 208),
 * so NO .sym addition is needed.  The near-twin OvlFunc_968_200af8c
 * (src/overlays/rom_7f2f14/ovl_30_c_c_a_a_c_a_c_b.c) is the same six-way on the
 * same halfword and supplied the spelling.  Six dense values would have been a
 * TABLE as a switch (count 6, range 5, threshold 5), so the chain is not a
 * compiler choice -- it is the source.
 *
 * LEVERS, MEASURED, in the order applied, by aligncmp percentage:
 *   1. first draft, structure read straight off the .s          68.2%
 *      (size +40, encodings +22)
 *   2. _AREA_* pooled comparisons AND the `(unsigned int)0`
 *      short-read idiom, applied together                       67.9%
 *      -- a NET NEGATIVE as a pair, and worth recording as such: the _AREA_
 *      half is certainly right (it is the only thing that produces the pooled
 *      `cmp r2,r3`), so the loss was entirely the second half being applied
 *      before lever 3 existed.
 *   3. `int *p = (int *)(iwram_3001ebc + k); *p = ...`
 *      instead of `*(int *)(iwram_3001ebc + k) = ...`           75.0%  (+7.1)
 *      THE SINGLE BIGGEST LEVER ON THIS FUNCTION, 112 encodings.  With `k` a
 *      live variable the store compiles to `str r3,[r0,r1]` (register offset)
 *      and `k` is driven into r8; through a declared pointer it is the ROM's
 *      `str r3,[r0,#0]`, and `k` dies immediately.  The ROM reuses that one
 *      pointer for the second store in the 0xb5 arm, which is why it must be a
 *      variable and not a repeated expression.
 *   4. a local pair per 5th/6th stack argument, 61 SITES        79.8%  (+4.8)
 *      This is the landed twin's lever verbatim ("a separate local pair per
 *      __Func_8010704 stack-argument site").  Without it gcc reuses ONE
 *      register for both outgoing stack words; the ROM holds both at once
 *      (`mov r3,#7 / mov r2,#0x10 / str r3,[sp] / str r2,[sp,#4]`).
 *   5. `__MapActor_GetActor(n)->field = v` -- NO local -- wherever
 *      only ONE field is written through the returned pointer,
 *      plus a `goto` tail in place of a duplicated tail call    82.1%  (+2.3)
 *      Read off `adds r0,#0x55 / strb r5,[r0]`: the ROM writes through r0 and
 *      discards it.  Assigning to a local costs two register copies per site.
 *      Where TWO OR MORE fields are written the ROM does keep r7, so the local
 *      is right there -- the discriminator is the number of field writes.
 *      This took the counts from +20 encodings to -7, i.e. straight through
 *      exact, which is why lever 6 reads as a regression on count and a gain
 *      on alignment.
 *   6. `k2 = 0xe1 << 1; g2 = (unsigned char *)&gState + k2;`
 *      -- the offset as a VARIABLE                              82.8%  (+0.7)
 *      With the literal, gcc folds the whole address into ONE pool word with a
 *      relocation addend (`.word gState+0x1c2`) and the read is three
 *      instructions; the ROM loads `=gState` and adds the offset at runtime,
 *      six.  8 sites.  A symbol+addend pool word is the tell.
 *   7. PINXZ -- `register int qx __asm__("r1")` / `qz __asm__("r2")`
 *      on the 11 `__MapActor_SetPos` sites whose shifted
 *      constants repeat inside one block                        84.0%  (+1.2)
 *      Same mechanism the landed twin records for __Func_8012330: the ROM
 *      REBUILDS `0xc2 << 18` at each of three call sites (`mov r1,#0xc2 /
 *      lsl r1,#18`), gcc CSEs it into a callee-saved register and copies.
 *   8. a bare FALLTHROUGH from each `OvlFunc_968_20087d8()` arm
 *      into the `__Func_8091ff0(0xaa)` arm of the same switch    85.4%  (+1.4)
 *      AND THIS IS THE ONE THAT MADE THE CALL MULTISET EXACT.  Written as two
 *      self-contained arms the two bodies are textually identical, jump.c
 *      cross-jumps them into one, and the object ends up with TWO `bl
 *      OvlFunc_968_20087d8` where the ROM has THREE.  Spelled as a fallthrough
 *      the `bl` block is followed by a fallthrough at RTL generation, the
 *      second-level merge does not happen, and the reference's three survive.
 *      A missing CALL is visible in the call multiset and in nothing else --
 *      size, encoding count and the opcode histogram all absorbed it.
 *
 * MEASURED INERT -- EXACTLY inert, every figure identical to the digit.  Do not
 * re-try (3 builds):
 *   * `t = v - 0x12; if (t <= 1)` as an explicit HImode temp, and
 *     `if ((unsigned short)(v - 0x12) <= 1)` as an explicit cast, in place of
 *     `if (v == 0x12 || v == 0x13)`.  All three fold to the same RTL.
 *   * moving `g2` after `i` in the declaration list.
 *   * swapping the `i` and `st` declaration order.
 *   The last two matter as a bound: this function's residue is NOT a
 *   declaration-order problem, so the allocno_compare lever has nothing to
 *   grip here.
 *
 * THE RESIDUE, NAMED.  Four instructions and 287 aligned differences, in three
 * kinds, none of them a wrong program:
 *   (1) `ldrsh +2` -- the 0xba sub-area 3..12/20 arm's exit test.  The ROM does
 *       `ldrh r2,[r5,#0] / mov r3,r2 / sub r3,#18 / lsl r3,#16` -- the subtract
 *       in HImode, then the shift.  gcc re-associates to `(v << 16) + K` with K
 *       pooled, and because the zero-extension is dead after `<< 16` it then
 *       picks `ldrsh`, which in Thumb-1 has NO immediate-offset form and so
 *       costs an extra `mov rZ,#0`.  Three spellings probed, all inert -- this
 *       is a combine association with no source handle found.
 *   (2) scheduling and allocator permutations: `adds r3,r3,r2` against
 *       `adds r5,r3,r2` for the gState scratch, r5 against r6 for the search
 *       loop counter, and one `str rX,[r7,#12]` that the ROM emits after the
 *       following `mov r0,#15`.  Compiler temps with no declaration to pin --
 *       which the two inert reorders above independently confirm.
 *   (3) `b`/`bl` long-branch encoding, explained above and not a defect.
 * *
 * THREE INSTRUMENT CHECKS RUN ON THIS FUNCTION, all clean, all recorded because
 * each one is a way the figures above could have been wrong:
 *   * `.call_via` sites: ZERO (`grep -cE 'call_via'`).  The macro expands to
 *     `mov r12,pc` + `bx`, so a histogram over a RAW reference that has veneer
 *     sites under-counts `mov` and `bx` by one each.  With none here the
 *     RUNG-8 histogram above is trustworthy as taken.  (Func_80bbb0c has one
 *     such site and LuckyDiceMain has two -- theirs are not.)
 *   * sp0 PAIRED, not just counted.  [sp] is 59 stores / 0 loads and [sp,#4] is
 *     59 stores / 0 loads -- genuine outgoing argument space.  [sp,#8] is 1
 *     store / 1 load -- a spill slot.  The raw store count is only a screen;
 *     the pairing is the discriminator, and here it separates the two kinds
 *     cleanly.
 *   * LOOP CENSUS BY BACKWARD EDGE, not by mnemonic.  Classifying every branch
 *     whose target is at a lower address gives EXACTLY ONE backward edge in
 *     1,431 instructions, and it closes on `bls` -- unsigned.  So
 *     `for (i = 0; i <= 3; i++)` with `unsigned int i` is the right loop form
 *     and there is nothing else to get wrong.  The raw mnemonic census would
 *     have been actively misleading: the function carries 20 `beq`, 13 `bne`,
 *     6 `bls`, 1 `bhi` and 40 `cmp`, and all but one are dispatch guards, flag
 *     tests or field compares -- NOT loop closures.

 * This stem matches only the generic `asm/%.o: src/%.c` Makefile rule -- no O1,
 * no per-file flag group -- so every figure above is a PRODUCTION-FLAG figure
 * at the tree default -O2.
 */
#include "gba/types.h"
#include "gba/io.h"

struct Actor {
    unsigned char pad00[8];
    int f08;
    int f0c;
    int f10;
    unsigned char pad14[0x23 - 0x14];
    unsigned char f23;
    unsigned char pad24[0x3c - 0x24];
    int f3c;
    unsigned char pad40[0x55 - 0x40];
    unsigned char f55;
    unsigned char pad56[3];
    unsigned char f59;
    unsigned char pad5a[0x64 - 0x5a];
    unsigned short f64;
    unsigned char pad66[0x6c - 0x66];
    void *f6c;
};

extern unsigned char *iwram_3001ebc;

typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_b5;
extern int _AREA_b6;
extern int _AREA_b7;
extern int _AREA_b8;
extern int _AREA_b9;
extern int _AREA_ba;

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);

#define PINXZ register int qx __asm__("r1"); register int qz __asm__("r2")
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8011ae0(void);
extern void __Func_8091494(int n);
extern void __Func_8091ff0(int n);
extern void __Func_8092b08(int slot, int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern int __StartTask(void *f, int n);

extern void OvlFunc_968_2008058(int a, int b, int c, int d);
extern void OvlFunc_968_2008558(void);
extern void OvlFunc_968_20087d8(void);
extern void OvlFunc_968_20088c8(void);
extern void OvlFunc_968_200894c(struct Actor *a);
extern void OvlFunc_968_20089c8(void);
extern void OvlFunc_968_2008b98(void);
extern void OvlFunc_968_20098f8(void);
extern void OvlFunc_968_20099c0(void);
extern void OvlFunc_968_2009a14(struct Actor *a);
extern void OvlFunc_968_2009d48(void);
extern void OvlFunc_968_2009f60(void);
extern void OvlFunc_968_200a2c8(int n);
extern void OvlFunc_968_200ab14(void);
extern void OvlFunc_968_200b00c(int n);
extern void OvlFunc_968_200b050(void);
extern void OvlFunc_968_200c2bc(void);
extern void OvlFunc_968_200c5f0(void);
extern void OvlFunc_968_200c600(void);
extern void OvlFunc_968_200c610(void);
extern void OvlFunc_968_200c7c0(void);
extern void OvlFunc_968_200ca2c(void);

int OvlFunc_968_200b068(void)
{
    struct Actor *a;
    struct Actor *b;
    struct Actor *c;
    unsigned char *g2;
    int *p;
    unsigned int k;
    unsigned int k2;
    int st;
    unsigned int i;
    int x;
    int y;
    unsigned short v;
    unsigned short t;
    int e1, f1, e2, f2, e3, f3, e4, f4, e5, f5, e6, f6, e7, f7, e8, f8, e9, f9, e10, f10, e11, f11, e12, f12, e13, f13, e14, f14, e15, f15, e16, f16, e17, f17, e18, f18, e19, f19, e20, f20, e21, f21, e22, f22, e23, f23, e24, f24, e25, f25, e26, f26, e27, f27, e28, f28, e29, f29, e30, f30, e31, f31, e32, f32, e33, f33, e34, f34, e35, f35, e36, f36, e37, f37, e38, f38, e39, f39, e40, f40, e41, f41, e42, f42, e43, f43, e44, f44, e45, f45, e46, f46, e47, f47, e48, f48, e49, f49, e50, f50, e51, f51, e52, f52, e53, f53, e54, f54, e55, f55, e56, f56, e57, f57, e58, f58, e59, f59, e60, f60, e61, f61;

    __CutsceneWait(1);
    if (__GetFlag(0x109))
        OvlFunc_968_2008558();
    __SetFlag(0x88 << 1);
    k = 0xe0 << 1;
    p = (int *)(iwram_3001ebc + k);
    *p = 0x81 << 2;
    st = *(short *)((unsigned char *)&gState + k);
    if (st == (int)(&_AREA_b5)) {
        *p = 0x80 << 1;
        if (__GetFlag(0x981) == 0) {
            OvlFunc_968_200b00c(8);
        } else {
            e1 = 7;
            f1 = 0x10;
            __Func_8010704(7, 0x11, 2, 1, e1, f1);
        }
        OvlFunc_968_200b00c(9);
        OvlFunc_968_200b00c(0xa);
        OvlFunc_968_200b00c(0xb);
        __Func_8092b08(0xb, 2);
        OvlFunc_968_200b00c(0xc);
        __Func_8092b08(0xc, 2);
        OvlFunc_968_200b00c(0xd);
        OvlFunc_968_200b00c(0xe);
    } else if (st == (int)(&_AREA_b6)) {
        k2 = 0xe1 << 1;
        g2 = (unsigned char *)&gState + k2;
        switch (*(short *)(g2 + (unsigned int)0)) {
        case 1:
        case 2:
            OvlFunc_968_200b00c(8);
            break;
        case 5:
            __ClearFlag(0x90 << 1);
            /* fallthrough */
        case 3:
        case 4:
        case 6:
            OvlFunc_968_200b00c(9);
            break;
        case 20:
        case 21:
            if (__GetFlag(0x982)) {
                e2 = 7;
                f2 = 8;
                __CopyMapTiles(0x67, 0x1b, 0x59, 0x1b, e2, f2);
                e3 = 3;
                f3 = 2;
                __CopyMapTiles(0x29, 0x5a, 0x1b, 0x5c, e3, f3);
                e4 = 3;
                f4 = 2;
                __CopyMapTiles(0x29, 0x5a, 0x1d, 0x5d, e4, f4);
                e5 = 3;
                f5 = 2;
                __CopyMapTiles(0x29, 0x5a, 0x1b, 0x5e, e5, f5);
                e6 = 3;
                f6 = 2;
                __CopyMapTiles(0x29, 0x5a, 0x1b, 0x60, e6, f6);
                e7 = 3;
                f7 = 2;
                __CopyMapTiles(0x29, 0x5a, 0x1d, 0x61, e7, f7);
                e8 = 3;
                f8 = 2;
                __CopyMapTiles(0x29, 0x60, 0x19, 0x5b, e8, f8);
                e9 = 3;
                f9 = 2;
                __CopyMapTiles(0x29, 0x5c, 0x19, 0x5d, e9, f9);
                e10 = 3;
                f10 = 2;
                __CopyMapTiles(0x29, 0x60, 0x19, 0x5f, e10, f10);
                e11 = 3;
                f11 = 2;
                __CopyMapTiles(0x29, 0x60, 0x19, 0x61, e11, f11);
                e12 = 3;
                f12 = 2;
                __CopyMapTiles(0x29, 0x60, 0x1b, 0x60, e12, f12);
                e13 = 3;
                f13 = 2;
                __CopyMapTiles(0x29, 0x60, 0x1d, 0x61, e13, f13);
            } else {
                if (__GetFlag(0x983) == 0)
                    break;
                e14 = 7;
                f14 = 8;
                __CopyMapTiles(0x6f, 0x1b, 0x59, 0x1b, e14, f14);
                e15 = 3;
                f15 = 2;
                __CopyMapTiles(0x29, 0x5a, 0x19, 0x5b, e15, f15);
                e16 = 3;
                f16 = 2;
                __CopyMapTiles(0x29, 0x5a, 0x19, 0x5d, e16, f16);
                e17 = 3;
                f17 = 2;
                __CopyMapTiles(0x29, 0x5a, 0x19, 0x5f, e17, f17);
                e18 = 3;
                f18 = 2;
                __CopyMapTiles(0x29, 0x5a, 0x19, 0x61, e18, f18);
                e19 = 3;
                f19 = 2;
                __CopyMapTiles(0x29, 0x5a, 0x1b, 0x60, e19, f19);
                e20 = 3;
                f20 = 2;
                __CopyMapTiles(0x29, 0x5a, 0x1d, 0x61, e20, f20);
                e21 = 3;
                f21 = 2;
                __CopyMapTiles(0x29, 0x5e, 0x1b, 0x5c, e21, f21);
                e22 = 3;
                f22 = 2;
                __CopyMapTiles(0x29, 0x60, 0x1d, 0x5d, e22, f22);
                e23 = 3;
                f23 = 2;
                __CopyMapTiles(0x29, 0x5e, 0x1b, 0x5e, e23, f23);
                e24 = 3;
                f24 = 2;
                __CopyMapTiles(0x29, 0x60, 0x1b, 0x60, e24, f24);
                e25 = 3;
                f25 = 2;
                __CopyMapTiles(0x29, 0x60, 0x1d, 0x61, e25, f25);
            }
            break;
        case 26:
            __ClearFlag(0x121);
            /* fallthrough */
        case 22:
        case 23:
            __ClearFlag(0x12f);
            if (__GetFlag(0x80 << 2) == 0)
                break;
            e26 = 3;
            f26 = 5;
            __CopyMapTiles(0x2c, 0x75, 0x29, 0x75, e26, f26);
            break;
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            if (__GetFlag(0x987) == 0)
                break;
            __MapActor_SetPos(0xc, 0xda << 18, 0xb0 << 15);
            a = __MapActor_GetActor(0xc);
            a->f0c = 0xffe80000;
            a->f3c = 0x80 << 24;
            break;
        }
    } else if (st == (int)(&_AREA_b7)) {
        k2 = 0xe1 << 1;
        g2 = (unsigned char *)&gState + k2;
        switch (*(short *)(g2 + (unsigned int)0)) {
        case 16:
            __ClearFlag(0x12f);
            break;
        case 21:
            OvlFunc_968_200c2bc();
            break;
        case 20:
            __Func_8091ff0(0xaa);
            if (__GetFlag(0x109))
                break;
            OvlFunc_968_20089c8();
            break;
        case 9:
        case 10:
            a = __MapActor_GetActor(0xb);
            OvlFunc_968_20099c0();
            if ((a->f08 >> 20) == 8)
                OvlFunc_968_2009a14(a);
            a = __MapActor_GetActor(0xc);
            if ((a->f08 >> 20) == 7)
                OvlFunc_968_2009a14(a);
            OvlFunc_968_2008058(0xce << 16, 0, 0x1c10000, 0xdf);
            OvlFunc_968_2008058(0xd2 << 16, 0, 0x1c10000, 0xdf);
            break;
        case 7:
        case 8:
            __Func_8091494(0);
            __WaitFrames(2);
            a = __MapActor_GetActor(8);
            a->f55 = 0;
            a->f6c = OvlFunc_968_20088c8;
            a = __MapActor_GetActor(9);
            a->f55 = 0;
            a->f6c = OvlFunc_968_20088c8;
            a = __MapActor_GetActor(0xa);
            a->f55 = 0;
            a->f6c = OvlFunc_968_20088c8;
            OvlFunc_968_20098f8();
            break;
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            __Func_8091ff0(0xaa);
            __Func_8091494(0);
            __WaitFrames(2);
            if (__GetFlag(0xc0 << 2) == 0)
                break;
            e27 = 5;
            f27 = 2;
            __CopyMapTiles(0x6f, 5, 0x75, 5, e27, f27);
            e28 = 5;
            f28 = 2;
            __CopyMapTiles(0x6f, 0xa, 0x75, 0xa, e28, f28);
            e29 = 5;
            f29 = 2;
            __CopyMapTiles(0x6f, 7, 0x6f, 5, e29, f29);
            e30 = 5;
            f30 = 2;
            __CopyMapTiles(0x6f, 7, 0x6f, 0xa, e30, f30);
            e31 = 0x36;
            f31 = 3;
            __Func_80105d4(0x30, 3, 3, 0xa, e31, f31);
            e32 = 0x30;
            f32 = 3;
            __Func_80105d4(0x37, 0x1a, 3, 0xa, e32, f32);
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 19:
            __Func_8091ff0(0xaa);
            break;
        }
    } else if (st == (int)(&_AREA_b8)) {
        k2 = 0xe1 << 1;
        g2 = (unsigned char *)&gState + k2;
        switch (*(short *)(g2 + (unsigned int)0)) {
        case 2:
            OvlFunc_968_20087d8();
            /* fallthrough */
        case 1:
            __Func_8091ff0(0xaa);
            break;
        case 4:
        case 6:
            __Func_8091494(0);
            break;
        case 9:
        case 10:
            a = __MapActor_GetActor(8);
            a->f55 = 0;
            a->f0c = 0;
            b = __MapActor_GetActor(9);
            b->f55 = 0;
            b->f59 = 0;
            if (__GetFlag(0x301)) {
                __WaitFrames(1);
                e33 = 1;
                f33 = 2;
                __CopyMapTiles(0x7c, 0x29, 0x6e, 0x29, e33, f33);
                e34 = 0x2e;
                f34 = 0x2a;
                __Func_8010704(0x2e, 0x29, 1, 1, e34, f34);
                { PINXZ; qx = 0xba << 18; qz = 0xb6 << 18; __MapActor_SetPos(9, qx, qz); }
                b->f55 = 0;
                b->f0c = 0xfff00000;
                __Func_8092b08(9, 3);
                b->f23 = 2;
                e35 = 0x2e;
                f35 = 0x2d;
                __Func_8010704(0x2d, 0x2d, 1, 1, e35, f35);
                __MapActor_SetAnim(0xa, 7);
                __Func_8092b08(0xa, 1);
                c = __MapActor_GetActor(0xa);
                c->f59 = 0;
                c->f23 = 2;
                __MapActor_SetPos(0xa, 0x2e70000, 0xae << 18);
                c->f6c = OvlFunc_968_2008b98;
            }
            OvlFunc_968_2009d48();
            break;
        case 11:
            *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x202;
            __MapActor_GetActor(0)->f0c = 0xfffe0000;
            /* fallthrough */
        case 7:
        case 8:
            __Func_8091ff0(0xaa);
            __Func_8011ae0();
            REG_BLDCNT = 0;
            if (__GetFlag(0xc0 << 2)) {
                e36 = 3;
                f36 = 3;
                __CopyMapTiles(0xf, 0x60, 9, 0x60, e36, f36);
                e37 = 3;
                f37 = 3;
                __CopyMapTiles(0xc, 0x60, 0xf, 0x60, e37, f37);
                e38 = 3;
                f38 = 4;
                __CopyMapTiles(5, 0x32, 0xf, 0x20, e38, f38);
                e39 = 3;
                f39 = 4;
                __CopyMapTiles(0x19, 0x2d, 9, 0x20, e39, f39);
                e40 = 9;
                f40 = 0x20;
                __Func_8010704(0xf, 0x20, 3, 1, e40, f40);
                e41 = 0xf;
                f41 = 0x20;
                __Func_8010704(0xc, 0x20, 3, 1, e41, f41);
            }
            k2 = 0xe1 << 1;
            g2 = (unsigned char *)&gState + k2;
            if (*(short *)(g2 + (unsigned int)0) != 0xb)
                break;
            __MapTransitionIn();
            __WaitMapTransition();
            *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x81 << 2;
            break;
        }
    } else if (st == (int)(&_AREA_b9)) {
        k2 = 0xe1 << 1;
        g2 = (unsigned char *)&gState + k2;
        switch (*(short *)(g2 + (unsigned int)0)) {
        case 19:
            OvlFunc_968_200c610();
            break;
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 20:
            __Func_8091ff0(0xaa);
            if (__GetFlag(0x306)) {
                e42 = 0x1a;
                f42 = 0xc;
                __Func_80105d4(0x35, 0xc, 3, 0xd, e42, f42);
                e43 = 9;
                f43 = 2;
                __CopyMapTiles(0x51, 0x29, 0x59, 0xe, e43, f43);
                __WaitFrames(1);
                __StartTask(OvlFunc_968_200c5f0, 0xc8 << 4);
            }
            if (__GetFlag(0x307)) {
                e44 = 0x22;
                f44 = 0xc;
                __Func_80105d4(0x3a, 0xc, 3, 0xd, e44, f44);
                e45 = 5;
                f45 = 2;
                __CopyMapTiles(0x51, 0x29, 0x61, 0xe, e45, f45);
                __WaitFrames(1);
                __StartTask(OvlFunc_968_200c600, 0xc8 << 4);
            }
            k2 = 0xe1 << 1;
            g2 = (unsigned char *)&gState + k2;
            x = *(short *)(g2 + (unsigned int)0);
            if (x == 0xb) {
                OvlFunc_968_20087d8();
                break;
            }
            if (x != 0x14)
                break;
            OvlFunc_968_200c7c0();
            break;
        case 4:
        case 5:
            OvlFunc_968_20087d8();
            /* fallthrough */
        case 6:
            __Func_8091ff0(0xaa);
            break;
        case 15:
        case 16:
            a = __MapActor_GetActor(8);
            a->f55 = 0;
            a->f0c = 0;
            __MapActor_GetActor(9)->f55 = 0;
            __MapActor_GetActor(0xa)->f55 = 0;
            __MapActor_GetActor(0xb)->f55 = 0;
            if (__GetFlag(0xc1 << 2) == 0)
                goto tail_9f60;
            __WaitFrames(1);
            e46 = 1;
            f46 = 2;
            __CopyMapTiles(0x6f, 0x3b, 0x6d, 0x25, e46, f46);
            e47 = 0x2d;
            f47 = 0x26;
            __Func_8010704(0x2d, 0x25, 1, 1, e47, f47);
            if (__GetFlag(0x302)) {
                { PINXZ; qx = 0xc2 << 18; qz = 0xa6 << 18; __MapActor_SetPos(9, qx, qz); }
                { PINXZ; qx = 0xd2 << 18; qz = 0xa6 << 18; __MapActor_SetPos(0xa, qx, qz); }
                { PINXZ; qx = 0xc2 << 18; qz = 0xae << 18; __MapActor_SetPos(0xb, qx, qz); }
            } else {
                { PINXZ; qx = 0xd2 << 18; qz = 0xa6 << 18; __MapActor_SetPos(9, qx, qz); }
                { PINXZ; qx = 0xc2 << 18; qz = 0xae << 18; __MapActor_SetPos(0xa, qx, qz); }
                { PINXZ; qx = 0xd2 << 18; qz = 0xae << 18; __MapActor_SetPos(0xb, qx, qz); }
            }
            __Func_8092b08(9, 3);
            a = __MapActor_GetActor(9);
            a->f0c = 0xfff00000;
            a->f23 = 2;
            __Func_8092b08(0xa, 3);
            a = __MapActor_GetActor(0xa);
            a->f0c = 0xfff00000;
            a->f23 = 2;
            __Func_8092b08(0xb, 3);
            a = __MapActor_GetActor(0xb);
            a->f0c = 0xfff00000;
            a->f23 = 2;
            __MapActor_SetAnim(0xc, 7);
            __Actor_SetSpriteFlags(__MapActor_GetActor(0xc), 0);
            __Func_8092b08(0xc, 1);
            a = __MapActor_GetActor(0xc);
            a->f59 = 0;
            a->f23 = 2;
            __MapActor_SetPos(0xc, 0x2d70000, 0x9e << 18);
            a->f6c = OvlFunc_968_2008b98;
        tail_9f60:
            OvlFunc_968_2009f60();
            break;
        }
    } else if (st == (int)(&_AREA_ba)) {
        k2 = 0xe1 << 1;
        g2 = (unsigned char *)&gState + k2;
        switch (*(short *)(g2 + (unsigned int)0)) {
        case 1:
        case 2:
            if (__GetFlag(0x109)) {
                OvlFunc_968_200a2c8(0);
                OvlFunc_968_200894c(__MapActor_GetActor(0));
                for (i = 0; i <= 3; i++) {
                    a = __MapActor_GetActor(i + 0xa);
                    x = a->f08 >> 20;
                    if (x == 0xd) {
                        y = a->f10 >> 20;
                        if (y == 7) {
                            if (__GetFlag(i + (0x80 << 2)))
                                goto found;
                        }
                    }
                }
            } else {
                a = __MapActor_GetActor(8);
                a->f55 = 0;
                a->f0c = 0xffd00000;
                a->f23 |= 2;
                a->f59 &= 0xfe;
                a->f64 = 3;
                __Func_8092b08(8, 1);
                a = __MapActor_GetActor(9);
                a->f55 = 0;
                a->f0c = 0xffd00000;
                a->f23 |= 2;
                a->f59 &= 0xfe;
                a->f64 = 3;
                __Func_8092b08(9, 1);
                a = __MapActor_GetActor(0xa);
                a->f55 = 0;
                a->f64 = 0;
                __Actor_SetSpriteFlags(__MapActor_GetActor(0xa), 0);
                a = __MapActor_GetActor(0xb);
                a->f55 = 0;
                a->f64 = 0;
                __Actor_SetSpriteFlags(__MapActor_GetActor(0xb), 0);
                a = __MapActor_GetActor(0xc);
                a->f55 = 0;
                a->f64 = 0;
                __Actor_SetSpriteFlags(__MapActor_GetActor(0xc), 0);
                a = __MapActor_GetActor(0xd);
                a->f55 = 0;
                a->f64 = 0;
                __Actor_SetSpriteFlags(__MapActor_GetActor(0xd), 0);
            }
            break;
        case 13:
        case 14:
            __Func_8091ff0(0xaa);
            break;
        case 18:
        case 19:
            *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x202;
            __MapActor_GetActor(0)->f0c = 0xfffe0000;
            /* fallthrough */
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 20:
            __MapActor_GetActor(0x14)->f55 = 4;
            __MapActor_GetActor(0x14)->f23 |= 2;
            __MapActor_GetActor(0x14)->f0c = 0xffef8000;
            __Func_8011ae0();
            REG_BLDCNT = 0;
            if (__GetFlag(0x306)) {
                __Func_8091ff0(0xaa);
                e48 = 3;
                f48 = 2;
                __CopyMapTiles(0x24, 0x51, 0x20, 0x51, e48, f48);
                e49 = 3;
                f49 = 2;
                __CopyMapTiles(0x24, 0x53, 0x24, 0x51, e49, f49);
                e50 = 0x20;
                f50 = 0x11;
                __Func_8010704(0x24, 0x11, 3, 1, e50, f50);
                e51 = 0x24;
                f51 = 0x11;
                __Func_8010704(0x24, 0x12, 3, 1, e51, f51);
                e52 = 1;
                f52 = 1;
                __CopyMapTiles(0x3f, 0x1d, 0x21, 0x14, e52, f52);
                e53 = 3;
                f53 = 4;
                __CopyMapTiles(0x14, 0x38, 0x24, 0x11, e53, f53);
            }
            if (__GetFlag(0x307)) {
                e54 = 3;
                f54 = 2;
                __CopyMapTiles(0x2c, 0x51, 0x30, 0x51, e54, f54);
                e55 = 3;
                f55 = 2;
                __CopyMapTiles(0x2c, 0x53, 0x2c, 0x51, e55, f55);
                e56 = 0x30;
                f56 = 0x11;
                __Func_8010704(0x2c, 0x11, 3, 1, e56, f56);
                e57 = 0x2c;
                f57 = 0x11;
                __Func_8010704(0x2c, 0x12, 3, 1, e57, f57);
                e58 = 1;
                f58 = 1;
                __CopyMapTiles(0x3f, 0x1d, 0x31, 0x14, e58, f58);
                e59 = 3;
                f59 = 4;
                __CopyMapTiles(0x29, 0x38, 0x2c, 0x11, e59, f59);
            }
            k2 = 0xe1 << 1;
            g2 = (unsigned char *)&gState + k2;
            v = *(unsigned short *)g2;
            t = v - 0x12;
            if (t <= 1) {
                __MapTransitionIn();
                __WaitMapTransition();
                *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x81 << 2;
                v = *(unsigned short *)g2;
            }
            if (v != 0x14)
                break;
            OvlFunc_968_200ca2c();
            break;
        found:
            a->f23 |= 2;
            a->f59 = 0;
            a->f55 = 0;
            __Func_8010704(4, 0x13, 1, 1, x, y);
            break;
        case 15:
        case 16:
            __WaitFrames(1);
            __StartTask(OvlFunc_968_200b050, 0xc8 << 4);
            a = __MapActor_GetActor(0xe);
            a->f55 = 0;
            a->f0c = 0;
            __MapActor_GetActor(0xf)->f55 = 0;
            __MapActor_GetActor(0x10)->f55 = 0;
            __MapActor_GetActor(0x11)->f55 = 0;
            __MapActor_GetActor(0x12)->f55 = 0;
            if (__GetFlag(0xc2 << 2) == 0)
                goto tail_ab14;
            __WaitFrames(1);
            e60 = 1;
            f60 = 2;
            __CopyMapTiles(0x5f, 0x38, 0x4d, 0x23, e60, f60);
            e61 = 0xd;
            f61 = 0x24;
            __Func_8010704(0xd, 0x23, 1, 1, e61, f61);
            { PINXZ; qx = 0x84 << 17; qz = 0xba << 18; __MapActor_SetPos(0xf, qx, qz); }
            a = __MapActor_GetActor(0xf);
            a->f0c = 0xfff00000;
            a->f23 = 2;
            __Func_8092b08(0xf, 3);
            { PINXZ; qx = 0xb8 << 16; qz = 0x9e << 18; __MapActor_SetPos(0x10, qx, qz); }
            a = __MapActor_GetActor(0x10);
            a->f0c = 0xfff00000;
            a->f23 = 2;
            __Func_8092b08(0x10, 3);
            { PINXZ; qx = 0xe8 << 16; qz = 0xae << 18; __MapActor_SetPos(0x11, qx, qz); }
            a = __MapActor_GetActor(0x11);
            a->f0c = 0xfff00000;
            a->f23 = 2;
            __Func_8092b08(0x11, 3);
            { PINXZ; qx = 0xb8 << 16; qz = 0xa6 << 18; __MapActor_SetPos(0x12, qx, qz); }
            a = __MapActor_GetActor(0x12);
            a->f0c = 0xfff00000;
            a->f23 = 2;
            __Func_8092b08(0x12, 3);
            __MapActor_SetAnim(0x13, 7);
            __Actor_SetSpriteFlags(__MapActor_GetActor(0x13), 0);
            __Func_8092b08(0x13, 1);
            a = __MapActor_GetActor(0x13);
            a->f59 = 0;
            a->f23 = 2;
            __MapActor_SetPos(0x13, 0xd7 << 16, 0x96 << 18);
            a->f6c = OvlFunc_968_2008b98;
        tail_ab14:
            OvlFunc_968_200ab14();
            break;
        }
    }
    return 0;
}
