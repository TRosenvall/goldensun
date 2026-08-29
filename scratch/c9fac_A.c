extern unsigned char gState[];
extern char *iwram_3001f30;
extern void __Func_8091220(int a, int b);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern void __WaitFrames(int n);

void OvlFunc_922_2009fac(void)
{
    unsigned char *g;
    char *p;
    char *q;

    g = gState;
    if (*(short *)(g + (0xe1 << 1)) <= 6) {
        p = iwram_3001f30;
        q = *(char **)((char *)&iwram_3001f30 - 0x64);
        p[0x34] = 1;
        q[0x53e] = 0;
        q[0x53c] = 1;
        q[0x53d] = 1;
        __Func_8091220(0, 1);
        __Func_8091200(0x203108, 1);
        __Func_8091254(0x10);
        __WaitFrames(0x10);
    }
}
