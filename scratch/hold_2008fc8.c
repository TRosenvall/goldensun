extern unsigned char *__GetUnit(int i);
extern int __GiveItemTo(int who, int item);
extern unsigned char *__GetItemInfo(int id);
extern void __Func_8078948(int who, int slot);
extern void __EquipItem(int who, int slot);

void OvlFunc_903_2008fc8(void)
{
    unsigned char *unit;
    unsigned char *p;
    unsigned char *info;
    int n, j, item, mask, v, z;
    unsigned char *q;

    item = 0x41;
    unit = __GetUnit(2);
    n = 0;
top:
    n = n + 1;
    if (n > 0xfa << 2) {
        q = unit;
        q += 0xf4;
        z = 0;
        *(unsigned short *)q = z;
    }
    if (__GiveItemTo(2, 0x41) == -1) {
        p = unit;
        j = 0;
        p += 0xd8;
    l1:
        info = __GetItemInfo(*(unsigned short *)p);
        p += 2;
        if (info[2] == 1)
            goto found;
        j++;
        if (j <= 0xe)
            goto l1;
        p = unit;
        mask = 0x8ff;
        j = 0;
        p += 0xd8;
    l2:
        info = __GetItemInfo(*(unsigned short *)p);
        if ((*(unsigned short *)(info + 2) & mask) == 0 && info[0xc] == 1) {
        found:
            __Func_8078948(2, j);
            goto top;
        }
        j++;
        p += 2;
        if (j > 0xe)
            goto top;
        goto l2;
    }
    p = unit;
    j = 0;
    p += 0xd8;
l3:
    v = *(unsigned short *)p;
    p += 2;
    if (v == item)
        __EquipItem(2, j);
    j++;
    if (j <= 0xe)
        goto l3;
}
