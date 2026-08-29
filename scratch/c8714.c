extern int OvlFunc_956_20086a4(int a, int b);

int OvlFunc_956_2008714(int a, int b)
{
    if (OvlFunc_956_20086a4(a, b + 0xffe80000))
        return -1;
    if (OvlFunc_956_20086a4(a, b + 0xfff80000))
        return -1;
    if (OvlFunc_956_20086a4(a, b + (0x80 << 12)))
        return -1;
    if (OvlFunc_956_20086a4(a, b + (0xc0 << 13)))
        return -1;
    return 0;
}
