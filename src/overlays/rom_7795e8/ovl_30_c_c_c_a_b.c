/* OvlFunc_880_20091e4  --  0x020091e4, split out of
 * asm/overlays/rom_7795e8/ovl_30_c_c_c_a.s; OvlFunc_880_2008de4 stays in _a.s.
 * Matched from scratch. Index the output by its running count, `out[n] = v`:
 * loop.c builds the pointer as out + n before the loop, not knowing n is 0,
 * which is the ROM's `add r0, r10`; a pointer variable folds it away.
 */
int OvlFunc_880_20091e4(unsigned char *buf, int len, unsigned char *out)
{
    int key;
    int n;
    int i;
    int bit;
    int idx;
    int sum;
    int cnt;
    int v;
    int j;
    unsigned char *p;
    int b;

    key = buf[len - 1];
    n = 0;
    for (i = 0; i != len - 1; i++)
        buf[i] ^= key;
    bit = 0;
    idx = 0;
    sum = 0;
    cnt = 0;
    do {
        v = 0;
        j = 0;
        p = (unsigned char *)(idx + (int)buf);
        for (; j != 6; j++) {
            b = (*p >> (7 - bit)) & 1;
            bit++;
            if (bit == 8) {
                bit = 0;
                p++;
                idx++;
            }
            v |= b << (5 - j);
            if (idx == len)
                break;
        }
        cnt++;
        out[n] = v;
        n++;
        sum += v;
        if (cnt == 9) {
            out[n] = sum & 0x3f;
            n++;
            sum = 0;
            cnt = 0;
        }
    } while (idx != len);
    for (i = 0; i != n; i++)
        out[i] = (out[i] + i) & 0x3f;
    return n;
}
