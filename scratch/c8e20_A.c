struct A { unsigned char pad00[0x10]; int f10; };
struct B { unsigned char pad00[0x18]; struct A *f18; };

extern unsigned char gState[];
extern struct B *iwram_3001ee0;
extern int _AREA_ac;
extern struct A *__MapActor_GetActor(int slot);

void OvlFunc_964_2008e20(void)
{
    struct A *a;
    struct B *b;
    unsigned char *g;
    int lim;

    a = __MapActor_GetActor(0);
    b = iwram_3001ee0;
    g = gState;
    lim = 0;
    if (*(short *)(g + (0xe0 << 1)) == (int)&_AREA_ac) {
        switch (*(short *)(g + (0xe1 << 1))) {
        case 3:
        case 4:
            lim = 0x5e;
            break;
        case 8:
        case 9:
            lim = 0x4a;
            break;
        case 12:
        case 13:
            lim = 0x76;
            break;
        }
    } else if (*(short *)(g + (0xe1 << 1)) == 0xc) {
        lim = 0x5d;
    }
    if ((a->f10 >> 19) <= lim)
        b->f18 = 0;
    else
        b->f18 = a;
}
