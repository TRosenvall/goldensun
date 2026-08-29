struct Actor {
    unsigned char pad00[6];
    unsigned short facing;
    int x;
    unsigned char pad0a[2];
    short fa;
    int y;
    int z;
    short f12;
    unsigned char pad14[0x22 - 0x14];
    unsigned char f22;
    unsigned char pad23;
    int f24;
    unsigned char pad28[0x2c - 0x28];
    int f2c;
    int f30;
    int f34;
    int f38;
    unsigned char pad3c[0x40 - 0x3c];
    int f40;
    unsigned char pad44[0x59 - 0x44];
    unsigned char f59;
    unsigned char pad5a[0x62 - 0x5a];
    unsigned char f62;
};

extern int L6190[] __asm__(".L6190");
extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __Actor_WaitMovement(struct Actor *a);
extern int __TestCollision(struct Actor *a, int *v);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __Func_809202c(void);
extern struct Actor *OvlFunc_883_200806c(int *v, struct Actor *a);

void OvlFunc_883_20080c4(void)
{
    int v[3];
    struct Actor *p;
    struct Actor *b;
    struct Actor *o;
    int i;
    int d;
    int z;

    p = __MapActor_GetActor(0);
    i = p->facing >> 12;
    d = L6190[i];
    v[0] = p->x + (d & 0xffff0000);
    v[1] = p->y;
    v[2] = p->z + (d << 16);
    b = OvlFunc_883_200806c(v, p);
    if (b == 0)
        return;
    d = L6190[i];
    v[0] = b->x + (d & 0xffff0000);
    v[1] = b->y;
    v[2] = b->z + (d << 16);
    o = OvlFunc_883_200806c(v, b);
    if (o != 0 && (o->f59 & 1) != 0)
        return;
    v[0] = b->x;
    v[1] = b->y + (0x80 << 13);
    v[2] = b->z;
    o = OvlFunc_883_200806c(v, b);
    if (o != 0 && (o->f59 & 1) != 0)
        return;
    b->f22 = 2;
    d = L6190[i];
    v[0] = b->x + (d & 0xffff0000);
    v[1] = b->y;
    v[2] = b->z + (d << 16);
    if (__TestCollision(b, v) > 0)
        return;
    z = b->f62;
    if (z != 0)
        return;
    __Actor_SetAnim(p, 8);
    __WaitFrames(0xf);
    __PlaySound(0xb9);
    b->f30 = 0x3333;
    b->f34 = 0x3333;
    __Actor_TravelTo(b, v[0], v[1], v[2]);
    p->f30 = 0x3333;
    p->f34 = 0x3333;
    __Actor_TravelTo(p, v[0], v[1], v[2]);
    __Actor_WaitMovement(b);
    __Func_809202c();
    b->x = v[0];
    b->z = v[2];
    b->f24 = z;
    b->f2c = z;
    p->f38 = 0x80 << 24;
    p->f40 = 0x80 << 24;
    p->x = p->fa << 16;
    p->f24 = z;
    p->f2c = z;
    p->z = p->f12 << 16;
    __Actor_SetAnim(p, 1);
}
