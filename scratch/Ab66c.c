extern int __GetFlag(int id);
extern void __WaitFrames(int n);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void OvlFunc_945_200b7b4(void);
extern void OvlFunc_945_200b8ac(void);
extern void OvlFunc_945_200c254(int n);
extern void OvlFunc_945_200c890(int a, int b, int c, int d);
extern void OvlFunc_945_200c8e8(int a, int b, int c);

void OvlFunc_945_200b66c(void)
{
    __WaitFrames(1);
    OvlFunc_945_200b7b4();
    if (__GetFlag(0x93e)) {
        OvlFunc_945_200c8e8(4, 4, 0);
        OvlFunc_945_200c890(8, 0xce << 1, 0xde, 0xc0 << 6);
        OvlFunc_945_200c890(9, 0xe5 << 1, 0xa1, 0x80 << 8);
        return;
    }
    if (__GetFlag(0x8a << 4)) {
        __MapActor_SetPos(8, 0xec << 17, 0x98 << 16);
        __MapActor_SetAnim(9, 5);
        OvlFunc_945_200c8e8(4, 4, 0);
        return;
    }
    if (__GetFlag(0x92b)) {
        OvlFunc_945_200c8e8(0x10, 0, 0);
        OvlFunc_945_200c8e8(4, 4, 0);
        OvlFunc_945_200c254(3);
        return;
    }
    if (__GetFlag(0x92a)) {
        OvlFunc_945_200c8e8(0x10, 0, 0);
        OvlFunc_945_200c8e8(4, 3, 0);
        OvlFunc_945_200c254(2);
        return;
    }
    if (__GetFlag(0x929)) {
        OvlFunc_945_200c8e8(0x10, 0, 0);
        OvlFunc_945_200c8e8(4, 2, 0);
        OvlFunc_945_200c254(1);
        return;
    }
    if (__GetFlag(0x928)) {
        OvlFunc_945_200c8e8(0x10, 0, 0);
        __MapActor_SetPos(0xa, 0, 0);
        OvlFunc_945_200c254(0);
        return;
    }
    __MapActor_SetAnim(9, 5);
    if (__GetFlag(0x925)) {
        if (__GetFlag(0x926) == 0)
            OvlFunc_945_200b8ac();
    }
}
