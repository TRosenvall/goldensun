/* Func_80788c4 (0x080788c4) -- NON-MATCHING.
 * Blocker class: a narrowing shift before a `!= 0` test that gcc-2.96 folds.
 * Same wall as src/non_matching/rom_15000/801f730.c, in its HALFWORD flavour.
 *
 * 65 lines against the ROM's 65, 30 differing, all inside the compaction loop.
 * The first difference is the test itself:
 *
 *     rom    ldrh r2, [r4] / lsl r3, r2, #0x10 / add r4, #2 / cmp r3, #0
 *     ours   ldrh r3, [r1] / add r1, #2 / cmp r3, #0
 *
 * The ROM widens the loaded halfword to the top of the word and tests THAT;
 * gcc tests the loaded value directly, which is one instruction shorter, and
 * every register after it permutes.
 *
 * THIS IS THE SAME FOLD, ONE WIDTH UP. 801f730.c records ten spellings probed
 * directly against gcc-2.96 for the byte case (`ldrb` + `lsl #24` + `cmp #0`),
 * none of which produces the shift, with the decisive one being that gcc knows
 * the loaded value's range and folds `(x << 24) != 0` to `x != 0`. The same
 * reasoning applies verbatim to `(x << 16) != 0` on an `ldrh` value, so the ten
 * probes were not repeated here.
 *
 * That is the useful addition: the earlier park could have been read as
 * something about `signed char`. It is not -- it is about the FOLD, and it
 * generalises to any narrowing shift before a zero test.
 *
 * WHAT IS RIGHT AND SHOULD BE KEPT: the frame is exact at 65 lines. The
 * quantity decrement through `v - 0x800` on the packed bits 11..15, the
 * register-offset load and store at `slot * 2 + 0xd8`, the three return values
 * (-1, 1, 2) through a single `ret` variable reaching one shared CalcStats
 * tail, the 15-iteration compaction with separate src and dst cursors, and the
 * `0xf - n` tail-zeroing loop with its POOLED zero (`ldr r2, =0`, which a plain
 * literal produces because the ROM pools it -- the halfword exception running
 * in the ROM-pools direction) are all correct.
 *
 * NEXT: nothing source-level, unless the fold is ever defeated for 801f730,
 * in which case both close together.
 */
extern unsigned char *GetUnit(int who);
extern void CalcStats(int who);

int Func_80788c4(int who, int slot)
{
    unsigned char *u;
    unsigned short *base;
    unsigned short *src;
    unsigned short *dst;
    unsigned short *p;
    int off;
    int ret;
    int v;
    int n;
    int i;
    int k;

    u = GetUnit(who);
    off = slot * 2 + 0xd8;
    v = *(unsigned short *)(u + off);
    ret = -1;
    if (v != 0) {
        if ((v & 0xf800) != 0) {
            *(unsigned short *)(u + off) = v - 0x800;
            ret = 1;
        } else {
            base = (unsigned short *)(u + 0xd8);
            *(unsigned short *)(u + off) = 0;
            src = base;
            n = 0;
            dst = base;
            i = 0xe;
            do {
                v = *src++;
                if (v != 0) {
                    *dst++ = v;
                    n++;
                }
                i--;
            } while (i >= 0);
            if (n <= 0xe) {
                p = base + n;
                k = 0xf - n;
                do {
                    k--;
                    *p++ = 0;
                } while (k != 0);
            }
            ret = 2;
        }
    }
    CalcStats(who);
    return ret;
}
