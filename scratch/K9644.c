extern void *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);

void OvlFunc_968_2009644(void)
{
    unsigned char *a;
    int v;
    int m;
    int n;

    a = (unsigned char *)__MapActor_GetActor(0xd);
    __CutsceneStart();
    if ((*(int *)(a + 8) >> 20) == 0x2a) {
        __CutsceneWait(0x1e);
        __PlaySound(0xbc);
        a[0x55] = 0;
        v = 0xfffe0000;
        *(int *)(a + 0x14) = v;
        *(int *)(a + 0xc) = v;
        __SetFlag(0x80 << 2);
        m = 3;
        n = 5;
        __CopyMapTiles(0x2c, 0x75, 0x29, 0x75, m, n);
    }
    __CutsceneEnd();
}
