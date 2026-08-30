extern unsigned char L371e0[] __asm__(".L371e0");
extern int _MSG_09;

extern void Func_8016478(int a);
extern void Func_801e41c(int a, int b, int c, int d, int e);
extern void Func_801e8b0(unsigned char *a, int b, int c, int d);
extern void UIDrawText(void *s, int b, int c, int d);
extern void Func_801e9d4(int a, int b, int c, int d, int e);
extern void Func_801e7c0(int a, int b, int c, int d);
extern void *Func_801f680(int a, void *b);
extern void Func_801ea08(int a, int b, int c, int d, int e);

void Func_8020198(int a, unsigned char *p)
{
    int buf[4];
    int n;

    if (a == 0)
        return;
    Func_8016478(a);
    Func_801e41c(a, 0, 4, 0xd, 4);
    Func_801e8b0(p + 0x10, a, 0, 0);
    n = 0;
    UIDrawText(L371e0, a, 0x48, 0);
    Func_801e9d4(p[0x1c], 2, a, 0x50, n);
    Func_801e7c0(p[0x1d] + 0x741, a, 0, 0x10);
    Func_801e7c0((int)&_MSG_09, a, 0, 0x20);
    UIDrawText(Func_801f680(*(int *)(p + 0x20), buf), a, 0x30, 0x28);
    n = 0x30;
    Func_801ea08(*(int *)(p + 0x24), 6, a, 0, n);
    Func_801e7c0(0xc88, a, 0x30, 0x30);
}
