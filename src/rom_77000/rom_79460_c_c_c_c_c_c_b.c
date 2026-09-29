/* Func_807a664 (0x0807a664) -- 143 encodings, 316 bytes, exact.
 *
 * Three constructs here are deliberate and each is worth encodings; do not
 * "simplify" them without re-running objcmp:
 *   - `f` is a THIRD variable and the count is written before it, so the fill
 *     cursor is `n * 2 + (unsigned int)base` and not `base + n`.  expand_binop
 *     (optabs.c:657) swaps commutative operands when target == op1, so only a
 *     distinct destination with the operands in this order gives `add f,n,base`.
 *   - the ewram_2000438 base is carried in the now-dead `base`, not a fresh
 *     pointer.  REG_ALLOC_ORDER is {3,2,1,0} (arm.h:989); the reuse keeps the
 *     pseudo on r2, which already holds the reload constant, so the load cannot
 *     float above the increment.  A fresh variable is 29 differing.
 *   - `v` is a one-member HImode carrier set in the preheader.  It buys the
 *     register, not the position: a bare literal is loop-hoisted into r3 and
 *     blocks sched2 on a WAR.  An int carrier is SImode, loses the pool word,
 *     and comes out 312 bytes.
 *
 * `tryc.py --align` reports 10 instructions in disagreeing regions on this
 * byte-identical function.  objcmp is the authority.
 */
extern unsigned short ewram_2001078[];
extern unsigned short ewram_2000438[];
extern unsigned char gState[];
extern unsigned char *GetUnit(unsigned int unit);
extern unsigned char *GetItemInfo(int item);
extern void Func_8079ae8(unsigned int pc);
extern void CalcStats(unsigned int pc);
extern void Func_807a628(int a, int b);
extern void SetFlag(int id);
extern void Func_807808c(int a);

struct ItemSlot {
    unsigned short id;
};

struct Unit {
    unsigned char pad00[0xd8];
    unsigned short items[15];
};

void Func_807a664(void)
{
    unsigned short *p;
    unsigned short *src;
    unsigned short *dst;
    unsigned short *base;
    struct Unit *u;
    unsigned char *info;
    unsigned char *g;
    short s220;
    short s222;
    int i;
    int n;
    int j;
    struct ItemSlot v;
    unsigned short *f;

    p = ewram_2001078;
    if (*p != 0x6774) {
        *p = 0x6774;
        p++;
        g = gState;
        s220 = *(short *)(g + 0x220);
        s222 = *(short *)(g + 0x222);
        for (i = 0; i <= 3; i++) {
            u = (struct Unit *)GetUnit(i);
            for (n = 0; n <= 14; n++)
                *p++ = u->items[n];
            for (n = 0; n <= 14; n++) {
                info = GetItemInfo(u->items[n]);
                if (info[2] != 6)
                    u->items[n] = 0;
            }
            base = u->items;
            n = 0;
            src = base;
            dst = base;
            for (j = 0; j <= 14; j++) {
                v.id = *src++;
                if ((v.id << 16) != 0) {
                    *dst++ = v.id;
                    n++;
                }
            }
            if (n <= 14) {
                f = (unsigned short *)(n * 2 + (unsigned int)base);
                v.id = 0;
                n = 15 - n;
                do {
                    n--;
                    *f++ = v.id;
                } while (n != 0);
            }
            Func_8079ae8(i);
            CalcStats(i);
        }
        *p = s220;
        p++;
        *p = s222;
        base = ewram_2000438;
        p++;
        *p = base[0];
        p[1] = base[1];
        Func_807a628(0, 0x10);
        SetFlag(0x952);
    }
    Func_807808c(1);
}
