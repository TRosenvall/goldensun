struct Actor {
    int f00;
    unsigned char pad04[4];
    int f08;
    int f0c;
    int f10;
    unsigned char pad14[0x22 - 0x14];
    unsigned char f22;
    unsigned char f23;
    unsigned char pad24[0x44 - 0x24];
    int f44;
    int f48;
    unsigned char pad4c[4];
    unsigned char *f50;
    unsigned char pad54[1];
    unsigned char f55;
    unsigned char pad56[3];
    unsigned char f59;
    unsigned char pad5a[0x63 - 0x5a];
    unsigned char f63;
};

extern unsigned int L5128[][2] __asm__(".L5128");
extern unsigned char L5d12[] __asm__(".L5d12");
extern unsigned char gScript_968__0200d488[];
extern unsigned char gScript_968__0200d508[];

extern void *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern int __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8010560(void *s, int a, int b);
extern void __Func_8092b08(int a, int b);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __DeleteActor(void *a);
extern void *OvlFunc_968_2008098(int a, int b, int c, int d);
extern void OvlFunc_968_200894c(struct Actor *a);
extern struct Actor *OvlFunc_968_2008c5c(int a, int b, void *s);

void OvlFunc_968_2009f60(void)
{
    struct Actor *e;
    struct Actor *o;
    struct Actor *c1;
    struct Actor *c2;
    void *n;
    struct Actor *b;
    unsigned int s;
    unsigned int j;
    unsigned int k;
    int d;
    int e1, f1, e3, f3, e5, f5, e6, f6, e7, f7;
    register int e4 __asm__("r3");
    register int f4 __asm__("r2");

    n = 0;
    b = __MapActor_GetActor(0);
    __CutsceneStart();
    e1 = 0x2c;
    f1 = 0x27;
    __Func_8010704(0x6c, 0x27, 0xd, 7, e1, f1);
    for (s = 9; s <= 0xb; s++) {
        e = __MapActor_GetActor(s);
        if (e->f23 != 2) {
            e3 = e->f08 >> 20;
            f3 = e->f10 >> 20;
            __Func_8010704(0x2f, 0x27, 1, 1, e3, f3);
        } else {
            e7 = e->f08 >> 20;
            f7 = e->f10 >> 20;
            __Func_8010704(0x2e, 0x27, 1, 1, e7, f7);
        }
        k = 5;
        for (j = 0; j <= 3; j++) {
            if ((e->f08 >> 20) == L5128[j][0] && (e->f10 >> 20) == L5128[j][1]
                && e->f0c >= 0) {
                k = j;
                break;
            }
        }
        if (k == 5)
            continue;
        for (j = 9; j <= 0xb; j++) {
            o = __MapActor_GetActor(j);
            if (s != j && (e->f08 >> 20) == (o->f08 >> 20)
                && (e->f10 >> 20) == (o->f10 >> 20)) {
                k = 5;
                break;
            }
        }
        if (k == 5)
            continue;
        d = ((unsigned int)b->f50[9] << 28) >> 30;
        if ((b->f10 >> 20) <= L5128[k][1]) {
            n = OvlFunc_968_2008098(e->f08, e->f0c, e->f10 - 0x40000, 0x14);
            __Func_8092b08(0, 3);
        }
        __Actor_SetSpriteFlags(__MapActor_GetActor(s), 0);
        e->f22 = 0;
        e->f55 = 3;
        e->f48 = 0x1999;
        e->f44 = 0;
        e4 = L5128[k][0];
        f4 = L5128[k][1];
        __Func_8010704(0x2a, 0x29, 1, 1, e4, f4);
        OvlFunc_968_200894c(e);
        __PlaySound(0xbc);
        e->f59 = 0;
        e->f55 = 0;
        e->f0c = 0xfff00000;
        __Func_8092b08(s, 3);
        e->f23 = 2;
        e5 = L5128[k][0];
        f5 = L5128[k][1];
        __Func_8010704(0x2e, 0x27, 1, 1, e5, f5);
        __Func_8092b08(0, d);
        ((struct Actor *)__MapActor_GetActor(0))->f23 |= 1;
        if (n != 0)
            __DeleteActor(n);
        if (__GetFlag(0xc1 << 2)) {
            __CutsceneEnd();
            return;
        }
        if (((((struct Actor *)__MapActor_GetActor(9))->f23
              & ((struct Actor *)__MapActor_GetActor(0xa))->f23
              & ((struct Actor *)__MapActor_GetActor(0xb))->f23) & 2) == 0)
            continue;
        c1 = OvlFunc_968_2008c5c(0xde << 2, 0xaa << 2, gScript_968__0200d488);
        c2 = OvlFunc_968_2008c5c(0xde << 2, 0xaa << 2, gScript_968__0200d508);
        while (c1->f00 != 0 || c2->f00 != 0) {
            if (c1->f63 != 0 || c2->f63 != 0) {
                __CutsceneWait(0x1e);
                __PlaySound(0x9e);
                __Func_8010560(L5d12, 0x6d, 0x25);
                e6 = 0x2d;
                f6 = 0x26;
                __Func_8010704(0x2d, 0x25, 1, 1, e6, f6);
                if ((((struct Actor *)__MapActor_GetActor(9))->f08 >> 20) == L5128[0][0]
                    && (((struct Actor *)__MapActor_GetActor(9))->f10 >> 20) == L5128[0][1])
                    __SetFlag(0x302);
                else
                    __SetFlag(0x303);
                __SetFlag(0xc1 << 2);
                break;
            }
            __WaitFrames(1);
        }
    }
    __CutsceneEnd();
}
