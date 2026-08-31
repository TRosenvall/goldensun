extern int L12c4 __asm__(".L12c4");
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_916_2008150(void)
{
    short *p;
    int v;
    int w;

    p = *(short **)&L12c4;
    if (*(short *)((char *)p + (unsigned int)0) == 1) {
        w = 9;
        v = 4;
        __Func_8010704(0, 0, 1, v, v, w);
    } else {
        v = 6;
        w = 9;
        __Func_8010704(0, 0, 1, 4, v, w);
    }
}
