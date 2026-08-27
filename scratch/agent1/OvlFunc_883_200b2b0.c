struct S {
    unsigned char pad00[0x26];
    unsigned char f26;
};

struct A {
    unsigned char pad00[0x50];
    struct S *f50;
    unsigned char pad54[1];
    unsigned char f55;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_80921c4(int slot, int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8092158(int slot, int a, int b);
extern void __CutsceneWait(int frames);

void OvlFunc_883_200b2b0(int slot, int anim1, int anim2, int flag)
{
    struct A *a;
    struct S *s;

    a = __MapActor_GetActor(slot);
    s = a->f50;
    __MapActor_SetSpeed(slot, 0x80 << 9, 0x80 << 8);
    __Func_80921c4(slot, 0xc4 << 1, 0x376);
    __Func_8092adc(0, 0xc0 << 8, 0xa);
    a->f55 = 0;
    s->f26 = 0;
    __MapActor_SetAnim(slot, anim1);
    __MapActor_SetSpeed(slot, 0x4ccc, 0x2666);
    __Func_8092158(slot, 0xc4 << 1, 0x36b);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(slot, anim2);
    __MapActor_SetSpeed(slot, 0x80 << 9, 0x80 << 8);
    __Func_8092158(slot, 0xc4 << 1, 0x35b);
    s->f26 = 1;
    if (flag != 0)
        a->f55 = 3;
    __CutsceneWait(0xa);
    __MapActor_SetAnim(slot, 1);
}
