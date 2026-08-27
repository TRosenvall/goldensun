struct M {
    unsigned char pad00[0xb];
    unsigned char fb;
    unsigned char pad0c[2];
    unsigned short fe;
};

extern struct M *Func_807882c(void *u, int n);
extern int CheckEquipmentCritBoost(void *u);
extern int RPGRandom(void);

int Func_8079d1c(void *u)
{
    struct M *m;
    int v;

    if (*((unsigned char *)u + 0x129) == 0)
        return 1;
    m = Func_807882c(u, 1);
    if (m == 0)
        return 1;
    if (m->fe == 0)
        return 1;
    v = ((CheckEquipmentCritBoost(u) + m->fb * 5) << 16) / 100;
    if (v > (RPGRandom() & 0xffff))
        return m->fe;
    return 1;
}
