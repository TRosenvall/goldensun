extern unsigned char *iwram_3001ebc;
extern void *GetFieldActor(int slot);
extern int _Func_8017658(int msg, int x, int y, int flag);
extern int _Func_8017394(int handle);
extern void WaitFrames(int n);

void Func_80930bc(int packed)
{
    int x;
    int y;
    int xx;
    int yy;
    unsigned char *s;
    int slot;
    int handle;

    s = iwram_3001ebc;
    slot = packed & 0xfff;
    GetFieldActor(slot);
    *(int *)(s + (0xfa << 1)) = slot;
    if (*(int *)(s + (0xe6 << 1)) == 0) {
        yy = y;
        xx = x;
        if (yy > 0x77)
            yy += 0x20;
        else
            yy -= 0x20;
        xx = xx < 8 ? 8 : xx;
        if (xx > (0x9c << 1))
            xx = 0x9c << 1;
        yy = yy < 0x14 ? 0x14 : yy;
        if (yy > 0xdc)
            yy = 0xdc;

        handle = _Func_8017658(*(short *)(s + (0xec << 1)), xx, yy, 1);
        *(int *)(s + (0xfc << 1)) = handle;
        while (_Func_8017394(handle) == 0)
            WaitFrames(1);
    }
    *(unsigned short *)(s + (0xec << 1)) += 1;
}
