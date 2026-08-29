extern unsigned char *iwram_3001e8c;
extern void Func_8018efc(int a, int b, int c, int d, int e, int f);

void Func_8017c8c(short *a, int b, int c, int d)
{
    unsigned char *base;
    unsigned short *q;
    int i;
    int t;
    int s;
    unsigned short u;
    int w;

    base = iwram_3001e8c;
    t = c;
    s = (short)c;
    if (a == 0) {
        q = (unsigned short *)(base + 0x12b2);
        i = *q;
        *(unsigned short *)(base + 0xeb0 + i * 2) = 0;
        a = (short *)(base + 0xeb0);
        *q = (i + 1) & 0x1ff;
    }
    w = *a++ << 16;
    if (w != 0) {
        do {
            u = (unsigned int)w >> 16;
            if (u <= 0x1e) {
                switch (u) {
                case 3:
                    t = s;
                    d++;
                    break;
                case 14:
                case 15:
                case 28:
                    a++;
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 17:
                case 29:
                    a++;
                    break;
                }
            } else {
                Func_8018efc(b, u, t, d, 0, d);
                if ((unsigned short)(u - 0xde) > 1)
                    t++;
            }
            w = *a++ << 16;
        } while (w != 0);
    }
    *(base + 0xea3) = 1;
}
