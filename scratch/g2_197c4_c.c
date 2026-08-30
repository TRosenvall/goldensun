extern unsigned char *iwram_3001e8c;
extern void CloseUIBox(void *box, int mode);
extern void WaitFrames(int n);

void Func_80197c4(void)
{
    unsigned char *base;
    unsigned char *p;
    unsigned char *q;
    unsigned char *w;
    unsigned char *v;
    int i;
    int done;

    base = iwram_3001e8c;
    p = base + (0xc4 << 3);
    q = base + (0xa0 << 3);
    i = 0;
L1:
    w = *(unsigned char **)p;
    if (w != 0 && *(unsigned short *)(w + 0x16) != 0)
        CloseUIBox(w, 0);
    i++;
    p += 0x28;
    if (i != 3)
        goto L1;
L2:
    p = base + (0xc4 << 3);
    done = 1;
    i = 0;
L3:
    v = *(unsigned char **)p;
    if (v != 0) {
        if (*(int *)(v + 0x18) == 0 && *(unsigned short *)(v + 0x16) == 0)
            *(unsigned char **)p = 0;
        else
            done = 0;
    }
    i++;
    p += 0x28;
    if (i != 3)
        goto L3;
    i = 0;
    if (done == 0) {
        WaitFrames(1);
        goto L2;
    }
    goto L5;
L4:
    if (*(unsigned short *)(q + 0x16) != 0)
        CloseUIBox(q, 0);
    q += 0x24;
    i++;
L5:
    if (i != 8)
        goto L4;
}
