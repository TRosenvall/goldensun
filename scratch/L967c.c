extern int __GetFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __MapActor_SetPos(int slot, int x, int z);
extern unsigned char *__MapActor_GetActor(int slot);

void OvlFunc_946_200967c(void)
{
    unsigned char *a;
    int e1, f1;
    int x1, y1, x2, y2, x3, y3, x4, y4;

    x1 = 0xf2 << 18;  y1 = 0xf2 << 18;
    x2 = 0xf2 << 18;  y2 = 0xf2 << 18;
    x3 = 0xf2 << 18;  y3 = 0xf2 << 18;
    x4 = 0xf2 << 18;  y4 = 0xf2 << 18;
    if (__GetFlag(0x8c4) != 0) {
        e1 = 8;
        f1 = 0x15;
        __Func_8010704(0, 0, 1, 1, e1, f1);
        __MapActor_SetPos(0xf, x1, y1);
    } else {
        a = __MapActor_GetActor(0xf);
        *(int *)(a + 0x1c) = 0x19999;
    }
    if (__GetFlag(0x8c5) != 0) {
        __MapActor_SetPos(0x10, x2, y2);
    } else {
        a = __MapActor_GetActor(0x10);
        *(int *)(a + 0x1c) = 0x19999;
    }
    if (__GetFlag(0x8c6) != 0) {
        __MapActor_SetPos(0x11, x3, y3);
    } else {
        a = __MapActor_GetActor(0x11);
        *(int *)(a + 0x1c) = 0x19999;
    }
    if (__GetFlag(0x8c7) != 0) {
        __MapActor_SetPos(0x12, x4, y4);
    } else {
        a = __MapActor_GetActor(0x12);
        *(int *)(a + 0x1c) = 0x19999;
    }
}
