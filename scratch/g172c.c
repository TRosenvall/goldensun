struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x28 - 0x14];
    int f28;
    unsigned char pad2c[0x55 - 0x2c];
    unsigned char f55;
};

extern unsigned char L7[] __asm__(".L7");
extern unsigned int __Random(void);
extern void __vec3_translate(int a, int b, int *v);
extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __Actor_SetScript(struct Actor *a, unsigned char *s);
extern void __Actor_SetAnim(struct Actor *a, int n);

void OvlFunc_common1_172c(struct Actor *a)
{
    int v[3];
    struct Actor *q;
    unsigned int r1;
    unsigned int r2;

    if (a->f28 >= -0xff && a->f28 <= 0xff)
        a->f55 = 0;
    if (__Random() * 0x64 >> 16 <= 9) {
        v[0] = a->f8;
        v[1] = a->fc;
        v[2] = a->f10;
        r1 = __Random();
        r2 = __Random();
        __vec3_translate(r1 << 4, r2, v);
        q = __CreateActor(0x11d, v[0], v[1], v[2]);
        if (q != 0) {
            q->f55 = 0;
            __Actor_SetSpriteFlags(q, 0);
            __Actor_SetScript(q, L7);
            __Actor_SetAnim(q, 1);
            __Actor_SetAnim(q, 0);
        }
    }
}
