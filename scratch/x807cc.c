extern unsigned char gState[];
extern volatile unsigned int gKeyHeld;
extern int __Func_8019da8(int a, int b, int c, int d);
extern void __Func_8019908(int a, int b);
extern void OvlFunc_974_200804c(int a);
extern void __WaitFrames(int n);
extern void __Func_8019a54(void);
extern void __CloseUIBox(int h, int n);

void OvlFunc_974_200807c(int a, int n)
{
    int h;
    int i;

    gState[0x83 << 2] = 2;
    h = __Func_8019da8(0x7d, 0, 0, 0);
    for (i = 0; i < n; i++) {
        __Func_8019908(1, 1);
        __Func_8019908(0x8d, 2);
        __Func_8019908(0x1e240, 5);
        OvlFunc_974_200804c(a);
        for (;;) {
            if (gKeyHeld & 2)
                goto done;
            if ((gKeyHeld & 1) || (gKeyHeld & 0x80)) {
                a++;
                break;
            }
            if (gKeyHeld & 0x40) {
                a--;
                break;
            }
            if (gKeyHeld == 0)
                __WaitFrames(1);
            else
                break;
        }
    }
done:
    __Func_8019a54();
    __CloseUIBox(h, 2);
}
