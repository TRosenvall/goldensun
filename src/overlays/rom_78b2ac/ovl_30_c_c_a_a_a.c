extern int OvlFunc_890_200a5b0(void);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int n);
extern void OvlFunc_890_20089f4(void);
extern void OvlFunc_890_2009be8(void);

void OvlFunc_890_2008054(void)
{
    if (OvlFunc_890_200a5b0() != 0) {
        if (__GetFlag(0x201) != 0)
            return;
        __CutsceneStart();
        __Func_8091200(0x2051cc, 1);
        __Func_8091254(0x14);
        __SetFlag(0x201);
        __ClearFlag(0x80 << 2);
        __ClearFlag(0x202);
        if (__GetFlag(0x80a) == 0)
            OvlFunc_890_20089f4();
        if (OvlFunc_890_200a5b0() != 0 && __GetFlag(0x811) == 0)
            OvlFunc_890_2009be8();
        __CutsceneEnd();
    } else {
        if (__GetFlag(0x80 << 2) != 0)
            return;
        __CutsceneStart();
        __Func_8091200(0x80 << 9, 1);
        __Func_8091254(0x14);
        __SetFlag(0x80 << 2);
        __ClearFlag(0x201);
        __ClearFlag(0x202);
        __CutsceneEnd();
    }
}
