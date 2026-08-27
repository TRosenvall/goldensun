extern char *iwram_3001ebc;
extern unsigned char gState[];
extern char *__MapActor_GetActor(int slot);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __StartTask(void *fn, int prio);
extern void OvlFunc_902_2008570(int a, int x, int y, int z);
extern void OvlFunc_902_2008030(void);

int OvlFunc_902_20084e4(void)
{
    char *p;
    unsigned char *g;
    char *a;
    int e;

    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x209;
    g = gState;
    e = *(short *)(g + (0xe1 << 1));
    if (e == 5) {
        __CopyMapTiles(0, 0x78, 8, 0x43, 4, 3);
        a = __MapActor_GetActor(8);
        a[0x55] = 0;
        a = __MapActor_GetActor(8);
        *(int *)(a + 0xc) = 0;
        a = __MapActor_GetActor(8);
        *(int *)(a + 0x14) = 0;
    } else if (e == 7 || e == 0xb) {
        OvlFunc_902_2008570(0xe7, 0x8e << 18, 0x80 << 13, 0xa8 << 18);
        __StartTask(OvlFunc_902_2008030, 0xc8 << 4);
    }
    return 0;
}
