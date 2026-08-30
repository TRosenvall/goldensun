extern unsigned char gState[];
extern volatile unsigned int gKeyRepeat;
extern volatile unsigned int gKeyPress;

extern void __CutsceneStart(void);
extern int __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern void __Func_809280c(int a, int b, int c);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern int __CreateUIBox(int a, int b, int c, int d, int e);
extern void __Func_8016478(int box);
extern void __Func_801e9a0(int a, int b, int c, int d, int e);
extern void __CloseUIBox(int box, int n);
extern int __Debug_LoadPresetParty(int n);

int OvlFunc_971_2008e10(int a)
{
    unsigned char *g;
    int box;
    int sel;
    int last;

    __CutsceneStart();
    g = gState;
    __Func_809280c(a, *(int *)(g + (0xfa << 1)), 0);
    __MessageID(0x989);
    __ActorMessage(a, 0);
    box = __CreateUIBox(0, 0, 6, 4, 2);
    sel = 0;
    last = -1;
    for (;;) {
        if (sel != last) {
            __Func_8016478(box);
            __Func_801e9a0(sel, 3, box, 0, 0);
            last = sel;
        }
        if (gKeyRepeat & 0x20)
            sel -= 1;
        if (gKeyRepeat & 0x10)
            sel += 1;
        if (sel < 0)
            sel = 0;
        if (gKeyPress & 1)
            break;
        if (gKeyPress & 2) {
            sel = -1;
            break;
        }
        __WaitFrames(1);
    }
    __CloseUIBox(box, 1);
    if (sel < 0)
        goto msg_a;
    if (__Debug_LoadPresetParty(sel))
        goto msg_b;
    goto msg_c;
msg_a:
    __MessageID(0x98a);
    goto shared;
msg_b:
    __MessageID(0x98b);
shared:
    __ActorMessage(9, 0);
    goto tail;
msg_c:
    __MessageID(0x98c);
    __ActorMessage(9, 0);
tail:
    __WaitFrames(0xa);
    return __CutsceneEnd();
}
