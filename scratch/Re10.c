extern short L33[] __asm__(".L33");
extern short L22[] __asm__(".L22");
extern short L36[] __asm__(".L36");
extern short L46[] __asm__(".L46");
extern short L24[] __asm__(".L24");
extern void *L37[] __asm__(".L37");
extern int L25[] __asm__(".L25");
extern unsigned char L11[] __asm__(".L11");
extern unsigned char L2[] __asm__(".L2");
extern unsigned char L12[] __asm__(".L12");
extern unsigned char L3[] __asm__(".L3");
extern unsigned char L13[] __asm__(".L13");
extern void OvlFunc_common1_920(void);
extern void __StartTask(void (*f)(void), int n);

void OvlFunc_common1_e10(int a, int b)
{
    unsigned char *p;

    *L33 = a;
    *L22 = b << 4;
    __StartTask(OvlFunc_common1_920, 0xc8 << 4);
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
    *L36 = 0;
    *L37 = p;
    *L46 = 0;
    *L24 = 0;
    *L25 = 0;
}
