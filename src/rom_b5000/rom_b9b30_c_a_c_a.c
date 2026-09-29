/* Func_80ba978  --  0x080ba978, was asm/rom_b5000/rom_b9b30_c_a_c_a.s (this
 * function alone, no data), so it converts whole.
 *
 * Parked at 1 of 273. The last encoding was ONE LINE: `zi = s->f0 | s->f0;`.
 * Every compare-operand read of s->f0 expands as (set (reg:QI) (mem:QI)), and
 * GCSE saves the first one after the atan2 call in a pseudo (r4) and turns the
 * later reads into copies. A plain `zi = s->f0` expands instead as
 * (zero_extend:SI (mem:QI)) -- a DIFFERENT expression to GCSE's hash -- so it
 * stays an `ldrb`. `x | x` is not folded by the front end: it expands as two
 * QI loads and an ior, CSE collapses them to one QI load, GCSE makes it a copy
 * of the saved pseudo, and combine drops the extension, giving the ROM's
 * `mov r2,r4`. `+0`, `^0`, `<<0`, `&0xff`, a char cast, a statement expression
 * and `b ? x : x` all fold or stay an extend-load. Read from the .00.rtl,
 * .03.cse and .07.gcse dumps.
 */
struct Src {
    unsigned char f0;
    unsigned char pad01[0x2 - 0x1];
    unsigned char f2;
    unsigned char pad03[0x50 - 0x3];
    int f50;
    unsigned char pad54[0x58 - 0x54];
    int f58;
    int f5c;
};

struct Ctx {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    int f18;
    int f1c;
    int f20;
    short f24[8];
    unsigned char f34[8][4];
};

struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[0x10 - 0xc];
    int f10;
};

struct BA {
    struct A *f0;
};

struct Part {
    unsigned char pad00[5];
    unsigned char f5;
};

struct Spr {
    unsigned char pad00[0x27];
    unsigned char f27;
    struct Part *parts[1];
};

extern int *iwram_3001f00;
extern unsigned char *iwram_3001e74;

extern int InitAnimContext(struct Src *s, struct Ctx *c);
extern void Func_80c10e8(int a, int b);
extern struct BA *GetBattleActor(int id);
extern void _Actor_SetAnim(struct A *a, int anim);
extern void _Actor_SetAnimSpeed(struct A *a, int speed);
extern struct Spr *Func_80b7f70(struct A *a, int i);
extern void StartTask(void (*task)(void), int priority);
extern void Func_80bd898(void);
extern void _Anim_Attack(struct Ctx *c);
extern void _Anim_Func(struct Ctx *c);
extern void Func_80be02c(void);
extern void Func_80b8000(int a);
extern void Func_80bbabc(int a, int b);
extern void Func_80bb938(void);
extern void Func_80c1a14(void);
extern void _Func_801f200(int n);
extern void _PlaySound(int sfx);
extern int atan2(int dz, int dx);
extern void Anim_MoveIntro(int a, int b, int c, int d);

int Func_80ba978(struct Src *s, int b)
{
    struct Ctx c;
    struct A *a;
    struct A *a2;
    struct Spr *spr;
    int *q;
    unsigned char *iw;
    int t;
    int v;
    int ang;
    int base;
    int k;
    unsigned int zi;
    int b1;
    int i;
    int j;

    q = iwram_3001f00;
    if ((s->f58 & (0x80 << 11)) != 0) {
        if (s->f0 <= 7)
            t = -0x2000;
        else
            t = 0xa0 << 7;
        *q = t;
        q[1] = 0x3c;
    } else {
        a2 = GetBattleActor(s->f0)->f0;
        ang = (unsigned short)atan2(a2->f8, a2->f10);
        if (s->f0 <= 7)
            v = ang - 0x1800;
        else
            v = ang + 0x1800;
        v = (short)v;
        v = v + ((s->f0 <= 7 ? 0x2000 : -0x2000) - v) * 3 / 4;
        zi = s->f0 | s->f0;
        if (s->f2 <= 7) {
            k = zi <= 7;
            if (!k)
                goto done;
        } else {
            k = zi > 7;
            if (!k)
                goto done;
        }
        if (s->f0 <= 7)
            v = 0x2400;
        else
            v = -0x2400;
    done:
        if (*q != v)
            *q = v;
    }
    if ((s->f58 & (0x80 << 12)) != 0) {
        if (s->f0 <= 7)
            t = -0x2000;
        else
            t = 0x2000;
        *q = t;
        q[1] = 0x3c;
    }
    InitAnimContext(s, &c);
    b1 = b & 1;
    if (b1)
        c.f1c = 1;
    Func_80c10e8(0, 0);
    iw = iwram_3001e74;
    _Func_801f200(iw[0x41] & -2);
    a = GetBattleActor(c.f8)->f0;
    _Actor_SetAnim(a, 3);
    _Actor_SetAnimSpeed(a, 0x10);
    _PlaySound(0x9a);
    if ((b & 2) != 0)
        Anim_MoveIntro(c.f8, s->f50, 1, 0);
    else if (!b1)
        Anim_MoveIntro(c.f8, s->f50, 0, 0);
    if (s->f2 <= 7)
        c.f4 = 1;
    else
        c.f4 = 0;
    for (i = 0; i != c.f14; i++) {
        spr = Func_80b7f70(GetBattleActor(c.f24[i])->f0, 0);
        for (j = 0; j != spr->f27 - 1; j++)
            c.f34[i][j] = spr->parts[j]->f5;
    }
    if (s->f5c != 0) {
        if (s->f5c == 1) {
            Func_80bbabc(0, s->f0);
            Func_80bbabc(4, 0x856);
        } else {
            Func_80bbabc(4, 0x855);
        }
        Func_80bb938();
        Func_80c1a14();
    } else {
        StartTask(Func_80bd898, 0xc8 << 4);
        if (c.f0 != 0) {
            if ((s->f58 & (0x80 << 7)) != 0)
                _Anim_Attack(&c);
            else
                _Anim_Func(&c);
        } else {
            Func_80c1a14();
        }
        Func_80be02c();
        _Actor_SetAnim(a, 1);
        for (i = 0; i != c.f14; i++)
            Func_80b8000(c.f24[i]);
    }
    return 0;
}
