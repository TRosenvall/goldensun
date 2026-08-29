extern unsigned char *iwram_3001ebc;
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_911_20082b4(int n)
{
    unsigned char *a;
    unsigned int q;

    a = __MapActor_GetActor(0);
    a[0x55] = 0;
    __MapActor_SetSpeed(0, 0x80 << 8, 0x80 << 7);
    __MapActor_SetAnim(0, 2);
    __Func_809228c(0, 0, -8);
    q = (unsigned int)iwram_3001ebc;
    q += 0xe4 << 1;
    *(int *)q = 0x10;
    __Func_8091e9c(n);
}
