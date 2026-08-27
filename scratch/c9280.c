struct A {
    unsigned char pad00[0xa];
    short fa;
    unsigned char pad0c_[0];
    int fc;
    unsigned char pad10[2];
    short f12;
};

extern int L6668[] __asm__(".L6668");
extern struct A *__MapActor_GetActor(int slot);
extern int OvlFunc_945_2009144(int x, int y);
extern int __TestCollision(struct A *a, int *v);

int OvlFunc_945_2009280(int dir)
{
    struct A *a;
    int v;
    int x;
    int y;
    int t[3];

    a = __MapActor_GetActor(0);
    v = L6668[dir];
    x = a->fa + (v >> 16);
    y = a->f12 + (short)v;
    if (OvlFunc_945_2009144(x, y) == 0) {
        t[0] = x << 16;
        t[1] = a->fc;
        t[2] = y << 16;
        if (__TestCollision(a, t) == 0)
            return 1;
    }
    return 0;
}
