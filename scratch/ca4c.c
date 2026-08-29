extern unsigned char gState[];
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __WaitFrames(int n);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __AddPartyMember(int n);
extern void __Func_807a664(void);
extern void OvlFunc_953_200960c(void);
extern void OvlFunc_953_2009298(void);
extern void OvlFunc_953_2009688(void);
extern void OvlFunc_953_2009cd4(void);
extern void OvlFunc_953_200a3e0(void);
extern void OvlFunc_953_200a5f0(void);
extern void OvlFunc_953_200ab1c(void);
extern void OvlFunc_953_200a4d8(void);
extern void OvlFunc_953_200a668(void);
extern void OvlFunc_953_200a820(void);
extern void OvlFunc_953_200a904(void);
extern void OvlFunc_953_200a964(void);

void OvlFunc_953_2009a4c(void)
{
    unsigned char *g;

    __WaitFrames(1);
    g = gState;
    switch (*(short *)(g + (0xe1 << 1))) {
    case 5:
        __MapActor_SetAnim(8, 2);
        __MapActor_SetAnim(9, 2);
        break;
    case 69:
        __MapActor_SetAnim(8, 2);
        __MapActor_SetAnim(9, 2);
        if (__GetFlag(0x109) == 0)
            OvlFunc_953_200960c();
        break;
    case 7:
        OvlFunc_953_2009298();
        break;
    case 70:
        OvlFunc_953_2009688();
        break;
    case 64:
        OvlFunc_953_2009cd4();
        __Func_807a664();
        break;
    case 65:
        OvlFunc_953_200a3e0();
        break;
    case 66:
        OvlFunc_953_200a5f0();
        break;
    case 12:
        __SetFlag(0xa2 << 1);
        OvlFunc_953_200ab1c();
        if (__GetFlag(0x109) == 0)
            OvlFunc_953_200a4d8();
        break;
    case 21:
        __AddPartyMember(1);
        __AddPartyMember(2);
        __AddPartyMember(3);
        __SetFlag(0x90e);
        OvlFunc_953_200a668();
        break;
    case 67:
        OvlFunc_953_200a820();
        break;
    case 68:
        OvlFunc_953_200a904();
        break;
    case 31:
        __AddPartyMember(1);
        __AddPartyMember(2);
        __AddPartyMember(3);
        __SetFlag(0x90f);
        OvlFunc_953_200a964();
        break;
    }
}
