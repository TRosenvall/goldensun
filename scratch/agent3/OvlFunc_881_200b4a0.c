struct Part {
    unsigned char pad_00[9];
    unsigned char b0 : 2, sel : 2, b4 : 4;   /* 0x09 */
    unsigned char pad_0a[0x1c];
    unsigned char f26;                       /* 0x26 */
};

struct Actor {
    unsigned char pad_00[8];
    int x;                      /* 0x08 */
    int y;                      /* 0x0c */
    int z;                      /* 0x10 */
    unsigned char pad_14[4];
    int f18;                    /* 0x18 */
    int f1c;                    /* 0x1c */
    unsigned char pad_20[3];
    unsigned char f23;          /* 0x23 */
    unsigned char pad_24[0x2c];
    struct Part *part;          /* 0x50 */
    unsigned char pad_54[1];
    unsigned char f55;          /* 0x55 */
};

extern unsigned int iwram_3001e40;
extern struct Actor *__MapActor_GetActor(int slot);
extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern unsigned int __Random(void);
extern void __Func_80929d8(struct Actor *a, int n);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __Actor_SetScript(struct Actor *a, unsigned char *s);
extern unsigned char gScript_881__0200e73c[];

void OvlFunc_881_200b4a0(void)
{
    struct Actor *a;
    struct Actor *n;
    struct Part *s;
    int d;

    if ((iwram_3001e40 & 0xf) == 0) {
        a = __MapActor_GetActor(8);
        n = __CreateActor(0xde, a->x - 0x200000, a->y, a->z - 0x100000);
        if (n != 0) {
            n->f18 = 0x80 << 8;
            n->f1c = 0x80 << 8;
            s = n->part;
            if ((__Random() * 2) >> 16) {
                d = __Random();
                d *= 3;
                d <<= 4;
                d = (unsigned int)d >> 16;
                d <<= 16;
                n->x -= d >> 1;
                n->z -= d;
            } else {
                d = __Random() * 32;
                d = (unsigned int)d >> 16;
                d <<= 16;
                n->x += d;
                d >>= 1;
                n->z += d;
            }
            s->f26 = 0;
            s->sel = a->part->sel;
            n->f23 |= 2;
            n->f55 = a->f55;
            __Func_80929d8(n, 9);
            __Actor_SetAnim(n, 2);
            __Actor_SetScript(n, gScript_881__0200e73c);
        }
    }
}
