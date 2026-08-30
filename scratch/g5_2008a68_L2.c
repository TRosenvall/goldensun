extern unsigned char L160c[] __asm__(".L160c");

extern unsigned char *__MapActor_GetActor(int slot);
extern unsigned char *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetAnim(unsigned char *e, int n);
extern void __Actor_SetScript(unsigned char *e, int s);
extern void __Func_80929d8(unsigned char *e, int n);
extern void OvlFunc_905_2008a00(void);

struct S3 { int a; int b; int c; };

void OvlFunc_905_2008a68(int p0, int p1, int p2, int p3,
                         int e, unsigned int f, unsigned int g)
{
    int v[3];
    char *vp;
    unsigned char *a0;
    unsigned char *ent;
    unsigned char *q;
    unsigned int h;
    int mask;
    int u;
    int w;

    mask = -13;
    a0 = __MapActor_GetActor(0);
    vp = (char *)v;
    *(struct S3 *)vp = *(struct S3 *)L160c;
    ent = __CreateActor(0xde, p0, p1, p2);
    if (ent != 0) {
        q = *(unsigned char **)(ent + 0x50);
        __Actor_SetAnim(ent, (f + 1) & 0xf);
        __Actor_SetScript(ent, *(int *)(vp + ((f & 0xf) << 2)));
        __Func_80929d8(ent, (f >> 16) & 0xf);
        ent[0x55] = 0;
        q[0x26] = 0;
        *(void **)(ent + 0x6c) = (void *)OvlFunc_905_2008a00;
        *(int *)(ent + 0x30) = p3;
        *(int *)(ent + 0x34) = e;
        *(short *)(ent + 0x66) = (short)g;
        h = g >> 16;
        switch (h) {
        case 0:
            u = (*(unsigned char **)(a0 + 0x50))[9] & 0xc;
            q[9] = (q[9] & mask) | u;
            break;
        case 1:
        case 2:
        case 3:
            ent[0x23] &= 0xfe;
            h &= 3;
            q[9] = (q[9] & mask) | (h << 2);
            break;
        }
    }
}
