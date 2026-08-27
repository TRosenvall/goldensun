extern char *iwram_3001ebc;
extern unsigned char gState[];
extern void OvlFunc_common0_70(int a, int b, int c, int d);
extern void __ClearFlag(int id);
extern void __MapActor_SetAnim(int slot, int n);

int OvlFunc_929_2008598(void)
{
    char *p;
    unsigned char *g;
    int area;
    int x1;
    int y1;
    int y2;

    x1 = 0xe6 << 17;
    y1 = 0x8e << 18;
    y2 = 0x8e << 18;
    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x209;
    g = gState;
    area = *(short *)(g + 0x1c2);
    if (area == 4 || area == 7) {
        OvlFunc_common0_70(0xf8 << 16, 0, 0x1a10000, 0x14);
    } else if (area == 6) {
        OvlFunc_common0_70(x1, 0, y1, 0x14);
        OvlFunc_common0_70(0xf2 << 17, 0, y2, 0x14);
    } else if (area == 8) {
        __ClearFlag(0x12f);
        __MapActor_SetAnim(0xa, 6);
    }
    return 0;
}
