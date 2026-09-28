extern unsigned int L3738[] __asm__(".L3738");

extern int __GetFlag(int id);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern int __StartTask(void *fn, int n);
extern void __StopTask(void *fn);
extern void __SetIntrHandler(int a, int b, void *f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);
extern void OvlFunc_947_2008cc0(int a, int b, int c, int d, int e, int f, int g);
extern void OvlFunc_947_20095cc(void);
extern void OvlFunc_947_2009578(void);

void OvlFunc_947_20095fc(void)
{
    if (!__GetFlag(0x200)) {
        OvlFunc_947_2008cc0(0xa, 0x13, 0x10, 5, 0, 0xa, 0x1f);
        OvlFunc_947_2008cc0(0xa, 0x33, 0x10, 5, 1, 0xa, 0x1f);
        OvlFunc_947_2008cc0(0x2a, 0x33, 0x10, 5, 2, 0xa, 0x1f);
    } else {
        OvlFunc_947_2008cc0(0xa, 0x13, 0x10, 5, 0, 0xa, 0x1f);
        OvlFunc_947_2008cc0(0xa, 0x53, 0x10, 5, 1, 0xa, 0x1f);
        OvlFunc_947_2008cc0(0x2a, 0x53, 0x10, 5, 2, 0xa, 0x1f);
    }
    *L3738 = 0;
    __StartTask(OvlFunc_947_20095cc, 0xc80);
    __WaitFrames(1);
    __SetIntrHandler(1, 0, OvlFunc_947_2009578);
    __PlaySound(0xe7);
    *L3738 = 0;
    do {
        __WaitFrames(1);
    } while ((int)(*L3738 += 1) <= 0x64);
    __PlaySound(0x121);
    if (!__GetFlag(0x200)) {
        __CopyMapTiles(0, 0x20, 0x20, 0, 0x20, 0x20);
        __CopyMapTiles(0x20, 0x20, 0x40, 0, 0x20, 0x20);
    } else {
        __CopyMapTiles(0, 0x40, 0x20, 0, 0x20, 0x20);
        __CopyMapTiles(0x20, 0x40, 0x40, 0, 0x20, 0x20);
    }
    __WaitFrames(1);
    __SetIntrHandler(1, 0, 0);
    __WaitFrames(1);
    __StopTask(OvlFunc_947_20095cc);
    __Func_800fe9c();
    __WaitFrames(0x1e);
}
