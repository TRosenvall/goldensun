typedef unsigned char u8;

extern u8 gState[];
extern unsigned int iwram_3001ebc;
extern void MapActor_GetActor(int id);
extern int *GetFieldActor(int i);

void Func_809c314(void)
{
    u8 *g;
    u8 *w;
    int *p;
    int *a;
    int x;
    int y;
    int x0;
    int x1;
    int y0;
    int y1;
    int ax;
    int ay;
    unsigned int i;

    g = gState;
    g += 0xfa * 2;
    MapActor_GetActor(*(int *)g);
    w = (u8 *)iwram_3001ebc;
    w += 0xf0 * 2;
    x = (*(int **)w)[2];
    x0 = x + 0xfec00000;
    x1 = x + (0xa0 << 17);
    y = (*(int **)w)[4];
    y0 = y + 0xfda80000;
    y1 = y + (0xc8 << 17);
    for (i = 8; i <= 0x41; i++) {
        a = GetFieldActor(i);
        if (a != 0) {
            ax = a[2];
            ay = a[4];
            if (ax < x0 || ax > x1 || ay < y0 || ay > y1)
                *((u8 *)a + 0x54) = 0;
            else
                *((u8 *)a + 0x54) = 1;
        }
    }
}
