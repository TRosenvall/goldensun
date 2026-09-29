/* OvlFunc_924_200cfcc  --  overlay 924, 168 instructions.  NON-MATCHING,
 * PARKED AT 5 ENCODINGS OF 179, and this IS a true distance: SIZE EXACT,
 * INSTRUCTION COUNT EXACT (179 = 179), all relocations identical.
 *
 * BOTH MEASURES:
 *   objcmp        XX ENCODINGS differ in 5 place(s) (ref 179, ours 179)
 *   tryc --align  5 instruction(s) in disagreeing regions, of 177
 * The two agree in sign and magnitude.
 *
 * NOT flag-conditional: measured under the Makefile's own GCC296_CFLAGS.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py scratch_elev/b295/A/t5_park.c \
 *     asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_c_a.s --func OvlFunc_924_200cfcc
 *
 * SPLIT SHAPE.  TWO functions in the reference (this one and
 * OvlFunc_924_200d158), target FIRST, so a TEXT split with no _a part.
 * EXPORT LIST: EMPTY.  datacheck prints nothing (no data section).  The two
 * scripts this function installs, gScript_924__0200de20 and
 * gScript_924__0200de38, are already `.global` in the sibling
 * asm/overlays/rom_7ac2d8/ovl_35b8_c.s.
 *
 * SHIMS: `register ... __asm__` 0, `__asm__(".equ ...")` 0, other `__asm__` 0
 * in this file.  include/dma.h's DMA3_SET contributes four register pins of its
 * own.
 *
 * PRIOR ART READ FIRST: src/overlays/rom_7ac2d8/ovl_35b8_a_c_a_c.c
 * (OvlFunc_924_200d458) and its twin src/overlays/rom_7aa430/ovl_1a3c_a_c_a_b.c
 * spawn the same actor with the same script, and three of their four levers
 * transferred directly.
 *
 * ============== LOAD-BEARING CONSTRUCTS, WITH SINGLE DROPS ==============
 *
 * 1. SEPARATE NAMED CARRIERS PER SITE FOR THE SMALL STORED CONSTANTS -- 19 of
 *    179 when the three sites share one `int v`.  THIS IS THE LARGEST ITEM and
 *    it EXTENDS the twin file's lever 3 ("a named local wrapping ONE of two
 *    identical constants splits a CSE") in a direction that file did not cover:
 *    here the problem is the OPPOSITE, one local used at three sites, and
 *    splitting it into three (`v` for the `4`, `w` for the sprite block's
 *    0 / -0xd chain, `y` for the second spawn's two zeroes) is what puts the
 *    ADDRESS in r2 and the VALUE in r3 at all three, as the ROM has.  With one
 *    shared carrier every one of the three blocks comes out with the two
 *    registers exchanged.
 *
 * 2. THE gState INDEX BUILT AT RUNTIME -- 151 of 179 and SIZE WRONG (392
 *    against 396) without it, the largest single drop in the function, and the
 *    twin file's lever 1 verbatim: `gState + (0xfa << 1)` written inline folds
 *    to a single `=gState+500` pool word, where the ROM has
 *    `ldr r3, =gState / mov r2, #0xfa / lsl r2, #1 / add r3, r2`.  The
 *    `g = (unsigned int)&gState; k = 0xfa; k <<= 1; g += k;` chain reproduces it.
 *
 * 3. THE ADDRESS COMPUTED BEFORE THE VALUE IN THE SPRITE BLOCK -- `u = s + 0x26;`
 *    then `w = 0; *u = w;` -- 7 of 179 without it (the value's `mov r3, #0` lands
 *    one insn early).  Note this is the MIRROR of what OvlFunc_890_2008d9c needed
 *    in the same batch, where naming the VALUE and not the address was the fix;
 *    both spellings exist in the corpus and must be measured per site.
 *
 * 4. THE -0xd MASK DERIVED FROM THE ZERO ALREADY IN THE REGISTER -- 6 of 179
 *    with a plain literal mask -- `w = 0;
 *    *u = w; w -= 0xd; s[9] = (w & s[9]) | 4;` reproduces the ROM's
 *    `mov r3, #0 / strb r3, [r2] / sub r3, #0xd`.  Written as a literal mask gcc
 *    narrows it to QImode and emits `mov r3, #0xf3`, a different instruction.
 *    (reload_cse_move2add chains the two literals through the one hard register
 *    once the source puts them in one variable.)
 *
 * ================== THE NAMED BLOCKER FOR THE LAST 5 ==================
 *
 * All five encodings are ONE cluster, and it is one decision:
 *
 *   ROM   ldr r2, =gBuffer ... mov r0, r8 / add r2, r3 / ldr r3, [r0] /
 *         mov r10, r2      ... ldr r3, [r0, #0x14]
 *   ours  ldr r2, =gBuffer ... add r3, r3, r2 / mov r2, r8 / mov r10, r3 /
 *         ldr r3, [r2]     ... ldr r3, [r2, #0x14]
 *
 * The ROM's `add` ties its destination to gBuffer's register; ours ties it to
 * the index's.  That frees r2 in ours, so the context pointer's low copy lands
 * in r2 instead of the ROM's r0, and `cell` ends up in r3 instead of r2 -- five
 * encodings from one tie.
 *
 * THE PASS IS cse, AT cse.c:3652, and it is not reachable from the source.
 * `fold_rtx`'s commutative canonicalisation reads
 *
 *     if (must_swap || (const_arg0
 *                       && (const_arg1 == 0
 *                           || (GET_CODE (const_arg0) == CONST_INT
 *                               && GET_CODE (const_arg1) != CONST_INT))))
 *       ... swap XEXP (x, 0) and XEXP (x, 1) ...
 *
 * i.e. a commutative operand whose value is a KNOWN CONSTANT -- and a SYMBOL_REF
 * counts -- is moved to position 1.  Read off the dumps: `.00.rtl` insn 118 is
 * `(set (reg 37) (plus (reg 67) (reg 66)))` with gBuffer FIRST, exactly as the
 * ROM wants, and reg 67 carries `REG_EQUAL (symbol_ref "gBuffer")` from its pool
 * load.  By `.03.cse` the same insn reads `(plus (reg 66) (reg 67))`.  Then
 * local-alloc.c:1131 ties the output to the FIRST input it can combine with and
 * breaks out of the loop, so the swapped order decides the register.
 *
 * WHY NO SOURCE SPELLING REACHES IT.  Writing the addition the other way round
 * is inert, because `fold` puts a TREE_CONSTANT address second BEFORE cse ever
 * sees it -- so both `gBuffer + i` and `i + gBuffer` arrive at cse as
 * `(plus index gBuffer)` and both come out the same.  Measured inert, all 5:
 *   `(idx << 2) + (unsigned int)gBuffer`, `(int *)gBuffer + idx`,
 *   `&gBuffer[idx * 4]`, a 4-byte-struct `gBuffer[]` with `&gBuffer[idx]`.
 * And every spelling that DOES break the constant equivalence breaks the pool
 * load's PLACEMENT instead, which costs more than it saves -- measured, all
 * worse: a named `unsigned char *` or `unsigned int` base local (21), the same
 * with a `"+r"` barrier (33), a `register ... __asm__("r2")` pin on the base
 * (33), `cell = gBuffer; cell += ...` (8, and it puts the whole computation in
 * r10).  Pinning the index or the result instead is inert (5).
 * `-fno-regmove` is 11 and does not touch this cluster; `-fno-gcse` is 5.
 *
 * MEASURED INERT, recorded so the next reader does not repeat them:
 *   - moving the `cell` computation down to just before its only use: 70.
 *   - a 4-byte-struct type for gBuffer: 5, unchanged.
 */
#include "dma.h"

extern unsigned char gState[];
extern unsigned char gBuffer[];
extern unsigned char gScript_924__0200de20[];
extern unsigned char gScript_924__0200de38[];

extern void *__galloc_ewram(int tag, int size);
extern int __GetFlag(int id);
extern unsigned char *__GetFieldActor(int id);
extern unsigned char *__CreateActor(int kind, int x, int y, int z);
extern void __Actor_SetScript(unsigned char *a, unsigned char *s);
extern void __Sprite_SetAnim(unsigned char *s, int n);

void OvlFunc_924_200cfcc(int a, unsigned char *c)
{
    unsigned char *t;
    unsigned char *n;
    unsigned char *s;
    unsigned char *cell;
    unsigned int g;
    int k;
    int v;
    unsigned char *u;
    int w;
    int y;

    *(unsigned char **)__galloc_ewram(0x23, 4) = c;
    if (__GetFlag(0x109) == 0) {
        DMA3_CLEAR(c, 0x1c);
        *(int *)(c + 4) = a;
        return;
    }
    g = (unsigned int)&gState;
    k = 0xfa;
    k <<= 1;
    g += k;
    t = __GetFieldActor(*(int *)g);
    cell = gBuffer + (((*(int *)(t + 0x10) / 0x100000) * 0x80
                       + *(int *)(t + 8) / 0x100000) << 2);
    if (*(int *)c != 0 && *(int *)(c + 0x14) != 0) {
        n = __CreateActor(0x1a, *(int *)(t + 8),
                          *(int *)(t + 0xc) + (0xc0 << 13),
                          *(int *)(t + 0x10));
        if (n != 0) {
            *(int *)(n + 0x14) = *(int *)(t + 0x14);
            s = *(unsigned char **)(n + 0x50);
            __Actor_SetScript(n, gScript_924__0200de38);
            *(unsigned char **)(n + 0x68) = t;
            v = 4;
            n[0x55] = v;
            *(int *)(n + 0xc) += 0xffff8000;
            if (s != 0) {
                __Sprite_SetAnim(s, 6 - *(int *)c);
                u = s + 0x26;
                w = 0;
                *u = w;
                w -= 0xd;
                s[9] = (w & s[9]) | 4;
            }
            *(unsigned char **)(c + 0x14) = n;
        }
    } else {
        *(unsigned char **)(c + 0x14) = 0;
    }
    if (cell[2] == a && *(int *)(c + 0x18) != 0) {
        n = __CreateActor(0x1a, *(int *)(t + 8), *(int *)(t + 0xc),
                          *(int *)(t + 0x10));
        if (n == 0)
            return;
        *(int *)(n + 0x14) = *(int *)(t + 0x14);
        s = *(unsigned char **)(n + 0x50);
        __Actor_SetScript(n, gScript_924__0200de20);
        y = 0;
        n[0x55] = y;
        *(short *)(n + 0x64) = y;
        n[0x23] = 2;
        *(int *)(n + 0x30) = 0x80 << 11;
        if (s != 0) {
            __Sprite_SetAnim(s, 6);
            s[0x26] = 0;
        }
        *(unsigned char **)(c + 0x18) = n;
    } else {
        *(unsigned char **)(c + 0x18) = 0;
    }
}
