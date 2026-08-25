/* GetSpriteVoice extracted from goldensun/asm/rom_8a000/rom_91584_a_c_a_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * UNPARKED. The park was 20 lines against the ROM's 21, which is the
 * "ours is SHORTER" signature -- gcc saving an instruction the original
 * compiler did not.
 *
 * TWO ARMS RETURN ZERO AND THE ROM SHARES ONE BLOCK FOR BOTH, placed BEFORE
 * the non-zero arm:
 *
 *     cmp r0, #0xff / bne <add>
 *     <zero>: mov r0, #0 / b <out>
 *     <add>:  mov r3, #0x80 / lsl r3, #1 / add r0, r3
 *
 * Written as two separate `return 0;` statements gcc emits one shared block
 * too -- but places it AFTER the non-zero arm, and inverts the branch. The
 * source has to name the join and put the zero first:
 *
 *     if (p != 0xff) goto add;
 *   zero:
 *     return 0;
 *   add:
 *     return p + (0x80 << 1);
 *
 * Getting the join right alone is not enough; the ORDER of the two blocks is
 * what the `bne` against `beq` reports, and it is read off which arm falls
 * through.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char *GetSpriteVoiceEntry(void);

int GetSpriteVoice(void)
{
    unsigned char *e;
    unsigned int base;
    int p;

    base = (unsigned int)&gState;
    base += 0x20a;
    if (*(unsigned char *)base == 0)
        goto zero;
    e = GetSpriteVoiceEntry();
    p = e[2];
    if (p != 0xff)
        goto add;
zero:
    return 0;
add:
    return p + (0x80 << 1);
}
