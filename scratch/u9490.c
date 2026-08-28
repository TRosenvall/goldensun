extern unsigned char gScript_883__0200e248[];
extern void OvlFunc_883_200d72c(void);

struct Actor { unsigned char pad00[0x18]; int f18; int f1c; unsigned char pad20[0x4c]; void *f6c; };

extern struct Actor *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_809259c(int a, int b);
extern int __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __CutsceneWait(int n);
extern void __MapActor_RunScript(int slot, unsigned char *s);
extern void __MapActor_WaitScript(int slot);
extern void __Func_80925cc(int a, int b);
extern void __MessageID(int id);
extern void __Func_8093054(int a, int b);

void OvlFunc_883_2009490(unsigned char *beh, unsigned char *scr)
{
    struct Actor *a;
    struct Actor *q;
    int v;

    a = __MapActor_GetActor(0x16);
    __CutsceneStart();
    __Func_809259c(0x16, 2);
    __MapActor_Emote(0x16, 0x80 << 1, 0x14);
    __Func_809259c(0, 2);
    __MapActor_Emote(0, 0x81 << 1, 0x28);
    __MapActor_SetBehavior(0, beh);
    __CutsceneWait(0xa);
    __MapActor_Emote(0x16, 0x103, 0);
    __MapActor_RunScript(0x16, scr);
    __MapActor_WaitScript(0);
    v = 0x80 << 9;
    __CutsceneWait(0x14);
    __Func_80925cc(0x16, 2);
    a->f18 = v;
    a->f1c = v;
    q = __MapActor_GetActor(0);
    q->f18 = v;
    q->f1c = v;
    __MessageID(0xfce);
    __Func_8093054(0x16, 0);
    __MapActor_GetActor(0x16)->f6c = OvlFunc_883_200d72c;
    __MapActor_SetBehavior(0x16, gScript_883__0200e248);
    __CutsceneEnd();
}
