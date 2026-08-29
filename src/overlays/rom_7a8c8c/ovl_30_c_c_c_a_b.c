extern void __PlaySound(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_808e118(void);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern void __WaitFrames(int n);
extern void __CopyMapTiles(int sx, int sy, int w, int h, int dx, int dy);
extern void __StartTask(void *fn, int prio);
extern void __StopTask(void *fn);
extern void __SetFlag(int flag);
extern void __Func_8078a08(int a);
extern void __PlayMapMusic(void);
extern void OvlFunc_922_2009d78(void);

void OvlFunc_922_2009e08(void)
{
    unsigned int i;
    int a;
    int b;
    int c;

    __PlaySound(0x13);
    __PlaySound(0xb6);
    __CutsceneStart();
    __Func_808e118();
    i = 0;
    a = 8;
    b = 7;
    c = 1;
    for (; i < 4; i++) {
        __Func_8091200(0x204318, 1);
        __Func_8091254(1);
        __WaitFrames(2);
        if (i == 0) {
            __CopyMapTiles(0x1e, 8, 0xc, 8, a, b);
            __CopyMapTiles(0x1e, 0x39, 0x13, 0x39, c, c);
        }
        __Func_8091200(0x203108, 1);
        __Func_8091254(1);
        __WaitFrames(2);
    }
    __WaitFrames(0x1e);
    __StartTask(OvlFunc_922_2009d78, 0xc8 << 4);
    __WaitFrames(0x28);
    __Func_8091200(0x201090, 1);
    __Func_8091254(0x28);
    __WaitFrames(0x50);
    __StopTask(OvlFunc_922_2009d78);
    __WaitFrames(0x14);
    __Func_8091200(0x80 << 9, 1);
    __Func_8091254(0x50);
    __WaitFrames(0x50);
    __SetFlag(0x82 << 4);
    __Func_8078a08(0xe6);
    __PlayMapMusic();
    __CutsceneEnd();
}
