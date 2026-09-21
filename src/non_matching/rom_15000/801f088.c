/* Func_801f088 (0x0801f088) -- NON-MATCHING, 140 differing of 175.
 * Blocker class: global_alloc priority, and it is now PRICED WITH THE FORMULA.
 * Never attempted before batch 277.
 *
 * asm/rom_15000/rom_1de5c_c_c_c_c_a_a_a_c_c_c.s (2 functions after batch 277's split, with
 * the 435-instruction Func_801f200, so landing needs another split).
 *
 * THE STRUCTURE IS SETTLED AND CORRECT: the two nested loops, the rotate-mask ladder, the
 * nibble recode, and the `while (k <= (n0 ? 2 : 0))` middle loop all line up.
 *
 * THE OUTER LOOP MUST BE A `goto` LOOP. As an ordinary `for (cnt = 4; cnt >= 0; cnt--)`,
 * strength_reduce folds `base[row + a]` into a walking `ldrh` / `add #2` pointer where the
 * ROM RECOMPUTES `(row + a) * 2` every iteration. 165 -> 150, and the goto form is also what
 * produces the ROM's `sub sp, #0x14`. That is the recorded two-sign rule firing on its
 * ROM-recomputes side.
 *
 * THE RESIDUE IS ONE SPILL SWAP PLUS A WHOLE-FUNCTION LOW-REGISTER PERMUTATION, and the
 * batch-277 priority formula prices it exactly:
 *
 *     priority = floor_log2(REG_N_REFS) * REG_N_REFS / REG_LIVE_LENGTH
 *
 * The ROM spills {row, n0, base, cnt, a} and keeps the masked tile value `v` in r10; we spill
 * {row, n0, base, cnt, v} and keep `a` in r11.
 *
 *     a = 3 * 9 / 94  = 0.287
 *     v = 3 * 9 / 118 = 0.229
 *
 * So `v` must outrank `a`, and needs EITHER 12 refs (one more depth-2 reference) OR a live
 * length under 94. Writing the load/store address twice instead of through an `idx` local
 * does NOT buy it: cse2 hoists `v * 32` into its own pseudo (8 refs / 74) and `v` stays at 9.
 *
 * ALSO ESTABLISHED, and it is a reading tool rather than a lever: gcc ASSIGNS SPILL SLOTS IN
 * ASCENDING PSEUDO NUMBER TO DESCENDING sp OFFSET. So the ROM's slots (`a` at 0x10, `cnt` at
 * 0xc, `base` at 8, `n0` at 4, `row` at 0) read directly as a declaration order of cnt,
 * base, n0, row. Reordering the declarations to match moved nothing (144 against 140) -- THE
 * SLOT ORDER IS A CONSEQUENCE, NOT A LEVER. Worth knowing before spending a round on it, and
 * worth reading beside the batch-277 note that spill-slot order follows declaration order:
 * both are true, and neither gives you the allocation.
 *
 * MEASURED, all against 175: goto outer loop 140; ordinary `for` 165; the address written
 * twice instead of via `idx` 167; `int v` / `unsigned short w` / `unsigned a` / `(row+a)*2`
 * spelled as a cast / statement swaps / three declaration orders -- ALL EXACTLY 140, inert.
 *
 * NEXT: give `v` one more depth-2 reference or shorten its live range below 94. That is a
 * concrete, checkable target rather than a spelling hunt, which is what the formula buys.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern char *iwram_3001e8c;
extern void *GetSpritePalette(void);

int Func_801f088(unsigned short *p, int a, int c, int n)
{
    char *base;
    int n0;
    int row;
    int cnt;
    unsigned int v;
    unsigned int ma;
    unsigned int mb;
    int sh;
    int k;
    int off;
    int b;
    unsigned int word;
    unsigned int acc;
    int idx;
    unsigned int nib;

    base = iwram_3001e8c;
    n0 = n;
    if (base[0xea5] == 0) {
        DMA3_SET(GetSpritePalette(), (void *)0x50001c0, 0x80000010);
        *(unsigned short *)0x50001dc = *(unsigned short *)0x50001e8;
    }
    a += p[6];
    row = (c + p[7]) * 32;
    cnt = 4;
top:
    {
        v = ((unsigned short *)base)[row + a] & 0x3ff;
        ma = 0x22222222;
        mb = 0xcccccccc;
        if (n > 7) {
            ma = 0x88888888;
            mb = 0xdddddddd;
        } else if (n >= 0) {
            sh = n * 4;
            ma = (0x22222222 << sh) | (0x88888888 >> (32 - sh));
            mb = (0xcccccccc << sh) | (0xdddddddd >> (32 - sh));
        }
        off = 0;
        k = 0;
        while (k <= (n0 != 0 ? 2 : 0)) {
            idx = v * 32 - off;
            word = *(unsigned int *)(0x600001c + idx);
            acc = 0;
            for (b = 0; b <= 7; b++) {
                nib = word & 0xf;
                if (nib == 0xe)
                    acc |= (0xf << (b * 4)) & ma;
                else if (nib == 1)
                    acc |= (0xf << (b * 4)) & mb;
                else
                    acc |= nib << (b * 4);
                word >>= 4;
            }
            *(unsigned int *)(0x600001c + idx) = acc;
            off += 4;
            k++;
        }
        a++;
        n -= 8;
        cnt--;
    }
    if (cnt >= 0)
        goto top;
    return;
}
