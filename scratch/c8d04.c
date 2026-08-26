struct A { unsigned char pad00[8]; int f8; };

extern void *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __SetFlag(int id);
extern void OvlFunc_903_2008dd8(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_903_2008d04(void)
{
    int y;
    unsigned char *p;
    int c;

    __CutsceneStart();
    y = ((struct A *)__MapActor_GetActor(8))->f8 >> 20;
    if (y == 0xb) {
        OvlFunc_903_2008dd8(8);
        p = (unsigned char *)__MapActor_GetActor(8) + 0x23;
        *p |= 2;
        c = 0xc;
        __Func_8010704(0x27, 0xc, 3, 1, 8, c);
        __Func_8010704(0x2b, 0xb, 3, 1, c, y);
        __SetFlag(0x86 << 4);
    }
    __CutsceneEnd();
}
