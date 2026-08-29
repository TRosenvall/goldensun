extern short L45[] __asm__(".L45");
extern short L29[] __asm__(".L29");
extern short L19[] __asm__(".L19");
extern short L26[] __asm__(".L26");
extern short L32[] __asm__(".L32");
extern void OvlFunc_common1_1334(void);
extern void OvlFunc_common1_1354(void);
extern void __StartTask(void (*f)(void), int n);

void OvlFunc_common1_1490(int a, int b, int c)
{
    OvlFunc_common1_1334();
    *L45 = a;
    *L29 = b;
    *L19 = c & 3;
    *L26 = 0;
    *L32 = 0;
    __StartTask(OvlFunc_common1_1354, 0xc80);
}
