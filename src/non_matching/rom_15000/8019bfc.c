/* Func_8019bfc -- 0x08019bfc, asm/rom_15000/rom_1908c_c_c_b_a.s (this function alone, so it
 * NON-MATCHING: 105 encodings of 135 differ (objcmp; size identical).
 * converts whole). The Thumb twin of the ARM Huffman decoder Func_8015430 (docs/ui-text.md):
 * st[0] = previous char (tree selector), st[1] = stream pointer, st[2] = bit buffer. Walks the
 * tree bitstream at p (sentinel-bit byte reader, 1 = leaf), consumes a data bit per internal
 * node, skips the 0-subtree by counting leaves, then reads the 12-bit leaf value stored
 * BACKWARDS below the tree start.
 *
 * NOT MATCHING: 105 differing of 135 encodings, SIZE IDENTICAL. Every instruction, branch and
 * block is in the ROM's order; the residue is REGISTER ALLOCATION of the loop state:
 *
 *                 b (bit)   bits (tree)   buf (data)   p (tree ptr)   outer const 1
 *     ROM         r2        r4            r0           r1             r3
 *     ours        r3        r2            r4           r0             r1
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_15000/8019bfc.c asm/rom_15000/rom_1908c_c_c_b_a.s --func Func_8019bfc
 *   python3 tools/tryc.py src/non_matching/rom_15000/8019bfc.c --ref asm/rom_15000/rom_1908c_c_c_b_a.s --full
 *
 * THE BLOCKER (from .17.lreg and .18.greg): global-alloc priority order is
 * b (84 refs / 36 insns) >> bits (71/112) > q > buf > const-1 > p ..., so b takes r3 and bits r2.
 * In the ROM bits lands in r4, AFTER r0-r3, ip AND lr are all taken -- i.e. bits was allocated
 * LAST among the loop state, and b was allocated after whatever took r3 (the outer constant 1,
 * the inner depth). Nothing tried moved bits down the order.
 *
 * INERT (all 105): permuting the declarations of p/leaf/src/buf/bits/b (720 compiles, floor
 * 105); splitting b into one variable per read site (b1/b2/b3 -- three pseudos, all still get
 * r3); `while` instead of `do` for the skip loop; inverting the skip loop's if; nesting the
 * refill test. WORSE: `bits = 1` hoisted to the top (109); a goto-built outer loop (134, and it
 * loses loop.c's constant hoisting into r7/r8).
 *
 * WHAT LANDED ON THE WAY (from 134 differing lines at the first tryc screen):
 *   - `c = st[0]; hi = c >> 8; c &= 0xff;` -- the ROM reuses one register for the selector, its
 *     low byte and the final result, and `hi` must be unsigned (lsr).
 *   - `p = HT[hi].base; p += HT[hi].offs[c];` as two statements -- the ROM loads base first.
 *   - the leaf address `n = count * 3; if (((n << 2) & 7) == 0)`, `leaf - (n >> 1)` in BOTH arms,
 *     and a post-decremented `*q--` in each arm (no cross-jumped tail).
 */
struct HuffTree {
    unsigned char *base;
    unsigned short *offs;
};

extern struct HuffTree HuffmanTreePointers[];

int Func_8019bfc(int *st)
{
    unsigned int hi;
    unsigned char *p;
    unsigned char *leaf;
    unsigned char *src;
    int buf;
    int bits;
    int b;
    int count;
    int depth;
    unsigned int n;
    unsigned int c;
    unsigned char *q;

    c = st[0];
    hi = c >> 8;
    c &= 0xff;
    p = HuffmanTreePointers[hi].base;
    p += HuffmanTreePointers[hi].offs[c];
    src = (unsigned char *)st[1];
    leaf = p - 1;
    buf = st[2];
    bits = 1;
    count = 0;
    for (;;) {
        b = bits & 1;
        bits >>= 1;
        if (b != 0 && bits == 0) {
            bits = *p++;
            b = bits & 1;
            bits = (bits >> 1) | 0x80;
        }
        if (b != 0)
            break;
        b = buf & 1;
        buf >>= 1;
        if (b == 0)
            continue;
        if (buf == 0) {
            buf = *src++;
            b = buf & 1;
            buf = (buf >> 1) | 0x80;
        }
        if (b == 0)
            continue;
        depth = 0;
        do {
            b = bits & 1;
            bits >>= 1;
            if (b != 0 && bits == 0) {
                bits = *p++;
                b = bits & 1;
                bits = (bits >> 1) | 0x80;
            }
            if (b == 0) {
                depth++;
            } else {
                count++;
                depth--;
            }
        } while (depth >= 0);
    }
    n = count * 3;
    if (((n << 2) & 7) == 0) {
        q = leaf - (n >> 1);
        c = *q-- << 4;
        c |= *q >> 4;
    } else {
        q = leaf - (n >> 1);
        n = *q-- & 0xf;
        c = *q | (n << 8);
    }
    st[2] = buf;
    st[0] = c;
    st[1] = (int)src;
    return c;
}
