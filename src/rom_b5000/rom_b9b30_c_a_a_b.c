/* Cluster Func_80ba584..Func_80ba584 extracted from goldensun/asm/rom_b5000/rom_b9b30_c_a.s.
 *
 * Total .text for this TU = 296 bytes (= 0x128). Never attempted before batch 279.
 * No pins, no volatile, no flags.
 *
 * ONE LEVER, AND IT IS PLACEMENT: `flags & 2` must be a NAMED LOCAL assigned in the block
 * BETWEEN `_GetUnit(s->f2)` and the `Func_80b82c4` call. The ROM's spec is
 * `mov r8, r1` (argument 1 held in a high callee-saved register) plus a mid-function
 * `mov r7, #2 / mov r2, r8 / and r7, r2` -- TWO pseudos, not one.
 *
 *     inline `if ((flags & 2) != 0)`                    17 differing
 *     `x = flags & 2` after Func_80c10e8(0, 0)           6
 *     `x = flags & 2` after _GetUnit(s->f2)              0
 *     `x = flags & 2` at the top of the function         18
 *
 * So it is neither the naming nor the declaration but WHERE the value is born -- block 0 is the
 * worst of the four. That is the recorded assignment-position axis, and this is its cleanest
 * four-point measurement.
 *
 * AND FIXING IT FIXED A SECOND DEFECT FOR FREE. The `_Func_800be20` argument-fill order
 * (`mov r2,#1 / mov r1,#0 / ldrsh / mov r1,#2`) corrected itself. I had that listed as a
 * separate problem, and it was a consequence -- the batch-276 rule that an argument-fill
 * difference can be downstream of an allocation, holding again.
 */
struct Src {
    unsigned char f0;
    signed char f1;
    unsigned char f2;
    unsigned char pad03[0x55];
    int f58;
};

struct Ctx {
    int f0;
    int f4;
    int f8;
    unsigned char pad0c[0x54 - 0xc];
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

struct Spr {
    unsigned char pad00[0x28];
    short *parts[1];
};

extern int *iwram_3001f00;

extern struct BA *GetBattleActor(int id);
extern int atan2(int y, int x);
extern void WaitFrames(int n);
extern void Func_80c10e8(int a, int b);
extern int InitAnimContext(struct Src *s, struct Ctx *c);
extern void _GetUnit(int id);
extern struct Spr *Func_80b7f70(struct A *a, int i);
extern int _Func_800be20(int a, int b, int c);
extern void Func_80b82c4(int a, int b, int c, int d);
extern void _Actor_SetAnimSpeed(struct A *a, int speed);
extern void Func_80b8178(int a);
extern void Func_80b8000(int a);
extern void _Anim_Attack(struct Ctx *c);
extern void Func_80bb938(void);

int Func_80ba584(struct Src *s, int flags)
{
    struct Ctx c;
    int *q;
    struct A *a;
    int t;
    int x;
    int v;

    q = iwram_3001f00;
    a = GetBattleActor(s->f0)->f0;
    v = (unsigned short)atan2(a->f8, a->f10);
    if (s->f0 <= 7)
        t = v - 0x2000;
    else
        t = v + 0x6000;
    t = ((t & 0x7fff) - 0x2000) / 2 + 0x2000;
    if (*q == t) {
        *q = t;
        WaitFrames(5);
    } else {
        *q = t;
        WaitFrames(0x14);
    }
    Func_80c10e8(0, 0);
    InitAnimContext(s, &c);
    _GetUnit(c.f8);
    _GetUnit(s->f2);
    x = flags & 2;
    Func_80b82c4(c.f8, s->f2,
                 _Func_800be20(*Func_80b7f70(GetBattleActor(s->f0)->f0, 0)->parts[0], 2, 1),
                 0);
    _Actor_SetAnimSpeed(GetBattleActor(c.f8)->f0, 0x10);
    GetBattleActor(s->f2);
    if (s->f2 <= 7)
        c.f4 = 1;
    else
        c.f4 = 0;
    if (x != 0) {
        WaitFrames(0xa);
        Func_80b8178(s->f2);
        WaitFrames(2);
        WaitFrames(4);
        WaitFrames(0xa);
        Func_80b8000(s->f2);
    } else {
        _Anim_Attack(&c);
        Func_80bb938();
        Func_80b8000(s->f2);
    }
    Func_80b8000(c.f8);
    return 0;
}
