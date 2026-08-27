extern void __Func_8010704(int, int, int, int, int, int);
extern void __Func_808edac(int, int, int);
extern void __MapActor_SetPos(int, int, int);

void OvlFunc_964_2009fdc(void)
{
    int s;

    s = 0x31;
    __Func_8010704(0x48, 0x31, 1, 1, 8, s);
    __Func_8010704(0x71, 0x2b, 1, 1, s, 0x2b);
    __Func_808edac(0x64, 0, 0);
    __Func_808edac(0x65, 0, 0);
    __MapActor_SetPos(0xf, 0x88 << 16, 0xc6 << 18);
    __MapActor_SetPos(0x10, 0xc6 << 18, 0xae << 18);
}
