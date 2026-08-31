extern void Func_809b8f4(unsigned char *a);
extern void Func_809b86c(unsigned char *a);

void Func_809b804(unsigned char *a)
{
    signed char *p;
    int s;
    int v;

    p = (signed char *)(a + 0x45);
    if (p[(unsigned int)0] == 0)
        return;
    *(unsigned short *)(a + 0x38) += 1;
    v = *(unsigned short *)(a + 0x3a);
    s = *(short *)(a + 0x3a);
    if (s != 0) {
        *(unsigned short *)(a + 0x3a) = v - 1;
    } else {
        if (*(int *)(a + 0x34) != 0)
            ((void (*)(unsigned char *))*(int *)(a + 0x34))(a);
    }
    if (p[(unsigned int)0] == 0)
        return;
    if (*(signed char *)(a + 0x43) != 0)
        Func_809b8f4(a);
    if (*(signed char *)(a + 0x44) != 0)
        Func_809b86c(a);
}
