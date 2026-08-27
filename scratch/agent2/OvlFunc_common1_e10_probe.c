extern short L33 __asm__(".L933");
extern short L22 __asm__(".L922");
extern short L36 __asm__(".L936");
extern short L46 __asm__(".L946");
extern short L24 __asm__(".L924");
extern int L25 __asm__(".L925");
extern unsigned char *L37 __asm__(".L937");
extern unsigned char L11[] __asm__(".L911");
extern unsigned char L2[] __asm__(".L902");
extern unsigned char L12[] __asm__(".L912");
extern unsigned char L3[] __asm__(".L903");
extern unsigned char L13[] __asm__(".L913");

extern void OvlFunc_common1_920(void);
extern void __StartTask(void (*f)(void), int a);

void OvlFunc_common1_e10(int a, int b)
{
    unsigned char *p;

    L33 = a;
    L22 = b << 4;
    __StartTask(OvlFunc_common1_920, 0xc80);
    p = L11;
    if (a == 2)
        p = L2;
    if (a == 4)
        p = L12;
    if (a == 3) {
        if (b != 0)
            p = L3;
        else
            p = L13;
    }
    L36 = 0;
    L37 = p;
    L46 = 0;
    L24 = 0;
    L25 = 0;
}
