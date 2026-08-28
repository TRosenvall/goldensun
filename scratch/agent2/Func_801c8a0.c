typedef unsigned short u16;
typedef unsigned char u8;

extern u8 gState[];
extern u16 ewram_2000462;

void Func_801c8a0(int *outA, int *outB, u16 *tbl)
{
    u8 *g;
    u16 *q;
    unsigned int h;
    int i;

    *outA = 0;
    *outB = 0;
    g = gState;
    h = *(u16 *)(g + 0x220);
    q = tbl;
    for (i = 0; i <= 0x1bf; i++) {
        if (q[1] == (h & 0x3ff) && q[0] == (h >> 10)) {
            *outA = i;
            break;
        }
        q += 2;
    }
    q = tbl;
    for (i = 0; i <= 0x1bf; i++) {
        if (q[1] == (ewram_2000462 & 0x3ff) && q[0] == (ewram_2000462 >> 10)) {
            *outB = i;
            break;
        }
        q += 2;
    }
}
