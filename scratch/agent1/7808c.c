struct Unit {
    unsigned char pad00[0x14];
    short h14;
    short h16;
    unsigned char pad18[0x1c];
    short h34;
    short h36;
    short h38;
    short h3a;
    unsigned char pad3c[0xf5];
    unsigned char b131;
    unsigned char pad132[0xe];
    unsigned char b140;
};

extern unsigned char gState[];
extern int GetPartySize(void);
extern struct Unit *GetUnit(int id);

void Func_807808c(int sel)
{
    struct Unit *u;
    int n, i, q, t;

    n = GetPartySize();
    for (i = 0; i < n; i++) {
        u = GetUnit(gState[0x1f8 + i]);
        u->h38 = u->h34;
        u->h3a = u->h36;
        q = (u->h38 << 14) / u->h34;
        t = 0x4000;
        if (q <= 0x4000) {
            t = 0;
            if (q >= 0)
                t = q;
        }
        u->h14 = t;
        if (u->h14 == 0 && u->h38 != 0)
            u->h14 = 1;
        q = (u->h3a << 14) / u->h36;
        t = 0x4000;
        if (q <= 0x4000) {
            t = 0;
            if (q >= 0)
                t = q;
        }
        u->h16 = t;
        if (u->h16 == 0 && u->h3a != 0)
            u->h16 = 1;
        if (sel == 1) {
            u->b131 = 0;
            u->b140 = 0;
        }
    }
}
