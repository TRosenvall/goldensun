typedef struct {
    unsigned char pad0[0x205];
    unsigned char hour;
    unsigned char min;
} GlobalState;

extern GlobalState gState;
extern unsigned char tbl[] __asm__(".L36750");

void Func_801ca1c(void *dst)
{
    short t;
    short i;
    int m;
    int a, b, c;

    t = ((gState.hour + 12) % 24) * 4;
    m = gState.min - 7;
    i = t % 96;
    a = tbl[i] + m;
    b = tbl[(t + 0x20) % 96] + m;
    c = tbl[(t + 0x40) % 96] + m;
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
