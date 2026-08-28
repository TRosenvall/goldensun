extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetBehavior(int slot, void *b);
extern unsigned char gScript_960__020097a8[];

void OvlFunc_960_2008adc(void)
{
    int x;
    int z;

    x = 0xf0 << 15;
    z = 0xce << 18;
    if (__GetFlag(0x9b7) == 0) {
        __SetFlag(0x20e);
        __MapActor_SetPos(0xc, x, z);
        __MapActor_SetBehavior(0xc, gScript_960__020097a8);
    }
}
