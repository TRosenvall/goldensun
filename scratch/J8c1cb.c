extern unsigned char *__MapActor_GetActor(int slot);
extern void __PlaySound(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __CutsceneWait(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_901_2008a80(int a, int b, int c);

void OvlFunc_901_2008c1c(void)
{
    unsigned char *a;
    unsigned char *s;
    unsigned char *p;
    unsigned char m;
    int t;
    int e0;
    int e1;

    a = __MapActor_GetActor(0);
    s = *(unsigned char **)(a + 0x50);
    __PlaySound(0xbc);
    t = 2;
    __CopyMapTiles(0x2a, 0x21, 0x22, 0x10, t, t);
    __CopyMapTiles(0x2a, 0x23, 0x24, 0x10, t, t);
    __CutsceneWait(4);
    __CopyMapTiles(0x28, 0x21, 0x22, 0x10, t, t);
    __CopyMapTiles(0x28, 0x23, 0x24, 0x10, t, t);
    __CutsceneWait(4);
    e0 = 3;
    e1 = 0x10;
    __Func_8010704(0x21, 0x15, 2, 2, e0, e1);
    p = a + 0x23;
    m = 0xfe;
    m = m & *p;
    *p = m;
    s[9] = 0xc | s[9];
    OvlFunc_901_2008a80(0x40, 0x88 << 1, 0xb);
}
