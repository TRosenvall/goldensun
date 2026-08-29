struct Sprite {
    unsigned char pad00[9];
    unsigned char b0 : 2,
                  b2 : 2,
                  b4 : 4;
    unsigned char pad0a[0x26 - 0xa];
    unsigned char f26;
};

struct Actor {
    unsigned char pad00[0x18];
    int f18;
    unsigned char pad1c[0x23 - 0x1c];
    unsigned char f23;
    unsigned char pad24[0x30 - 0x24];
    int f30;
    int f34;
    unsigned char pad38[0x50 - 0x38];
    struct Sprite *f50;
    unsigned char pad54;
    unsigned char f55;
};

extern unsigned int iwram_3001e40;
extern int L36a0 __asm__(".L36a0");
extern unsigned char gScript_911__0200b5d8[];
extern void __PlaySound(int id);
extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __Actor_SetScript(struct Actor *a, unsigned char *s);

void OvlFunc_911_200a608(void)
{
    struct Actor *q;
    struct Sprite *s;
    int z;
    int c1;
    int c2;
    int t1;
    int t2;

    c1 = 0xc4 << 15;
    c2 = 0xd2 << 15;
    t1 = 0xc4 << 15;
    t2 = 0x10d0000;
    z = iwram_3001e40 & 7;
    if (z == 0) {
        if (L36a0 != 0)
            __PlaySound(0xc8);
        q = __CreateActor(0x1a, c1, 0, c2);
        if (q != 0) {
            s = q->f50;
            s->f26 = z;
            q->f23 &= 0xfe;
            s->b2 = 1;
            q->f18 = 0x1999;
            q->f30 = 0x80 << 12;
            q->f34 = 0x80 << 12;
            q->f55 = z;
            __Actor_SetAnim(q, 2);
            __Actor_TravelTo(q, t1, 0, t2);
            __Actor_SetScript(q, gScript_911__0200b5d8);
        }
    }
}
