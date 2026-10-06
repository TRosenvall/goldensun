/* OvlFunc_923_2009a3c -- candidate produced in batch 316 by PORTING the body
 * of OvlFunc_924_200cfcc, which tools/dupfuncs.py reports as a BYTE-IDENTICAL
 * DUPLICATE of this function.
 *
 * **5 ENCODINGS OF 179, DOWN FROM 181.**  Instruction count exact
 * (179 = 179), size exact, pool identical, all relocations identical.
 * tools/shimcount.py: ZERO register pins, so no fakematch.txt row is needed.
 * Production flags, no per-file Makefile adjustment (checked).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7aa430/2009a3c.c \
 *     asm/overlays/rom_7aa430/ovl_1a3c_a_a_a_a.s --func OvlFunc_923_2009a3c
 *   XX ENCODINGS differ in 5 place(s) (ref 179, ours 179)
 *      first at index 54: ref 4640  ours 189b
 *
 * HISTORY -- THE SUPERSEDED BODY.  THE FIGURES IN THIS PARAGRAPH ARE DEAD.
 * Batch 316 replaced this park wholesale.  The body it replaced was a
 * different, SATURATED one at 181 of 177, with 172 differing instructions at
 * nins 169/175 and +12 bytes, and its recorded blocker was "one allocno too
 * many, PLUS an unsolved pooled-zero construct".  None of that describes the
 * body in this file, whose figure is the 5 of 179 stated above.
 *
 * This paragraph is dated on purpose.  It previously read as a PROPOSAL --
 * "WHY THIS REPLACES THE EXISTING PARK WHOLESALE", describing "the installed
 * park" in the third person -- which was true when it was written and became
 * self-referential the moment the port was installed, because this file then
 * BECAME the installed park.  A reader scanning for a figure met "181 of 177
 * -- SATURATED" in a sentence that parsed as current state.  parkcheck never
 * flagged it: it takes the FIRST claim match, which is the correct 5.  Batch
 * 329 brief A read the 181 and reported a "free improvement 181 -> 5" that
 * batch 316 had already banked.  When a port is installed, rewrite its
 * proposal prose into the past tense -- see docs/elevation.md, "prose that
 * looks like machinery".
 *
 * The port is THREE RENAMES and nothing else:
 *   OvlFunc_924_200cfcc    -> OvlFunc_923_2009a3c
 *   gScript_924__0200de20  -> gScript_923__0200a7d0
 *   gScript_924__0200de38  -> gScript_923__0200a7e8
 * (offset order fixes the script pairing: de20 < de38 and a7d0 < a7e8).  The
 * reference's `=` operands are otherwise identical to 200cfcc's: 0x109,
 * 0x85000007, 0xffff8000, 0xfffff, REG_DMA3SAD, gBuffer, gState.
 *
 * THE REMAINING 5 ARE 200cfcc's RESIDUE, IDENTICALLY -- same count, same first
 * index 54.  See the corrected header of
 * src/non_matching/ovl_7ac2d8/200cfcc.c for the mechanism: cse.c:3652's
 * commutative canonicalisation puts gBuffer's register SECOND in the PLUS, and
 * reload.c's find_dummy_reload then takes its reload register from
 * XEXP (plus, 0).  Whatever closes 200cfcc closes this, and vice versa
 */
#include "dma.h"

extern unsigned char gState[];
extern unsigned char gBuffer[];
extern unsigned char gScript_923__0200a7d0[];
extern unsigned char gScript_923__0200a7e8[];

extern void *__galloc_ewram(int tag, int size);
extern int __GetFlag(int id);
extern unsigned char *__GetFieldActor(int id);
extern unsigned char *__CreateActor(int kind, int x, int y, int z);
extern void __Actor_SetScript(unsigned char *a, unsigned char *s);
extern void __Sprite_SetAnim(unsigned char *s, int n);

void OvlFunc_923_2009a3c(int a, unsigned char *c)
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
            __Actor_SetScript(n, gScript_923__0200a7e8);
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
        __Actor_SetScript(n, gScript_923__0200a7d0);
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
