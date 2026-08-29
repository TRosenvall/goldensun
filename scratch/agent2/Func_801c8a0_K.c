typedef unsigned short u16;
typedef unsigned char u8;

extern u8 gState[];
extern u16 ewram_2000462;

void Func_801c8a0(int *outA, int *outB, u16 *tbl)
{
    u8 *g;
    u16 *q1;
    unsigned int h;
    unsigned int mask;
    unsigned int hi;
    unsigned int v;
    unsigned int m;
    int n1;
    int n2;
    int i;

    *outA = 0;
    *outB = 0;
    g = gState;
    h = *(u16 *)(g + 0x220);
    i = 0;
    mask = h & 0x3ff;
    hi = h >> 10;
    n1 = 0x1bf;
    q1 = tbl;
    do {
        if (q1[1] == mask && q1[0] == hi) {
            *outA = i;
            break;
        }
        i++;
        q1 += 2;
    } while (i <= n1);
    i = 0;
    do {
        v = ewram_2000462;
        m = 0x3ff;
        m &= v;
        if (tbl[2 * i + 1] == m && tbl[2 * i] == (v >> 10)) {
            *outB = i;
            break;
        }
        i++;
        n2 = 0x1bf;
    } while (i <= n2);
}
