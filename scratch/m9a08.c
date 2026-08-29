extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_8093304(int n);
extern void __Func_8019aa0(int a, int b, int c);
extern void __MapActor_SetSpeed(int slot, int vx, int vz);
extern void __Func_80921c4(int a, int b, int c);
extern int OvlFunc_945_200c880(int slot, int v);

void OvlFunc_945_2009a08(void)
{
    int v;

    v = 0xcc << 1;
    if (__GetFlag(0x301)) {
        __CutsceneStart();
        __Func_8093304(8);
        __Func_8019aa0(0x1e48, 1, 8);
        __MapActor_SetSpeed(0, 0x19999, 0xcccc);
        __Func_80921c4(0, v, 0x86);
        OvlFunc_945_200c880(0, 0x80 << 7);
        __CutsceneEnd();
    }
}
