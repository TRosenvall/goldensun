extern unsigned char *iwram_3001f2c;
extern void _DeleteSprite(int s);
extern int _CreateSprite(int res);
extern void _Sprite_SetAnim(int s, int n);
extern int StartTask(void (*f)(void), int k);
extern void Func_80ad35c(void);
extern int gResTable[] __asm__(".Laf304");

void Func_80ad274(int window, int unused)
{
    unsigned int base;
    int *p;
    short *q;
    char *off;
    char *k;
    int tbl;
    int i, s, z;

    base = (unsigned int)iwram_3001f2c;
    z = 0;
    for (off = (char *)(0x89 << 2), i = 3; i >= 0; i--, off += 4) {
        if (*(int *)(off + base) != 0) {
            _DeleteSprite(*(int *)(off + base));
            *(int *)(off + base) = z;
        }
    }
    tbl = (int)gResTable;
    q = (short *)(base + (0x8d << 2));
    p = (int *)(base + (0x89 << 2));
    for (k = 0, i = 3; i >= 0; i--, k += 4, q++) {
        s = _CreateSprite(*(int *)(k + tbl));
        if (s != 0)
            _Sprite_SetAnim(s, 2);
        q[0] = 0x10;
        q[4] = 0x20;
        *p++ = s;
    }
    StartTask(Func_80ad35c, 0xc8 << 4);
}
