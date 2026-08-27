struct Actor {
    unsigned char pad00[6];
    short f6;
    unsigned char pad08[0x5b - 8];
    unsigned char f5b;
    unsigned char pad5c[0x62 - 0x5c];
    unsigned char f62;
    unsigned char f63;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetAnim(struct Actor *a, int anim);

void OvlFunc_945_2008284(struct Actor *a)
{
    struct Actor *b;
    int m;

    b = __MapActor_GetActor(9);
    if (a->f5b != 0)
        return;
    m = b->f63;
    if (m == 1) {
        a->f6 = 0xd0 << 8;
        a->f62 = 1;
        b->f63 = 0;
    } else if (m == 2) {
        if (a->f62 != 0)
            __Actor_SetAnim(a, 3);
        a->f62 = 0;
        b->f63 = 0;
    } else if (m == 3) {
        a->f6 = 0;
        b->f63 = 0;
    }
}
