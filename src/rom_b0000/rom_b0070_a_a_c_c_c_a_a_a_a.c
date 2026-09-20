/* Cluster Func_80b11c4..Func_80b11c4 extracted from goldensun/asm/rom_b0000/rom_b0070_a_a_c_c_c_a_a.s.
 *
 * Total .text for this TU = 156 bytes (= 0x9c).
 * Preserves the original ROM layout when slotted between
 * asm/rom_b0000/rom_b0070_a_a_c_c_c_a_b.o and asm/rom_b0000/rom_b0070_a_a_c_c_c_a_a_a_b.o in
 * goldensun/stage1.ld.
 *
 * Never attempted before batch 274. EXACT ON THE FIRST CANDIDATE. No pins, no flags,
 * no symbols.
 *
 * Reads two parallel 16-element arrays at +0x114 (pointers) and +0x154 (ints) and a
 * short array at +0x36e.
 *
 * ONE READING WORTH KEEPING: the loop bound is `(signed char)st->f3a7`, a CAST rather
 * than a signed field. The ROM has `ldrb` followed by `lsl / asr #24`, and that pair is
 * the tell -- a genuinely `signed char` FIELD would compile to `ldrsb` with no shifts.
 * Since `s8` is unsigned in this tree, an `unsigned char` member with an explicit
 * `(signed char)` cast at the use site is the only spelling that produces it.
 */
typedef struct {
    unsigned char pad000[0x114];
    unsigned char *sprites[16];
    int vals[16];
    unsigned char pad194[0x36e - 0x194];
    short f36e[1];
    unsigned char pad370[0x3a7 - 0x370];
    unsigned char f3a7;
} State;

extern unsigned char iwram_3001f2c[];

extern void _Sprite_SetAnim(unsigned char *sprite, int anim);
extern int _Func_807845c(int a, int b);

void Func_80b11c4(int a, int b, int c)
{
    State *st;
    int i;

    st = *(State **)iwram_3001f2c;
    if (a == 0)
        return;
    for (i = 0; i < (signed char)st->f3a7; i++) {
        if (i == b)
            _Sprite_SetAnim(st->sprites[i], 0x1e);
        else
            _Sprite_SetAnim(st->sprites[i], 1);
        st->vals[i] = 0x10000;
        if (_Func_807845c(st->f36e[i], c) == 0)
            st->vals[i] = 0xcccc;
    }
}
