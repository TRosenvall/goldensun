extern void _Func_8016498(void *p);
extern void *_CreateUIBox(int a, int b, int c, int d, int e);

int Func_80a10d0(void **slot, int a, int b, int c, int d, int e)
{
    if (*slot != 0) {
        if ((e & (0x80 << 1)) != 0)
            return 0;
        _Func_8016498(*slot);
        return 0;
    }
    *slot = _CreateUIBox(a, b, c, d, e & 0xff);
    return 1;
}
