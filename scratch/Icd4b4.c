extern char *iwram_3001e74[];
extern void _UploadBGPalette(void *a, void *b, int c, int d);

void Func_80cd4b4(void)
{
    char *p;
    char *g;
    int *ctr;
    int n;

    p = iwram_3001e74[0x1e];
    g = iwram_3001e74[0];
    ctr = (int *)(p + 0x77b4);
    if (*ctr > 0) {
        n = *(int *)(p + 0x77b8) + 1;
        *(int *)(p + 0x77b8) = n;
        _UploadBGPalette(g + 0x544, (void *)0x50000c0, (0x80 << 9) - n * 1092, 0x80);
        *ctr = *ctr - 1;
    }
}
