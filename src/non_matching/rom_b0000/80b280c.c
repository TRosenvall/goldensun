/* Func_80b280c -- 0x080b280c, asm/rom_b0000/rom_b0070_c_c_a_a_a_c.s
 *
 * Counts how many entries of the short array at iwram_3001f2c + 0x36e satisfy
 * Func_80b27b0, using the signed byte at +0x3aa as the second argument and the
 * signed byte at +0x3a7 as the loop bound. The bound is re-read every
 * iteration; the key is held across the call.
 *
 * 53 of 55. Candidate below.
 *
 * BLOCKER: REGISTER PRESSURE, and the ROM has MORE of it than any spelling I
 * can write. It pushes TWO callee-saved high registers --
 * `mov r7, r10 / mov r6, r8 / push {r6, r7}` -- and additionally spills the
 * running count to the stack across the call (`str r2, [sp]` / `ldr r2, [sp]`).
 * Every candidate here needs only ONE high register, so the prologue differs at
 * instruction 1 and every register assignment downstream rotates. That is why
 * the count is 53 of 55 rather than something proportional to the real
 * disagreement: the whole body is shifted, not wrong.
 *
 * WHAT THE ROM IS DOING WITH THE EXTRA REGISTER, and it is a cost decision, not
 * a source one. gcc strength-reduces the array access into a base plus a
 * stepping offset, and the ROM's build split it as
 *
 *     add r3, r5, #2      <- base  = s + 2, parked in r8
 *     mov r6, #0xdb
 *     lsl r6, #2          <- offset = 0x36c, stepping by 2
 *     ldrsh r0, [r1, r6]
 *
 * where 0x36c is `0xdb << 2` and so costs `mov`+`lsl` with NO POOL ENTRY, while
 * the arithmetically obvious base `s + 0x36e` needs `ldr rN, =0x36e` because
 * 0x36e is not a shifted eight-bit value. The ROM absorbed the odd +2 into the
 * base to keep the constant shiftable. Ours emits the pool load every time.
 *
 * THE POOL ENTRY IS THE DIAGNOSTIC. The reference has no `=0x36e`; any
 * candidate that does has lost before the registers are even considered.
 *
 * TRIED, and the tie is the point -- FIVE spellings across 53 and 54 with no
 * structural movement:
 *   a  address expression inline, `((short *)(s + 0x36e))[i]`          54
 *   b  named `short *arr = (short *)(s + 0x36e);` before the loop      53
 *   c  `while` loop with `*(short *)(s + 0x36e + i * 2)`               54
 *   d  the ROM's own split as a named base, `((short *)(s + 2))[0x1b6 + i]` 54
 *   e  the same split written inline                                   54
 *
 * That (d) and (e) do NOT reproduce the ROM is the informative result. Handing
 * gcc the exact base/offset decomposition the ROM used still produces the pool
 * load and the single high register, which means the split is NOT reachable by
 * writing the address differently -- gcc re-derives its own induction variables
 * from whatever it is given, and it chooses the base first and the constant
 * second. The choice is made in loop.c's strength reduction on the basis of
 * register cost, and the source has no vote.
 *
 * SO THIS IS THE ALLOCATION-ORDER CLASS, not an address-spelling one. It
 * belongs with the parks that need a differently configured gcc rather than a
 * different C. The specific prediction, if anyone rebuilds the compiler: the
 * ROM's build was willing to spend a second callee-saved high register on the
 * induction base where ours spends a pool load, which is what a different
 * REG_ALLOC_ORDER or a different `-fcall-used-*` set would change.
 *
 * NOT tried, and worth one screen if this is revisited: a struct declaration
 * for iwram_3001f2c rather than `unsigned char *`, on the chance that a
 * distinct alias set changes what loop.c believes about the re-read bound at
 * +0x3a7 and therefore the pressure. Every candidate above used the
 * `unsigned char *` convention taken from the sibling
 * src/non_matching/rom_b0000/80b2ed8.c.
 */

extern unsigned char *iwram_3001f2c;
extern int Func_80b27b0(int a, int b);

int Func_80b280c(void)
{
    unsigned char *s;
    short *arr;
    int count;
    int i;
    int key;

    s = iwram_3001f2c;
    key = (signed char)s[0x3aa];
    count = 0;
    arr = (short *)(s + 0x36e);
    for (i = 0; i < (signed char)s[0x3a7]; i++) {
        if (Func_80b27b0(arr[i], key))
            count++;
    }
    return count;
}
