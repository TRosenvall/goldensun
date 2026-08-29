struct Actor {
    unsigned char pad0[0x64];
    unsigned short goalFacing;
    unsigned short unk_66;
    int unk_68;
    void *update;
};

extern int iwram_3001e40;
extern struct Actor *__CreateActor(int a, int b, int c, int d);
extern void OvlFunc_931_2008c0c(void);
extern void OvlFunc_931_2008c44(void);
extern void __Actor_SetAnim(struct Actor *a, int n);

void OvlFunc_931_2008d08(void)
{
    struct Actor *a;
    int v;
    int n;
    int c1, c3;

    c1 = 0x80 << 15;
    c3 = 0xc8 << 17;
    v = iwram_3001e40;
    v &= 3;
    if (v == 0) {
        a = __CreateActor(0xde, c1, 0, c3);
        if (a != 0) {
            n = 0x14;
            a->goalFacing = n;
            a->unk_66 = v;
            a->unk_68 = n;
            OvlFunc_931_2008c0c();
            a->update = (void *)OvlFunc_931_2008c44;
            __Actor_SetAnim(a, 1);
        }
    }
}
