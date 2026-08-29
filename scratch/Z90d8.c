extern int *iwram_3001e70;

extern int __GetFlag(int id);
extern void __MessageID(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern int *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_801776c(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_800fe9c(void);

void OvlFunc_883_20090d8(void)
{
    int buf[3];
    int *p;
    int save;
    int *a;
    int i;

    if (__GetFlag(0x808))
        return;
    p = iwram_3001e70;
    __CutsceneStart();
    __MapActor_SetSpeed(0, 0x80 << 9, 0x80 << 8);
    __MapActor_SetAnim(0, 1);
    __CutsceneWait(2);
    __MessageID(0xf4d);
    __Func_8093040(0xf, 0, 2);
    __Func_8093040(0x10, 0, 2);
    a = __MapActor_GetActor(0);
    buf[0] = a[2];
    buf[1] = a[3];
    save = *p;
    buf[2] = a[4];
    *p = (int)buf;
    for (i = 0; i != 0x28; i++) {
        buf[2] += 0x80 << 10;
        __CutsceneWait(1);
        __Func_800fe9c();
    }
    __CutsceneWait(0x3c);
    __Func_801776c(0xf4f, 1);
    __CutsceneWait(6);
    for (i = 0; i != 0x28; i++) {
        buf[2] += 0xfffe0000;
        __CutsceneWait(1);
        __Func_800fe9c();
    }
    *p = save;
    __CutsceneWait(0x3c);
    __Func_80921c4(0, 0x46, 0x2e5);
    __CutsceneEnd();
}
