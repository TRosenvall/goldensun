extern unsigned char *iwram_3001f30;
extern void __Func_80958a8(void);
extern void __Func_80b0840(int a);
extern void __Func_80974d8(int *v);
extern void __Func_809ba90(unsigned char *p, int a, int b, int c);
extern void __Func_809ba7c(unsigned char *p, void (*f)(unsigned char *));
extern void __Func_809ba70(unsigned char *p, int n);
extern void __Sprite_SetColorswap(int a, int b);
extern unsigned int __Random(void);
extern void __WaitFrames(int n);
extern void __Func_8010788(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
extern void __Func_80b0894(void);
extern void __Func_80958e4(void);
extern void OvlFunc_957_200ba30(unsigned char *e);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_957_200bad4(void)
{
    int v[3];
    int *bp;
    unsigned char *base;
    unsigned char *p;
    unsigned char *q;
    int val;
    int s0;
    int s1;
    int t0;
    int t1;
    int i;

    __Func_80958a8();
    base = iwram_3001f30;
    __Func_80b0840(0x202108);
    v[0] = 0xfc << 17;
    v[1] = 0xc0 << 13;
    v[2] = 0x90 << 16;
    __Func_80974d8(v);
    p = base + 0x58;
    i = 0x17;
loop1:
    __Func_809ba90(p, 0x8e << 1, v[0], v[2]);
    __Func_809ba7c(p, OvlFunc_957_200ba30);
    __Func_809ba70(p, 7);
    __Sprite_SetColorswap(*(int *)p, (__Random() * 7) >> 16);
    *(int *)(p + 0x2c) = __Random() / 3 + (0xc0 << 9);
    *(int *)(p + 0x28) = *(int *)(p + 0x2c);
    i--;
    __WaitFrames(1);
    p += 0x48;
    if (i >= 0)
        goto loop1;
    __WaitFrames(0x50);
    s0 = 0x1e;
    s1 = 0x37;
    __Func_8010788(0x29, 0x37, 3, 2, s0, s1);
    t0 = 0x1f;
    t1 = 8;
    __Func_8010704(0x2a, 8, 1, 1, t0, t1);
    __WaitFrames(0x32);
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666; __Func_8012330(q0, q1, q2); }
    __WaitFrames(0x1e);
    q = base;
    val = 2;
    q += 0x98;
    i = 0x17;
loop2:
    if (*(signed char *)(q + 5) != 0)
        *q = val;
    i--;
    q += 0x48;
    if (i >= 0)
        goto loop2;
    __Func_8012350();
    __Func_80b0894();
    __Func_80958e4();
}
