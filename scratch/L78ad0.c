extern unsigned char L7b490[] __asm__(".L7b490");
extern int Func_8078aa0(int n);

int Func_8078ad0(int id)
{
    int i;
    int r;

    i = L7b490[0x1ff & id];
    r = 0;
    if (i != 0)
        r = Func_8078aa0(i - 1);
    return r;
}
