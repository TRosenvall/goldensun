/* Func_80c24b0  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_b5000/rom_c1a34_a_c.s
 * Best screen: 17 instructions in disagreeing regions, of 26 (streams same length).
 *
 * BLOCKER CLASS: register allocation, and ONLY that. The instruction sequence
 * is exact -- every mnemonic and every immediate lines up -- and r0, r1 and r2
 * are rotated among themselves. The count is high because the rotation touches
 * most of the function, not because much is wrong.
 *
 *      rom   mov r2, #0xa6 / ldr r0, [r3] / lsl r2, #3 / add r1, r0, r2
 *      ours  mov r0, #0xa6 / ldr r1, [r3] / lsl r0, #3 / add r2, r1, r0
 *
 * Everything structural is already right and should not be re-derived: the
 * constant assigned before the global load, the destructive `p += j` that lets
 * the loaded pointer become the walking one, the shared zero used for the
 * halfword store and the three word stores, and the loop counting down with the
 * decrement before the store and a `p -= 2` walk.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char *iwram_3001e74;

void Func_80c24b0(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned char *g;
    unsigned int k;
    unsigned int j;
    int z;
    int n;

    k = 0xa6 << 3;
    p = iwram_3001e74;
    q = p + k;
    j = 0x8f << 2;
    g = (unsigned char *)&gState + j;
    z = 0;
    *(short *)g = z;
    *(int *)q = z;
    *(int *)(q + 4) = z;
    *(int *)(q + 8) = z;
    j = 0x542;
    n = 3;
    p += j;
    do {
        n--;
        *(short *)p = z;
        p -= 2;
    } while (n >= 0);
}
