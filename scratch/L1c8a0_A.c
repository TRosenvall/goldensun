extern unsigned int gState;
extern unsigned short ewram_2000462;

void Func_801c8a0(int *out1, int *out2, unsigned short *arr)
{
    unsigned short *p;
    unsigned int key;
    unsigned int v;
    unsigned int r2;
    unsigned int r3;
    int a;
    int b;
    int i;

    *out1 = 0;
    *out2 = 0;

    r2 = 0x88;
    r3 = (unsigned int)&gState;
    r2 <<= 2;
    r3 += r2;
    key = *(unsigned short *)r3;
    a = key & 0x3ff;
    b = key >> 10;
    p = arr;
    i = 0;
    do {
        if (p[1] == a && p[0] == b) {
            *out1 = i;
            break;
        }
        i++;
        p += 2;
    } while (i <= 0x1bf);

    p = arr;
    i = 0;
loop2:
    v = ewram_2000462;
    if (p[1] == (v & 0x3ff) && p[0] == (v >> 10)) {
        *out2 = i;
        goto done2;
    }
    i++;
    p += 2;
    if (i <= 0x1bf)
        goto loop2;
done2:
    ;
}
