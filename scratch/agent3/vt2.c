extern unsigned char L5160[] __asm__(".L5160");
extern unsigned char gScript_943__0200c628[];
extern unsigned char gScript_943__0200c980[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __LoadFieldActors(void *tbl);
extern void __WaitFrames(int n);
extern void __MapActor_SetPos(int slot, int x, int z);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetBehavior(int slot, void *script);
extern int __GetFlag(int id);
extern void OvlFunc_943_200c218(void);

void OvlFunc_943_2009920(void)
{
    unsigned char *a;
    unsigned char *q;
    unsigned char m;

    __CutsceneStart();
    __LoadFieldActors(L5160);
    __WaitFrames(1);
    __MapActor_SetPos(0x14, 0, 0);
    __MapActor_SetPos(0x17, 0xee << 16, 0x2720000);
    __MapActor_SetPos(0x16, 0x86 << 17, 0x2a60000);
    a = __MapActor_GetActor(0x16);
    {
        int zero = 0;
        *(short *)(a + 6) = zero;
    }
    __MapActor_SetBehavior(0x16, gScript_943__0200c980);
    q = __MapActor_GetActor(0x15) + 0x59;
    m = 0x80;
    *q = m | *q;
    __MapActor_SetBehavior(0x15, gScript_943__0200c628);
    if (__GetFlag(0x109) != 0) {
        OvlFunc_943_200c218();
    }
    __CutsceneEnd();
}
