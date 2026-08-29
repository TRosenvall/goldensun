extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void *__Func_808e078(int a, int b, int c);
extern int __Func_8091a58(int a, int b);
extern void __MapActor_SetAnim(int slot, int a);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __PlaySound(int id);
extern void __DeleteActor(void *h);

void OvlFunc_909_20084ec(int slot, int b, int flag)
{
    void *h;

    __CutsceneStart();
    h = __Func_808e078(0, slot, b);
    if (__Func_8091a58(b, 0) != -1) {
        __MapActor_SetAnim(slot, 2);
        __SetFlag(0x84e);
        __SetFlag(flag);
        __ClearFlag(0x322);
        __ClearFlag(0x202);
    } else {
        __PlaySound(0x7d);
        __MapActor_SetAnim(slot, 5);
    }
    __DeleteActor(h);
    __CutsceneEnd();
}
