typedef unsigned char u8;

struct Sub {
    u8 pad0[5];
    u8 b5;
    u8 pad6[3];
    u8 b9;
    u8 padA[0x12];
    u8 b1c;
    u8 pad1d[9];
    u8 b26;
    u8 b27;
};

struct Actor {
    u8 pad0[0x28];
    int f28;
    u8 pad2c[0x1c];
    int f48;
    u8 pad4c[4];
    struct Sub *f50;
};

extern u8 gScript_881__0200cbe4[];

extern struct Actor *__CreateActor(int id);
extern int __CheckPartyItem(int item);
extern int __CheckItem(int a, int item);
extern void __Actor_SetScript(struct Actor *a, u8 *script);
extern void *__galloc_iwram(int id, int size);
extern void __LoadItemIcon(int item);
extern void __UploadSpriteGFX(int a, int b, void *c);
extern void __gfree(int id);
extern void __PlaySound(int id);
extern void __Func_808f140(struct Actor *a, int b);
extern int __Func_8078948(int a, int b);
extern void __GiveItemTo(int a, int b);
extern void __DeleteActor(struct Actor *a);
extern void __MapActor_SetAnim(int a, int b);

int OvlFunc_896_200c260(int a)
{
    struct Actor *act;
    struct Sub *s;
    u8 *p;
    int slot;
    int r;
    int t;
    int u;
    int q;

    q = 0;
    act = __CreateActor(0x16);
    slot = __CheckPartyItem(0xe0);
    r = __CheckItem(slot, 0xe0);
    if (act == 0)
        return slot;
        __Actor_SetScript(act, gScript_881__0200cbe4);
        s = act->f50;
        p = &s->b26;
        *p = q;
        p++;
        *p = q;
        u = s->b5;
        t = ~0x20;
        t &= u;
        s->b5 = t;
        u = s->b9;
        t = 0xf;
        t &= u;
        s->b9 = t;
        act->f28 = 0x28000;
        act->f48 = 0x4000;
        q = (int)__galloc_iwram(0x11, 0x608);
        __LoadItemIcon(a);
        __UploadSpriteGFX(s->b1c, 0x80, (u8 *)(q + 0x400));
        __gfree(0x11);
        __PlaySound(0x53);
        __Func_808f140(act, 3);
        __Func_8078948(slot, r);
        __GiveItemTo(slot, a);
        __DeleteActor(act);
        __MapActor_SetAnim(0, 1);
    return slot;
}
