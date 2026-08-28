typedef unsigned short u16;
typedef unsigned char u8;

extern u8 gState[];
extern u16 ewram_2000462;

void Func_801c8a0(int *outA, int *outB, u16 *tbl)
{
    u8 *g;
    u16 *q1;
    u16 *q2;
    unsigned int h;
    unsigned int mask;
    unsigned int hi;
    unsigned int v;
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
    q2 = tbl;
    i = 0;
    do {
        v = ewram_2000462;
        if (q2[1] == (v & 0x3ff) && q2[0] == (v >> 10)) {
            *outB = i;
            break;
        }
        i++;
        q2 += 2;
        n2 = 0x1bf;
    } while (i <= n2);
}
