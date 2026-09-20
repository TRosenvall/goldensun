/* Cluster Func_801e318..Func_801e318 extracted from goldensun/asm/rom_15000/rom_1de5c_a.s.
 *
 * Total .text for this TU = 176 bytes (= 0xb0). Never attempted before batch 274.
 * No pins, no flags. Its sibling Func_801e3c8 stays in assembly and is parked at
 * src/non_matching/rom_15000/801e3c8.c.
 *
 * FIVE LEVERS:
 *   - THE INNER BOUND MUST BE A VARIABLE (`w = 0x1e`). That is what keeps the ROM's
 *     `mov r3, r9 / cmp r3, #0 / beq` entry guard -- gcc does not propagate the 30 into
 *     the comparison.
 *   - THE `0xda0 + idx` MUST BE ITS OWN STATEMENT to get the ROM's register-offset
 *     `ldrb r3, [r5, r1]` rather than a folded pointer.
 *   - THE OUTER LOOP MUST BE `do { } while (--y != 0)`. A `for (y = 0; y < 0x14; y++)`
 *     reverses to `mov #0x13 / bge` instead of the ROM's `mov #0x14 / bne`.
 *   - REUSING `y` FOR THE TRAILING 256-BYTE PASS puts the counter in the ROM's r6; a
 *     separate `i` leaves it in r12 and costs three instructions for the high-register
 *     decrement.
 *   - THE STORED 0 MUST BE A NAMED LOCAL ASSIGNED BEFORE THE COUNTER AND POINTER. The
 *     ROM's preheader order is `mov r1, #0 / mov r6, #0xff / add r2, r5, r3`, and A
 *     LOOP-INVARIANT LITERAL 0 IS ALWAYS EMITTED AFTER THE SOURCE-ORDER PREHEADER
 *     STATEMENTS -- so no arrangement of a `*q = 0` loop reaches it. `z = 0; y = 0xff;
 *     q = base + 0xda0;` gives exactly that order.
 *
 * MEASURED (rom 84): first candidate 81/82; the `k` statement 86/59; reusing the counter
 * 84/2; six final-loop index spellings all 84/2; pointer and countdown forms 84/3; the
 * explicit `z` exact. -fno-schedule-insns2 moves the tie but breaks 18 elsewhere;
 * -fno-rerun-cse-after-loop is 85/62.
 */
extern unsigned char *iwram_3001e8c;

void Func_801e318(void)
{
    unsigned char *base;
    unsigned short *p;
    int flag;
    int w;
    int y;
    int x;
    unsigned int v;
    unsigned int k;
    unsigned char *q;
    int z;

    base = iwram_3001e8c;
    w = 0x1e;
    flag = base[0xea2];
    p = (unsigned short *)base;
    y = 0x14;
    do {
        for (x = 0; x < w; x++) {
            v = *p++ & 0x3ff;
            if (v - 0x80 <= 0x7f
                || (flag != 0 && v > 0x1ff && v <= 0x27f)) {
                k = ((v & 0xff) ^ 0x80) + 0xda0;
                base[k] |= 2;
            }
        }
    } while (--y != 0);
    z = 0;
    y = 0xff;
    q = base + 0xda0;
    do {
        if (*q == 1)
            *q = z;
        q++;
    } while (--y >= 0);
}
