typedef struct { unsigned char b[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char tbl[] __asm__(".L36750");

void Func_801ca1c(void *dst)
{
    unsigned char *g;
    unsigned char *p;
    unsigned char *tb;
    short t;
    short i;
    int m;
    int a, b, c;

    g = (unsigned char *)&gState;
    p = g + 0x205;
    t = ((*p + 12) % 24) * 4;
    p = g + 0x206;
    m = *p - 7;
    tb = tbl;
    i = t % 96;
    a = tb[i];
    b = tb[(t + 0x20) % 96];
    c = tb[(t + 0x40) % 96];
    a += m;
    b += m;
    c += m;
    if (a < 0) a = 0;
    if (b < 0) b = 0;
    if (c < 0) c = 0;
    if (a > 31) a = 31;
    if (b > 31) b = 31;
    if (c > 31) c = 31;
    *(short *)((char *)dst + 0x576) = a;
    *(short *)((char *)dst + 0x578) = b;
    *(short *)((char *)dst + 0x57a) = c;
}
