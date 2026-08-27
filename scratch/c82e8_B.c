struct A { unsigned char pad00[6]; unsigned short f6; };

extern char *iwram_3001ebc;
extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int arg);
extern void __MapActor_DoAnim(int slot, int n);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092c40();
extern int __Func_8091c7c(int a, int b);

void OvlFunc_936_20082e8(void)
{
    struct A *a;
    unsigned short *p;

    a = __MapActor_GetActor(0);
    if ((unsigned short)(a->f6 - 0x1000) > 0x6000) {
        __CutsceneStart();
        __Func_8092848(0, 8, 0);
        __CutsceneWait(0xa);
        __MessageID(0x2584);
        __Func_8092c40(8, 0);
        if (__Func_8091c7c(0, 0) == 0) {
            __MapActor_DoAnim(8, 4);
            __ActorMessage(8, 0);
        } else {
            p = (unsigned short *)(iwram_3001ebc + (0xec << 1));
            *p = *p + 1;
            __MapActor_DoAnim(8, 3);
            __ActorMessage(8, 0);
        }
        __CutsceneEnd();
    }
}
