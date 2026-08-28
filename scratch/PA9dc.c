typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern void *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(void *a, int f);
extern void __MapActor_SetPos(int slot, int x, int z);

void OvlFunc_932_200a9dc(void)
{
    unsigned char *p;
    unsigned int off;
    short v;

    __Actor_SetSpriteFlags(__MapActor_GetActor(9), 0);
    p = (unsigned char *)&gState;
    off = 0xe1;
    off <<= 1;
    p += off;
    off = 0;
    v = *(short *)(p + off);
    if (v == 2)
        __MapActor_SetPos(9, 0xb8 << 16, 0xa4 << 17);
}
