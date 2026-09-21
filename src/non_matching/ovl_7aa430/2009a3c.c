/* OvlFunc_923_2009a3c (0x02009a3c) -- NON-MATCHING, 90 of 177.
 * Blocker class: one allocno too many, PLUS an unsolved pooled-zero construct.
 *
 * asm/overlays/rom_7aa430/ovl_1a3c_a_a_a.s (2 functions, so landing needs a split).
 * TWIN: OvlFunc_924_200cfcc -- 181 vs 181 lines, two per-overlay symbols differ, no constant.
 *
 * Structure recovered: `__galloc_ewram(0x23, 4)`, `__GetFlag(0x109)`, the
 * `DMA3_SET(&z, e, 0x85000007)` block with the zero coming from the branch-proven GetFlag
 * result, the gBuffer cell index `((ay / 0x100000) << 7) + (ax / 0x100000)`, both spawn blocks,
 * and the `p = &n[0x55]; *p = z; p += 0xf; *(short *)p = z;` idiom lifted from the 2009df8 park.
 * The runtime-gState fix that landed the file-mate OvlFunc_923_2009ec8 was worth 96 -> 90 here.
 *
 * BLOCKER 1 -- ONE ALLOCNO MORE THAN FITS, priced from `.17.lreg`:
 *     Register 32 (the `k` parameter) used 3 times across 88 insns; crosses 6 calls
 *     -> priority 1*3/88 = 0.034, DEAD LAST of 17 allocnos, so it SPILLS
 * where the ROM keeps `k` in r9 from the first instruction. Ours also burns r11 on `cell` where
 * the ROM uses r10. Folding `idx` into the `cell` expression, and scoping the DMA zero into its
 * branch, were both inert at 90.
 *
 * BLOCKER 1b -- AND THIS ONE IS SHARED, UNSOLVED, AND CAPS THE FUNCTION. The second sprite
 * block's `s[0x26] = 0` is `ldr r3, .L1b88 @ 0 / strb r3,[r2]` with an inline
 * `.align 2,0 / .word 0 / .pool` -- a POOLED ZERO where gcc emits `mov r3,#0`, and the
 * mid-function pool forces a `b` over it, so the ROM is two instructions longer than any
 * `mov #0` spelling can be. src/non_matching/ovl_7aa430/2009df8.c documents this as open in
 * this same overlay family. EVEN WITH THE ALLOCATION FIXED, THIS FUNCTION CANNOT REACH EXACT
 * UNTIL THAT IS SOLVED.
 *
 * AND THERE IS AN INTERNAL CONTROL FOR IT, which is what makes it interesting: the FIRST sprite
 * block in this same function has a plain `mov r3, #0` that is then reused as `sub r3, #0xd`.
 * TWO ZEROS IN ONE FUNCTION, SPELLED DIFFERENTLY IN THE ROM -- which is itself evidence the
 * pooled one is a SYMBOL rather than a literal, on the same reasoning const.sym uses. Reported
 * and deliberately NOT added, since it does not complete the function.
 *
 * NEXT: the pooled zero, jointly with 2009df8. The allocation is secondary.
 */
#include "dma.h"

extern unsigned char gState[];
extern unsigned char gBuffer[];
extern unsigned char gScript_923__0200a7e8[];
extern unsigned char gScript_923__0200a7d0[];
extern unsigned int *__galloc_ewram(int a, int b);
extern int __GetFlag(int id);
extern unsigned char *__GetFieldActor(int slot);
extern unsigned char *__CreateActor(int kind, int x, int y, int z);
extern void __Actor_SetScript(unsigned char *a, unsigned char *s);
extern void __Sprite_SetAnim(unsigned char *s, int n);

void OvlFunc_923_2009a3c(int k, unsigned char *e)
{
    unsigned int *p;
    unsigned char *t;
    unsigned char *cell;
    unsigned char *n;
    unsigned char *s;
    unsigned char *w;
    int ax;
    int ay;
    int idx;
    int z;
    unsigned int g;
    unsigned int sh;

    p = __galloc_ewram(0x23, 4);
    *p = (unsigned int)e;
    if (__GetFlag(0x109) == 0) {
        z = 0;
        DMA3_SET(&z, e, 0x85000007);
        *(int *)(e + 4) = k;
        return;
    }
    sh = 0xfa;
    g = (unsigned int)&gState;
    sh <<= 1;
    g += sh;
    t = __GetFieldActor(*(int *)g);
    ay = *(int *)(t + 0x10);
    ax = *(int *)(t + 8);
    idx = ((ay / 0x100000) << 7) + (ax / 0x100000);
    cell = gBuffer + idx * 4;
    if (*(int *)e != 0 && *(int *)(e + 0x14) != 0) {
        n = __CreateActor(0x1a, ax, *(int *)(t + 0xc) + (0xc0 << 13), ay);
        if (n != 0) {
            *(int *)(n + 0x14) = *(int *)(t + 0x14);
            s = *(unsigned char **)(n + 0x50);
            __Actor_SetScript(n, gScript_923__0200a7e8);
            n[0x55] = 4;
            *(unsigned char **)(n + 0x68) = t;
            *(int *)(n + 0xc) += 0xffff8000;
            if (s != 0) {
                int zz = 0;
                __Sprite_SetAnim(s, 6 - *(int *)e);
                s[0x26] = zz;
                s[9] = (s[9] & (zz - 0xd)) | 4;
            }
            *(unsigned char **)(e + 0x14) = n;
        }
    } else {
        *(int *)(e + 0x14) = 0;
    }
    if (cell[2] == k && *(int *)(e + 0x18) != 0) {
        n = __CreateActor(0x1a, *(int *)(t + 8), *(int *)(t + 0xc),
                          *(int *)(t + 0x10));
        if (n == 0)
            return;
        *(int *)(n + 0x14) = *(int *)(t + 0x14);
        s = *(unsigned char **)(n + 0x50);
        __Actor_SetScript(n, gScript_923__0200a7d0);
        {
            int y = 0;
            w = &n[0x55];
            *w = y;
            w += 0xf;
            *(short *)w = y;
        }
        n[0x23] = 2;
        *(int *)(n + 0x30) = 0x80 << 11;
        if (s != 0) {
            __Sprite_SetAnim(s, 6);
            s[0x26] = 0;
        }
        *(unsigned char **)(e + 0x18) = n;
    } else {
        *(int *)(e + 0x18) = 0;
    }
}
