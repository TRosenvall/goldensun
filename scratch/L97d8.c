extern unsigned char gState[];
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Actor_TravelTo(unsigned char *a, int x, int z, int y);
extern void __Actor_SetAnim(unsigned char *a, int anim);
extern void __Actor_WaitMovement(unsigned char *a);

void OvlFunc_934_20097d8(int slot, int dx, int dy)
{
    unsigned char *a;
    unsigned char *b;
    unsigned char *g;
    int x;
    int y;

    g = gState;
    a = __MapActor_GetActor(*(int *)(g + (0xfa << 1)));
    b = __MapActor_GetActor(slot);
    __CutsceneStart();
    x = ((*(int *)(a + 8) + (dx << 16)) & 0xfff00000) + (0x80 << 12);
    y = ((*(int *)(a + 0x10) + (dy << 16)) & 0xfff00000) + (0x80 << 12);
    *(int *)(a + 0x30) = 0x80 << 9;
    *(int *)(a + 0x34) = 0x80 << 8;
    __Actor_TravelTo(a, x, *(int *)(a + 0xc), y);
    __Actor_SetAnim(a, 0x1b);
    x = ((*(int *)(b + 8) + (dx << 16)) & 0xfff00000) + (0x80 << 12);
    y = ((*(int *)(b + 0x10) + (dy << 16)) & 0xfff00000) + (0x80 << 12);
    *(int *)(b + 0x30) = 0x80 << 9;
    *(int *)(b + 0x34) = 0x80 << 8;
    __Actor_TravelTo(b, x, *(int *)(b + 0xc), y);
    if (dx < 0 || dy < 0)
        __Actor_SetAnim(b, 4);
    else
        __Actor_SetAnim(b, 3);
    __Actor_WaitMovement(a);
    __CutsceneEnd();
}
