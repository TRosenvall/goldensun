/* Func_8018850 / MeasureLayoutRuns (0x08018850) -- NON-MATCHING: 143 encodings of 219 differ (objcmp).
 * Reference 219 instructions, ours 219; size and relocations IDENTICAL, so 143 is a
 * true distance. Its 29-entry jump table reproduces ENTRY FOR ENTRY on the first
 * candidate.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     scratch_elev/b291/C/Func_8018850.park.c \
 *     asm/rom_15000/rom_17e88_c_c.s --func Func_8018850
 *
 * NOTE: this file also needs a TEXT/DATA SPLIT before it can land --
 * asm/rom_15000/rom_17e88_c_c.s carries a 32-byte .rodata tail (.L33e40, one
 * exported label, read from asm/rom_15000/rom_17e88_a_a.s) and stage1.ld:615
 * points at rom_17e88_c_c.o(.rodata).
 *
 * BLOCKER: the same global-alloc priority class as Debug_FaceTest, here a two-way
 * swap between `maxw` and the byte index `line * 2`: the ROM puts maxw in r7 and the
 * index in r12, gcc does the reverse. Also missing is the sp+4 spill slot for the
 * division-loop counter -- the ROM spills it around __divsi3 where gcc keeps it in
 * r11 (dead after `*out2 = height`). Writing `j = 0;` at the top of the function DOES
 * produce the ROM's frame (0x20) and its whole slot layout (style 0, spill 4, out2 8,
 * out1 0xc, b 0x10, a 0x18) -- reload assigns spill slots in PSEUDO-NUMBER order, so
 * declaration order decides which of the two spills gets the higher offset -- but it
 * costs more than it buys (196 differ).
 *
 * MEASURED AND INERT at 143: declaration order of maxw/c/style/line, `unsigned`
 * on width and n, merging t into maxw (144, worse).
 *
 * WHAT IS RIGHT, and is the reusable part:
 *   - CASE ORDER IS SOURCE ORDER AND THE ROM'S BLOCK ADDRESSES RECOVER IT:
 *     case 3, then 0xe/0x1c, then 8/0xa/0xf/0x11, then 9, then 0/1.
 *   - `case 0xe: case 0x1c:` FALLS THROUGH into the 8/0xa/0xf/0x11 group, which is
 *     why the ROM has two consecutive `i = (i + 1) & 0x1ff` blocks.
 *   - The `c > 0x1c` test is gcc's OWN casesi bound check -- do not write it.
 *   - `unsigned int line` is required: the ROM's `cmp r3,#2 / bhi` and `cmp r2,r10 /
 *     bls` are unsigned where a signed local gives bgt/ble.
 *   - The 0xc00 clamp on the __divsi3 result is an UNSIGNED compare (`bls`), so the
 *     quotient needs its own `unsigned int` while the pre-clamp `v < 0` test stays int.
 *   - `unsigned int c` for the character code, and Data_32224 indexed as
 *     `Data_32224[(c - 0x20) * 16]`, give the ROM's `sub r2,#0x20 / lsl r2,#5`.
 *   - The ring is a STRUCT with a member array at 0xeb0 -- that is what produces
 *     `ldrh r2,[r4,r3]` with the base in a register and the whole displacement in the
 *     index, and it is what keeps the ring pointer in a LOW register.
 *   - `*out3 = 0;` written as a bare literal is what produces the ROM's pooled
 *     HImode zero (`ldr r3, .L189ec @ 0`). Do not reach for an int carrier here.
 *   - The dead `str r3,[sp,#0]` of the 0xeac style value falls out on its own.
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

void Func_8018850(int i, int *out1, int *out2, unsigned short *out3)
{
    struct R *ring;
    unsigned short a[4];
    unsigned short b[4];
    unsigned int c;
    unsigned int maxw;
    int w;
    int width;
    int n;
    int height;
    unsigned int line;
    int style;
    int j;
    int t;
    int v;
    unsigned int u;

    ring = iwram_3001e8c;
    line = 0;
    height = 0xf;
    n = 0;
    maxw = 0;
    width = 0;
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
                if (maxw < width)
                    maxw = width;
                if (line <= 2)
                    line++;
                height += 0xf;
                n = 0;
                width = 0;
                break;
            case 0xe:
            case 0x1c:
                i = (i + 1) & 0x1ff;
            case 8:
            case 0xa:
            case 0xf:
            case 0x11:
                i = (i + 1) & 0x1ff;
                break;
            case 9:
                ring->feac = ring->text[i];
                i = (i + 1) & 0x1ff;
                break;
            case 0:
            case 1:
                a[line] = ++n;
                b[line] = width;
                if (maxw < width)
                    maxw = width;
                if (ring->fea4 != 0)
                    maxw += 2;
                *out1 = maxw;
                *out2 = height;
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
                        if (u > 0xc00)
                            u = 0x200;
                        *out3 = u;
                    }
                    out3++;
                    j++;
                } while (j <= line);
                return;
            }
        }
    }
}
