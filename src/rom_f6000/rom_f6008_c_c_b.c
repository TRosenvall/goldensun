/* Func_80f7e60  --  0x080f7e60, split out of asm/rom_f6000/rom_f6008_c.s, which
 * was first renamed rom_f6008_c_c.s because its _b suffix was taken by an
 * elevated neighbour; Func_80f7df0/80f7e34 (parked) stay in _c_a.s, 80f7f30
 * and 80f7f78 in _c_c.s. Matched from scratch.
 *
 * `b->tbl[k & 0x3ff]` as a MEMBER ARRAY gives `str [r1,r3]` with
 * r3 = k*4 + 0x3404; raw casts fold the base in. The struct puts cnt at 0x4438
 * and end at 0x4440. Its two callees' parks declare ewram_2004c00 as char * /
 * int -- reconcile if they ever share this TU.
 */
struct Dict {
    char pad[0x3404];
    int tbl[0x40d];
    int cnt;
    int pad2;
    int end;
};
extern struct Dict *ewram_2004c00;
extern void Func_80f7e34(int i);
extern void Func_80f7df0(int i);

void Func_80f7e60(int pos, int n, unsigned char *src)
{
    int i;
    struct Dict *b;
    int c;

    for (i = 0; i < n; i++) {
        int k = pos + i;
        Func_80f7e34((pos + 0x124 + i) & 0x3ff);
        b = ewram_2004c00;
        c = src[b->cnt++];
        if (b->cnt == b->end) {
            b->tbl[k & 0x3ff] = -1;
            break;
        }
        b->tbl[k & 0x3ff] = c;
        Func_80f7df0(k & 0x3ff);
    }
    for (i++; i < n; i++) {
        int k = (pos + i) & 0x3ff;
        Func_80f7e34(k);
        ewram_2004c00->tbl[k] = -1;
    }
}
