/* Func_80ba978 -- NON-MATCHING, 1 ENCODING OF 273.  Size equal (612 bytes),
 * relocations identical, 273 = 273 encodings.  264 instructions.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_b5000/80ba978.c \
 *     asm/rom_b5000/rom_b9b30_c_a_c_a.s
 * ONE function, no data -- CONVERTS WHOLE OUTRIGHT when it lands, no split, no linker
 * change, no .sym entry.
 *
 * BLOCKER CLASS: CSE reach on a member re-read in a JOIN block.  The single differing
 * insn is at the dominator of the `s->f2` two-arm boolean: the ROM has `mov r2, r4`
 * (`1c22`), we emit `ldrb r2, [r7]` (`783a`).  Both one instruction and two bytes, which
 * is why size and count agree.
 *
 * THE EMPIRICAL RULE, measured across five read sites: A MEMBER READ USED DIRECTLY AS A
 * COMPARE OPERAND IS TURNED INTO A COPY OF THE FIRST LOAD'S REGISTER; THE SAME MEMBER
 * READ ASSIGNED TO A NAMED LOCAL IN A JOIN BLOCK STAYS AN `ldrb`.  The ROM has ONE
 * `ldrb r4,[r7]` plus FOUR register copies.  Consequently:
 *   all five sites as member reads          4 copies in the arms + 1, but 265 insns vs 264
 *   a named local for the two arms          4 insns in the right places, dominator still
 *                                          ldrb, and the extra reference flips `s` from
 *                                          r7 to r6 (23 of 273)
 *   + `register int b1 __asm__("r6")` on    1 OF 273
 *     the `b & 1` local
 *
 * TEN FURTHER SPELLINGS FOR THAT ONE INSN ALL FAILED: unsigned char / unsigned int /
 * short carriers, `*(unsigned char *)s`, `((unsigned char *)s)[0]`, a pinned r2
 * destination, a pinned r4 carrier, an `unsigned char z` + `int zi` pair (the
 * "extension simplified to a copy" theory -- measured 269 insns, REFUTED), and assigning
 * the carrier at four different program points (263 where it coalesces, 264 with the
 * ldrb).  FLAG SWEEP: -fno-gcse, -fno-rerun-cse-after-loop, -fno-schedule-insns2,
 * -fno-strength-reduce, -fno-strict-aliasing, -ffixed-r7 and -O1 -- THE DEFAULT IS BEST
 * AT EVERY POINT (-fno-gcse 113 hunk-lines, -fno-rerun-cse-after-loop 165).  No flag row.
 *
 * TWO SECONDARY FINDINGS THAT CLOSED OFF LINES OF ATTACK:
 *
 * gcc-2.96's CODE HOISTING ONLY RUNS UNDER `optimize_size` -- gcse.c:755,
 * `if (optimize_size) ... one_code_hoisting_pass()`.  So the ROM's shared copy in a
 * dominator block CANNOT be explained by hoisting at -O2, which rules out that whole
 * approach rather than leaving it as an untried idea.
 *
 * A `goto` OUT OF EACH ARM SUPPRESSES CROSS-JUMPING, AND IS THE REASON THE BOOLEAN
 * REPRODUCES AT ALL.  `if (s->f2 <= 7) k = s->f0 <= 7; else k = s->f0 > 7; if (k) {...}`
 * lets jump.c cross-jump the shared `mov #1 / cmp #0 / branch` tail (262 vs 264).
 * Writing `if (!k) goto done;` inside EACH arm makes the two tails differ in branch
 * POLARITY, cross_jump gives up, and the ROM's duplicated `cmp r3,#0` appears.
 *
 * AND THE `base` IF/ELSE WANTS A TERNARY *INSIDE THE EXPRESSION*:
 * `v += ((s->f0 <= 7 ? 0x2000 : -0x2000) - v) * 3 / 4`.  The statement form gets
 * if-converted to preset-plus-overwrite; the COND_EXPR keeps the ROM's `b` over the else
 * arm.  Worth 57 -> 28 hunk-lines.
 *
 * NEXT: one instruction, and the rule above says what it is.  It wants a spelling that
 * makes the dominator read a COMPARE OPERAND rather than a named value -- or a reading of
 * why the ROM's fifth reference is a copy when a join-block assignment is not.
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
    register int b1 __asm__("r6");
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
        zi = s->f0;
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
