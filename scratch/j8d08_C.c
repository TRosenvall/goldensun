struct Actor {
    unsigned char pad00[0x64];
    short f64;
    short f66;
    int f68;
    void (*f6c)(void);
};

extern unsigned int iwram_3001e40;
extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void OvlFunc_931_2008c0c(void);
extern void OvlFunc_931_2008c44(void);

void OvlFunc_931_2008d08(void)
{
    struct Actor *q;
    int z;
    int k;
    int c1;
    int c2;
    short *p;

    c1 = 0x80 << 15;
    c2 = 0xc8 << 17;
    z = iwram_3001e40 & 3;
    if (z == 0) {
        q = __CreateActor(0xde, c1, 0, c2);
        if (q != 0) {
            p = &q->f64;
            k = 0x14;
            *p = k;
            p++;
            *p = z;
            q->f68 = k;
            OvlFunc_931_2008c0c();
            q->f6c = OvlFunc_931_2008c44;
            __Actor_SetAnim(q, 1);
        }
    }
}
