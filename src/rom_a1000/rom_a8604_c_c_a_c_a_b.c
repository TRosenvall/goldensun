/* Cluster Func_80a9cbc..Func_80a9cbc extracted from goldensun/asm/rom_a1000/rom_a8604_c_c_a_c_a.s.
 *
 * Total .text for this TU = 60 bytes (= 0x3c).
 * Preserves the original ROM layout when slotted between
 * asm/rom_a1000/rom_a8604_c_c_a_c_a_a.o and
 * asm/rom_a1000/rom_a8604_c_c_a_c_a_c.o in goldensun/stage1.ld.
 *
 * Walks 32 object slots at [iwram_3001f2c]+0x48 and, for each non-null one,
 * writes two halfwords and calls Func_80a17c4.
 *
 * THE ONE THING THAT MATTERS HERE is that the walking pointer is initialised
 * from a DERIVED expression, `(unsigned char **)(p + 0x48)`, in a single
 * statement.  Split into `q = p;` followed by `q += 0x48;`, gcc coalesces q
 * with p, loads the global straight into the callee-saved register and drops
 * the ROM's `mov r5, r3` -- two instructions of 28, and nothing else wrong.
 * Writing it as one derived expression makes the copy real:
 *
 *      ldr r3, [r3, #0x0]      <- the global lands in a scratch register
 *      mov r2, #0xf8
 *      mov r5, r3              <- and is COPIED to the walking register
 *      mov r8, r2
 *      add r5, #0x48
 *
 * This is the inverse of the pointer-walk lever: a destructive `+=` gives the
 * walk, but only a derived initialiser gives the copy that precedes it.
 *
 * The constant 0xf8 is assigned before the pointer, matching the ROM's
 * statement order; `t = a;` before the first store reproduces `mov r3, r8`,
 * which is forced because r8 is not addressable by `strh`.
 */
extern unsigned char *iwram_3001f2c;
extern void Func_80a17c4(void *x);

void Func_80a9cbc(void)
{
    unsigned char *p;
    unsigned char **q;
    unsigned char *x;
    int a;
    int b;
    int i;
    int t;

    p = iwram_3001f2c;
    a = 0xf8;
    q = (unsigned char **)(p + 0x48);
    b = 0xa8;
    i = 0x1f;
    do {
        x = *q++;
        if (x != 0) {
            t = a;
            *(unsigned short *)(x + 6) = t;
            *(unsigned short *)(x + 8) = b;
            Func_80a17c4(x);
        }
        i--;
    } while (i >= 0);
}
