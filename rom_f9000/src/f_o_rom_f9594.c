/* Func_f9594 -- GetCurrentMusicId
 *
 * Takes no arguments. Returns the byte at ewram_303c -- the id Func_f9080 last
 * accepted as music.
 *
 * STATUS: MATCHING.
 */
typedef unsigned char u8;

extern u8 ewram_303c;

u8 Func_f9594(void)
{
    /* The r3 pin is a matching aid: the original materialises the address in r3
     * and loads through it, where agbcc otherwise uses r0 for both. */
    register u8 *p asm("r3");

    p = &ewram_303c;
    return *p;
}
