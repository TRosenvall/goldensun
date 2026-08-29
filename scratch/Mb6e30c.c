extern char *iwram_3001e74;
extern void _PreloadSpriteGFX(int a, int b, int c, int d);

void Func_80b6e30(int slot)
{
    char *p;
    int i;
    int off;
    short z;

    p = iwram_3001e74;
    z = 0;
    i = 0;
    off = 4;
    do {
        if (*(short *)(p + off) == slot) {
            _PreloadSpriteGFX(i, 0, 0, 0);
            *(short *)(p + off) = z;
        }
        i++;
        off += 2;
    } while (i <= 5);
}
