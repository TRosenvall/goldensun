extern char *iwram_3001e74;
extern void _PreloadSpriteGFX(int a, int b, int c, int d);

void Func_80b6e30(int slot)
{
    char *p;
    int i;
    int off;

    p = iwram_3001e74;
    i = 0;
    off = 4;
    do {
        if (*(short *)(p + off) == slot) {
            _PreloadSpriteGFX(i, 0, 0, 0);
            *(short *)(p + off) = 0;
        }
        i++;
        off += 2;
    } while (i <= 5);
}
