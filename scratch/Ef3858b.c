extern char *iwram_3001ed0;
extern void Func_80f2ebc(void *a, void *b, void *c, int n);

void Func_80f3858(int frames)
{
    char *p;
    int off;

    p = iwram_3001ed0;
    if (p != 0) {
        off = 0x3001;
        p[off] = frames;
        off++;
        p[off] = 0;
        Func_80f2ebc(p + 0x400, p + 0x1000, p + 0x1c00, frames);
    }
}
