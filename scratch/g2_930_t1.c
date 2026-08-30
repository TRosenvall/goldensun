extern unsigned char *iwram_3001ebc;
extern void *GetFieldActor(int slot);
extern int _Func_8017658(int msg, int x, int y, int flag);
extern int _Func_8017394(int handle);
extern void WaitFrames(int n);

void Func_80930bc(int packed)
{
    unsigned char *s;
    int slot;
    int x;
    int y;
    int handle;
    int xx;
    int yy;
    int *p;
    unsigned short *m;

    s = iwram_3001ebc;
    slot = packed & 0xfff;
    GetFieldActor(slot);
    p = (int *)(s + (0xfa << 1));
    *p = slot;
    m = (unsigned short *)(s + (0xec << 1));
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
        handle = _Func_8017658(*(short *)m, xx, yy, 1);
        p[1] = handle;
        while (_Func_8017394(handle) == 0)
            WaitFrames(1);
    }
    *m += 1;
}
