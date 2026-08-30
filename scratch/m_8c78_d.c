extern void __MapActor_SetPos(int slot, int x, int y);
extern void OvlFunc_959_2008b4c(void);

void OvlFunc_959_2008c78(void)
{
    int x, y, s;

    x = 0xf8 << 16;
    y = 0xbc << 17;
    s = 0xc;
    __MapActor_SetPos(s, x, y);
    OvlFunc_959_2008b4c();
}
