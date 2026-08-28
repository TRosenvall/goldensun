typedef unsigned char u8;

extern u8 **GetBattleActor(int id);

void Func_80c0f98(int id, int pri)
{
    u8 **a;
    u8 *p;
    u8 *s;
    u8 **q;
    u8 *f;
    int t;
    int v;
    int i;
    int m;

    a = GetBattleActor(id);
    if (a == 0)
        return;
    p = *a;
    if (p == 0)
        return;
    f = p;
    f += 0x54;
    t = 0xf;
    t &= *f;
    switch (t) {
    case 1:
    {
        s = *(u8 **)(p + 0x50);
        pri &= 3;
        m = -13;
        v = pri << 2;
        s[5] = (m & s[5]) | v;
        s[0x11] = (m & s[0x11]) | v;
        break;
    }
    case 2:
    {
        pri &= 3;
        q = *(u8 ***)(p + 0x50);
        v = pri << 2;
        m = -13;
        for (i = 0; i <= 3; i++) {
            s = *q;
            q++;
            if (s == 0)
                return;
            s[5] = (m & s[5]) | v;
            s[0x11] = (m & s[0x11]) | v;
        }
        break;
    }
    }
}
