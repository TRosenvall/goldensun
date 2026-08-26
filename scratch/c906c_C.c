typedef struct {
    unsigned char pad00[0x1f4];
    int f1f4;
    unsigned char pad1f8[0x2c0 - 0x1f8];
} GlobalState;

extern GlobalState gState;
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __Func_809280c(int a, int b, int c);
/* __Func_8092c40 intentionally implicit */

void OvlFunc_971_200906c(int slot)
{
    int base;
    int n;

    n = 0;
    __CutsceneStart();
    switch (slot) {
    default:
        base = 0x2982;
        break;
    case 0xc:
        base = 0x2985;
        break;
    case 0xd:
        base = 0x297f;
        break;
    }
    __Func_809280c(slot, gState.f1f4, 0);
    if (__GetFlag(0xc1 << 2)) {
        n = 2 - (__GetFlag(0x305) != 0);
    }
    __MessageID(base + n);
    __Func_8092c40(slot, 0);
    __CutsceneEnd();
}
