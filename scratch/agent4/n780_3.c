extern unsigned char iwram_3001ebc[];
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __CutsceneWait(int n);
extern int __Func_809280c(int slot, int a, int b);
extern void __Func_8093040(int slot, int a, int b);
extern void __Func_8092adc(int slot, int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

void OvlFunc_968_2009780(void)
{
    unsigned char *base;
    short *p;
    int zero;
    int one;

    base = *(unsigned char **)iwram_3001ebc;
    *(short *)(base + 0xcba) = 0;
    *(short *)(base + 0xcb6) = 1;
    __CutsceneStart();
    __MessageID(0x267d);
    __Func_809280c(0xa, 0, 0);
    __CutsceneWait(0xa);
    __Func_8093040(0xa, 0, 0x14);
    __Func_8092adc(0xa, 0xe0 << 8, 0);
    __Func_80933d4(0x80 << 9, 0x80 << 6);
    __Func_80933f8(0xe0 << 17, -1, 0xd8 << 17, 1);
    __Func_8093530();
    __ActorMessage(0xa, 0);
    __CutsceneEnd();
}
