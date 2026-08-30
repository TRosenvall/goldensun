/*
 * Func_8019908 (RegisterCallback) -- asm/rom_15000/rom_1908c_c_a_c_c_c_b.s
 * SPLIT OUT this round; byte-neutral, verified.
 *
 * BLOCKER: the order in which the two parameters are copied to callee-saved
 * registers. 27 lines against 27, 9 differing:
 *
 *      rom   mov r7, r1 / ldr r1, [r3] / mov r6, r0
 *      ours  mov r6, r0 / ldr r0, [r3] / mov r7, r1
 *
 * The ROM saves the second parameter, loads the global, then saves the first.
 *
 * SETTLED, and two of these are worth reusing:
 *
 *   THE FUNCTION IS VOID. Its annotation says it returns the slot index, and it
 *   does compute one, but the epilogue is `pop {r5,r6,r7} / pop {r0} / bx r0` --
 *   lr is popped into r0, which gcc cannot do if r0 carries a return value.
 *   Declaring it `int` and returning the index gives `pop {r1} / bx r1`. The
 *   epilogue register is a reliable void/non-void tell.
 *
 *   The id table is indexed with the WALKING OFFSET as the addressing base:
 *   `ldrh r3, [r4, r1]` where r4 walks 0x12dc, 0x12de, ... and r1 holds the
 *   iwram pointer. Spelling it `*(unsigned short *)(q + (int)b)` with q the
 *   walking `unsigned char *` reproduces it; the natural subscript does not.
 *
 *   The loop bound 8 and the counter must be assigned BEFORE the second
 *   pointer, or `add r2, r1, r3` is emitted ahead of them.
 *
 * TRIED AND REJECTED: copying both parameters into locals assigned in the ROM's
 * order (9 differing, unchanged).
 */
extern unsigned char *iwram_3001e8c;

void Func_8019908(int cb, int id)
{
    unsigned char *b;
    int *p;
    unsigned char *q;
    int i;
    int n;

    b = iwram_3001e8c;
    q = (unsigned char *)0x12dc;
    n = 8;
    i = 0;
    p = (int *)(b + 0x12bc);
    do {
        if (*(unsigned short *)(q + (int)b) == 0) {
            *p = cb;
            *(unsigned short *)(q + (int)b) = id;
            break;
        }
        i++;
        p++;
        q += 2;
    } while (i != n);
}
