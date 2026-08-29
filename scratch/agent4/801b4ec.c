extern void Func_801b9a8(unsigned char *p);
extern void WaitFrames(int n);
extern void Func_801ba68(unsigned char *p, int n);
extern void Func_801b9ec(unsigned char *p, int n);
extern void Func_801b010(int a, int b);

void Func_801b4ec(unsigned char *p)
{
    unsigned short *A;
    unsigned short *B;
    unsigned short *C;
    unsigned char *q;
    int t, v, zero, k21, k8;

    A = (unsigned short *)(p + (0xe7 << 2));
    B = (unsigned short *)(p + 0x39e);
    t = *A + *B + 1;
    C = (unsigned short *)(p + (0xe5 << 2));
    if (t == *C)
        return;
    Func_801b9a8(p);
    k21 = 0x21;
    *(unsigned short *)(p + 0x3a2) = k21;
    WaitFrames(1);
    v = *B;
    *B = v + 1;
    if (*B == 4 && (unsigned int)(t + 1) < (unsigned int)*C) {
        *B = v + (0x80 << 9);
        k8 = 8;
        *(unsigned short *)(p + 0x3c) = k8;
        *A = *A + 1;
        Func_801ba68(p, 1);
        if (*A + *B + 2 != *C) {
            zero = 0;
            *(unsigned short *)(p + 0x3e) = zero;
        }
        *(unsigned short *)(p + 0xa) = 1;
    }
    *(unsigned short *)(p + 0x3a2) = 1;
    Func_801b9ec(p, *(unsigned short *)(p + 0x39e));
    WaitFrames(1);
    q = *(unsigned char **)(p + (0xd2 << 2));
    Func_801b010(*(unsigned short *)(q + 0xa), 0);
    WaitFrames(1);
}
