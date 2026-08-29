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
    unsigned int i;

    g = gState;
    g += 0xfa * 2;
    MapActor_GetActor(*(int *)g);
    w = (u8 *)iwram_3001ebc;
    w += 0xf0 * 2;
    p = *(int **)w;
    x = p[2];
    x0 = x + 0xfec00000;
    x1 = x + (0xa0 << 17);
    y = p[4];
    y0 = y + 0xfda80000;
    y1 = y + (0xc8 << 17);
    for (i = 8; i <= 0x41; i++) {
        a = GetFieldActor(i);
        if (a != 0) {
            if (a[2] >= x0 && a[2] <= x1 && a[4] >= y0 && a[4] <= y1)
                *((u8 *)a + 0x54) = 1;
            else
                *((u8 *)a + 0x54) = 0;
        }
    }
}
