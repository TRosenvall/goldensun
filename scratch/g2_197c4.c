extern unsigned char *iwram_3001e8c;
extern void CloseUIBox(void *box, int mode);
extern void WaitFrames(int n);

void Func_80197c4(void)
{
    unsigned char *base;
    unsigned char *p;
    unsigned char *q;
    unsigned char *w;
    int i;
    int done;

    base = iwram_3001e8c;
    p = base + (0xc4 << 3);
    q = base + (0xa0 << 3);
    for (i = 0; i < 3; i++) {
        w = *(unsigned char **)p;
        if (w != 0 && *(unsigned short *)(w + 0x16) != 0)
            CloseUIBox(w, 0);
        p += 0x28;
    }
    do {
        p = base + (0xc4 << 3);
        done = 1;
        for (i = 0; i < 3; i++) {
            w = *(unsigned char **)p;
            if (w != 0) {
                if (*(int *)(w + 0x18) == 0 && *(unsigned short *)(w + 0x16) == 0)
                    *(unsigned char **)p = 0;
                else
                    done = 0;
            }
            p += 0x28;
        }
        i = 0;
        if (done == 0)
            WaitFrames(1);
    } while (done == 0);
    for (; i < 8; i++) {
        if (*(unsigned short *)(q + 0x16) != 0)
            CloseUIBox(q, 0);
        q += 0x24;
    }
}
