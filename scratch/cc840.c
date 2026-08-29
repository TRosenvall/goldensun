extern int OvlFunc_899_200c7fc(int x, int y, int n);
extern int OvlFunc_899_200c7bc(int x, int y, int n);

int OvlFunc_899_200c840(unsigned char *p)
{
    int x;
    int y;

    if (p == 0)
        return 1;
    x = (p[0] << 19) + (0x90 << 15);
    y = (p[1] << 19) + (0x9e << 18);
    if (OvlFunc_899_200c7fc(x, y, 0))
        return -1;
    if (OvlFunc_899_200c7bc(x, y, 2))
        return -1;
    if (OvlFunc_899_200c7bc(x, y, 0x18))
        return -1;
    if (OvlFunc_899_200c7bc(x, y, 0x19))
        return -1;
    return 0;
}
