typedef unsigned char u8;

extern u8 ewram_2002004[];
extern u8 ewram_2000000[];
extern int _MSG_0a;
extern int _MSG_0b;
extern int Func_80056cc(void);
extern void Func_801776c(int msg, int a);
extern void PrepareSaveHeader(void);
extern int SomethingSaveHeader(int a, u8 *b);
extern void Func_8005cf8(void);

int Func_801f9b4(void)
{
    short *p;
    u8 *q;
    int v;
    int e;
    int res;

    p = (short *)ewram_2002004;
    res = 0;
    v = p[0];
    if (v == -1)
        return v;
    e = Func_80056cc();
    if (e != 0) {
        Func_801776c((int)&_MSG_0a, 1);
        res = -9;
    } else {
        PrepareSaveHeader();
        q = ewram_2000000;
        e = SomethingSaveHeader(p[0], q);
        q += 0x80 << 5;
        e |= SomethingSaveHeader(p[0] + 3, q);
        if (e != 0) {
            Func_801776c((int)&_MSG_0b, 1);
            res = -3;
        }
    }
    Func_8005cf8();
    return res;
}
