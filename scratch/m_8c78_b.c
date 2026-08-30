extern void __MapActor_SetPos(int slot, int x, int y);
extern void OvlFunc_959_2008b4c(void);

void OvlFunc_959_2008c78(void)
{
    int x, y;

    x = 0xf8 << 16;
    y = 0xbc << 17;
    __MapActor_SetPos(0xc, x, y);
    OvlFunc_959_2008b4c();
}
