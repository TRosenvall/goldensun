extern int L5144 __asm__(".L5144");
extern unsigned char gScript_936__0200bec0[];
extern unsigned char gScript_936__0200bfb0[];
extern short *__MapActor_GetActor(int slot);
extern void __MapActor_SetBehavior(int slot, unsigned char *script);

void OvlFunc_936_2009f14(void)
{
    switch (L5144) {
    case 0:
        __MapActor_GetActor(0x15)[0x32] = 0;
        __MapActor_SetBehavior(0x15, gScript_936__0200bec0);
        L5144++;
        break;
    case 1:
        if (__MapActor_GetActor(0x15)[0x32] == 0)
            return;
        __MapActor_GetActor(0x14)[0x32] = 0;
        __MapActor_SetBehavior(0x14, gScript_936__0200bfb0);
        L5144++;
        break;
    case 2:
        if (__MapActor_GetActor(0x14)[0x32] == 0)
            return;
        __MapActor_GetActor(0x14)[0x32] = 0;
        __MapActor_SetBehavior(0x14, gScript_936__0200bec0);
        L5144++;
        break;
    case 3:
        if (__MapActor_GetActor(0x14)[0x32] == 0)
            return;
        __MapActor_GetActor(0x15)[0x32] = 0;
        __MapActor_SetBehavior(0x15, gScript_936__0200bfb0);
        L5144++;
        break;
    case 4:
        if (__MapActor_GetActor(0x15)[0x32] == 0)
            return;
        L5144 = 0;
        break;
    }
}
