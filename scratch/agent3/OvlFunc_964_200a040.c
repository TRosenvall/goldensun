extern void __Func_8010704(int, int, int, int, int, int);
extern void __Func_808edac(int, int, int);
extern void __MapActor_SetPos(int, int, int);

void OvlFunc_964_200a040(void)
{
    int s;

    s = 0x31;
    __Func_8010704(8, 0x71, 1, 1, 8, s);
    __Func_8010704(0x31, 0x6b, 1, 1, s, 0x2b);
    __Func_808edac(0x64, -1, -1);
    __Func_808edac(0x65, -1, -1);
    __MapActor_SetPos(0xf, 0, 0);
    __MapActor_SetPos(0x10, 0, 0);
}
