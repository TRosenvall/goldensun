/* Func_8011fd8 -- asm/rom_9000/rom_11ce0_c_a_a.s
 *
 * BLOCKER: REGISTER ROTATION / birth order. 34 of 45, LENGTH EXACT.
 *
 * A tile lookup: two fixed-point coordinates shifted down 16 and divided by 16
 * (signed, via the `+0xf / asr #4` idiom), combined as y*128 + x, used to index
 * a table selected from one of four slots, then a byte from that entry indexes
 * ewram_202c000 and the low nibble is returned. Every arithmetic step, both
 * signed divisions, the default gBuffer table and the guarded override all
 * reproduce.
 *
 * The residue starts at instruction 2 and it is birth order:
 *
 *     rom    ldr r5, [r3]   (the base, FIRST) / mov r4, r2 / mov r6, r0
 *     ours   mov r5, r0     (a parameter first) / ldr r0, [r3]
 *
 * and from there the register assignment diverges throughout. The ROM also
 * keeps its table load register-offset -- `ldr r0, [r5, r3]`, base in one
 * register and the computed offset in another -- where ours folds the base
 * into the offset and loads with an immediate.
 *
 * MEASURED:
 *   offset expression written inline in the load     45 lines, 34 differ
 *   the offset assigned to its own named local       44 lines, 35 differ
 *
 * The second is a negative worth recording, because naming the byte offset is
 * a lever that WORKS elsewhere -- it restored register-offset addressing on
 * Func_8011b00 and on Sprite_DeleteLayer, closing both. Here it costs a line
 * and gains nothing.
 *
 * The difference between those cases and this one: there the base register was
 * already correct and only the addressing mode was wrong. Here the base is in
 * the wrong register to begin with, because it is born after two parameter
 * copies instead of before them. Fixing an addressing mode downstream of a
 * wrong register assignment does not help -- the third instance of that
 * pattern, after Func_8079664 and Func_80ae9f0 in this batch.
 *
 * NOT TRIED: nothing further. The base is the first statement in the C already;
 * gcc reorders it behind the parameter saves regardless.
 */
extern int iwram_3001e70;
extern unsigned char gBuffer[];
extern unsigned char ewram_202c000[];

int Func_8011fd8(int sel, int px, int py)
{
    char *base;
    unsigned char *tbl;
    int x;
    int y;

    base = (char *)iwram_3001e70;
    x = px >> 16;
    y = py >> 16;
    tbl = gBuffer;
    if (base != 0)
        tbl = *(unsigned char **)(base + (sel & 3) * 48 + 0x98 * 2);
    x = x / 16;
    y = y / 16;
    return ewram_202c000[tbl[((y << 7) + x) * 4 + 3] * 4] & 0xf;
}
