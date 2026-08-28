extern int Func_8000888(int, int);
extern unsigned char *iwram_3001ed8;
extern short iwram_3001ad0[];
extern short L14c8[] __asm__(".L14c8");

#define CALL_VIA(f, a, b)                                       \
    ({                                                          \
        register int _a __asm__("r0") = (a);                    \
        register int _b __asm__("r1") = (b);                    \
        __asm__ volatile (                                      \
            "\t.align\t2, 0\n"                                  \
            "\tmov\tr12, pc\n"                                  \
            "\tbx\t%1"                                          \
            : "=r" (_a)                                         \
            : "r" (f), "0" (_a), "r" (_b)                       \
            : "memory", "r12");                                 \
        _a;                                                     \
    })

void OvlFunc_970_2008f80(void)
{
    unsigned char *base;
    unsigned short *dst;
    int acc, step, amp, i, y, t;
    unsigned short add;

    base = iwram_3001ed8;
    y = iwram_3001ad0[7];
    {
        dst = (unsigned short *)(base + (base[0xf00] ^ 1) * 0x780);
        step = *(int *)(base + 0xf10);
        acc = *(int *)(base + 0xf08)
            * (*(unsigned short *)(base + 0xf02) + (unsigned short)y);
        amp = *(int *)(base + 0xf18);
        add = *(unsigned short *)((char *)iwram_3001ad0 + 0xc);
        {
        register int (*g)(int, int) __asm__("r8") = Func_8000888;
        for (i = 0; i != 0xa0; i++) {
            t = (unsigned short)(CALL_VIA(g, L14c8[(acc >> 16) & 0xff], amp) / 256) + add;
            *dst = t;
            acc += step;
            dst += 2;
        }
        }
    }
    {
        dst = (unsigned short *)(base + (base[0xf00] ^ 1) * 0x780 + 2);
        step = *(int *)(base + 0xf14);
        acc = *(int *)(base + 0xf0c)
            * (*(unsigned short *)(base + 0xf02) + (unsigned short)y);
        amp = *(int *)(base + 0xf1c);
        add = (unsigned short)y;
        {
        register int (*g)(int, int) __asm__("r8") = Func_8000888;
        for (i = 0; i != 0xa0; i++) {
            t = (unsigned short)(CALL_VIA(g, L14c8[(acc >> 16) & 0xff], amp) / 256) + add;
            *dst = t;
            acc += step;
            dst += 2;
        }
        }
    }

    (*(unsigned short *)(base + 0xf02))++;
    base[0xf00] ^= 1;
}
