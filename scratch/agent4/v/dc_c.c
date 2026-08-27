extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];
extern int Func_8000888(int a, int b);

struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
};

extern struct Actor *GetFieldActor(int id);

typedef int (*Fn)(int a, int b);

void Func_80982dc(void)
{
    unsigned char *s;
    unsigned char *g;
    struct Actor *act;
    short *pc;
    Fn fn;
    int dy, v, t;

    s = iwram_3001ebc;
    g = gState;
    act = GetFieldActor(*(int *)(g + (0xfa << 1)));
    if (*(short *)(s + (0xcc << 4)) != 0) {
        pc = (short *)(s + 0xcba);
        if (*pc != 0)
            t = *(unsigned short *)pc;
            t = t - 1;
            *pc = t;
    }
    fn = Func_8000888;
    v = fn(*(short *)(s + 0xcbc) - act->f8 / 0x10000, 0xd105);
    dy = *(short *)(s + 0xcbe) - (act->f10 - act->fc) / 0x10000;
    if (v * v + dy * dy >= 0xe1 << 4 || *(short *)(s + 0xcba) == 0)
        *(unsigned short *)(s + (0xbf << 1)) = 0x2090;
}
