extern unsigned char gScript_945__0200e958[];
extern unsigned char gScript_945__0200e840[];

extern int __GetFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void OvlFunc_945_200c890(int a, int b, int c, int d);
extern void OvlFunc_945_200c8e8(int a, int b, int c);
extern void OvlFunc_945_200d004(void);

void OvlFunc_945_200b51c(void)
{
    int p, q, r, s;

    if (__GetFlag(0x93e)) {
        __MapActor_SetPos(8, 0, 0);
        __MapActor_SetPos(9, 0, 0);
        __MapActor_SetPos(0xa, 0, 0);
        __MapActor_SetPos(0xb, 0, 0);
        __MapActor_SetPos(0xc, 0, 0);
        OvlFunc_945_200c8e8(0xe, 0, 0);
    } else if (__GetFlag(0x8a << 4)) {
        OvlFunc_945_200c890(8, 0x98, 0xde << 1, 0xc0 << 6);
        __MapActor_SetBehavior(8, gScript_945__0200e958);
        p = 0xf0 << 1;
        r = 0xb0 << 8;
        q = 0xf4 << 1;
        s = 0xd0 << 8;
        OvlFunc_945_200c890(0xa, 0xb8, p, r);
        OvlFunc_945_200c890(0xc, 0xaa, q, r);
        OvlFunc_945_200c890(0xd, 0x88, q, s);
        OvlFunc_945_200c890(0xf, 0x78, p, s);
        OvlFunc_945_200c890(0xe, 0xb8, 0x20e, r);
        OvlFunc_945_200c890(0xb, 0x88, 0x92 << 2, 0x80 << 8);
        __MapActor_SetBehavior(0xb, gScript_945__0200e840);
    } else if (__GetFlag(0x928)) {
        OvlFunc_945_200d004();
    } else if (__GetFlag(0x925)) {
        OvlFunc_945_200c8e8(0x12, 0, 0);
    } else if (__GetFlag(0x911)) {
        if (__GetFlag(0x922)) {
            OvlFunc_945_200c8e8(0xe, 0, 0);
            __MapActor_SetPos(0xc, 0, 0);
        }
    }
}
