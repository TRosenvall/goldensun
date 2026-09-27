/* Func_80ac8fc (CollectDjinn)  --  0x080ac8fc, split out of
 * asm/rom_a1000/rom_aa538_c_c_c_a.s; Func_80ab314 and Func_80ab5e4 stay in _a.s.
 * Matched from scratch.
 *
 * Read the loop-invariant word IN THE CONDITION rather than caching it in a
 * local: inner-loop hoisting then moves the load, and the outer loop reduces
 * its address to the ROM's walking pointer (a cached `set` local: 115 differing).
 */
struct Unit {
    unsigned char pad000[0xf8];
    unsigned int djinnHave[4];
    unsigned int djinnSet[4];
};

extern struct Unit *_GetUnit(int id);

int Func_80ac8fc(unsigned short *dst, int id, int elem)
{
    struct Unit *u;
    int n;
    int e;
    int j;
    int v;

    u = _GetUnit(id);
    n = 0;
    if (elem == -1) {
        for (e = 0; e < 4; e++) {
            for (j = 0; j < 20; j++) {
                if (u->djinnSet[e] & (1 << j)) {
                    v = (e << 5) | j | -0x8000;
                    dst[n++] = v | (id << 8);
                } else if (u->djinnHave[e] & (1 << j)) {
                    dst[n++] = (e << 5) | j | (id << 8);
                }
            }
        }
    } else {
        for (j = 0; j < 20; j++) {
            if (u->djinnSet[elem] & (1 << j))
                dst[n++] = (elem << 5) | j | -0x8000;
            else if (u->djinnHave[elem] & (1 << j))
                dst[n++] = (elem << 5) | j;
        }
    }
    return n;
}
