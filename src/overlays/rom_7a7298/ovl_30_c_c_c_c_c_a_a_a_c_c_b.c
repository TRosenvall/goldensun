extern int L31f0[] __asm__(".L31f0");
extern void __vec3_translate(int a, int b, int *v);
extern void __DeleteActor(void *a);

struct A {
    unsigned char pad00[8];
    int f08;
    int f0c;
    int f10;
};

void OvlFunc_921_200954c(struct A *a)
{
    int v[3];
    short *ctr;
    int n;

    if (a != 0) {
        ctr = (short *)((char *)a + 0x64);
        *ctr = *ctr - 1;
        n = *ctr;
        if (n != 0) {
            v[0] = L31f0[0];
            v[1] = L31f0[1] + (0x80 << 12);
            v[2] = L31f0[2];
            __vec3_translate(n << 16, (n << 11) + *(short *)((char *)a + 0x66), v);
            a->f08 = v[0];
            a->f0c = v[1];
            a->f10 = v[2];
        } else {
            __DeleteActor(a);
        }
    }
}
