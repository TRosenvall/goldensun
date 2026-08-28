extern int L441c __asm__(".L441c");
extern void OvlFunc_954_200804c(void);
extern void __StartTask(void (*f)(void), int n);

void OvlFunc_954_2008158(void)
{
    L441c = 0x42;
    __StartTask(OvlFunc_954_200804c, 0xc80);
}
