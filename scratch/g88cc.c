extern unsigned char gState[];
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_8092adc(int a, int b, int c);
extern void OvlFunc_942_2008ba0(void);

void OvlFunc_942_20088cc(void)
{
    unsigned char *g;

    g = gState;
    if (*(short *)(g + (0xe1 << 1)) == 1) {
        if (__GetFlag(0x8ac) == 0) {
            __SetFlag(0x8ac);
            OvlFunc_942_2008ba0();
        }
    }
    if (*(short *)(g + (0xe1 << 1)) == 2) {
        if (__GetFlag(0x109) == 0)
            __ClearFlag(0x8a9);
    }
    if (__GetFlag(0x911)) {
        if (__GetFlag(0x8a9) == 0) {
            __MapActor_SetPos(0xc, 0xb0 << 15, 0xa3 << 19);
            __Func_8092adc(0xc, 0, 0);
        }
    }
}
