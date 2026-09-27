/* Func_807a1f8  --  0x0807a1f8, was
 * asm/rom_77000/rom_79460_c_c_c_c_a_c_c_c_a_c_a_c.s (this function alone), so it
 * converts whole. Matched from scratch.
 *
 * An assignment INSIDE the condition, `if ((u->f0f8[a] & (mask = 1 << b)) == 0)`,
 * flips global-alloc's priority and gives the ROM's register roles for a*4 and
 * mask.
 */
struct Unit {
    unsigned char pad[0xf8];
    int f0f8[4];
    int f108[4];
    unsigned char f118[4];
    unsigned char f11c[4];
};
struct Ent { unsigned char a; unsigned char b; unsigned char c; signed char d; };
struct Rec { int pad[2]; struct Ent e[64]; int count; };
extern struct Unit *GetUnit(int unit);
extern struct Rec *Func_8077330(int bank);

int Func_807a1f8(int unit, int a, int b)
{
    struct Unit *u;
    struct Rec *rec;
    unsigned char *ent;
    int mask;
    int i;
    int d;
    int o;
    unsigned char *p;

    u = GetUnit(unit);
    if (u->f118[a] == 0)
        goto fail;
    if (u->f11c[a] > 9) {
        u->f11c[a] = 10;
        goto fail;
    }
    if ((u->f0f8[a] & (mask = 1 << b)) == 0)
        goto fail;
    if (u->f108[a] & mask)
        return 0;
    rec = Func_8077330((unsigned int)unit <= 7 ? 0 : 1);
    ent = (unsigned char *)rec->e;
    for (i = 0; i < *(int *)((char *)ent + 0x100); i++) {
        o = i * 4;
        if (a == ent[o] && b == (ent + o)[1])
            break;
    }
    if (i != *(int *)((char *)ent + 0x100)) {
        p = ent + i * 4;
        d = (signed char)p[3];
        if (d > 0 || d == -2)
            goto fail;
    }
    return 1;
fail:
    return 0;
}
