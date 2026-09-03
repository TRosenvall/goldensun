/* Func_80a3e28  --  0x080a3e28
 *
 * The tail of goldensun/asm/rom_a1000/rom_a1814_c_a_c_c_c_c_c_a_c.s; the three
 * functions ahead of it stay in _c_a.s and the pool of one of them with them.
 * No file-scope data, so the split is a pure text cut, verified byte-neutral
 * before this landed.
 *
 * The ROM annotation names it LoadInventoryIcons and documents the second
 * parameter as selecting the small or large icon set -- which independently
 * confirmed the `== 0` sense of the guard before any screen was run.
 *
 * MATCHED ON THE FIRST CANDIDATE, byte-identical, with no probing at all. The
 * finding is HOW the neighbour was found, and it sharpens the recorded habit.
 *
 * GREP ON CALLEE NAMES AND ON THE GLOBAL, NOT ON THE TARGET'S OWN STEM. The
 * stem-sibling here is literally the function this one tail-calls, and it was
 * structurally LESS useful, because it walks the same array to clear slots
 * rather than to make this call. The useful neighbour was in a different bank
 * entirely: a solved function that calls BOTH of this one's callees over the
 * same global's node array, with the same post-increment read, the same
 * skip-if-zero guard and the same descending counter. This function is that one
 * with a different loop bound, a two-arm guard in place of a fixed kind
 * constant, and one extra parameter. Transplanting its variable set,
 * declaration order and loop spelling gave a byte match immediately.
 * CALLEE-SET IDENTITY BEATS FILENAME ADJACENCY.
 *
 * Two confirmations, both measured. The duplicated arms are duplicated source:
 * each writes out the full node load and call, differing only in one constant,
 * and the ROM's branch over the second copy is gcc's own arm join rather than
 * cross-jumping. And the node pointer is ONE local reused across both arms
 * without triggering the register-rotation symptom -- because the two
 * assignments are in MUTUALLY EXCLUSIVE arms. The "never reuse one local for
 * two roles" rule is about roles that are simultaneously live, not about the
 * arms of an if.
 *
 * The two high registers came out with no lever at all: two values live across
 * a call, with r4 removed from the callee-saved set by -fcall-used-r4.
 */
extern unsigned char *iwram_3001f2c;
extern void _Func_801bcd4(int a, int b, int c, int d);
extern void Func_80a3d24(void *p);

void Func_80a3e28(unsigned short *src, int flag)
{
    unsigned char *p;
    unsigned char **q;
    unsigned char *node;
    unsigned short *s;
    int i;
    int v;

    p = iwram_3001f2c;
    q = (unsigned char **)(p + 0x48);
    s = src;
    i = 0xe;
    do {
        v = *s++;
        if (v != 0) {
            if (flag == 0) {
                node = *q;
                _Func_801bcd4(2, v, node[0xe], 0);
            } else {
                node = *q;
                _Func_801bcd4(7, v, node[0xe], 0);
            }
        }
        i--;
        q++;
    } while (i >= 0);
    Func_80a3d24(src);
}
