extern unsigned char *iwram_3001ebc;
extern void *GetFieldActor(int slot);
extern int _Func_8017658(int msg, int x, int y, int flag);
extern int _Func_8017394(int handle);
extern void WaitFrames(int n);

void Func_80930bc(int packed)
{
    register unsigned char *s __asm__("r8");
    int slot;
    int x;
    int y;
    int handle;

    s = iwram_3001ebc;
    slot = packed & 0xfff;
    GetFieldActor(slot);
    *(int *)(s + (0xfa << 1)) = slot;
    if (*(int *)(s + (0xe6 << 1)) == 0) {
        if (y > 0x77)
            y += 0x20;
        else
            y -= 0x20;
        x = x < 8 ? 8 : x;
        if (x > (0x9c << 1))
            x = 0x9c << 1;
        y = y < 0x14 ? 0x14 : y;
        if (y > 0xdc)
            y = 0xdc;
        handle = _Func_8017658(*(short *)(s + (0xec << 1)), x, y, 1);
        *(int *)(s + (0xfc << 1)) = handle;
        while (_Func_8017394(handle) == 0)
            WaitFrames(1);
    }
    *(unsigned short *)(s + (0xec << 1)) += 1;
}
