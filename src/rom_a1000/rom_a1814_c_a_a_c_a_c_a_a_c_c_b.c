/* Cluster Func_80a1e38..Func_80a1e38 extracted from goldensun/asm/rom_a1000/rom_a1814_c_a_a_c_a_c_a_a_c.s.
 *
 * Total .text for this TU = 316 bytes (= 0x13c). Never attempted before batch 277.
 * No pins, no flags. 146 instructions.
 *
 * NESTING A RETRY LOOP AS A REAL INNER LOOP IS A THREE-IN-ONE LEVER, and this function is
 * the clearest demonstration of why a one-lever-at-a-time search can stall completely.
 *
 * TWENTY-ONE SPELLINGS measured first, every one of them a single local change -- five
 * declaration orders, four `n++` spellings, five positions for `n = 0`, three pointer-copy
 * variants for src/dst/p, a `volatile` parameter, and `&list` -- and ALL TWENTY-ONE left
 * 110 differing. The restructure went to 0.
 *
 * The change is only this:
 *
 *     while (cats[j] != 0xff) { ...; if (best) { move } else j++; }     110 differing
 *     for (j = 0; cats[j] != 0xff; j++) { for (;;) { ...;
 *                                if (best == 0) break; move } }        exact
 *
 * The two compile to the SAME control flow. What differs is everything downstream of it:
 *
 *   (a) it raises `n`'s reference depth, so `n` beats the `list` parameter for r11 and
 *       `list` spills to the ROM's sp+0xc;
 *   (b) it makes `j` a genuine BIV, so `cats + j`'s giv initialiser is emitted by
 *       `strength_reduce` AFTER the LICM hoists -- the ROM's `mov r10, r9 / mov r7, r5`
 *       order;
 *   (c) it fixes a bare r2/r3 role swap in the category compare.
 *
 * All three were separately unreachable. Read this with the recorded rule that a `goto` loop
 * suppresses strength reduction: that is the same axis seen from the other end -- here the
 * ROM WANTS the biv, so the loop has to be a real loop for `strength_reduce` to see one.
 * When a residue spans register allocation AND a preheader order AND an argument-register
 * swap at once, the loop STRUCTURE is the common cause, and no amount of local respelling
 * will reach any of the three.
 */
extern void Func_a1f74(int order, unsigned char *buf);
extern unsigned char *_GetItemInfo(int id);

int Func_80a1e38(unsigned short *list, int order)
{
    unsigned short out[15];
    unsigned short tmp[15];
    unsigned char cats[32];
    unsigned char *info;
    int i, j, n, count, best, bestIdx, v;

    bestIdx = 0;
    n = 0;
    Func_a1f74(order, cats);
    for (i = 0; i < 15; i++)
        tmp[i] = list[i];
    count = 0;
    for (i = 0; i < 15; i++)
        if (tmp[i] != 0)
            count++;
    for (i = count; i < 15; i++)
        out[i] = 0;
    for (j = 0; cats[j] != 0xff; j++) {
        for (;;) {
            best = 0;
            for (i = 0; i < count; i++) {
                if (tmp[i] != 0) {
                    info = _GetItemInfo(tmp[i]);
                    if ((cats[j] & 0x7f) == info[2]) {
                        if ((cats[j] & 0x80) == 0 || (tmp[i] & 0x200) != 0) {
                            v = tmp[i] & 0x1ff;
                            if (best < v) {
                                bestIdx = i;
                                best = v;
                            }
                        }
                    }
                }
            }
            if (best == 0)
                break;
            out[n] = tmp[bestIdx];
            tmp[bestIdx] = 0;
            n++;
        }
    }
    for (i = 0; i < count; i++)
        list[i] = out[i];
    return 1;
}
