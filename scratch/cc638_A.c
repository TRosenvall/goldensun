extern int L5fa4[] __asm__(".L5fa4");
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int arg);
extern void __Func_8092adc(int a, int b, int c);
extern void __CutsceneWait(int n);

void OvlFunc_959_200c638(void)
{
    int id;

    switch (L5fa4[0]) {
    case 0:
        id = 0x2414;
        break;
    case 1:
        id = 0x2415;
        break;
    case 2:
        id = 0x2416;
        break;
    case 3:
        id = 0x2417;
        break;
    case 4:
        id = 0x2418;
        break;
    case 6:
        id = 0x241a;
        break;
    case 7:
        id = 0x241b;
        break;
    case 5:
        goto five;
    default:
        return;
    }
    __MessageID(id);
    __ActorMessage(0x15, 0);
    return;
five:
    __Func_8092adc(0x15, 0xd0 << 8, 0);
    __CutsceneWait(0x32);
    __Func_8092adc(0x15, 0xb0 << 8, 0);
    __CutsceneWait(0x32);
    __Func_8092adc(0x15, 0xa0 << 7, 0);
    __CutsceneWait(0x32);
    __MessageID(0x2419);
    __ActorMessage(0x15, 0);
}
