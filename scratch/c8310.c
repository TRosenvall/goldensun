typedef struct {
    unsigned char pad00[0x1c2];
    short f1c2;
    unsigned char pad1c4[0x2c0 - 0x1c4];
} GlobalState;

extern GlobalState gState;
extern int __GetFlag(int id);
extern short L61fc[] __asm__(".L61fc");
extern short L6250[] __asm__(".L6250");
extern short L5e30[] __asm__(".L5e30");

void *OvlFunc_899_2008310(void)
{
    short v;

    v = gState.f1c2;
    if (v <= 0x11 && v >= 0xf)
        return L61fc;
    if (__GetFlag(0x855))
        return L6250;
    return L5e30;
}
