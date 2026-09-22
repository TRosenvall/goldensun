/* Func_80b9ec0 -- NON-MATCHING, 317 encodings of 430, size 952 against the ROM's
 * 956 (-4).  Relocations: 34 in ref, 34 in ours, IDENTICAL SYMBOL SETS AND ORDER;
 * only the offsets shift because the bodies differ.  The prologue matches including
 * `sub sp, #124`, and the first difference is at index 8.
 *
 * Blocker class: A PRIORITY INVERSION BETWEEN TWO *ADJACENT* ALLOCNOS in
 * global.c's sort, worth ONE loop-depth-weighted reference.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_b5000/80b9ec0.c \
 *     asm/rom_b5000/rom_b9b30_a_c.s --func Func_80b9ec0
 * The reference holds TWO functions (Func_80b9dc4, Func_80b9ec0), so a split is
 * required to land this one.
 *
 * (This park was first written at 379 of 430 as an EARLY transcription with no
 * blocker isolated.  It has been replaced: the candidate is better and the blocker
 * is now located precisely.)
 *
 * ================================================================
 * THE BLOCKER, AND IT IS ONE POSITION IN A SORT
 * ================================================================
 *
 * The ROM puts `cp` (the struct Ctx *) in r7 and `n` (the Func_80b6c08 result) in
 * r8; gcc puts `n` in r7 and `cp` in r8.  The rest of the file then rotates (`buf`
 * r9 against the ROM's r10, `s` r10 against r9).
 *
 * WHY IT CASCADES SO FAR: `cp`'s def is `add rd, sp, #12`, WHICH REQUIRES LO_REGS
 * IN THUMB.  So with `cp` in r8, every one of ~18 `cp` references picks up a
 * `mov rX, r8` reload copy, and `ldr r7,[pc]` in the fade-in loop becomes
 * `ldr r3,[pc] / mov r8,r3 / mov r1,r8`.  That is the bulk of the 317.
 *
 * IT IS EXACTLY ONE POSITION.  From .18.greg the allocation order is
 *   ... 51 40 34 201 96 32 83 175 72 80 174 79 38 43 35 44 33
 * where pseudo 40 = `n` is 25th and takes r7, and pseudo 34 = `cp` is 26th and is
 * pushed to r8.  THEY ARE ADJACENT.  Priority is
 * floor_log2(n_refs)*n_refs/live_length*10000*size (global.c:597 allocno_compare)
 * with REG_N_REFS incremented by bb->loop_depth + 1 per reference
 * (flow.c:4435, 4948, 5115, 5556), so A SINGLE IN-LOOP REFERENCE EITHER WAY FLIPS
 * IT.
 *
 * AND IT IS A STRICT DIFFERENCE, NOT A TIE -- which is a useful negative.
 * allocno_compare tie-breaks on allocno NUMBER and `cp` is the LOWER number, so a
 * tie would already have gone our way.  That is why nine declaration-order and type
 * perturbations moved it NOT AT ALL.
 *
 * ================================================================
 * THE WORKING LEVER WAS THE OPPOSITE OF THE OBVIOUS ONE
 * ================================================================
 *
 * A single named pointer local for `buf` used at EVERY site (candidate c5) is the
 * WRONG lever -- it gets spilled in the prologue (376, and size 956 for the wrong
 * reason).  What worked was an EXPLICIT STEPPING BYTE-OFFSET LOCAL PER INDEXED LOOP
 * (`o = 0; ... o += 2;` in the body), which raises the frame-address pseudo's
 * loop-depth-weighted REG_N_REFS enough to win a hard register: 379 -> 320, AND it
 * fixed the frame to `sub sp, #124` so `buf` finally gets a hard reg at all.
 *
 * Placement inside that lever matters: `o += 2` in the BODY is 320; moving it into
 * the `for` increment clause is 334 in loop 1 and a tie in loop 3.
 *
 * REG_USERVAR_P / THE NAMED-MULTIPLIER LEVER IS INAPPLICABLE TO DEST_ADDR GIVS.
 * loop.c:4502 gates it on `v->dest_reg`, so it only fires for DEST_REG givs.  The
 * cp->f24[] loop's reduction is a DEST_ADDR giv and three spellings of the address
 * all failed to stop it.  This is a real limit on the lever recorded for
 * src/rom_b5000/rom_b9b30_c_a_a_c.c in the same batch -- that one WAS a DEST_REG giv.
 *
 * ================================================================
 * THE -4 BYTES IS ONE INDEPENDENT DEFECT: A CROSS-JUMP WE CANNOT SUPPRESS
 * ================================================================
 *
 * At 0x080ba064 (loop 2's `x == a` arm) the ROM emits
 * `add r0,sp,#4 / ldrh r0,[r0] / add r2,#1 / strh r0,[r1] / b`; ours cross-jumps
 * the whole tail away and emits only the `b`.  gcc's record_jump_equiv on `cmp x,a`
 * lets CSE replace (subreg:HI a) with x, and jump.c's cross-jumping then merges the
 * two store tails.  THAT IS THE ENTIRE -4 (4 instructions lost here, partly offset
 * by 2 extra we emit in the fade-in loop).  Three spellings measured and all tie at
 * 317: `buf[k] = a; k++;` split, `buf[k++] = (unsigned short)a;`, and `int i`.
 *
 * ================================================================
 * STRUCTURAL FINDINGS THAT REPRODUCE BYTE-EXACT -- reuse these
 * ================================================================
 *
 * FRAME: `sub sp,#0x7c` is 3 spill words (0=b, 4=a, 8=flags) + struct Ctx at 0xc
 * (0x54) + unsigned short buf[14] at 0x60 (0x1c).  DECLARATION ORDER IS REVERSED IN
 * MEMORY: declaring `buf` BEFORE `c` puts `c` at the low address, matching the ROM.
 *
 * iwram_3001e74 NEEDS TWO DIFFERENT SPELLINGS IN THE SAME FUNCTION, and that is
 * what produces the ROM's two separate pool words.  `extern unsigned char
 * iwram_3001e74[];` with a named local `g = iwram_3001e74;` then
 * `*(int **)(g + 0x8c)` gives `ldr r5,=iwram_3001e74 / adds r3,r5,#0 / adds r3,#140`
 * (base force_reg'd and CSE'd across both if-arms, byte-exact), while
 * `(*(unsigned char **)iwram_3001e74)[0x41]` is a bare SYMBOL_REF address that
 * reload does NOT CSE with `g`, so it reloads the pool word -- also byte-exact.
 * Same mechanism as src/non_matching/rom_b5000/80c0130.c.
 *
 * EVERY APPARENTLY-SILLY SMALL-CONSTANT POOL LOAD HERE IS A HImode STORE CONSTANT
 * RENDERED AS `ldr` BY THE EXTRACTOR: `ldr r3,.Lb9f74 @0x3f40` (REG_BLDCNT),
 * `ldr r3,=0xff` (buf[k] = 0xff).  The explicit-label form appears precisely
 * because *thumb_movhi_insn's 64-byte pool_range forces those words out ahead of the
 * SImode ones.  This candidate reproduces the WHOLE pool layout -- 0x3f40,
 * 0xffffe000, 0x04000050, 0x10, 0x1000, 0x04000052, 0x856, 0x855, 0xff x2, both
 * iwram_3001e74 words, Func_80bd898 -- same words in the same interior pools.
 *
 * `ldr r1,=0x10 / mov r11,r1` IS NOT AN ANOMALY: a small constant destined for a
 * HIGH register cannot use `mov rh,#imm`, so reload routes it through a pool load
 * in a low reg.  And `ldr r0,=REG_BLDALPHA` INSIDE the fade-out loop is a
 * rematerialised spilled constant (reg_equiv_constant) -- the ROM is one high
 * register short there precisely BECAUSE it keeps `buf` in r10.  Both reproduce.
 *
 * ALL COMPARISONS AGAINST 7 IN THIS FUNCTION ARE UNSIGNED (bls/bhi), so a, b and x
 * must be `unsigned int`; `int` gives ble/bgt.  The POLARITY genuinely differs
 * between loops: loops 1 and 3 test (b <= 7) != (x <= 7), loop 2 tests
 * (b > 7) != (x <= 7).
 *
 * `m = s->f1` as `signed char` gives the ROM's `movs r3,#1 / ldrsb r3,[r2,r3]`; the
 * loop is `for (j = 0; j < m; j++)` (gcc reverses it to a countdown and reloads `m`
 * after, matching `mov r0,r12`) and `buf[j] = 0xff` after it uses the exit value.
 *
 * THE cp->f4 BLOCK GENUINELY HAS TWO SPELLINGS OF THE SAME VALUE: the
 * `s->f58 & 0x8000` arm uses the cached local `b`, the else arm RE-READS `s->f2[0]`
 * (`ldrb r3,[r1,#2]`).  Writing both as `b` loses the re-read.
 *
 * Loop-bound tells: loops 1/3/5 are `i != n` with an `n == 0` guard; loop 2 alone
 * is `i < n` with unsigned bcs/bcc.
 *
 * ================================================================
 * MEASURED -- 20 candidates, do not re-run these
 * ================================================================
 *   c1 first reconstruction                      403, 936 (-20)
 *   c2 buf before c; a,b unsigned; named g       379, 936
 *   c3 + o2 = i*2 on the f24 loop                372, 952
 *   c4 + named o with (char*)buf + o             372  (no effect; pointer IV anyway)
 *   c5 one named `unsigned short *bp = buf`      376, 956  (bp SPILLED -- wrong lever)
 *   c6 explicit stepping `o` in loops 1/3        320, 952  (frame fixed)
 *   c7/c9/cD  o += 2 in the for-clause           334 / 334 / 334  worse
 *   cE for-clause, loop 3 only                   320  tie
 *   c8 c6 + separate o2 for f24                  320  (restores the f24 shape)
 *   cA f24 as natural cp->f24[i]                 347, 940  worse
 *   cB f24 with its own offset from 0x24         347, 940  worse
 *   cC split n into n/n2                         346, 944  worse
 *   cF/cG/cH unsigned o; n first; n last         320 each -- DECLARATION ORDER INERT
 *   cI separate t3/t4 for loop 2's side test     317, 952  BEST (this file)
 *   cJ a as unsigned short                       442, 1000  much worse
 *   cK/cL/cN/cO  four cross-jump spellings       317 each  tie
 *   cP separate counter for the two fade loops   323  worse
 *   cQ/cR/cS  three cp-address spellings         317 each  tie
 *   cT lever moved to the inner f34 write        330, 948  worse
 *
 * NO SYMBOL TELLS.  Both 0x3f40 words, both 0xff words, 0x10, 0x1000, 0x855, 0x856
 * and 0x3c all reproduce as plain in-function literals.  Four callee spellings are
 * introduced here that exist nowhere else in src/ -- Func_80b6c90(void),
 * Func_80c1a14(void), _Anim_Summon(struct Ctx *), Anim_MoveIntro(int,int,int,int).
 *
 * NEXT, and it is a sharp question rather than a spelling hunt: WHAT SOURCE
 * CONSTRUCT ADDS ONE LOOP-DEPTH-WEIGHTED REFERENCE TO `cp`, OR REMOVES ONE FROM
 * `n`, WITHOUT ADDING AN INSTRUCTION?  They are adjacent at positions 25/26, so one
 * weighted ref decides it.  Worth checking whether Func_80b6c08's ROM-side result is
 * consumed through a second variable that has been collapsed here.  The loop-2
 * cross-jump is INDEPENDENT and is the whole -4; defeating record_jump_equiv plus
 * cross-jumping there is a separate and probably harder question.
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
    int t3;
    int t4;
    int k;
    int m;
    int j;
    int np;
    int o;
    int o2;

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
    o = 0;
    for (i = 0; i != n; i++) {
        x = *(unsigned short *)((char *)buf + o);
        o += 2;
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
                t3 = 0;
                if (b > 7)
                    t3 = 1;
                t4 = 0;
                if (x <= 7)
                    t4 = 1;
                if (t3 != t4)
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
            o2 = i * 2;
            sp = Func_80b7f70(GetBattleActor(*(short *)((char *)cp + (o2 + 0x24)))->f0, 0);
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
    o = 0;
    for (i = 0; i != n; i++) {
        x = *(unsigned short *)((char *)buf + o);
        o += 2;
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
