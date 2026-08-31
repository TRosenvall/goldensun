extern char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __MapActor_Emote(int a, int b, int c);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8093054(int a, int b);

void OvlFunc_953_200839c(void)
{
    int e;

    e = 0x80 << 1;
    __CutsceneStart();
    if (__GetFlag(0x962) != 0) {
        if (__GetFlag(0xf0 << 2) != 0) {
            __MessageID(0x225e);
            __ActorMessage(0x10, 0);
        } else {
            __MessageID(0x225a);
            __Func_8092c40(0x10, 0);
            if (__Func_8091c7c(0, 0) == 0) {
                *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
                __MapActor_Emote(0x10, e, 0x28);
                __Func_8092c40(0x10, 0);
                if (__Func_8091c7c(0, 0) == 0)
                    *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
                __CutsceneWait(0x28);
                __ActorMessage(0x10, 0);
                __SetFlag(0xf0 << 2);
            } else {
                __ActorMessage(0x10, 0);
            }
        }
    } else {
        __MessageID(0x205e);
        __Func_8093054(0x10, 0);
    }
    __CutsceneEnd();
}
