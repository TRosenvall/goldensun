extern unsigned char gState[];
extern int _GetPartySize(void);
extern void *_GetUnit(int id);
extern void _ModifyHP(int id, int delta);
extern void Func_8091220(int a, int b);
extern void Func_8091254(int a);
extern void _PlaySound(int id);

int UpdatePoison(void)
{
    unsigned char *g;
    char *u;
    int n;
    int i;
    int worst;
    int d;
    int st;

    worst = 0;
    n = _GetPartySize();
    for (i = 0; i < n; i++) {
        u = (char *)_GetUnit(gState[(0xfc << 1) + i]);
        st = *(signed char *)(u + 0x131);
        switch (st) {
        case 1:
            d = -((*(short *)(u + 0x34) + 0xa) / 0x14);
            if (d == 0)
                d = -1;
            if (worst <= 0)
                worst = 1;
            break;
        case 2:
            d = -((*(short *)(u + 0x34) + 5) / 0xa);
            if (d == 0)
                d = -1;
            if (worst <= 1)
                worst = 2;
            break;
        default:
            d = 0;
            break;
        }
        _ModifyHP(gState[(0xfc << 1) + i], d);
    }
    if (worst != 0) {
        Func_8091220(0x1ff, 0);
        Func_8091254(4);
        _PlaySound(0x85);
    }
    return worst;
}
