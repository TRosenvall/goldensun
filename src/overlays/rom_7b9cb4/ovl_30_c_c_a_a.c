struct Actor {
    unsigned char pad00[6];
    unsigned short f6;
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0xe];
    unsigned char f22;
    unsigned char pad23[0xd];
    int f30;
    int f34;
    unsigned char pad38[0x18];
    void *f50;
    unsigned char pad54[7];
    unsigned char f5b;
    unsigned char pad5c[8];
    short f64;
    short f66;
};

extern unsigned char gState[];
extern int Func_8000888(int a, int b);
extern struct Actor *__MapActor_GetActor(int slot);
extern struct Actor *__GetFieldActor(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __vec3_translate(int a, int b, int *v);
extern int __Func_8011f54(int a, int b, int c);
extern int __Func_8012038(int a, int b, int c);
extern void __Sprite_AddLayer(void *s, int n);
extern void __SetCameraTarget(int slot, int a);
extern void __Func_8093530(void);
extern void __Func_80933d4(int a, int b);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __Actor_WaitMovement(struct Actor *a);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern void OvlFunc_932_200b5ac(struct Actor *a);
extern void OvlFunc_932_200b668(struct Actor *a);
extern void OvlFunc_932_200b724(struct Actor *a);
extern void OvlFunc_932_200b484(struct Actor *a);
extern int L51b4[] __asm__(".L51b4");

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")

static inline int call_via(int (*f)(int, int), int a, int b)
{
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile (
        "\t.align\t2, 0\n"
        "\tmov\tr12, pc\n"
        "\tbx\t%1"
        : "=r" (_a)
        : "r" (f), "0" (_a), "r" (_b)
        : "memory", "r12", "r2"
    );
    return _a;
}

void OvlFunc_932_200b738(struct Actor *a)
{
    int v[3];
    int ang;
    int t;
    short *q;
    int y1, y2;

    ang = a->f6 & 0xc000;
    t = a->fc / 0x10000;
    t = L51b4[a->f64 - t + 16];
    q = &a->f66;
    if (*q != 0) {
        (*q)--;
        if (*q == 0x14)
            __PlaySound(0xb8);
        if (*q == 0)
            __PlaySound(0xe9);
    }
    v[0] = a->f8;
    v[1] = a->fc;
    v[2] = a->f10;
    __vec3_translate(call_via(Func_8000888, t, 0xc000), ang, v);
    a->f8 = v[0];
    a->f10 = v[2];
    y1 = __Func_8011f54(2, v[0], v[2]);
    __vec3_translate(-call_via(Func_8000888, t, 0x18000), ang, v);
    y2 = __Func_8011f54(2, v[0], v[2]);
    if (*q <= 0x14) {
        if (y1 == y2)
            __Actor_SetAnim(a, 2);
        else if (y1 > y2)
            __Actor_SetAnim(a, 3);
        else
            __Actor_SetAnim(a, 4);
    }
}

void OvlFunc_932_200b850(int slot, int dir)
{
    int v[3];
    struct Actor *a;
    struct Actor *b;
    unsigned char *gs;
    int i;

    a = __MapActor_GetActor(slot);
    gs = gState;
    b = __GetFieldActor(*(int *)(gs + 0x1f4));
    __CutsceneStart();
    if (dir == -1)
        dir = a->f6;
    for (i = 0; i <= 3; i++) {
        v[0] = a->f8;
        v[1] = a->fc;
        v[2] = a->f10;
        __vec3_translate(0x80 << 13, dir, v);
        if (__Func_8011f54(2, v[0], v[2]) == a->fc)
            break;
        dir += 0x80 << 7;
    }
    if (i != 4) {
        a->f22 = 2;
        b->f8 = 0;
        b->f10 = 0;
        { register int n __asm__("r1") = 0x10; __Sprite_AddLayer(a->f50, n); }
        __SetCameraTarget(slot, 1);
        __Func_8093530();
        { PIN2; q0 = 0x80 << 13; q1 = 0x80 << 10; __Func_80933d4(q0, q1); }
        a->f6 = dir;
        a->f30 = 0x80 << 10;
        a->f34 = 0xccc;
        a->f5b = 0;
        a->f64 = a->fc / 0x10000;
        a->f66 = 0;
        v[0] = a->f8;
        v[1] = a->fc;
        v[2] = a->f10;
        __vec3_translate(0xc0 << 13, dir, v);
        __Actor_TravelTo(a, v[0], a->fc, v[2]);
        __Actor_WaitMovement(a);
        __PlaySound(0xe9);
        for (;;) {
            switch (__Func_8012038(2, a->f8, a->f10)) {
            case 0x62:
                OvlFunc_932_200b5ac(a);
                break;
            case 0x61:
                OvlFunc_932_200b668(a);
                break;
            case 0x60:
                OvlFunc_932_200b724(a);
                break;
            case 0x63:
                goto finish;
            }
            OvlFunc_932_200b738(a);
            __WaitFrames(1);
        }
    finish:
        OvlFunc_932_200b484(a);
        __CutsceneEnd();
    }
}
