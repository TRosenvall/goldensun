/* OvlFunc_970_2008f80 (0x02008f80) -- NON-MATCHING, ours 145 of ref 161. FIRST MEASUREMENT.
 * Blocker class: a CSE between two loop setups.
 *
 * asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a_c_c_c_c_c_a.s (1 function, so landing needs NO split).
 * Best candidate below, built with -fcall-saved-r4.
 *
 * TWO STRUCTURAL FINDINGS, both worth carrying:
 *
 * 1. A TABLE WITH NO C-LEGAL NAME CAN BE REACHED WITHOUT A .sym ENTRY. This function's table is
 *    `.L14c8`, made `.global` at asm/overlays/rom_7fa4ec/ovl_30_c_c_c_c.s:170.
 *    `extern const short tbl[] __asm__(".L14c8");` works and emits the correct `.word .L14c8`.
 *    So a dot-prefixed local label is NOT automatically a label.sym request -- try the
 *    __asm__ rename first.
 *
 * 2. THIS TU WANTS -fcall-saved-r4. The ROM keeps the accumulator in r4 ACROSS
 *    `mov ip, pc / bx r8`; under the tree's default -fcall-used-r4 gcc spills it
 *    (`str r4,[sp]` / `ldr r4,[sp]` every iteration). 153 -> 145. The precedent for a per-file
 *    flip is COMMON2_CFLAGS, which does exactly this substitution for the common2 stems -- and
 *    the one-grep test for a TU not built with -fcall-used-r4 is `push {r4`.
 *
 * STILL 16 INSTRUCTIONS SHORT: the second loop's setup (`(b[0xf00] ^ 1) * 1920` and
 * `*(u16 *)(b + 0xf02)`) is CSE'd with the first loop's. No flag combination moved it off 145.
 *
 * NEXT: break that CSE. And if it lands, the -fcall-saved-r4 row is a real Makefile entry to
 * add, with the push-list evidence above.
 */
extern unsigned char *iwram_3001ed8;
extern unsigned char iwram_3001ad0[];
extern int Func_8000888(int a, int b);
extern const short tbl[] __asm__(".L14c8");

void OvlFunc_970_2008f80(void)
{
    int (*fn)(int, int);
    unsigned char *b;
    short *o;
    unsigned int y;
    int acc;
    int step;
    int arg;
    int bias;
    unsigned int i;
    int v;

    b = iwram_3001ed8;
    y = (unsigned short)*(short *)(iwram_3001ad0 + 0xe);
    o = (short *)(b + (b[0xf00] ^ 1) * 1920);
    step = *(int *)(b + 0xf10);
    acc = *(int *)(b + 0xf08) * (*(unsigned short *)(b + 0xf02) + y);
    arg = *(int *)(b + 0xf18);
    bias = *(unsigned short *)(iwram_3001ad0 + 0xc);
    fn = Func_8000888;
    for (i = 0; i <= 0x9f; i++) {
        v = fn(tbl[(acc >> 16) & 0xff], arg);
        if (v < 0)
            v += 0xff;
        *o = ((unsigned int)(v << 8) >> 16) + bias;
        acc += step;
        o += 2;
    }
    o = (short *)(b + (b[0xf00] ^ 1) * 1920) + 1;
    step = *(int *)(b + 0xf14);
    acc = *(int *)(b + 0xf0c) * (*(unsigned short *)(b + 0xf02) + y);
    arg = *(int *)(b + 0xf1c);
    bias = y;
    fn = Func_8000888;
    for (i = 0; i <= 0x9f; i++) {
        v = fn(tbl[(acc >> 16) & 0xff], arg);
        if (v < 0)
            v += 0xff;
        *o = ((unsigned int)(v << 8) >> 16) + bias;
        acc += step;
        o += 2;
    }
    (*(unsigned short *)(b + 0xf02))++;
    b[0xf00] ^= 1;
}
