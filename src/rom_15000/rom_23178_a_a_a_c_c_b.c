extern unsigned char *iwram_3001f38;
extern void Func_8016478(void *w);
extern void DrawSmallText(int id, void *w, int x, int y);

void Func_8028b80(void)
{
    unsigned char *g;
    unsigned char *a;
    unsigned char *b;
    int m;

    g = iwram_3001f38;
    b = g + 0x96;
    a = g + 0x8c;
    if (*(short *)b != *(short *)a) {
        *(unsigned short *)b = *(unsigned short *)a;
        Func_8016478(*(void **)(g + 0x7c));
        if (*(short *)a == 0) {
            m = 0xc71;
            DrawSmallText(m, *(void **)(g + 0x7c), 0x10, 4);
            m += 1;
            DrawSmallText(m, *(void **)(g + 0x7c), 0x10, 0x10);
        } else {
            m = 0xc73;
            DrawSmallText(m, *(void **)(g + 0x7c), 0, 4);
            DrawSmallText(m + 1, *(void **)(g + 0x7c), 0, 0x10);
            m += 2;
            DrawSmallText(m, *(void **)(g + 0x7c), 0, 0x1c);
        }
    }
}
