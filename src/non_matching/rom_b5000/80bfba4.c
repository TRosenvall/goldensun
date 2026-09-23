/* Func_80bfba4 -- NON-MATCHING, 232 encodings of 454 positionally -- BUT READ THE NEXT
 * PARAGRAPH BEFORE USING THAT NUMBER.  SIZE EXACT (1044 bytes both), INSTRUCTION COUNT
 * EXACT (454 = 454), BOTH POOL CHUNKS REPRODUCE WITH IDENTICAL CONTENTS AND ORDER
 * (13 words), FRAME `sub sp,#32` with all four spill slots at the ROM's offsets, and 60
 * OF 60 RELOCATION SYMBOLS IDENTICAL AND IN IDENTICAL ORDER with only 7 offsets shifting.
 * 444 instructions.
 *
 * **THE 232 IS A MIS-RANK, AND THIS IS THE CLEAREST INSTANCE YET.**  A shift-tolerant
 * diff gives 124, and WITH REGISTER NAMES BLINDED **35 of 454**.  Since size and count
 * already agree, the positional count carries no ordering information (the fourth angle
 * of the measurement rule), and the register-blind number is what correctly ranked all 25
 * candidates.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_b5000/80bfba4.c \
 *     asm/rom_b5000/rom_bbb0c_c_c.s --func Func_80bfba4
 * ONE function plus a .rodata tail of 15 .incrom blocks.  A SPLIT IS NEEDED to move the
 * data to its own .s -- but ALL 15 LABELS ALREADY CARRY `.global` (including .Lc35bc,
 * exported in batch 282), so NO NEW EXPORT IS REQUIRED, unlike its landed file-mate's
 * prerequisite.  That file-mate, src/rom_b5000/rom_bbb0c_c_b.c (Func_80bf678), is
 * byte-exact and is this function's best donor -- its generated .s at lines 249-254 is a
 * direct template for the QImode idiom below.
 *
 * ================================================================
 * BLOCKER CLASS: loop.c's strength_reduce DECLINES ONE giv, WHICH SUPPRESSES
 * maybe_eliminate_biv -- AND THE .08.loop DUMP SAYS SO IN WORDS
 * ================================================================
 *
 * Residue window 1, reference 0x080bfc46-0x080bfc56, the arr[4] zero-init loop.  The
 * ROM's `cmp r3, ip / bge` is a SIGNED compare of two addresses.  Pointer comparison in C
 * is unsigned (bcs/bcc -- measured on two pointer-loop spellings), so a signed pointer
 * compare can only be maybe_eliminate_biv_1 replacing `j >= 0` IN PLACE, keeping the rtx
 * code and swapping in the giv plus emit_iv_add_mult.  The ROM eliminated the biv; we do
 * not.
 *
 * The dump states outright that it COULD be:
 *
 *   Insn 163: giv reg 78 src reg 53 benefit 2 lifetime 2 replaceable ncav mult 4 add 0
 *   biv 53 can be eliminated.
 *   giv of insn 163 not worth while, 0 vs 6.
 *
 * Insn 163 is the bare `j << 2` pseudo that `arr[j]` needs before the reg+reg Thumb
 * address, and it fails `benefit -= add_cost * bl->biv_count` (2 - 2*1 = 0) against
 * `v->lifetime * threshold * benefit < insn_count`.  maybe_eliminate_biv is then gated
 * behind `if (all_reduced == 1 && bl->eliminable ...)`.  SO THE RESIDUE IS ONE DECLINED
 * giv, NOT A SCHEDULING OR ALLOCATION FAULT.
 *
 * ESCAPES RULED OUT BY MEASUREMENT: flag_reduce_all_givs is off by default; `bl->reversed`
 * is set only inside check_dbra_loop, WHICH RUNS AFTER THIS TEST IN THE SAME
 * strength_reduce CALL, so an up-counting loop cannot reach it (measured: 456 insns, 88
 * blind, far worse).  A byte-offset loop kills the separate giv but switches the store to
 * reg+reg addressing and still does not eliminate (454 insns, 64 blind).  NINE zero-loop
 * spellings measured, best 35/454.
 *
 * Residue window 2, reference 0x080bfd52: `fold` REASSOCIATES every parenthesisation of
 * `unit + (0x12c + idx)` into `(unit + idx) + 0x12c`, costing 2 instructions and a zero
 * index register.  `idx += 0x12c;` MAKES THAT WINDOW BYTE-EXACT -- scratch's FINALplain.c
 * scores 29 register-blind at 452 insns / 1040 bytes, i.e. THE ENTIRE -2 THEN SITS IN THE
 * ZERO-INIT LOOP ALONE.  The exact-size spelling is kept here because its objcmp line is
 * meaningful, but **a finisher should start from FINALplain.c: its residue is one window.**
 *
 * ================================================================
 * SYMBOL TELL -- `_MSG_828 = 0x828;` REPORTED, NOT ADDED, WITH THE COMPILER LINE THAT
 * PROVES IT
 * ================================================================
 *
 * The ROM reaches 0x825 and 0x82b as REGISTER ARITHMETIC off a register holding 0x828
 * (`subs r1, r5, #3` / `adds r1, r5, #3`).  No plain-literal spelling can produce that,
 * and the reason is a single line: cse.c:1637 gates the related-value chain on
 *
 *     if (GET_CODE (x) == CONST)
 *
 * where CONST is `(const (plus (symbol_ref) (const_int)))` -- **A BARE CONST_INT NEVER
 * GETS A RELATED-VALUE CHAIN.**  Measured decisively: with `msg = 0x828` gcc emits
 * `ldr r1,=0x825` / `ldr r1,=0x82b`; with the symbol it emits the ROM's exact two
 * instructions.
 *
 * IN-FUNCTION CONTROL: seven other constants here (0x879, 0x84b, 0x851, 0x131, 0x141, and
 * 0x825/0x82b IN BLOCKS 1 AND 2) reproduce as PLAIN LITERALS in the reference's exact pool
 * slots and order.  So the claim is "this one id is reached by register arithmetic and only
 * a symbol can do that", not "this id space wants symbols" -- and 0x825/0x82b themselves
 * MUST stay plain literals, since they are pool words elsewhere in the function.
 *
 * Withheld because it does not complete the function.  But note the
 * use_related_value-only-for-CONST fact is new and general, and belongs in
 * docs/elevation.md independently of this entry.
 *
 * ================================================================
 * NINE LEVERS THAT PAID, each measured in isolation
 * ================================================================
 *
 * `bls` NOT `ble` MEANS THE VALUE IS UNSIGNED -- three sites.
 *
 * THE ROM'S THREE LOOP-COUNTER *HARD REGISTERS* PROVE THREE COUNTER *VARIABLES*.  One
 * pseudo gets one hard register, so r7, r4 and r6 cannot be one C variable.  Splitting
 * `i` into i/i2/i3 was worth 160 -> 150 shift-tolerant and unwound a cascade.  (Note this
 * is the OPPOSITE conclusion from Anim_Break's "a counter reused across several loops is
 * one variable" -- and the discriminator is exactly this: COUNT THE ROM'S HARD REGISTERS.)
 *
 * THE MULTIPLY'S FIRST OPERAND IS THE ONE TIED TO THE DESTINATION.  Thumb `muls Rd, Rm`
 * ties operand 1 to the output; writing the loop-invariant first was worth 204 -> 160
 * shift-tolerant and 91 -> 62 blind, the largest single edit after the structure.
 *
 * `*w = *w + 0xff;` NOT `n = *w; *w = n + 0xff;`.  For a QImode store combine narrows +255
 * to -1 and emits one `subs Rd,Rn,#1`; RE-READING `*w` keeps the ROM's
 * ldrb / mov / cmp / add #255 / strb / lsl #24 and the copy.  The landed file-mate already
 * carries this idiom.
 *
 * A NAMED OFFSET BEATS TWO STRENGTH-REDUCED POINTERS when the ROM indexes two bases by one
 * offset -- dropping it costs 45 blind and +8 insns.
 *
 * A MEMORY-VARIABLE POINTER IS NOT AN `add rN,sp,#imm`.  When `ap` is a genuinely spilled
 * pseudo, reload takes *thumb_addsi3's DISPARAGED `!k` alternative off the table and emits
 * the ROM's `mov r3,sp / adds r3,#16 / str r3,[sp,#0]`.  That fell out of the PRESSURE,
 * not from a spelling.
 *
 * The two `if (flag)` blocks are TWO STATEMENTS, not one -- the ROM re-loads and re-tests
 * flag from sp+4 after `free`.  `pop {r1} / bx r1` means it returns a value.  And
 * `ldr r1,[sp,#4]` for the first loop's entry guard is cse substituting the spilled
 * `flag = 0`, not a shared variable -- it falls out of the file-mate's
 * `d = &rec->l; i = 0; n = rec->l.count;` order, where four permutations measured
 * 150/150/152/152, so THE FILE-MATE'S ORDER IS BEST AND THE OTHERS ARE NOT DISASTERS
 * (refining its "reversing it costs 41").
 *
 * TRIED AND REJECTED: an honest `rec2` for the second _Func_8077330 costs +2 insns and 36
 * blind; a fresh `k` for the byte index costs 280 shift / 92 blind; reusing n, off or i3
 * for it gives 30-33 blind but 452 insns; splitting `d` into d/d2 is inert.
 *
 * No per-file Makefile flag override applies to this stem.
 */
typedef unsigned char u8;
typedef signed char s8;

struct Djinni {
    u8 f0;
    u8 f1;
    u8 f2;
    s8 f3;
};

struct List {
    struct Djinni e[0x40];
    int count;
};

struct Rec {
    u8 pad0[8];
    struct List l;
};

struct Ctx {
    u8 id;
    u8 pad1[0x5f];
    int dmg;
};

typedef void (*CopyFn)(void *dst, void *src, int len);

extern u8 *iwram_3001e74;

extern u8 *_GetUnit(int id);
extern struct Rec *_Func_8077330(int side);
extern int _Func_807a350(int a, int b, int c);
extern int Func_80b6b40(int kind, unsigned short *buf);
extern void *Func_8004970(int size);
extern void Func_8001af8(void *dst, void *src, int len);
extern void free(void *p);
extern void _CalcStats(int id);
extern void Func_80bdfec(void);
extern void Func_80bd808(int a);
extern int Func_80bbabc(int a, int b);
extern void _PlaySound(int a);
extern int *GetBattleActor(int id);
extern void _Actor_SetAnim(int a, int b);
extern void _Actor_SetAnimSpeed(int a, int b);
extern int Anim_MoveIntro(int a, int b, int c, int d);
extern void Func_80be02c(void);
extern int _ModifyHP(int id, int delta);
extern void Func_80bb938(void);

int Func_80bfba4(struct Ctx *ctx)
{
    struct Rec *rec;
    struct List *d;
    struct Djinni *p;
    u8 *unit;
    u8 *g;
    u8 *w;
    void *tmp;
    CopyFn copy;
    int *q;
    int arr[4];
    unsigned int id;
    int flag;
    int i;
    int i2;
    int i3;
    int n;
    int sel;
    int best;
    int max;
    int idx;
    int amt;
    int v;
    int j;
    int off;
    int msg;

    id = ctx->id;
    flag = 0;
    unit = _GetUnit(id);
    rec = _Func_8077330(id > 7);
    d = &rec->l;
    i = 0;
    n = rec->l.count;
    if (i < n) {
        p = d->e;
        do {
            if (p->f2 == id && p->f3 == -1)
                _Func_807a350(id, p->f0, p->f1);
            i++;
            p++;
        } while (i < d->count);
    }
    if (Func_80b6b40(1, 0) != 0 && Func_80b6b40(2, 0) != 0)
        flag = 1;
    rec = _Func_8077330(id > 7);
    d = &rec->l;
    for (j = 3; j >= 0; j--)
        arr[j] = 0;
    for (;;) {
        sel = -1;
        for (i2 = 0; i2 < d->count; i2++) {
            if (d->e[i2].f3 == -2) {
                sel = d->e[i2].f2;
                break;
            }
        }
        if (sel == -1)
            break;
        best = -1;
        for (i2 = 0; i2 < d->count; i2++) {
            if (d->e[i2].f2 == sel && d->e[i2].f3 > best)
                best = d->e[i2].f3;
        }
        best++;
        if (best < 2)
            best = 2;
        i2 = 0;
        if (i2 < d->count) {
            p = d->e;
            do {
                if (p->f2 == sel && p->f3 == -2) {
                    p->f3 = best;
                    arr[p->f0]++;
                    best++;
                }
                i2++;
                p++;
            } while (i2 < d->count);
        }
    }
    if (flag != 0) {
        max = 0;
        tmp = Func_8004970(0x14c);
        copy = Func_8001af8;
        copy(tmp, unit, 0x14c);
        idx = -1;
        q = arr;
        for (i3 = 0; i3 <= 3; i3++) {
            v = *q++;
            if (v > max) {
                max = v;
                idx = i3;
            }
        }
        if (idx >= 0) {
            if (*(s8 *)(unit + (0x12c + idx)) < max)
                *(s8 *)(unit + (0x12c + idx)) = max;
        }
        _CalcStats(id);
        off = 0x48;
        for (i3 = 0; i3 <= 3; i3++) {
            amt = *(short *)(unit + off) - *(short *)((u8 *)tmp + off);
            if (amt > 0) {
                Func_80bdfec();
                Func_80bd808(0x19);
                Func_80bbabc(0, id);
                Func_80bbabc(1, amt);
                Func_80bbabc(0xe, 0xaf);
                Func_80bbabc(4, i3 + 0x879);
                Func_80bbabc(0xb, id);
                _PlaySound(0xd4);
                _Actor_SetAnim(*GetBattleActor(id), 3);
                _Actor_SetAnimSpeed(*GetBattleActor(id), 0x20);
                Anim_MoveIntro(id, i3, 2, max - 1);
                Func_80be02c();
            }
            off += 4;
        }
        free(tmp);
    }
    if (flag != 0) {
        Func_80bdfec();
        if (ctx->dmg != 0) {
            Func_80bbabc(8, id);
            Func_80bbabc(0, id);
            Func_80bbabc(1, ctx->dmg);
            Func_80bbabc(4, 0x84b);
            if (_ModifyHP(id, -ctx->dmg) == 0) {
                Func_80bbabc(9, id);
                Func_80bbabc(0, id);
                if (id <= 7)
                    Func_80bbabc(4, 0x825);
                else
                    Func_80bbabc(4, 0x82b);
            } else {
                Func_80bbabc(0xb, id);
            }
        }
        Func_80bb938();
        Func_80bdfec();
        w = unit + 0x131;
        if (*(s8 *)w != 0) {
            v = *(short *)(unit + 0x34) * *(s8 *)w / 10;
            g = iwram_3001e74;
            Func_80bbabc(8, id);
            Func_80bbabc(0, id);
            Func_80bbabc(1, v);
            Func_80bbabc(4, 0x851);
            if (*(s8 *)w != 0)
                *(int *)(g + 0x820) = 0x86;
            else
                *(int *)(g + 0x820) = 0x85;
            if (_ModifyHP(id, -v) == 0) {
                Func_80bbabc(9, id);
                Func_80bbabc(0, id);
                if (id <= 7)
                    Func_80bbabc(4, 0x825);
                else
                    Func_80bbabc(4, 0x82b);
            } else {
                Func_80bbabc(0xb, id);
            }
        }
        Func_80bb938();
        Func_80bdfec();
        w = unit + 0x141;
        if (*w != 0) {
            *w = *w + 0xff;
            if (*w == 0) {
                if (_ModifyHP(id, 0xc0000000) == 0) {
                    msg = 0x828;
                    Func_80bbabc(0, id);
                    Func_80bbabc(4, msg);
                    Func_80bbabc(8, id);
                    Func_80bbabc(9, id);
                    Func_80bbabc(0, id);
                    if (id <= 7)
                        Func_80bbabc(4, msg - 3);
                    else
                        Func_80bbabc(4, msg + 3);
                }
            }
        }
        Func_80bb938();
    }
    _CalcStats(id);
}
