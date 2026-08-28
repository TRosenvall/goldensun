extern unsigned int gState;
extern int _MSG_294e;
extern int OvlFunc_971_2008f30(int a);
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_809280c(int a, int b, int c);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);

void OvlFunc_971_2008f8c(int slot)
{
    unsigned int g;
    unsigned int off;
    int a, b, f;
    int m;
    int k1, k2;

    k1 = 0xbc << 2;
    k2 = 0xbc << 2;
    m = (int)(&_MSG_294e);
    a = OvlFunc_971_2008f30(0);
    b = OvlFunc_971_2008f30(slot);
    __CutsceneStart();
    g = (unsigned int)&gState;
    off = 0xfa;
    off <<= 1;
    g += off;
    __Func_809280c(slot, *(int *)g, 0);
    if (__GetFlag(0xc1 << 2) != 0) {
        __GetFlag(k1);
        f = __GetFlag(slot + k2);
        if (__GetFlag(0x305) != 0) {
            if (f != 0)
                m = 0x2967;
            else
                m = 0x296c;
        } else {
            if (f != 0)
                m = 0x2971;
            else
                m = 0x2976;
        }
    } else {
        if (a == 0)
            m = 0x2958;
        else if (b == 0)
            m = 0x2953;
    }
    __MessageID(m + slot - 1);
    __ActorMessage(slot, 0);
    __CutsceneEnd();
}
