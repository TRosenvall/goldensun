extern void Func_801b9a8(unsigned char *w, int v);
extern void WaitFrames(int n);
extern void Func_801ba68(unsigned char *w, int v);
extern void Func_801b9ec(unsigned char *w, int v);
extern void Func_801b010(int a, int b);

void Func_801b5c0(unsigned char *w)
{
    unsigned char *a;
    unsigned char *p;
    int t;
    int c21;
    int c8;
    unsigned short *q;

    c21 = 0x21;
    c8 = 8;
    a = w + (0xe7 << 2);
    if (*(int *)a != 0) {
        Func_801b9a8(w, *(unsigned short *)(w + 0x39e));
        *(unsigned short *)(w + 0x3a2) = c21;
        WaitFrames(1);
        t = *(unsigned short *)(w + 0x39e);
        if (t == 1 && *(unsigned short *)a != 0) {
            *(unsigned short *)(w + 8) = c8;
            *(unsigned short *)a -= 1;
            Func_801ba68(w, 0);
            if (*(unsigned short *)a == 0)
                *(unsigned short *)(w + 0xa) = 0;
            *(unsigned short *)(w + 0x3e) = t;
        } else {
            *(unsigned short *)(w + 0x39e) -= 1;
        }
        q = (unsigned short *)(w + 0x3a2);
        *q = 1;
        Func_801b9ec(w, *(unsigned short *)(w + 0x39e));
        WaitFrames(1);
        p = *(unsigned char **)(w + (0xd2 << 2));
        Func_801b010(*(unsigned short *)(p + 0xa), 0);
        WaitFrames(1);
    }
}
