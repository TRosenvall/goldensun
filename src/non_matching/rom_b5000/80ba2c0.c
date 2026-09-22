/* Func_80ba2c0 (0x080ba2c0) -- NON-MATCHING, 205 encodings of 303, size 704
 * against the ROM's 708 (-4).  51 diff regions, and they have ONE root cause.
 *
 * Blocker class: a LIVE-RANGE SPLIT of an already-allocated pseudo.  This is
 * recorded as a park CLASS, not a spelling hunt -- see below.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_b5000/80ba2c0.c \
 *     asm/rom_b5000/rom_b9b30_c_a_a_a.s --func Func_80ba2c0
 * The reference .s holds this function alone, so no split is needed.
 *
 * THE SINGLE ROOT CAUSE, stated precisely.  The ROM SPILLS the `struct Src *s`
 * parameter to sp+0xc and reloads it NINE times; ours keeps it in r11.  So the
 * frame is 0x68 not 0x6c, every `ldr rX,[sp,#0xc]` is `mov rX,r11`, and
 * buf/c sit 4 bytes low.  That is the whole -4 and most of the 205.
 *
 * AND THE CAUSE OF THE CAUSE IS EXACTLY ONE REGISTER OF PRESSURE.  The ROM's
 * animation loop carries a REDUNDANT COPY of the context pointer into r8
 * (`mov r8, r7` in the preheader, `mov r3, r8` in the body -- three
 * instructions per iteration where two suffice).  That copy takes the fourth
 * high register and forces `s` out to the stack.  gcc, being correct, does not
 * make the redundant copy, so it has r8 free, so it keeps `s` in r11.
 *
 * WHY THIS IS A CLASS AND NOT A SPELLING.  To reproduce the ROM, a source
 * construct would have to make gcc hold a SECOND live copy of a pointer it
 * already has in a register, across a loop, without adding any other
 * instruction.  Nine spellings were tried and gcc COALESCES OR REVERTS EVERY
 * ONE: `cq = cp` inside the loop, `cq = &c` before it, direct `c.f8` in the
 * loop only (reverts everything, 119), two `int` temps for the two loads, a
 * separate variable for the be20 result (130), `signed char` vs
 * `(signed char)` for the 0x2c field (tie), and declaration reordering.
 * Coalescing is the correct behaviour and no C construct suppresses it here.
 *
 * TWO LEVERS THAT DID WORK, both worth recording:
 *
 *   A NAMED POINTER TO A STACK STRUCT: `cp = &c; cp->f8` instead of `c.f8`
 *   for a 0x54-byte context: 119 -> 51.  IT IS ALL-OR-NOTHING -- ONE direct
 *   `c.f8` anywhere reverts the whole function to frame-relative addressing
 *   (measured 119, identical to the no-cp number).
 *
 *   ONE VARIABLE SERVING TWO DISJOINT BRANCHES, confirmed NEGATIVELY: the
 *   _Func_800be20 result and the loop bound `k` are ONE variable, and
 *   SPLITTING them costs 130 against 51.  Same lever as
 *   src/non_matching/rom_8a000/808e23c.c from the same round, measured from the
 *   other direction.
 *
 * Remaining exact pairs beyond the spill, both plausibly downstream of it: the
 * _Func_800be20 argument fill (`mov r2,#1 / mov r1,#0 / ldrsh / mov r1,#2`
 * order) and `add r3,r5,r2` vs `add r5,r2` for u->f128.
 *
 * NEXT: nothing source-level identified.  If this is revisited, the question is
 * not "which spelling" but "is there any construct that defeats gcc-2.96's
 * coalescing of a copy whose source is still live" -- and the honest prior
 * from nine probes is no.  Park class is new enough to be worth naming:
 * REDUNDANT-COPY PRESSURE, where the ROM is LESS optimal than gcc and the
 * difference is a register the ROM wasted.
 */
#include "gba/io.h"

struct Src {
    unsigned char f0;
    unsigned char f1;
    unsigned char f2;
    unsigned char pad03[0x1e - 0x3];
    signed char f1e;
    unsigned char pad1f[0x2c - 0x1f];
    unsigned char f2c;
    unsigned char pad2d[0x58 - 0x2d];
    int f58;
};

struct Ctx {
    int f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[0x1c - 0x10];
    int f1c;
    unsigned char pad20[0x24 - 0x20];
    short f24;
    unsigned char pad26[0x54 - 0x26];
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

struct U {
    unsigned char pad00[0x128];
    unsigned char f128;
};

extern int *iwram_3001f00;
extern unsigned char *iwram_3001e74;

extern struct BA *GetBattleActor(int id);
extern int atan2(int y, int x);
extern void WaitFrames(int n);
extern void Func_80c10e8(int a, int b);
extern int InitAnimContext(struct Src *s, struct Ctx *c);
extern void _Func_801f200(int a);
extern struct U *_GetUnit(int id);
extern struct Spr *Func_80b7f70(struct A *a, int i);
extern int _Func_800be20(int a, int b, int c);
extern int GetEnemyAttackAnimParam(int a);
extern void Func_80b82c4(int a, int b, int c, int d);
extern void _Actor_SetAnimSpeed(struct A *a, int speed);
extern void Func_80b8178(int a);
extern void Func_80b8000(int a);
extern void Func_80bbabc(int a, int b);
extern void Func_80bb938(void);
extern void CreateBattleSpriteOverlays(short *buf, int b);
extern void Func_80c0df4(int a, int b, int c);
extern void StartTask(void (*task)(void), int priority);
extern void Func_80bd898(void);
extern void _Anim_Attack(struct Ctx *c);
extern void _Anim_Func(struct Ctx *c);
extern void Func_80be02c(void);
extern void Func_80b6cb0(void);
extern void Func_80c0cec(int a, int b, int c, int d);

int Func_80ba2c0(struct Src *s)
{
    struct Ctx c;
    struct Ctx *cp;
    short buf[3];
    struct A *a;
    struct U *u;
    int t;
    int v;
    int sc;
    int *q;
    int flag;
    int flagA;
    int k;
    int i;
    int m;

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
        WaitFrames(0xa);
    }
    Func_80c10e8(0, 0);
    cp = &c;
    InitAnimContext(s, cp);
    if (cp->f0 == 0x87)
        _Func_801f200(*(iwram_3001e74 + 0x41) & -2);
    u = _GetUnit(cp->f8);
    _GetUnit(cp->f24);
    sc = (signed char)s->f2c;
    flag = 0;
    if (s->f1e == 0)
        flag = 1;
    k = _Func_800be20(*Func_80b7f70(GetBattleActor(s->f0)->f0, 0)->parts[0], 2, 1);
    Func_80b82c4(cp->f8, cp->f24, k, GetEnemyAttackAnimParam(u->f128) << 16);
    _Actor_SetAnimSpeed(GetBattleActor(cp->f8)->f0, 0x10);
    GetBattleActor(cp->f24);
    if ((unsigned short)cp->f24 <= 7)
        cp->f4 = 1;
    else
        cp->f4 = 0;
    REG_WIN0H = 0xf0;
    REG_WIN0V = 0x1088;
    REG_WIN1H = 0xf0;
    REG_WIN1V = 0x1088;
    REG_WININ = 0x3537;
    REG_WINOUT = 0x3f21;
    REG_DISPCNT |= 0x6000;
    if (flag != 0) {
        WaitFrames(0xa);
        Func_80b8178(cp->f24);
        WaitFrames(2);
        WaitFrames(4);
        WaitFrames(0xa);
        Func_80bbabc(0, s->f2);
        Func_80bbabc(4, 0x853);
        Func_80bb938();
        Func_80b8000(cp->f24);
    } else {
        flagA = 0;
        cp->f1c = 0;
        if (s->f58 != 0)
            cp->f1c = 1;
        if (sc != 0) {
            cp->f0 += 0xc8;
            flagA = 1;
            q[5] = 1;
            buf[0] = cp->f8;
            buf[1] = cp->fc;
            buf[2] = 0xff;
            CreateBattleSpriteOverlays(buf, 0);
        }
        k -= 8;
        if (k <= 0)
            k = 1;
        m = 0;
        for (i = 0; i != k; i++) {
            if (flagA != 0)
                Func_80c0df4(cp->f8, cp->fc, m / k + 0x64);
            WaitFrames(1);
            m += 0x1e;
        }
        StartTask(Func_80bd898, 0xc8 << 4);
        if (cp->f0 != 0) {
            if ((s->f58 & (0x80 << 7)) != 0)
                _Anim_Attack(cp);
            else
                _Anim_Func(cp);
        }
        Func_80be02c();
        if (sc != 0) {
            q[5] = 0;
            Func_80b6cb0();
            Func_80c0cec(0, 0, 0, 0x64);
        }
        Func_80b8000(cp->f24);
    }
    Func_80b8000(cp->f8);
    return 0;
}
