extern unsigned char gState[];
extern int OvlFunc_888_200b2a8(void);
extern void OvlFunc_888_2008360(void);
extern void __UI_Sanctum(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int arg);

void OvlFunc_888_200b334(void)
{
    unsigned char *g;
    int area;

    if (OvlFunc_888_200b2a8()) {
        __UI_Sanctum(8);
        return;
    }
    __CutsceneStart();
    g = gState;
    area = *(short *)(g + (0xe1 << 1));
    switch (area) {
    case 0xa:
    case 0xc:
        if (__GetFlag(0x855))
            __MessageID(0x1376);
        else
            __MessageID(0x1288);
        break;
    case 0xb:
        __MessageID(0x1ce8);
        break;
    case 0x14:
    case 0x15:
    case 0x32:
        __CutsceneEnd();
        OvlFunc_888_2008360();
        return;
    }
    __ActorMessage(8, 0);
    __CutsceneEnd();
}
