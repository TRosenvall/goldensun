extern unsigned char iwram_3001f30[];
extern unsigned char gState[];
extern void _PlaySound(int id);
extern void StopTask(void *fn);
extern void Func_80982dc(void);
extern void Func_8098294(int a);
extern void Func_8091200(int a, int b);
extern void Func_8091254(int a);
extern void Func_8091220(int a, int b);
extern void WaitFrames(int n);
extern int Func_808e4b4(int a, int b, int *out);
extern void Func_8096b28(int a, int b, int c);

void Func_80984c0(void)
{
    unsigned char *a;
    unsigned char *b;
    unsigned char *c;
    unsigned char *g;
    int v;
    int k;
    int r;
    int z;

    a = *(unsigned char **)(iwram_3001f30);
    c = *(unsigned char **)(iwram_3001f30 - 0x64);
    b = *(unsigned char **)(iwram_3001f30 - 0x74);
    if (*(short *)(b + 0xcb8) != 0) {
        _PlaySound(0xa7);
        StopTask(Func_80982dc);
        z = 0;
        *(short *)(b + 0xcb8) = z;
        *(short *)(b + 0xcba) = z;
        Func_8098294(0);
        k = 0x80 << 9;
        Func_8091200(k, 1);
        Func_8091254(1);
        Func_8091220(0, 0);
        Func_8091200(k, 0);
        Func_8091254(0x1e);
        WaitFrames(1);
        r = Func_808e4b4(0x40000005, 8, &v);
        if (r != 0) {
            g = gState;
            Func_8096b28(r, *(int *)(g + (0xfa << 1)), v);
        }
        if (*(signed char *)(a + 0x34) == 0) {
            *(char *)(c + 0x53e) = 0;
            *(char *)(c + 0x53c) = 1;
            *(char *)(c + 0x53d) = 1;
            WaitFrames(0xa);
        }
    }
}
