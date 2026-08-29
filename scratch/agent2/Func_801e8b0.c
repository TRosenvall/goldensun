typedef unsigned char u8;
typedef unsigned short u16;

struct W { unsigned char pad0[0xc]; u16 fc; u16 fe; };

extern u16 *Func_8004970(int size);
extern unsigned int iwram_3001e8c;
extern int Func_801de5c(u16 *buf, u16 *a, u16 *b, int c);
extern void free(void *p);

void Func_801e8b0(u8 *s, struct W *w, unsigned int x, unsigned int y)
{
    u16 *buf;
    u16 *p;
    u16 *vram;
    unsigned int n;
    int m;

    buf = Func_8004970(0x80 * 4);
    vram = (u16 *)iwram_3001e8c;
    p = buf;
    while (*s != 0) {
        *p = *s;
        s++;
        p++;
    }
    *p = 0;
    n = (w->fe + (y >> 3) + 1) * 32 + (w->fc + (x >> 3)) + 1;
    if (n < 0xa0 * 4) {
        m = 7;
        m &= x;
        Func_801de5c(buf, vram + n, (u16 *)0x6002000 + n, m);
        free(buf);
    }
}
