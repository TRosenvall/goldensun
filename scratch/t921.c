struct A {
    unsigned char pad00[6];
    unsigned short f6;
    unsigned char pad08[0x52];
    unsigned char f5a;
    unsigned char pad5b[9];
    short f64;
};

extern unsigned int gKeyHeld;
extern short gTable_921__0200a3f0[];
extern void __Actor_SetAnim(struct A *a, int id);
extern void __Actor_SetAnimSpeed(struct A *a, int s);

void OvlFunc_921_2009f24(struct A *a)
{
    short d;
    int t;

    if (a->f64 != 0) {
        a->f64--;
        return;
    }
    a->f5a = 0;
    t = gTable_921__0200a3f0[(gKeyHeld >> 4) & 0xf];
    if (t == -1) {
        __Actor_SetAnim(a, 9);
        return;
    }
    d = t - a->f6;
    if (d > (0x80 << 5))
        d = 0x80 << 5;
    if (d < -0x1000)
        d = -0x1000;
    a->f6 = a->f6 + d;
    __Actor_SetAnim(a, 2);
    __Actor_SetAnimSpeed(a, 0x30);
}
