extern unsigned char *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetScript(void *a, void *s);
extern void __Sprite_SetAnim(void *s, int anim);
extern unsigned char gScript_907__02009d7c[];

void OvlFunc_907_2008f3c(unsigned char *src)
{
    unsigned char *actor;
    unsigned char *spr;
    int zero;
    int m;

    actor = __CreateActor(0x18, *(int *)(src + 8), *(int *)(src + 0xc),
                          *(int *)(src + 0x10));
    if (actor == 0)
        return;
    spr = *(unsigned char **)(actor + 0x50);
    __Actor_SetScript(actor, gScript_907__02009d7c);
    zero = 0;
    m = ~0xc;
    actor[0x55] = zero;
    actor[0x22] = 1;
    actor[0x23] = 2;
    if (spr == 0)
        return;
    __Sprite_SetAnim(spr, 2);
    spr[0x26] = zero;
    spr[5] = (spr[5] & m) | 4;
    spr[9] |= 0xc;
}
