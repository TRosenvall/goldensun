struct A { unsigned char pad00[0x68]; void *f68; };

extern unsigned char gState[];
extern unsigned char L9fd44[] __asm__(".L9fd44");
extern unsigned char L9fe00[] __asm__(".L9fe00");
extern unsigned char L9fe04[] __asm__(".L9fe04");
extern unsigned char L9fe10[] __asm__(".L9fe10");
extern unsigned char L9fecc[] __asm__(".L9fecc");
extern unsigned char L9ff18[] __asm__(".L9ff18");
extern unsigned char L9ff2c[] __asm__(".L9ff2c");

extern void *MapActor_GetActor(int slot);
extern void _Actor_SetScript(struct A *a, unsigned char *s);

void Actor_SetBehavior(struct A *a, unsigned char *s)
{
    unsigned char *g;

    switch ((int)s) {
    case 1:
        s = L9fe00;
        break;
    case 2:
        s = L9fd44;
        break;
    case 3:
        s = L9fe10;
        break;
    case 4:
        s = L9fecc;
        break;
    case 5:
        s = L9ff18;
        break;
    case 6:
        g = gState;
        a->f68 = MapActor_GetActor(*(int *)(g + (0xfa << 1)));
        s = L9ff2c;
        break;
    case 7:
        s = L9fe04;
        break;
    }
    _Actor_SetScript(a, s);
}
