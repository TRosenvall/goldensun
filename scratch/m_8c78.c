extern void __MapActor_SetPos(int slot, int x, int y);
extern void OvlFunc_959_2008b4c(void);

void OvlFunc_959_2008c78(void)
{
    __MapActor_SetPos(0xc, 0xf8 << 16, 0xbc << 17);
    OvlFunc_959_2008b4c();
}
