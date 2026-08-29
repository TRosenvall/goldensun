extern unsigned char gState[];
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetAnim(void *a, int anim);
extern void __Actor_TravelTo(void *a, int x, int y, int z);
extern void __Actor_WaitMovement(void *a);
extern void __MapActor_Surprise(int slot, int a);
extern void __Func_8092708(int slot, int a, int b);

void OvlFunc_956_2008b30(void)
{
    unsigned char *gs;
    int *slot;
    unsigned char *a;
    int dz;
    int t;
    int em;

    dz = 0xc0 << 12;
    em = 0x81 << 1;
    gs = gState;
    slot = (int *)(gs + 0x1f4);
    a = __MapActor_GetActor(*slot);
    if (*(int *)(a + 8) > (0xa6 << 18))
        *(int *)(a + 8) = 0xa6 << 18;
    *(int *)(a + 0x34) = 0x80 << 9;
    *(int *)(a + 0x30) = 0x80 << 10;
    __Actor_SetAnim(a, 5);
    t = *(int *)(a + 0x10) & 0xfff00000;
    __Actor_TravelTo(a, *(int *)(a + 8), *(int *)(a + 0xc), t + dz);
    __Actor_WaitMovement(a);
    __MapActor_Surprise(*slot, em);
    __Func_8092708(*slot, 6, 0);
}
