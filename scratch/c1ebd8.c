struct Y { unsigned char pad00[0xf]; unsigned char ff; };

extern int AllocSpriteSlot(void);
extern void LoadOldUIIcon(int a, int b, int *slot, int *out, int e);
extern struct Y *Func_801eadc(int slot, int m, int b, int c, int d);

struct Y *Func_801ebd8(int a, int b, int c, int d)
{
    int slot;
    int out;
    struct Y *p;

    slot = AllocSpriteSlot();
    if (slot == 0x60)
        return 0;
    LoadOldUIIcon(a, 1, &slot, &out, 1);
    p = Func_801eadc(slot, 0x80 << 23, b, c, d);
    p->ff = 0xfb;
    return p;
}
