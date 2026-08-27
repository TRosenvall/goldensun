extern char *iwram_3001ebc;
extern unsigned char gState[];
extern void __ClearFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);

int OvlFunc_900_20081e4(void)
{
    char *p;
    unsigned char *g;
    int area;
    unsigned char *q;

    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x209;
    g = gState;
    area = *(short *)(g + 0x1c2);
    if (area == 2) {
        __ClearFlag(0x12f);
    } else if (area == 0xa) {
        unsigned char m = 0x14;
        q = __MapActor_GetActor(8) + 0x59;
        *q = m | *q;
    } else {
        unsigned char m = 0x14;
        q = __MapActor_GetActor(8) + 0x59;
        *q |= 0x14;
        q = __MapActor_GetActor(9) + 0x59;
        *q |= 0x14;
        q = __MapActor_GetActor(0xa) + 0x59;
        *q = m | *q;
    }
    return 0;
}
