extern int iwram_3001e40;
extern unsigned char *__CreateActor(int a, int b, int c, int d);
extern void OvlFunc_931_2008c0c(void);
extern void OvlFunc_931_2008c44(void);
extern void __Actor_SetAnim(unsigned char *a, int n);

void OvlFunc_931_2008d08(void)
{
    unsigned char *a;
    int n;
    short *p;
    int v;
    int c1, c3;

    c1 = 0x80 << 15;
    c3 = 0xc8 << 17;
    v = iwram_3001e40;
    v &= 3;
    if (v == 0) {
        a = __CreateActor(0xde, c1, 0, c3);
        if (a != 0) {
            p = (short *)(a + 0x64);
            n = 0x14;
            *p = n;
            p++;
            *p = v;
            *(int *)(a + 0x68) = n;
            OvlFunc_931_2008c0c();
            *(void **)(a + 0x6c) = (void *)OvlFunc_931_2008c44;
            __Actor_SetAnim(a, 1);
        }
    }
}
