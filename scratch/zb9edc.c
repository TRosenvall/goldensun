extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_8092b08(int a, int b);

void OvlFunc_948_2009edc(void)
{
    unsigned char *p;
    int v;

    if (*(int *)(__MapActor_GetActor(0) + 0xc) > (0x80 << 13)) {
        v = 2;
        __MapActor_GetActor(8)[0x23] = v;
        if (*(int *)(__MapActor_GetActor(0xa) + 0xc) == 0)
            __MapActor_GetActor(0xa)[0x23] = v;
        p = __MapActor_GetActor(0xb);
    } else {
        if (*(int *)(__MapActor_GetActor(0xa) + 0xc) == 0
            && *(int *)(__MapActor_GetActor(0) + 0x10) / 0x100000 > 0x38) {
            __Func_8092b08(0xa, 3);
        } else {
            __Func_8092b08(0xa, 1);
            __MapActor_GetActor(0xa)[0x23] = 1;
        }
        p = __MapActor_GetActor(0xb);
        v = 0;
    }
    p[0x23] = v;
    __MapActor_GetActor(0xc)[0x23] = v;
}
