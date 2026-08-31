extern int __GetFlag(int id);
extern int OvlFunc_945_200cfa8(int a, int b);
extern void OvlFunc_945_200c890(int a, int b, int c, int d);
extern void __MapActor_SetPos(int a, int b, int c);

void OvlFunc_945_200b7d8(int arg)
{
    int r;
    int a1;
    int a2;
    int b1;
    int b2;

    a1 = 0xcd << 1;
    a2 = 0xeb << 1;
    b1 = 0xd0 << 8;
    b2 = 0xb0 << 8;
    if (arg != 0 && __GetFlag(0x929) == 0)
        return;
    r = OvlFunc_945_200cfa8(0, 0);
    if (r != 0) {
        OvlFunc_945_200c890(r, a1, 0xac, b1);
        __MapActor_SetPos(0xa, 0, 0);
    }
    if (arg != 0 && __GetFlag(0x92a) == 0)
        return;
    r = OvlFunc_945_200cfa8(1, 0);
    if (r != 0) {
        OvlFunc_945_200c890(r, a2, 0xac, b2);
        __MapActor_SetPos(0xb, 0, 0);
    }
    if (arg != 0 && __GetFlag(0x92b) == 0)
        return;
    r = OvlFunc_945_200cfa8(2, 0);
    if (r != 0) {
        OvlFunc_945_200c890(r, a1, 0xcc, b1);
        __MapActor_SetPos(0xc, 0, 0);
    }
    if (arg != 0)
        return;
    r = OvlFunc_945_200cfa8(3, 0);
    if (r != 0) {
        OvlFunc_945_200c890(r, a2, 0xcc, b2);
        __MapActor_SetPos(0xd, 0, 0);
    }
}
