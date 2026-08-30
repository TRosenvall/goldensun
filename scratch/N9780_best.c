struct Ctx {
    unsigned char pad000[0xcb6];
    short fcb6;
    unsigned char padcb8[2];
    short fcba;
};

extern struct Ctx *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __ActorMessage(int a, int b);

void OvlFunc_968_2009780(void)
{
    struct Ctx *p;

    p = iwram_3001ebc;
    p->fcba = 0;
    p->fcb6 = 1;
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
