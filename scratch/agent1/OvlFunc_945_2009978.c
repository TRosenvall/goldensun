extern unsigned char gState[];
extern int _AREA_6f;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_808e118(void);
extern void __ClearFlag(int flag);
extern int __GetFlag(int flag);
extern void __Func_8091f90(int area, int entrance);
extern void __Func_8091eb0(int a, int b);

void OvlFunc_945_2009978(void)
{
    unsigned char *p;

    __CutsceneStart();
    __Func_808e118();
    p = gState;
    p[0x22b] = 3;
    __ClearFlag(0x8f << 4);
    if (__GetFlag(0x928) == 0) {
        __Func_8091f90((int)&_AREA_6f, 0x10);
        __Func_8091eb0(0x3e, 0);
    } else if (__GetFlag(0x929) == 0) {
        __Func_8091f90((int)&_AREA_6f, 0x12);
        __Func_8091eb0(0x3e, 1);
    } else if (__GetFlag(0x92a) == 0) {
        __Func_8091f90((int)&_AREA_6f, 0x14);
        __Func_8091eb0(0x3e, 2);
    }
    __CutsceneEnd();
}
