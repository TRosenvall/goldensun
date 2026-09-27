/* Func_80b7548 and Func_80b75dc  --  0x080b7548 / 0x080b75dc, was
 * asm/rom_b5000/rom_b7410_a_a_c_a.s (these two, no data), so it converts whole;
 * verified from this combined file with objcmp --whole.
 *
 * Func_80b75dc was fresh; Func_80b7548 comes OUT OF A PARK that had tried about
 * 30 spellings. It closed on batch 286's queue spelling: the second queue at +2
 * is a member array read from the STRUCT pointer, `p->a[i + 0x32]`, and the copy
 * is `for (i = 0; i <= 5 && p->a[i + 0x32] != 0xff; i++) buf[i] = ...; n = i;`.
 */
extern signed char Lc2a62[] __asm__(".Lc2a62");
extern int Func_80b6a60(unsigned short *buf);
extern void *GetBattleActor(int id);
extern void Func_80b6f44(void *actor, int id, int x, int y);
extern void Func_80b7424(unsigned short *ids, int n, int *xs, int *ys);

struct BattleActor {
    void *sprite;
    unsigned char pad04[8];
    int fc;
    int f10;
};

struct F {
    short h;
    short a[0x2b];
    short q[7];
    unsigned char pad[0x2dc - 0x66];
    unsigned char map[14];
};

extern struct F *iwram_3001e74;

int Func_80b7548(void)
{
    unsigned short buf[14];
    int xs[6];
    int ys[6];
    struct F *p;
    int n;
    int i;
    int id;

    p = iwram_3001e74;
    for (i = 0; i <= 5 && p->a[i + 0x32] != 0xff; i++)
        buf[i] = p->a[i + 0x32];
    n = i;
    Func_80b7424(buf, n, xs, ys);
    for (i = 0; i < n; i++) {
        id = p->a[i + 0x32];
        if (id != 0xfe) {
            struct BattleActor *a = GetBattleActor(id);
            a->fc = xs[i] << 16;
            a->f10 = ys[i] << 16;
        }
    }
}

void Func_80b75dc(void)
{
    unsigned short buf[14];
    int xs[6];
    int ys[6];
    struct F *p;
    int n;
    int j;
    int k;
    int i;
    int id;
    int m;

    j = 0;
    m = 0;
    p = iwram_3001e74;
    n = Func_80b6a60(buf);
    for (i = 0; i < 14; i++)
        p->map[i] = 0xff;
    for (k = 0; k < 6; k++)
        p->map[k + 8] = k + 8;
    for (i = 0; i < n; i++) {
        id = buf[i];
        p->map[id] = j;
        Func_80b6f44(GetBattleActor(id), id, Lc2a62[j * 2], Lc2a62[j * 2 + 1]);
        j++;
    }
    for (i = 0; i <= 5 && p->a[i + 0x32] != 0xff; i++)
        buf[i] = p->a[i + 0x32];
    n = i;
    Func_80b7424(buf, n, xs, ys);
    for (i = 0; i < n; i++, m++) {
        id = p->a[i + 0x32];
        if (id != 0xfe)
            Func_80b6f44(GetBattleActor(id), id, xs[m], ys[m]);
    }
}
