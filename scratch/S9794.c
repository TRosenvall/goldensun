struct Actor {
    unsigned char pad0[0x68];
    int f68;
    void *f6c;
};

extern unsigned int iwram_3001e40;

extern struct Actor *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void OvlFunc_921_2009704(void);
extern void OvlFunc_921_200974c(void);

void OvlFunc_921_2009794(void)
{
    struct Actor *a;

    if (iwram_3001e40 % 0x3c == 0) {
        a = __CreateActor(0xde, 0x1cf0000, 0, 0x92 << 17);
        if (a != 0) {
            OvlFunc_921_2009704();
            a->f68 = 0x3c;
            a->f6c = (void *)OvlFunc_921_200974c;
            __Actor_SetAnim(a, 5);
        }
    }
    if ((iwram_3001e40 + 0x1e) % 0x3c == 0) {
        a = __CreateActor(0xde, 0xa0 << 17, 0x80 << 14, 0xb2 << 17);
        if (a != 0) {
            OvlFunc_921_2009704();
            a->f68 = 0x3c;
            a->f6c = (void *)OvlFunc_921_200974c;
            __Actor_SetAnim(a, 5);
        }
    }
    if ((iwram_3001e40 + 0xa) % 0x3c == 0) {
        a = __CreateActor(0xde, 0xec << 15, 0, 0x8c << 15);
        if (a != 0) {
            OvlFunc_921_2009704();
            a->f68 = 0x3c;
            a->f6c = (void *)OvlFunc_921_200974c;
            __Actor_SetAnim(a, 5);
        }
    }
    if ((iwram_3001e40 + 0x32) % 0x3c == 0) {
        a = __CreateActor(0xde, 0xab << 17, 0, 0xf8 << 15);
        if (a != 0) {
            OvlFunc_921_2009704();
            a->f68 = 0x3c;
            a->f6c = (void *)OvlFunc_921_200974c;
            __Actor_SetAnim(a, 5);
        }
    }
    if ((iwram_3001e40 + 0x50) % 0x3c == 0) {
        a = __CreateActor(0xde, 0x1af0000, 0, 0xab << 16);
        if (a != 0) {
            OvlFunc_921_2009704();
            a->f68 = 0x3c;
            a->f6c = (void *)OvlFunc_921_200974c;
            __Actor_SetAnim(a, 5);
        }
    }
}
