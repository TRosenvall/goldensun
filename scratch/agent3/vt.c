extern short L1ca8[] __asm__(".L1ca8");
extern int __GetFlag(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);

void OvlFunc_918_20097ec(void)
{
    char *t;
    int off;
    int one;

    t = (char *)L1ca8;
    if (*(short *)t != -1) {
        off = 0;
        one = 1;
        do {
            if (__GetFlag(*(short *)(t + off)) && *(short *)(t + off + 2) != 0)
                __CopyMapTiles(*(short *)(t + off + 4), *(short *)(t + off + 6),
                               *(short *)(t + off + 8), *(short *)(t + off + 10),
                               one, one);
            off += 0xc;
        } while (*(short *)(t + off) != -1);
    }
}
