extern unsigned char *_GetItemInfo(int item);

int Func_80b19cc(int item)
{
    int v;

    v = (short)*(unsigned short *)_GetItemInfo(item);
    if ((_GetItemInfo(item)[3] & 8) != 0) {
        v = 0;
    } else if ((item & (0x80 << 3)) != 0) {
        v = v / 2;
    } else {
        v = v * 3 / 4;
    }
    return v;
}
