extern unsigned char gScript_943__0200c4d8[];

extern int __GetFlag(int id);
extern unsigned int __Random(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __Func_8092b08(int a, int b);

void OvlFunc_943_2009684(void)
{
    unsigned char *q;
    unsigned char *s;
    int f;

    __Func_8092b08(0x1b, 1);
    __Func_8092b08(0x17, 1);
    __Func_8092b08(0x16, 1);
    __Func_8092b08(0x1a, 1);
    __Func_8092b08(0x18, 1);
    if (__GetFlag(0x92 << 4)) {
        __MapActor_SetPos(0x16, 0xa2 << 16, 0x29a0000);
        *(short *)(__MapActor_GetActor(0x16) + 6) = 0x80 << 8;
        __MapActor_SetPos(0x17, 0, 0);
        __MapActor_SetPos(0x14, 0, 0);
    }
    f = __GetFlag(0x922);
    if (f != 0) {
        __MapActor_SetPos(0x15, 0x84 << 17, 0x2be0000);
        *(short *)(__MapActor_GetActor(0x15) + 6) = 0xa0 << 7;
        q = __MapActor_GetActor(0x15);
        s = gScript_943__0200c4d8;
        *(short *)(q + 0x64) = __Random() % 0x5a + 0x3c;
        __MapActor_SetBehavior(0x15, s);
        __MapActor_SetPos(0x18, 0xf8 << 16, 0xaa << 18);
        q = __MapActor_GetActor(0x18);
        *(short *)(q + 0x64) = __Random() % 0x5a + 0x3c;
        __MapActor_SetBehavior(0x18, s);
        __MapActor_SetPos(0x16, 0, 0);
    } else if (__GetFlag(0x923)) {
        __MapActor_SetPos(0x14, 0xf6 << 16, 0x80 << 18);
        *(short *)(__MapActor_GetActor(0x14) + 6) = f;
    }
}
