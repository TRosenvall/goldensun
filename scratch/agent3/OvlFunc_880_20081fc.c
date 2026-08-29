extern short L16b2 __asm__(".L16b2");
extern short L16b4 __asm__(".L16b4");
extern short L16b6 __asm__(".L16b6");
extern short gScript_930__020096b8;
extern short L16ba __asm__(".L16ba");
extern short L16bc __asm__(".L16bc");
extern unsigned short L14d4[] __asm__(".L14d4");
extern unsigned short L14dc[] __asm__(".L14dc");
extern volatile unsigned int gKeyHeld;
extern void __PlaySound(int id);

void OvlFunc_880_20081fc(void)
{

    if (L16b2 == 0) {
        if (L16ba != 0) {
            if (gKeyHeld == 0)
                L16ba = 0;
        } else if (gKeyHeld != 0) {
            if (gKeyHeld == L14d4[L16b6]) {
                L16b6++;
                L16ba = 1;
                if (L14d4[L16b6] == 0) {
                    L16b2 = 1;
                    __PlaySound(0x6e);
                }
            } else {
                L16b6 = 0;
            }
        }
    }
    if (L16b4 == 0) {
        if (L16bc != 0) {
            if (gKeyHeld == 0)
                L16bc = 0;
        } else if (gKeyHeld != 0) {
            if (gKeyHeld == L14dc[gScript_930__020096b8]) {
                gScript_930__020096b8++;
                L16bc = 1;
                if (L14dc[gScript_930__020096b8] == 0) {
                    L16b4 = 1;
                    __PlaySound(0x6e);
                }
            } else {
                gScript_930__020096b8 = 0;
            }
        }
    }
}
