extern int _Func_80796c4(unsigned short *buf);
extern unsigned char *_GetUnit(int id);
extern int _GetMoveInfo(int m);

int Func_801c7fc(unsigned short *out)
{
    unsigned short buf[15];
    unsigned short *ptr;
    unsigned short *q;
    unsigned short *w;
    int n, k, j, m, id, mask;

    n = 0;
    k = _Func_80796c4(buf);
    if (n < k) {
        ptr = buf;
        do {
            id = *ptr;
            ptr++;
            q = (unsigned short *)(_GetUnit(id) + 0x58);
            mask = 0x3fff;
            m = mask & *q;
            j = 0;
            if (m != 0) {
                w = (unsigned short *)((n << 2) + (char *)out);
                do {
                    _GetMoveInfo(m);
                    j++;
                    w[0] = id;
                    w[1] = m;
                    n++;
                    w += 2;
                    if (j > 0x1f)
                        break;
                    q += 2;
                    m = mask & *q;
                } while (m != 0);
            }
            k--;
        } while (k != 0);
    }
    return n;
}
