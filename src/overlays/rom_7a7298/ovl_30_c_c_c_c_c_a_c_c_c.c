extern int iwram_3001e70;
extern unsigned char *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);

void OvlFunc_921_20098c4(void)
{
    unsigned char *a;
    int x;
    int y;
    int u;

    a = __MapActor_GetActor(0);
    x = *(int *)(a + 8);
    u = x >> 19;
    if (u >= 0x18 && u <= 0x1f)
        goto setA;
    y = *(int *)(a + 0x10);
    if ((y >> 19) < 0x24 || (y >> 19) > 0x2d)
        goto other;
    if (u < 0x16 || u > 0x1f)
        goto other;
setA:
    if (__GetFlag(0x80 << 2) != 0)
        return;
    ((char *)iwram_3001e70)[0x17] = 0;
    __SetFlag(0x80 << 2);
    __ClearFlag(0x201);
    return;
other:
    if (x > (0xe8 << 16) && *(int *)(a + 0xc) > (0xf0 << 13) && y > (0xd4 << 16)) {
        ((char *)iwram_3001e70)[0x17] = 0;
        __SetFlag(0x80 << 2);
        __ClearFlag(0x201);
        return;
    }
    if (__GetFlag(0x201) != 0)
        return;
    ((char *)iwram_3001e70)[0x17] = 1;
    __SetFlag(0x201);
    __ClearFlag(0x80 << 2);
}
