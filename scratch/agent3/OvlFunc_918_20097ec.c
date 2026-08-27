extern short L1ca8[] __asm__(".L1ca8");
extern int __GetFlag(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);

#define T(o) (*(short *)((char *)L1ca8 + (o)))

void OvlFunc_918_20097ec(void)
{
    int off;
    int one;

    if (T(0) != -1) {
        off = 0;
        one = 1;
        do {
            if (__GetFlag(T(off)) && T(off + 2) != 0)
                __CopyMapTiles(T(off + 4), T(off + 6), T(off + 8), T(off + 10),
                               one, one);
            off += 0xc;
        } while (T(off) != -1);
    }
}
