extern unsigned char L5160[] __asm__(".L5160");
extern unsigned char gScript_943__0200c980[];
extern unsigned char gScript_943__0200c628[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __LoadFieldActors(unsigned char *p);
extern void __WaitFrames(int n);
extern void __MapActor_SetPos(int slot, int x, int y);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetBehavior(int slot, unsigned char *script);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern int __GetFlag(int flag);
extern void OvlFunc_943_200c218(void);

void OvlFunc_943_2009920(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned char bit;
    int z;

    __CutsceneStart();
    __LoadFieldActors(L5160);
    __WaitFrames(1);
    __MapActor_SetPos(0x14, 0, 0);
    __MapActor_SetPos(0x17, 0xee << 16, 0x2720000);
    __MapActor_SetPos(0x16, 0x86 << 17, 0x2a60000);
    q = __MapActor_GetActor(0x16);
    z = 0;
    *(short *)(q + 6) = z;
    __MapActor_SetBehavior(0x16, gScript_943__0200c980);
    p = __MapActor_GetActor(0x15) + 0x59;
    bit = 0x80;
    *p = bit | *p;
    __MapActor_SetSpeed(0x15, 0xcccc, 0x6666);
    __MapActor_SetBehavior(0x15, gScript_943__0200c628);
    if (__GetFlag(0x109) != 0)
        OvlFunc_943_200c218();
    __CutsceneEnd();
}
