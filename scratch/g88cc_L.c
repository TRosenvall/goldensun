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
    int x;
    int y;

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
        x = 0xb0 << 15;
        y = 0xa3 << 19;
        if (__GetFlag(0x8a9) == 0) {
            __MapActor_SetPos(0xc, x, y);
            __Func_8092adc(0xc, 0, 0);
        }
    }
}
