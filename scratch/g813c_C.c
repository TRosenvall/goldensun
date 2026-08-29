struct Sprite {
    unsigned char pad00[9];
    unsigned char b0 : 1,
                  b1 : 1,
                  b2 : 2,
                  b4 : 4;
};

struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x18 - 0x14];
    int f18;
    int f1c;
    unsigned char pad20[0x50 - 0x20];
    struct Sprite *f50;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[0x64 - 0x56];
    short f64;
    unsigned char pad66[0x6c - 0x66];
    void (*f6c)(void);
};

extern unsigned int iwram_3001e40;
extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern void __PlaySound(int id);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void OvlFunc_881_200811c(void);

void OvlFunc_881_200813c(struct Actor *a)
{
    struct Actor *q;
    struct Sprite *s;
    int id;

    id = 0x11d;
    if (iwram_3001e40 & 4) {
        a->f18 = 0x14ccc;
        a->f1c = 0x14ccc;
    } else {
        a->f18 = 0x80 << 9;
        a->f1c = 0x80 << 9;
    }
    if (iwram_3001e40 & 2) {
        q = __CreateActor(id, a->f8, a->fc, a->f10);
        __PlaySound(0xf6);
        if (q != 0) {
            q->f55 = 0;
            s = q->f50;
            s->b2 = 1;
            __Actor_SetSpriteFlags(q, 0);
            __Actor_SetAnim(q, 1);
            q->f64 = 0;
            q->f6c = OvlFunc_881_200811c;
        }
    }
}
