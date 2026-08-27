struct Actor {
    unsigned char pad00[8];
    int x;
    int y;
    int z;
    unsigned char pad14[0x18 - 0x14];
    int f18;
    int f1c;
    unsigned char pad20[0x38 - 0x20];
    int f38;
    int f3c;
    int f40;
    unsigned char pad44[0x50 - 0x44];
    unsigned char *f50;
};

extern void __vec3_translate(int a, int b, int *v);
extern void __Func_8003f3c(int n);
extern void __DeleteActor(struct Actor *a);

void OvlFunc_917_200952c(struct Actor *a)
{
    int v[3];
    short *t;
    int n;
    int d;

    t = (short *)((char *)a + 0x64);
    n = *t;
    if (n > 0x77) {
        __Func_8003f3c(a->f50[0x1c]);
        __DeleteActor(a);
        return;
    }
    v[0] = a->f38;
    v[1] = a->f3c;
    v[2] = a->f40;
    d = *(short *)((char *)a + 0x66);
    __vec3_translate(n << 16, ((n * 3) << 8) + d, v);
    a->x = v[0];
    a->y = v[1];
    a->z = v[2];
    a->f18 += 0x147;
    a->f1c += 0x147;
    *(unsigned short *)t = *(unsigned short *)t + 1;
}
