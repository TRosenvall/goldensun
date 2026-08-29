extern char *iwram_3001f2c;
extern int Func_80a10d0(void *p, int a, int b, int c, int d, int e);
extern void *_Func_801ec6c(int a, int b, int c, void *d, int e, int f);
extern void Func_80a33d4(void *g, void *w);
extern void Func_80a9cf8(void *w);
extern void Func_80a8604(void *w, int page, int flags);

void Func_80a8088(int page)
{
    char *g;
    void *w;
    char *n;
    int created;

    g = iwram_3001f2c;
    w = *(void **)(g + 0x24);
    created = 0;
    if (w == 0) {
        created = Func_80a10d0(g + 0x24, 0, 5, 0x1e, 0xf, 2);
        w = *(void **)(g + 0x24);
    }
    if (created != 0) {
        char *q;
        n = (char *)_Func_801ec6c(page, 0, 0, w, 0, 0);
        q = g + 0x17c;
        *(void **)q = n;
        n[0xf] = 0xf0;
        if (*(unsigned short *)(g + 0x220) == 3)
            Func_80a33d4(g, w);
        Func_80a9cf8(w);
        Func_80a8604(w, page, 0x100);
    } else {
        Func_80a8604(w, page, 0);
    }
}
