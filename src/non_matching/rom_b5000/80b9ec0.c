/* Func_80b9ec0 -- NON-MATCHING, 379 encodings of 430, size 936 against the ROM's 956 (-20).
 *
 * READ THIS FIRST: THIS IS AN EARLY-STAGE TRANSCRIPTION, NOT AN ANALYSED PARK.
 * No blocker class has been isolated. It is committed because the structural
 * reading is worth more than re-doing it from scratch, and because an honest
 * record of how far a target actually got is better than silence about it.
 * Do not read the difference count as a near miss and do not quote it as a
 * measured plateau -- nothing here has been driven to a plateau.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_b5000/80b9ec0.c \
 *     asm/rom_b5000/rom_b9b30_a_c.s --func Func_80b9ec0
 * The reference holds TWO functions, so a split is required to land this one.
 *
 * HOW IT GOT HERE, AND WHY IT STOPPED. Batch 281 assigned this to an agent that
 * landed its first target byte-exact (Func_80ba6ac, src/rom_b5000/rom_b9b30_c_a_a_c.c)
 * and then delegated the remaining four to parallel workers. Those workers
 * produced partial candidates and the round ended before any of them converged.
 * Two of the four never got past a skeleton and are NOT parked at all --
 * Func_80a96d8 and Func_80a90bc reached 36 of 319 and 32 of 303 encodings
 * respectively, i.e. they are unfinished transcriptions rather than attempts, and
 * are recorded as UNATTEMPTED in reports/batch-281.md rather than dressed up as
 * parks.
 *
 * WHAT IS ALREADY KNOWN ABOUT THIS BANK, and it is a lot -- start here, not from
 * the disassembly:
 *
 * src/rom_b5000/rom_b9b30_c_a_a_c.c (landed in the same round) carries four
 * levers, three of them new, and its file-neighbours are this function's
 * neighbours:
 *
 *   - A NAMED MULTIPLIER LOCAL SUPPRESSES loop.c's giv STRENGTH REDUCTION.
 *     `off = n * 16;` as a named local makes the pseudo REG_USERVAR_P, flipping
 *     strength_reduce's `v->lifetime * threshold * benefit < insn_count` test
 *     (loop.c ~4500). Worth 231 -> 179 there. AND -fno-strength-reduce is
 *     STRICTLY WORSE (34 -> 59) because it also kills the reduction in loops that
 *     already match -- so this class has a source cure and wants no flag row.
 *   - AN INTERMEDIATE VALUE SPELLED AS A VARIABLE IS NOT FREE. Inlining a bound
 *     instead of naming it was worth 202 -> 18 there, because the name forced the
 *     value into r4 where the ROM wanted ip. Read whether the ROM SPENDS a
 *     register on a value before naming it. This is the counterweight to every
 *     naming lever in docs/elevation.md and it is easy to get backwards.
 *   - A HImode CONSTANT STORE FORCES AN EARLY POOL DUMP, and it HIDES: GAS renders
 *     *thumb_movhi_insn alt 1's `ldrh` as a 2-byte pc-relative `ldr`, so it looks
 *     SImode in the disassembly. Route it through an `int` shared by both arms of
 *     an if/else, with `continue` for the no-store path so combine cannot fold the
 *     constant back into the store.
 *   - The tree's assignment-as-expression idiom from
 *     src/rom_b5000/rom_bffb8_a_c_a_a_b.c reproduced a 20-instruction palette loop
 *     byte-exact on the FIRST compile.
 *
 * ALSO CHECK src/non_matching/rom_b5000/80ba2c0.c BEFORE SPENDING A BUDGET HERE.
 * It defines the REDUNDANT-COPY PRESSURE class -- the ROM being LESS optimal than
 * gcc, carrying a redundant pointer copy that costs it a high register and forces
 * a parameter to spill. Nine spellings were measured and gcc coalesces or reverts
 * every one. If you see a ROM spill gcc does not make, check for that shape before
 * treating it as a spelling problem.
 *
 * NEXT: re-derive from the disassembly with the four levers above applied from the
 * first candidate rather than discovered. Given the size (+4 and -20 bytes
 * respectively across these two parks), start by settling the POOL and the FRAME,
 * which are the two hard binary signals, before reading any register.
 */
#include "gba/io.h"

struct Src {
    unsigned char f0;
    signed char f1;
    unsigned char f2[0x50 - 0x2];
    int f50;
    unsigned char pad54[0x58 - 0x54];
    int f58;
    int f5c;
};

struct Ctx {
    unsigned char pad00[4];
    int f4;
    int f8;
    unsigned char pad0c[0x14 - 0xc];
    int f14;
    unsigned char pad18[0x24 - 0x18];
    short f24[8];
    unsigned char f34[8][4];
};

struct A {
    unsigned char pad00[4];
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
    struct Part *parts[8];
};

extern unsigned char iwram_3001e74[];

extern int InitAnimContext(struct Src *s, struct Ctx *c);
extern void Func_80c10e8(int a, int b);
extern void _Func_801f200(int a);
extern struct BA *GetBattleActor(int id);
extern int Func_80b6c08(int kind, unsigned short *buf);
extern void _Actor_SetAnim(struct A *a, int anim);
extern void Func_80c0f98(int id, int flag);
extern void _PlaySound(int id);
extern void Anim_MoveIntro(int a, int b, int c, int d);
extern void WaitFrames(int n);
extern void Func_80bbabc(int a, int b);
extern void Func_80bb938(void);
extern void Func_80c1a14(void);
extern void CreateBattleSpriteOverlays(unsigned short *buf, int b);
extern struct Spr *Func_80b7f70(struct A *a, int i);
extern void StartTask(void (*task)(void), int priority);
extern void Func_80bd898(void);
extern void _Anim_Summon(struct Ctx *c);
extern void _Anim_Attack(struct Ctx *c);
extern void _Anim_Func(struct Ctx *c);
extern void Func_80be02c(void);
extern void Func_80b6c90(void);
extern void Func_80c0cec(int a, int b, int c, int d);

int Func_80b9ec0(struct Src *s, int flags)
{
    unsigned short buf[14];
    struct Ctx c;
    struct Ctx *cp;
    struct A *actor;
    struct Spr *sp;
    int *p;
    unsigned char *g;
    int v;
    unsigned int n;
    unsigned int i;
    unsigned int x;
    unsigned int a;
    unsigned int b;
    int t1;
    int t2;
    int k;
    int m;
    int j;
    int np;

    cp = &c;
    g = iwram_3001e74;
    InitAnimContext(s, cp);
    a = s->f0;
    b = s->f2[0];
    if ((s->f58 & (0x80 << 8)) != 0) {
        p = *(int **)(g + 0x8c);
        v = 0x80 << 6;
        if (a > 7)
            v = 0xa0 << 7;
        p[0] = v;
        p[1] = 0x3c;
    } else {
        p = *(int **)(g + 0x8c);
        v = -(0x80 << 6);
        if (a <= 7)
            v = 0x80 << 6;
        if (*p != v)
            *p = v;
    }
    Func_80c10e8(0, 0);
    _Func_801f200((*(unsigned char **)iwram_3001e74)[0x41] & -2);
    actor = GetBattleActor(a)->f0;
    REG_BLDCNT = 0x3f40;
    n = Func_80b6c08(3, buf);
    for (i = 0; i != n; i++) {
        x = buf[i];
        if (x == 0xfe)
            continue;
        if (x != a) {
            t1 = 0;
            if (b <= 7)
                t1 = 1;
            t2 = 0;
            if (x <= 7)
                t2 = 1;
            if (t1 != t2)
                Func_80c0f98(x, 1);
        } else {
            _Actor_SetAnim(actor, 3);
        }
    }
    _PlaySound(0x9a);
    Anim_MoveIntro(cp->f8, s->f50, 0, 0);
    if ((flags & 1) != 0)
        Func_80c0f98(a, 1);
    for (i = 0; i != 0x10; i++) {
        REG_BLDALPHA = (0x10 - i) | 0x1000;
        WaitFrames(1);
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
        k = 0;
        for (i = 0; i < n; i++) {
            x = buf[i];
            if (x == a) {
                if ((flags & 1) == 0)
                    buf[k++] = a;
            } else {
                t1 = 0;
                if (b > 7)
                    t1 = 1;
                t2 = 0;
                if (x <= 7)
                    t2 = 1;
                if (t1 != t2)
                    buf[k++] = x;
            }
        }
        buf[k] = 0xff;
        CreateBattleSpriteOverlays(buf, 0);
        m = s->f1;
        for (j = 0; j < m; j++)
            buf[j] = s->f2[j];
        buf[j] = 0xff;
        for (i = 0; i != cp->f14; i++) {
            sp = Func_80b7f70(GetBattleActor(cp->f24[i])->f0, 0);
            np = sp->f27 - 1;
            for (j = 0; j != np; j++)
                cp->f34[i][j] = sp->parts[j]->f5;
        }
        if ((s->f58 & (0x80 << 8)) != 0) {
            if (b <= 7)
                cp->f4 = 1;
            else
                cp->f4 = 0;
        } else {
            if (s->f2[0] <= 7)
                cp->f4 = 1;
            else
                cp->f4 = 0;
        }
        if ((s->f58 & (0x80 << 10)) != 0)
            cp->f4 ^= 1;
        StartTask(Func_80bd898, 0xc8 << 4);
        if ((s->f58 & (0x80 << 8)) != 0)
            _Anim_Summon(cp);
        else if ((s->f58 & (0x80 << 7)) != 0)
            _Anim_Attack(cp);
        else
            _Anim_Func(cp);
        Func_80be02c();
    }
    Func_80b6c90();
    n = Func_80b6c08(3, buf);
    REG_BLDCNT = 0x3f40;
    for (i = 0; i != n; i++) {
        x = buf[i];
        if (x == 0xfe)
            continue;
        if (x == a)
            continue;
        t1 = 0;
        if (b <= 7)
            t1 = 1;
        t2 = 0;
        if (x <= 7)
            t2 = 1;
        if (t1 != t2)
            Func_80c0f98(x, 1);
    }
    for (i = 0; i != 0x10; i++) {
        REG_BLDALPHA = i | 0x1000;
        WaitFrames(1);
    }
    for (i = 0; i != n; i++)
        Func_80c0f98(buf[i], 0);
    Func_80c0cec(0, 0, 0, 0x64);
    WaitFrames(1);
    return 0;
}
