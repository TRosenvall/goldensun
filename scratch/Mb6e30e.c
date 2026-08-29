extern char *iwram_3001e74;
extern void _PreloadSpriteGFX(int a, int b, int c, int d);

int Func_80b6e30(int slot)
{
    int p;
    char *off;
    int i;

    p = (int)iwram_3001e74;
    i = 0;
    off = (char *)4;
    do {
        if (*(short *)(off + p) == slot) {
            _PreloadSpriteGFX(i, 0, 0, 0);
            *(short *)(off + p) = 0;
        }
        i++;
        off += 2;
    } while (i <= 5);
}
