extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_8012078(int a, int b, int c, int d);

void OvlFunc_891_2009624(void)
{
    unsigned char *a;
    int u;
    int v;
    int x1, y1, x2, y2;

    x1 = 0x88 << 17;
    y1 = 0x80 << 16;
    x2 = 0x90 << 17;
    y2 = 0x80 << 16;
    a = __MapActor_GetActor(0);
    u = *(int *)(a + 8) >> 20;
    a = __MapActor_GetActor(0);
    v = *(int *)(a + 0x10) >> 20;
    if (v == 8 && (unsigned int)(u - 0x11) <= 1) {
        __Func_8012078(2, x1, y1, 0xff);
        __Func_8012078(2, x2, y2, 0xff);
    }
}
