/* Func_8018a50 / DrawLayoutRuns (0x08018a50) -- NON-MATCHING: 149 encodings of 265 differ (objcmp).
 * Reference 265 instructions, ours 263; reference 604 bytes, ours 600. A count is NOT
 * a distance while those disagree.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     scratch_elev/b291/C/Func_8018a50.park.c \
 *     asm/rom_15000/rom_17e88_c_c.s --func Func_8018a50
 *
 * Needs the same TEXT/DATA SPLIT as its twin (see Func_8018850.park.c).
 *
 * THE TWINS ARE ONE JOB AND THE READING TRANSFERS WHOLE: identical run walk,
 * identical 29-entry jump table shape, identical glyph/space/control dispatch,
 * identical `+0x13 / lsr #3 / lsl #3 / -0x10` rounding and __divsi3 column loop.
 * Every lever in Func_8018850.park.c applies here unchanged.
 *
 * BUT THE CASE ORDER IS DIFFERENT, and that is the twins' one real divergence:
 *   Func_8018850:  3, 0xe/0x1c, 8/0xa/0xf/0x11, 9, 0/1
 *   Func_8018a50:  3, 0, 1, 0xe/0x1c, 9, 8/0xa/0xf/0x11
 * In this one cases 0 and 1 are SEPARATE blocks with the same five opening
 * instructions -- 0 leaves the loop, 1 continues it -- and `case 0xe: case 0x1c:`
 * cannot fall through because case 9 sits between it and its target, so its body
 * ends in a bare `goto` into the 8/0xa/0xf/0x11 label. Case 9 DOES fall through
 * into that group.
 *
 * THE PROLOGUE LEVER, worth 250 -> 149: the five counter zeroes must be written
 * BEFORE the sixteen `h[k] = 0xf;` stores, and `line = 0;` must come FIRST among
 * them. This is the same statement-order lever that took Debug_IconTest from 13
 * differing to exact in this batch. The 0x40-byte style stack is `unsigned int
 * h[16]`, written as sixteen assignments -- an aggregate initialiser costs 60 bytes.
 *
 * MEASURED: zeroes after the array init 250; all zeroes before it 151; line = 0
 * last among them 149 (this file); line = 0 after the array init 245 (but with the
 * instruction count EXACT at 265, so that variant is where to resume).
 *
 * RESIDUE: a whole-function register permutation of the same global-alloc priority
 * class as its twin -- the ROM has out3 in r8, &h in r1, out2 in r14 and `line` in
 * memory at sp+4; gcc has out3 in r10, &h in r4, and spills out2 instead of line.
 */

struct R {
    unsigned char pad000[0xea4];
    unsigned char fea4;
    unsigned char padea5[7];
    unsigned short feac;
    unsigned char padeae[2];
    unsigned short text[0x200];
};

extern struct R *iwram_3001e8c;
extern unsigned short Data_32224[];

void Func_8018a50(int i, unsigned int *out1, unsigned int *out2, unsigned short *out3)
{
    struct R *ring;
    unsigned int h[16];
    unsigned short a[4];
    unsigned short b[4];
    unsigned int line;
    unsigned int maxw;
    unsigned int width;
    unsigned int n;
    unsigned int page;
    unsigned int c;
    unsigned int j;
    int w;
    int style;
    int t;
    int v;
    unsigned int u;
    unsigned int *p;

    ring = iwram_3001e8c;
    line = 0;
    h[0] = 0xf;
    h[1] = 0xf;
    h[2] = 0xf;
    h[3] = 0xf;
    h[4] = 0xf;
    h[5] = 0xf;
    h[6] = 0xf;
    h[7] = 0xf;
    h[8] = 0xf;
    h[9] = 0xf;
    h[10] = 0xf;
    h[11] = 0xf;
    h[12] = 0xf;
    h[13] = 0xf;
    h[14] = 0xf;
    h[15] = 0xf;
    maxw = 0;
    width = 0;
    n = 0;
    page = 0;
    while (1) {
        c = ring->text[i];
        i = (i + 1) & 0x1ff;
        if (c > 0x1f) {
            if (c == 0x20) {
                width += 5;
                n++;
            } else {
                w = Data_32224[(c - 0x20) * 16];
                style = ring->feac;
                if (style == 1 || style == 5)
                    w++;
                width += w;
            }
        } else {
            switch (c) {
            case 3:
                a[line] = ++n;
                b[line] = width;
                if (page == 0 && maxw < width)
                    maxw = width;
                if (line <= 2)
                    line++;
                h[page] += 0xf;
                n = 0;
                width = 0;
                break;
            case 0:
                a[line] = ++n;
                b[line] = width;
                if (page == 0 && maxw < width)
                    maxw = width;
                page++;
                goto finish;
            case 1:
                a[line] = ++n;
                b[line] = width;
                if (page == 0 && maxw < width)
                    maxw = width;
                page++;
                break;
            case 0xe:
            case 0x1c:
                i = (i + 1) & 0x1ff;
                goto bump;
            case 9:
                ring->feac = ring->text[i];
            case 8:
            case 0xa:
            case 0xf:
            case 0x11:
            bump:
                i = (i + 1) & 0x1ff;
                break;
            }
        }
    }
finish:
    if (ring->fea4 != 0)
        maxw += 2;
    j = 0;
    p = h;
    while (j < page) {
        if (j == 0)
            *out2 = h[0];
        else if (*out2 < *p)
            *out2 = *p;
        j++;
        p++;
    }
    *out1 = maxw;
    t = ((maxw + 0x13) >> 3) * 8 - 0x10;
    if (out3 == 0)
        return;
    j = 0;
    do {
        if (a[j] <= 1) {
            *out3 = 0;
        } else {
            v = t - b[j] - 4;
            if (v < 0)
                v = 0;
            u = (v << 8) / (a[j] - 1);
            *out3 = u;
        }
        out3 += 1;
        j++;
    } while (j <= line);
}
