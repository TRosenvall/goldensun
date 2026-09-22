/* WHOLE-FILE CONVERSION of asm/rom_b5000/rom_b9b30_c_a_a_c.s -- Func_80ba6ac is
 * its only function, so no split and no linker change.  264 instructions, 620
 * bytes, 275 encodings and 27 relocations identical.  Verified stable across five
 * repeat compiles.
 *
 * Path: 231 -> 220 -> 179 -> 168 -> 18 -> 8 -> 0 of 275.  Four levers, three of
 * which are new or sharpen an existing entry.
 *
 * A NAMED MULTIPLIER LOCAL SUPPRESSES loop.c's giv STRENGTH REDUCTION, AND THIS IS
 * THE SOURCE CURE FOR A CLASS TWO OTHER BATCH-281 AGENTS COULD ONLY REACH WITH A
 * FLAG.  The ROM recomputes `lsls r1, r4, #4` every iteration where gcc built a
 * walking pointer.  Writing `off = n * 16;` as a named local makes the pseudo
 * REG_USERVAR_P, which flips strength_reduce's
 * `v->lifetime * threshold * benefit < insn_count` test (loop.c ~4500).  Worth
 * 231 -> 179.
 *
 * AND -fno-strength-reduce IS STRICTLY WORSE HERE: 34 -> 59 differing regions,
 * because it also kills the reduction in the loops that ALREADY MATCH.  That is
 * the argument against a Makefile row for this class anywhere -- the flag is
 * whole-function and the defect is per-loop.  Compare
 * src/non_matching/rom_c9000/cfef4_ShiningStar.c and
 * src/non_matching/rom_c9000/cf2a0_Revive.c, both of which hit this class and
 * measured only the flag.
 *
 * AN INTERMEDIATE VALUE SPELLED AS A VARIABLE IS NOT FREE -- three instances in
 * one function.  `cnt = spr->f27 - 1` forced the bound into r4; inlining it as
 * `for (j = 0; j != spr->f27 - 1; j++)` put it in ip via the ROM's `mov ip, r3`
 * and was worth 202 -> 18.  Splitting a call result out of a scalar's variable
 * took 179 -> 168 and made indices 0-47 exact.  And `off += 0x2f4` instead of a
 * third named offset coalesced the last three-register cycle, 8 -> 0.  This is the
 * counterweight to the naming levers: read whether the ROM SPENDS a register on
 * the value before giving it a name.
 *
 * THE HImode CONSTANT STORE / EARLY POOL DUMP LEVER, confirmed from the other
 * side.  `tbl[].f8 = 0xffff` is *thumb_movhi_insn alt 1 at pool_range 64, and GAS
 * renders its `ldrh` as a 2-byte pc-relative `ldr` so it LOOKS SImode in the
 * disassembly.  Routing it through an `int` shared by both arms of the if/else
 * (with `continue` for the no-store path) took TWO INTERIOR POOLS -> ONE END POOL,
 * byte-identical to the reference's 11 words at identical addresses, in one edit.
 *
 * Two existing levers confirmed: a separate pointer local per global-access site
 * (two sites, declaration order load-bearing, 18 -> 8); and the tree's own
 * assignment-as-expression idiom from src/rom_b5000/rom_bffb8_a_c_a_a_b.c --
 * `Upload(pal, (void *)0x50000c0, *(int *)(g + 0x644) = (0x80 << 9) - n * 1092,
 * 0x80)` -- which reproduced a whole 20-instruction palette loop byte-exact on the
 * first compile.
 *
 * The stem builds under the default rule; no per-file Makefile flag row applies.
 */
struct Src {
    unsigned char f0;
    unsigned char pad01[0x2 - 0x1];
    unsigned char f2;
    unsigned char pad03[0x58 - 0x3];
    int f58;
};

struct Ctx {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    unsigned char pad18[0x24 - 0x18];
    short f24[8];
    unsigned char f34[8][4];
};

struct A {
    int dummy;
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

struct Unit {
    unsigned char pad00[0xd8];
    unsigned short items[32];
};

struct Tgt {
    short f0;
    unsigned char pad02[0x8 - 0x2];
    short f8;
};

struct Info {
    unsigned char pad00[0xc];
    unsigned char fc;
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
extern void UploadBGPalette(void *src, void *dst, int amount, int count);
extern void WaitFrames(int n);
extern void _Anim_Attack(struct Ctx *c);
extern void _Anim_Func(struct Ctx *c);
extern void Func_80be02c(void);
extern void Func_80b8000(int a);
extern struct Unit *_GetUnit(int id);
extern struct Info *_GetItemInfo(int item);
extern int _Func_80788c4(int a, int b);
extern int _RPGRandom(void);
extern void Func_80bbabc(int a, int b);
extern void _BreakItem(int a, int b);
extern void Func_80bb938(void);

int Func_80ba6ac(struct Src *s, int b, struct Tgt *p)
{
    struct Ctx c;
    struct A *a;
    struct Spr *spr;
    struct Unit *u;
    struct Info *info;
    int *q;
    int t;
    int r;
    int i;
    int j;
    unsigned int n;
    int slot;
    int off;
    int o0;
    int o1;
    int sv;
    int nv;
    int item;
    unsigned char *iwA;
    unsigned char *iwB;

    q = iwram_3001f00;
    if (s->f0 > 4)
        t = -0x2000;
    else
        t = 0x2000;
    if (*q != t)
        *q = t;
    InitAnimContext(s, &c);
    Func_80c10e8(0, 0);
    a = GetBattleActor(c.f8)->f0;
    _Actor_SetAnim(a, 3);
    _Actor_SetAnimSpeed(a, 0x10);
    if (s->f2 <= 7)
        c.f4 = 1;
    else
        c.f4 = 0;
    for (i = 0; i != c.f14; i++) {
        spr = Func_80b7f70(GetBattleActor(c.f24[i])->f0, 0);
        for (j = 0; j != spr->f27 - 1; j++)
            c.f34[i][j] = spr->parts[j]->f5;
    }
    StartTask(Func_80bd898, 0xc8 << 4);
    if (c.f0 != 0) {
        for (i = 0; i <= 0x13; i++) {
            iwA = iwram_3001e74;
            if (i <= 0x13) {
                UploadBGPalette(iwA + 0x544, (void *)0x50000c0,
                                *(int *)(iwA + 0x644) = (0x80 << 9) - i * 1092, 0x80);
            }
            WaitFrames(1);
        }
        if ((s->f58 & (0x80 << 7)) != 0)
            _Anim_Attack(&c);
        else
            _Anim_Func(&c);
    } else {
        WaitFrames(0x3c);
    }
    Func_80be02c();
    _Actor_SetAnim(a, 1);
    for (i = 0; i != c.f14; i++)
        Func_80b8000(c.f24[i]);
    u = _GetUnit(p->f0);
    item = u->items[p->f8];
    info = _GetItemInfo(item);
    if (info->fc == 1) {
        r = _Func_80788c4(p->f0, p->f8);
        slot = p->f8;
        if (r == 2) {
            iwB = iwram_3001e74;
            for (n = 0; n <= 0x13; n++) {
                off = n * 16;
                o0 = off + 0x2f0;
                if (*(short *)(iwB + o0 + 2) != 2)
                    continue;
                o1 = off + 0x2ec;
                if (*(short *)(iwB + o1) != p->f0)
                    continue;
                off += 0x2f4;
                sv = *(short *)(iwB + off);
                nv = *(unsigned short *)(iwB + off);
                if (sv == slot)
                    nv = 0xffff;
                else if (sv > slot)
                    nv = nv - 1;
                else
                    continue;
                *(short *)(iwB + off) = nv;
            }
        }
    } else if (info->fc == 2) {
        if ((_RPGRandom() & 7) == 0) {
            Func_80bbabc(2, u->items[p->f8]);
            Func_80bbabc(4, 0x81c);
            _BreakItem(p->f0, p->f8);
            Func_80bb938();
        }
    } else if (info->fc == 4) {
        if ((item & 0x1ff) == 0xb8)
            item = 0xb9;
        u->items[p->f8] = item;
    }
    return 0;
}
