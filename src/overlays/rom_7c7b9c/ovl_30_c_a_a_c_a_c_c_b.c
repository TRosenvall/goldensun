/* Cluster OvlFunc_943_200b464..OvlFunc_943_200b464 extracted from goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_c.s.
 *
 * Preserves the original ROM layout when slotted into
 * goldensun/overlays/rom_7c7b9c/overlay.ld.
 *
 * Given a small selector, pick a base flag id and return the first entry of a
 * word table whose corresponding flag is set, or 0 if none of nine is.
 *
 * ONE OF THREE BYTE-IDENTICAL COPIES, one per overlay --
 * OvlFunc_881_200b448 (rom_77a7c8), OvlFunc_943_200b464 (rom_7c7b9c) and
 * OvlFunc_944_200915c (rom_7ca63c). They differ only in the name and in which
 * overlay-local `.L` table they read, so the C is the same file three times.
 * Found with tools/find_twins.py rather than by noticing.
 *
 * THE SELECTOR IS UNSIGNED, and that is the whole difficulty. gcc lays a small
 * switch out as an if-chain, and the shape of that chain reports the type:
 *
 *     signed    cmp r0, #1 / beq / cmp r0, #1 / bgt / cmp r0, #0 / beq ...
 *     unsigned  cmp r0, #1 / beq / cmp r0, #1 / bcc / cmp r0, #2 / beq ...
 *
 * The ROM has `bcc`. With a signed parameter the chain also grows four
 * instructions, 47 against 43, because gcc has to handle negatives the
 * unsigned form rules out. So the `bcc` is not a detail of the comparison --
 * it changes the whole switch.
 *
 * The table is an overlay-local `.L` symbol, reached with a gcc asm-label
 * since C cannot spell the name. The loop is gcc's ordinary rotated form and
 * needed no goto.
 */
extern int L5b08[] __asm__(".L5b08");

int OvlFunc_943_200b464(unsigned int kind)
{
    int base;
    unsigned int i;

    base = 0;
    switch (kind) {
    case 0: base = 0x92c; break;
    case 1: base = 0x935; break;
    case 2: base = 0x917; break;
    case 3: base = 0x99 << 4; break;
    }
    for (i = 0; i <= 8; i++) {
        if (__GetFlag(base + i) != 0)
            return L5b08[i];
    }
    return 0;
}
