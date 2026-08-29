/* Func_8099678 -- PARKED, 15 of 90 aligned.  Ref: asm/rom_8a000/rom_97b54_a_c_c_c.s
   Structure is right: the two `x < 0 ? x + 0x1fffff : x` biases are written
   as plain signed divisions by 0x200000 / 0x100000, the second global is
   reached as *(T **)((unsigned char *)&iwram_3001ebc - 0x4c) (the symbol
   add/sub chain), and the two `short` accesses on iwram_3001ebc are struct
   members so the address is materialised with a zero index.
   Residue: (a) the 0x1f mask should be applied in its own statement right
   after each division so gcc keeps it in r1 across both; (b) `bhi` on the
   +0x22 byte wants an unsigned compare; (c) the ROM uses the three-operand
   `asr r3, r0, #0x15` where we get the destructive form.
   scratch/agent2/99678_B.c applies (a) and (b): it fixes those but turns the
   whole function into a clean r1/r2/r3/r0 permutation, 25 of 90 -- worse.
   scratch/agent2/99678_A.c names the loaded values to force the three-operand
   shift: 91 lines against 90 and much worse (the copies are real).
   Blocker class: register birth order / allocation permutation.  */
struct W {
    unsigned char pad0[0x17e];
    short f17e;
    unsigned char pad1[0x19e - 0x180];
    short f19e;
};

extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;
extern unsigned char ewram_2020000[];
extern unsigned char gBuffer[];

extern unsigned char *GetFieldActor(int e);

void Func_8099678(void)
{
    unsigned char *g;
    struct W *base;
    unsigned char *a;
    unsigned char *p;
    unsigned char *tbl;
    int mode;
    int x;
    int y;
    int off;
    int k;

    g = gState;
    g += 0x1f4;
    base = (struct W *)iwram_3001ebc;
    a = GetFieldActor(*(int *)g);
    mode = base->f19e;
    tbl = *(unsigned char **)((unsigned char *)&iwram_3001ebc - 0x4c);
    if (mode == 3) {
        x = *(int *)(a + 8) / 0x200000;
        y = *(int *)(a + 0x10) / 0x200000;
        p = ewram_2020000 + ((x & 0x1f) + ((y & 0x1f) << 5)) * 4;
    } else {
        k = *(a + 0x22);
        if (k <= 2) {
            off = k * 48 + 0x130;
            p = *(unsigned char **)(tbl + off);
        } else {
            p = gBuffer;
        }
        x = *(int *)(a + 8) / 0x100000;
        y = *(int *)(a + 0x10) / 0x100000;
        p += (x + (y << 7)) * 4;
    }
    if (p[2] != 0xfb)
        base->f17e = 0x2092;
}
