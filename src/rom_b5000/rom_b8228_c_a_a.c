/* Func_80b83b4  --  0x080b83b4
 *
 * The whole of goldensun/asm/rom_b5000/rom_b8228_c_a_a.s, which held this
 * function and no data, so the .o keeps its name and its slot in stage1.ld is
 * unchanged.
 *
 * Finds the midpoint between two battle actors and hands it to Func_80b83b0 --
 * used to aim an effect at the space between attacker and target.
 *
 * 0x80000000 IS A SENTINEL, NOT A COORDINATE. Each actor carries a pair of
 * override fields at +0x38 and +0x40, and the ROM tests each against
 * `mov r4, #0x80 / lsl r4, #24` before falling back to the live position at +8
 * and +0x10. Four independent `if (v == 0x80000000) v = ...;` guards, which is
 * why the same compare appears four times against one register.
 *
 * `(x1 + x2) / 2` is a SIGNED divide, hence `lsr r2, r3, #31 / add r3, r2 /
 * asr r3, #1` -- gcc's rounding-toward-zero sequence, not something the source
 * spells. Written as `>> 1` it would be a bare `asr` and would not match.
 *
 * Matched on the first screen.
 */
struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[0x38 - 0x14];
    int f38;
    unsigned char pad3c[4];
    int f40;
};

struct C { struct A *f0; };

extern struct C *GetBattleActor(int n);
extern void Func_80b83b0(int *v, int n);

void Func_80b83b4(int a, int b)
{
    struct A *p;
    struct A *q;
    int x1;
    int y1;
    int x2;
    int y2;
    int t[3];

    p = GetBattleActor(a)->f0;
    q = GetBattleActor(b)->f0;
    x1 = p->f38;
    if (x1 == 0x80000000)
        x1 = p->f8;
    y1 = p->f40;
    if (y1 == 0x80000000)
        y1 = p->f10;
    x2 = q->f38;
    if (x2 == 0x80000000)
        x2 = q->f8;
    y2 = q->f40;
    if (y2 == 0x80000000)
        y2 = q->f10;
    t[0] = (x1 + x2) / 2;
    t[1] = 0;
    t[2] = (y1 + y2) / 2;
    Func_80b83b0(t, 0x80 << 5);
}
