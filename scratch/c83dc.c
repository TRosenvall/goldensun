typedef struct {
    unsigned char pad00[0x1c2];
    short f1c2;
    short f1c4;
    short f1c6;
    unsigned char pad1c8[0x2c0 - 0x1c8];
} GlobalState;

extern unsigned char iwram_3001ebc[];
extern GlobalState gState;
extern void *__MapActor_GetActor(int slot);
extern void __ClearFlag(int id);
extern void __Actor_SetSpriteFlags(void *a, int f);

void OvlFunc_940_20083dc(void)
{
    short e;

    *(int *)(*(char **)iwram_3001ebc + (0xe0 << 1)) = 0x209;
    e = gState.f1c2;
    if (e == 0xa) {
        __ClearFlag(0x12f);
        gState.f1c4 = 0x69;
        gState.f1c6 = e;
    }
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x17), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x18), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x19), 0);
}
