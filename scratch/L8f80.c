extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_926_2008f80(void)
{
    unsigned char *a;
    int v;
    int w1, w2, w3, w4;

    w1 = 0x80 << 6;
    w2 = 0xa0 << 7;
    w3 = 0xe0 << 8;
    w4 = 0x80 << 6;
    a = __MapActor_GetActor(0);
    v = *(unsigned short *)(a + 6);
    if ((unsigned short)(v - 0x2000) <= 0x3fff) {
        __Func_80921c4(0xf, 0xd8, 0xa8);
        __Func_80921c4(0xf, 0xe0, 0xa8);
        __Func_8092adc(0xf, w1, 0x14);
    } else if ((unsigned short)(v - 0x6000) <= 0x3fff) {
        __Func_80921c4(0xf, 0xe8, 0xa0);
        __Func_8092adc(0xf, w2, 0x14);
    } else if ((unsigned short)(v + (0xc0 << 7)) <= 0x3fff) {
        __Func_80921c4(0xf, 0xd8, 0xa8);
        __Func_80921c4(0xf, 0xe0, 0xac);
        __Func_8092adc(0xf, w3, 0x14);
    } else {
        __Func_80921c4(0xf, 0xe8, 0xa0);
        __Func_8092adc(0xf, w4, 0x14);
    }
}
