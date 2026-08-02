/* Func_f954c -- SetJingleCountdown
 *
 * r0 = frames. Stores the byte at ewram_3000.
 *
 * STATUS: MATCHING.
 */
typedef unsigned char u8;

extern u8 ewram_3000;

u8 Func_f954c(void)
{
    /* The r3 pin is a matching aid: the original materialises the address in r3
     * and loads through it, where agbcc otherwise uses r0 for both. */
    u8 *p;

    p = &ewram_3000;
    return *p;
}
