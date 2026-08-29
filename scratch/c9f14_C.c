extern int L5144 __asm__(".L5144");
extern unsigned char gScript_936__0200bec0[];
extern unsigned char gScript_936__0200bfb0[];
extern char *__MapActor_GetActor(int slot);
extern void __MapActor_SetBehavior(int slot, unsigned char *script);

void OvlFunc_936_2009f14(void)
{
    short zero = 0;

    switch (L5144) {
    case 0:
        *(short *)(__MapActor_GetActor(0x15) + 0x64) = zero;
        __MapActor_SetBehavior(0x15, gScript_936__0200bec0);
        L5144++;
        break;
    case 1:
        if (*(short *)(__MapActor_GetActor(0x15) + 0x64) == 0)
            return;
        *(short *)(__MapActor_GetActor(0x14) + 0x64) = zero;
        __MapActor_SetBehavior(0x14, gScript_936__0200bfb0);
        L5144++;
        break;
    case 2:
        if (*(short *)(__MapActor_GetActor(0x14) + 0x64) == 0)
            return;
        *(short *)(__MapActor_GetActor(0x14) + 0x64) = zero;
        __MapActor_SetBehavior(0x14, gScript_936__0200bec0);
        L5144++;
        break;
    case 3:
        if (*(short *)(__MapActor_GetActor(0x14) + 0x64) == 0)
            return;
        *(short *)(__MapActor_GetActor(0x15) + 0x64) = zero;
        __MapActor_SetBehavior(0x15, gScript_936__0200bfb0);
        L5144++;
        break;
    case 4:
        if (*(short *)(__MapActor_GetActor(0x15) + 0x64) == 0)
            return;
        L5144 = 0;
        break;
    }
}
