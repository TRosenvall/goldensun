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
    unsigned int a;
    unsigned int b;

    buf = Func_8004970(0x80 * 4);
    vram = (u16 *)iwram_3001e8c;
    p = buf;
    while (*s != 0) {
        *p = *s;
        s++;
        p++;
    }
    *p = 0;
    a = w->fe + (y >> 3);
    a += 1;
    b = w->fc + (x >> 3);
    a <<= 5;
    a += b;
    n = a + 1;
    if (n < 0xa0 * 4) {
        Func_801de5c(buf, vram + n, (u16 *)0x6002000 + n, x & 7);
        free(buf);
    }
}
