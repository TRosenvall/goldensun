extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __MapActor_SetAnim(int a, int b);
extern void __Func_8091ff0(int n);
extern void __StopTask(void *fn);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __PlaySound(int id);
extern void __Func_80925cc(int a, int b);
extern void OvlFunc_931_20087b8(void);
extern void __SetCameraTarget(int slot, int n);
extern void __MapActor_DoAnim(int a, int b);
extern void __SetFlag(int id);
extern void OvlFunc_931_2008d08(void);

void OvlFunc_931_2008d58(void)
{
    int m1, m2;

    __CutsceneStart();
    __Func_80933d4(0x6666, 0xccc);
    m1 = -1;
    __Func_80933f8(0xfc << 14, m1, 0xe1 << 17, 1);
    __Func_8093530();
    __CutsceneWait(0x1e);
    __MapActor_SetAnim(0x12, 1);
    m2 = -1;
    __Func_8091ff0(m2);
    __StopTask(OvlFunc_931_2008d08);
    __CutsceneWait(0x14);
    __Func_809280c(0, 0x12, 0);
    __Func_8092adc(0, 0x80 << 7, 0);
    __Func_8092adc(0x12, 0, 0x14);
    __Func_8092adc(0x12, 0xd0 << 8, 0x28);
    __PlaySound(0x93);
    __Func_80925cc(0x12, 2);
    __CutsceneWait(0x14);
    __Func_8092adc(0x12, 0xb0 << 8, 0x28);
    OvlFunc_931_20087b8();
    __SetCameraTarget(0, 1);
    __Func_8093530();
    __MapActor_DoAnim(0xe, 4);
    __SetFlag(0x8ff);
    __CutsceneEnd();
}
