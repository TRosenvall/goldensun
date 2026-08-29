typedef unsigned char u8;

struct A {
    u8 pad0[8];
    int f8;
    int fc;
    int f10;
    int f14;
    u8 pad18[0x28 - 0x18];
    int f28;
    u8 pad2c[0x3c - 0x2c];
    int f3c;
    u8 pad40[0x55 - 0x40];
    u8 f55;
};

extern struct A *MapActor_GetActor(int id);
extern void _PlaySound(int id);
extern void _Actor_SetAnim(struct A *a, int n);
extern void WaitFrames(int n);
extern void _Actor_TravelTo(struct A *a, int x, int y, int z);
extern void Func_8092adc(int id, int a, int b);
extern void Func_8092624(struct A *a, int flag);
extern void _Actor_WaitMovement(struct A *a);
extern void Func_809202c(void);

void Func_8092708(int id, int anim, int flag)
{
    struct A *a;
    u8 *p;
    int y0;
    unsigned int i;
    int y;
    u8 two;

    a = MapActor_GetActor(id);
    y0 = a->f10;
    if (a == 0)
        return;
    _PlaySound(0x121);
    _Actor_SetAnim(a, anim);
    p = (u8 *)a;
    p += 0x55;
    WaitFrames(10);
    _Actor_SetAnim(a, 1);
    two = 2;
    *p = two | *p;
    a->f28 = 0x80 << 11;
    _Actor_TravelTo(a, a->f8, a->fc, y0 + (0xc0 << 12));
    WaitFrames(6);
    _PlaySound(0xd9);
    i = 0;
    Func_8092adc(id, 0xa0 << 7, 0);
    *p = i;
    do {
        y = a->fc + 0xfffe0000;
        a->fc = y;
        a->f3c = y;
        WaitFrames(1);
        if (flag != -1 && (i & 1) != 0)
            Func_8092624(a, flag);
        i++;
    } while (i <= 0xd);
    *p = 3;
    a->f28 = 0xc0 << 10;
    _Actor_TravelTo(a, a->f8, a->fc, y0 + (0x80 << 13));
    _Actor_WaitMovement(a);
    i = 0;
    if (a->fc > a->f14) {
        do {
            WaitFrames(1);
            i++;
            if (i > 0xb3)
                break;
        } while (a->fc > a->f14);
    }
    WaitFrames(2);
    Func_809202c();
}
