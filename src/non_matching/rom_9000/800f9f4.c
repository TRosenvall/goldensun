/* DecodeMetatileset -- asm/rom_9000/rom_f9cc_a_c_b.s
 *
 * BLOCKER: REGISTER ROTATION (allocator preference). 41 of 78, two short.
 *
 * A three-mode decoder: plain halfword copy, byte-plane merge with a running
 * xor, and halfword running xor. THE ENTIRE SWITCH REPRODUCES INSTRUCTION FOR
 * INSTRUCTION -- gcc builds the same comparison chain the ROM has (test 1,
 * then >1, then ==0, then ==2, with the same fallthroughs and the same `b`
 * to the common exit), which is the part that looked hardest. The signed
 * `(n - 1) / 2` and the odd-n guard are exact.
 *
 * What differs is which register holds what, from instruction 4 onward:
 *
 *     rom    count->r4  src->r6  dst->r5
 *     ours   count->r6  src->r2  dst->r4
 *
 * MEASURED:
 *   separate `unsigned short *` and `unsigned char *` views of the source
 *                                                       76 lines, 41 differ
 *   ONE `unsigned char *` reloaded per mode, cast at each use
 *                                                       75 lines, 46 differ
 *
 * The second is a clean negative and it is worth stating, because it looked
 * like the better model of the ROM: the ROM loads `ewram_2010002` into r6 and
 * RELOADS THE SAME REGISTER in mode 1, which reads as one variable reassigned
 * rather than two views. Collapsing to one pointer makes it WORSE by five
 * differences and three lines. Matching the ROM's register reuse in the source
 * is not the same as matching its register allocation.
 *
 * THIS IS THE THIRD FUNCTION IN ONE ROUND stopped by the same thing --
 * Func_80c0228 and this one, after Func_8029274 and Func_80f4100 in earlier
 * rounds. The pattern across all four: the loop-body candidate class yields
 * outright matches when the body has FEW simultaneously-live locals (three
 * matched on first or near-first contact), and stalls on a whole-function
 * register rotation once there are four or more. That is a usable screening
 * signal, not just a blocker: count the ROM's live values before starting.
 */
extern unsigned char ewram_2010001;
extern unsigned char ewram_2010002[];
extern unsigned short ewram_2020000[];

void DecodeMetatileset(int n)
{
    int count;
    int i;
    unsigned short *s;
    unsigned short *d;
    unsigned char *b;
    unsigned char *b2;
    int prev;

    count = (n - 1) / 2;
    s = (unsigned short *)ewram_2010002;
    d = ewram_2020000;
    if (n & 1) {
        switch (ewram_2010001) {
        case 0:
            for (i = 0; i < count; i++) {
                *d = *s;
                s++;
                d++;
            }
            break;
        case 1:
            b = ewram_2010002;
            i = 0;
            prev = 0;
            b2 = b + count;
            while (i < count) {
                prev = ((*b << 8) | *b2) ^ prev;
                i++;
                *d = prev;
                b2++;
                b++;
                d++;
            }
            break;
        case 2:
            prev = 0;
            if (count > 0) {
                i = count;
                do {
                    prev = *s ^ prev;
                    i--;
                    *d = prev;
                    s++;
                    d++;
                } while (i != 0);
            }
            break;
        }
    }
}
