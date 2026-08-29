struct E { unsigned char pad00[6]; unsigned short f6; };

extern struct E *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __Func_80b3284(int inn, int slot);
extern void __Func_8093054(int slot, int n);

void OvlFunc_940_200808c(void)
{
    struct E *e;

    e = __MapActor_GetActor(0);
    if ((unsigned int)(e->f6 - 0xa001) <= 0x3ffe) {
        if (__GetFlag(0x941)) {
            __Func_80b3284(8, 0x11);
            return;
        }
    }
    __CutsceneStart();
    if (__GetFlag(0x941)) {
        __MessageID(0x24fb);
        __Func_8093054(0x11, 0);
    } else {
        __MessageID(0x1bd0);
        __Func_8093054(0x11, 0);
    }
    __CutsceneEnd();
}
