typedef unsigned short u16;
typedef unsigned char u8;

extern u8 gState[];
extern u16 ewram_2000462;

void Func_801c8a0(int *outA, int *outB, u16 *tbl)
{
    u8 *g;
    u16 *q;
    unsigned int h;
    unsigned int mask;
    unsigned int hi;
    int n1;
    int n2;
    int i;

    *outA = 0;
    *outB = 0;
    g = gState;
    h = *(u16 *)(g + 0x220);
    mask = h & 0x3ff;
    hi = h >> 10;
    n1 = 0x1bf;
    q = tbl;
    i = 0;
    do {
        if (q[1] == mask && q[0] == hi) {
            *outA = i;
            break;
        }
        i++;
        q += 2;
    } while (i <= n1);
    n2 = 0x1bf;
    q = tbl;
    i = 0;
    do {
        if (q[1] == (ewram_2000462 & 0x3ff) && q[0] == (ewram_2000462 >> 10)) {
            *outB = i;
            break;
        }
        i++;
        q += 2;
    } while (i <= n2);
}
