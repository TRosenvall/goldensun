struct A {
    unsigned char pad00[0xa];
    short fa;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[0x46];
    unsigned char f5a;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __PlaySound(int id);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int a);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);

void OvlFunc_891_200995c(void)
{
    struct A *a;
    int s1;
    int s2;
    int s3;
    int s4;

    s1 = 0x3333;
    s2 = 0x1999;
    s3 = 0x3333;
    s4 = 0x1999;
    a = __MapActor_GetActor(0x11);
    if (a != 0 && (a->f10 >> 20) == 8) {
        __CutsceneStart();
        __PlaySound(0xb9);
        __MapActor_SetSpeed(0x11, s1, s2);
        __MapActor_SetSpeed(0, s3, s4);
        __MapActor_GetActor(0x11)->f5a &= 0xfe;
        __MapActor_SetAnim(0, 8);
        __MapActor_TravelTo(0, __MapActor_GetActor(0)->fa, 0x88);
        __MapActor_TravelTo(0x11, 0x90 << 1, 0x78);
        __MapActor_WaitMovement(0x11);
        __MapActor_SetAnim(0, 1);
        __CutsceneEnd();
    }
}
