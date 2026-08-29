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
    int s0;
    int s1;
    int zero;
    int e;

    zero = 0;
    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x209;
    g = gState;
    e = *(short *)(g + (0xe1 << 1));
    if (e == 5) {
        s0 = 4;
        s1 = 3;
        __CopyMapTiles(0, 0x78, 8, 0x43, s0, s1);
        *(__MapActor_GetActor(8) + 0x55) = zero;
        *(int *)(__MapActor_GetActor(8) + 0xc) = zero;
        *(int *)(__MapActor_GetActor(8) + 0x14) = zero;
    } else if (e == 7 || e == 0xb) {
        OvlFunc_902_2008570(0xe7, 0x8e << 18, 0x80 << 13, 0xa8 << 18);
        __StartTask(OvlFunc_902_2008030, 0xc8 << 4);
    }
    return 0;
}
