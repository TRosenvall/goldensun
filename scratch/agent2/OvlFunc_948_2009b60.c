extern void *__MapActor_GetActor(int slot);
extern void __Func_8093fa0(void);
extern void __Func_8093e28(void);
extern void OvlFunc_948_20099e8(void);
extern void OvlFunc_948_20080c4(void);
extern void OvlFunc_948_2009ac8(void);

void OvlFunc_948_2009b60(void)
{
    unsigned char *a;
    unsigned char *b;
    int x;
    int y;
    unsigned short f;

    a = (unsigned char *)__MapActor_GetActor(0);
    b = (unsigned char *)__MapActor_GetActor(8);
    x = *(int *)(a + 8) / 0x100000;
    y = *(int *)(b + 8) / 0x100000;
    if (x == 0x26 && y != 0x26) {
        f = *(unsigned short *)(a + 6);
        if (f == 0xc000) {
            __Func_8093fa0();
            goto out;
        }
        if (f == 0x4000) {
            __Func_8093e28();
            goto out;
        }
    }
    OvlFunc_948_20099e8();
    OvlFunc_948_20080c4();
    OvlFunc_948_2009ac8();
out:
    ;
}
