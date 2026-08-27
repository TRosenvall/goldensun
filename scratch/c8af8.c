struct A { unsigned char pad00[8]; int f8; };

extern struct A *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_942_2008b68(int slot);

void OvlFunc_942_2008af8(void)
{
    int y1;
    int y2;
    int f;

    y1 = __MapActor_GetActor(0xe)->f8 >> 20;
    y2 = __MapActor_GetActor(0xf)->f8 >> 20;
    f = 0xb;
    __Func_8010704(5, 0xc, 5, 1, 5, f);
    __Func_8010704(1, 0, 1, 1, y2, f);
    __Func_8010704(1, 0, 1, 1, y1, f);
    OvlFunc_942_2008b68(0xe);
    OvlFunc_942_2008b68(0xf);
}
