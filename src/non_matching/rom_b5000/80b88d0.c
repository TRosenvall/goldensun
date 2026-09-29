/* Func_80b88d0 -- RunActionPhase, 0x080b88d0, 268 ROM instructions.
 * NON-MATCHING, 228 of 280 encodings differ.
 * Size 636 against the ROM's 632 (+4) and 282 encodings against 280 (+2), so
 * 228 is NOT a true distance.  The alignment-tolerant view (tools/aligncmp.py)
 * reads 207 aligned-equal of 280, 84 differing in 48 hunks -- and the FIRST 29
 * ENCODINGS ARE IDENTICAL, prologue and both early guard returns included.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_b5000/80b88d0.c \
 *     asm/rom_b5000/rom_b8228_c_a_c_a_c_c_a.s --func Func_80b88d0
 *
 * THE SPLIT: NONE NEEDED.  asm/rom_b5000/rom_b8228_c_a_c_a_c_c_a.s holds this
 * function ALONE and has no data section (datacheck clean), so landing it is a
 * whole-file conversion -- use `objcmp --whole` before committing.
 *
 * SHIMS: ZERO.  No register pin, no `__asm__(".equ ...")`, no per-file flag.
 *
 * STRUCTURE, all reproduced: the two `Func_80b8808(...) < 0` guards returning
 * -1; the fade write through `*(struct Fade **)&iwram_3001f00`; the three
 * `Func_80c0f98(i + base, flag)` loops; both 16-step REG_BLDALPHA fades; the
 * two target-collection arms; the `buf[n] = 0xff` terminator; the struct Ctx
 * fill and `_Anim_Summon(&c)`; the frame 0x80 = 4 spill words + struct Ctx at
 * 0x10 + `unsigned short buf[14]` at 0x64, with buf DECLARED BEFORE c (the
 * reversal rule, same as the sibling park 80b9ec0.c).
 *
 * THREE TELLS THAT ARE WORTH KEEPING, each measured:
 *  - THE THREE COMPARISONS AGAINST b ARE NOT THE SAME COMPARISON.  The ROM has
 *    `cmp r2,#7 / bhi` (UNSIGNED) for the mode/base choice, `cmp r2,#0x7f / ble`
 *    (SIGNED) for the collection arms, and `cmp r2,#7 / bhi` again for
 *    `c.f4`.  So `b` is `unsigned int` and the middle test alone is written
 *    `(int)b > 0x7f`.  Reading all three as 0x7f cost 5 aligned instructions.
 *  - OPERAND ORDER: the ROM has `adds r3, r5, r0` -- the LOOP COUNTER is rn and
 *    `base` is rm -- so the source is `i + base`, not `base + i`.  Three sites,
 *    worth 5 aligned instructions.
 *  - THE TWO `Func_80b6b40` ARMS EACH HAVE THEIR OWN `m`.  The ROM puts arm 1's
 *    count in r8 and arm 2's in r7; one shared local puts both in r8.
 *
 * ============================================================
 * THE BLOCKER: ONE EXTRA `mov r1, sl` IN THE SECOND `Func_80b6b40` ARM, and the
 * reload-register cascade behind it.  Pass: reload (scratch selection).
 *
 *   rom arm 1   add r3,sp,#0x64 / mov sl,r3 / mov r0,#2 / mov r1,sl / bl   (5)
 *   rom arm 2   add r1,sp,#0x64 / mov r0,#1 / mov sl,r1 /            bl   (4)
 *   ours arm 2  add r4,sp,#0x64 / mov sl,r4 / mov r0,#1 / mov r1,sl / bl   (5)
 *
 * `buf`'s address is one long-lived pseudo in sl (r10), and `add rd, sp, #imm`
 * needs a LO register, so reload must pick a scratch and then copy.  In arm 2
 * the ROM's reload picked r1 -- which IS the argument register -- so the
 * separate `mov r1, sl` disappeared; in arm 1 it picked r3 and the copy stayed.
 * Ours picks r4 in arm 2 and pays for it.  Those 2 extra bytes then force a
 * 2-byte `.align 2, 0` pad before the first interior pool, which is the other
 * +2 bytes and the +2 encodings: EVERY DIFFERENCE FROM INDEX ~119 ON IS THE
 * RESULTING 2-INDEX / 4-BYTE SHIFT, not a wrong instruction.  This is the
 * recorded "reload's spill-register round robin is a PHASE, not a function of
 * the source" class (see src/non_matching/rom_a1000/80ab314.c).
 *
 * MEASURED (aligned-equal of 280 / encodings):
 *   this file, `base = 0;` before the arm-2 call .......... 207 / 282
 *   `base = 0;` after it (natural order) ................. 204 / 282
 *   `Func_80b6b40(1, &buf[0])` in arm 2 .................. 204 / 282
 *   a named `unsigned short *lp = buf;` used everywhere ... 199 / 280  (the count
 *     and size match, but only by losing two encodings elsewhere -- the extra
 *     `mov r1, sl` and the pad are BOTH still there, so 280 is coincidence)
 *
 * The residues left after the shift are all the same class: which LO register
 * reload picks to rebuild a spilled value (r0/r1/r2/r4 permutations at the
 * `ldrsh` index, at `b`'s reload, and at the `movs rN,#1` that increments r9).
 *
 * NEXT, if reopened: find a source form that removes ONE earlier reload in the
 * first arm so arm 2's scratch lands on r1.  Nothing in the head looks like it
 * has room -- the first 29 encodings already match exactly.
 */
#include "gba/io.h"

struct In {
    short s0;
    unsigned char pad2[6];
    short s8;
    short sa;
};

struct Ctx {
    int f0;
    int f4;
    int f8;
    unsigned char pad0c[0x14 - 0xc];
    int f14;
    unsigned char pad18[0x24 - 0x18];
    unsigned short f24[8];
    unsigned char f34[8][4];
};

struct Fade {
    int cur;
    int step;
};

struct A {
    unsigned char pad00[4];
};

struct BA {
    struct A *f0;
};

struct Unit {
    unsigned char pad00[0x38];
    short f38;
};

extern unsigned int iwram_3001f00;
extern void WaitFrames(int n);
extern int Func_80b8808(int id);
extern int Random(void);
extern struct BA *GetBattleActor(int id);
extern int Func_80b6b40(int mode, unsigned short *list);
extern void _Actor_SetAnim(struct A *a, int anim);
extern void Func_80c0f98(int id, int flag);
extern void _Func_801f200(int a);
extern struct Unit *_GetUnit(int id);
extern void CreateBattleSpriteOverlays(unsigned short *buf, int b);
extern void _Anim_Summon(struct Ctx *c);
extern void Func_80b6c90(void);
extern void Func_80c0cec(int a, int b, int c, int d);

int Func_80b88d0(struct In *in)
{
    unsigned short buf[14];
    struct Ctx c;
    struct Fade *f;
    struct A *act;
    int a;
    unsigned int b;
    int base;
    int cnt;
    int m1;
    int m2;
    int n;
    int i;
    int id;
    int v;

    n = 0;
    a = in->s0;
    if (Func_80b8808(a) < 0)
        return -1;
    b = in->sa;
    if (Func_80b8808(b) < 0)
        return -1;
    f = *(struct Fade **)&iwram_3001f00;
    v = 0xa0 << 7;
    if (in->s0 <= 4)
        v = 0x80 << 6;
    f->cur = v;
    f->step = 0x3c;
    WaitFrames(0xa);
    Random();
    act = GetBattleActor(a)->f0;
    if (b <= 7) {
        cnt = Func_80b6b40(2, buf);
        base = 0x80;
    } else {
        base = 0;
        cnt = Func_80b6b40(1, buf);
    }
    for (i = 0; i != cnt; i++)
        if (i + base == a)
            _Actor_SetAnim(act, 3);
    WaitFrames(0x1e);
    REG_BLDCNT = 0x3f40;
    for (i = 0; i != cnt; i++)
        Func_80c0f98(i + base, 1);
    for (i = 0; i != 0x10; i++) {
        REG_BLDALPHA = (0x10 - i) | 0x1000;
        WaitFrames(1);
    }
    _Func_801f200(9);
    if ((int)b > 0x7f) {
        m1 = Func_80b6b40(2, buf);
        for (i = 0; n != m1; i++) {
            id = i + 0x80;
            if (_GetUnit(id)->f38 > 0) {
                buf[n] = id;
                n++;
            }
        }
    } else {
        m2 = Func_80b6b40(1, buf);
        for (i = 0; n != m2; i++) {
            if (_GetUnit(i)->f38 > 0) {
                buf[n] = i;
                n++;
            }
        }
    }
    buf[n] = 0xff;
    CreateBattleSpriteOverlays(buf, 0);
    c.f0 = in->s8;
    c.f8 = a;
    for (i = 0; i != n; i++)
        c.f24[i] = buf[i];
    c.f14 = n;
    c.f4 = b <= 7;
    _Anim_Summon(&c);
    WaitFrames(0xa);
    Func_80b6c90();
    REG_BLDCNT = 0x3f40;
    for (i = 0; i != cnt; i++)
        Func_80c0f98(i + base, 1);
    for (i = 0; i != 0x10; i++) {
        REG_BLDALPHA = i | 0x1000;
        WaitFrames(1);
    }
    for (i = 0; i != cnt; i++)
        Func_80c0f98(i + base, 0);
    Func_80c0cec(0, 0, 0, 0x64);
    WaitFrames(3);
    return 0;
}
