/* Func_80bf678 -- 499 instructions, 1324 bytes, 530 encodings and 115 relocations
 * identical.  Split out of asm/rom_b5000/rom_bbb0c_c.s; Func_80bfba4 and the .rodata
 * tail stay in asm as rom_bbb0c_c_c.s.
 *
 * THREE SEPARATE PREREQUISITES, each gated on its own before this .c was written:
 *   1. `.global .Lc35bc` added to the .s keeping the data tail (zero bytes; that
 *      block already carried 14 such exports for its neighbours).  tools/split_s.py
 *      REFUSED to split until this was done and told me to gate it separately, so
 *      the two changes stayed attributable.
 *   2. the split itself, verified byte-neutral.
 *   3. `_MSG_820 = 0x820;` in message.sym -- see below.
 *
 * ================================================================
 * THIS FUNCTION'S PARK WAS WRONG ABOUT ITS OWN REASON FOR WITHHOLDING A SYMBOL
 * ================================================================
 *
 * src/non_matching/rom_b5000/80bf678.c reported _MSG_820 with the strongest
 * in-function control on file -- the ONLY shiftable id among seventeen, all sixteen
 * others reproducing as plain literals in the reference's exact pool order -- and
 * then withheld it "because 8 encodings remain ... it does not COMPLETE the
 * function".  Those 8 are now closed, so it DOES complete the function, and on this
 * tree's stated criterion (evidence AND completion) it qualifies.  Measured:
 * byte-exact with it, 263 differing and +1 instruction with a plain literal.
 *
 * The lesson is about the criterion, not the symbol: "does not complete its
 * function" IS A STATEMENT ABOUT THE CURRENT CANDIDATE, NOT ABOUT THE SYMBOL.  Any
 * withheld entry whose park is later advanced should be re-examined rather than
 * left withheld by inertia.
 *
 * ================================================================
 * THE PARK'S RESIDUE WINDOW WAS AN ALIAS DEPENDENCE, NOT A SCHEDULING TIE
 * ================================================================
 *
 * The park called it "a sched2 load-hoist across two frame stores that gcc declines
 * to perform" and left it at 8.  .23.sched2 says otherwise: the .Lc35bc pool load
 * has priority 10 against the spill reload's 5 -- NOT CLOSE, so it was never a tie.
 *
 * The 10 comes from `ldrb r3,[r1]` (`*f`) DEPENDING on the `order` block store,
 * because a bare `unsigned char *` deref gets ALIAS SET 0.  Retyping `f` as
 * `struct Flag { unsigned char v; } *` and reading `f->v` drops the chain and the
 * reload wins.  Ablation: revert the struct and keep everything else -> 5 differ.
 *
 * THAT IS THE *LOAD* SIDE OF BATCH 281'S ALIAS-SET-0 LEVER, which was recorded for
 * STORES (a QImode store with alias set 0 is an unconditional true dependence).
 * The same `lang_get_alias_set` rule makes a char-precision LOAD depend on
 * everything before it.  Both directions now have a landed instance.
 *
 * ================================================================
 * A NESTED if/else ALWAYS EMITS [X][Y][Z] AND THE ROM EMITS [X][Z][Y]
 * ================================================================
 *
 * For `if (A) { if (B) X else Y } else Z`, stmt.c's `expand_start_else` emits the
 * endif jump and the else label IN PLACE, so THE INNER ELSE CAN NEVER FOLLOW THE
 * OUTER ONE.  A `goto` out of the then-arm is the only escape, and it must KEEP the
 * outer `else` rather than becoming a second `goto` -- otherwise gcse merges the
 * three identical `sel = &order.a` assignments.  Ablation: revert the goto and keep
 * the struct -> 2 differ.
 *
 * `expand_start_else` had zero hits in docs/elevation.md before this; the `goto`
 * lever is documented but not for this job.
 *
 * The park's five other findings all held and are not repeated here -- see it for
 * the epilogue-pop return-mode tell, the live-range inflation measurements, the
 * crossjumping lever worth 242 encodings, and why a local aggregate initializer
 * must live in an inner block with its data referenced through a compiler-generated
 * rodata label.
 *
 * No per-file Makefile flag override applies to this stem.
 */
/* Func_80bf678 -- NON-MATCHING, 8 encodings of 530, SIZE DELTA ZERO (1324 bytes
 * both), and all 118 relocation offsets identical to the reference EXCEPT the one
 * documented symbol-tell word.  This is the closest park of batch 281.
 *
 * Blocker class: a sched2 load-hoist across two frame stores that gcc declines to
 * perform.  ONE instruction out of place in one 7-instruction window.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_b5000/80bf678.c \
 *     asm/rom_b5000/rom_bbb0c_c.s --func Func_80bf678
 * The reference holds TWO functions plus a data tail, so a split is required.
 *
 * (This park replaces an EARLY transcription committed at 504 of 530 with no
 * blocker isolated.)
 *
 * TWO EXTRA SPLIT REQUIREMENTS, neither performed:
 *   1. The data tail defines `.Lc35bc:` (ROM 0x0c35bc, contents {1, 2}) and the C
 *      references it by name, so the .s keeping that data needs
 *      `.global .Lc35bc` -- exactly the precedent in asm/rom_b5000/rom_c1a34_c.s,
 *      which carries `.global .Lc5c38` for src/rom_b5000/rom_c1a34_a_a_a_a_a.c.
 *   2. _MSG_820 would need adding to message.sym (below).
 *
 * ================================================================
 * SYMBOL TELL -- REPORTED, NOT ADDED, AND ITS CONTROL IS THE STRONGEST ON FILE
 * ================================================================
 *
 *     _MSG_820 = 0x820;
 *
 * 0x820 == 0x82 << 4 is thumb_shiftable_const, so the K-constraint split in arm.md
 * (~line 3859) makes gcc BUILD it as `movs r0,#130 / lsls r0,r0,#4`; the ROM spends
 * a pool word.  CONST_OK_FOR_THUMB_LETTER(...,'K') is consulted on the CONST_INT
 * value alone, so NO SOURCE SPELLING REACHES A POOL LOAD.
 *
 * THE IN-FUNCTION CONTROL IS UNUSUALLY STRONG AND WAS RE-VERIFIED INDEPENDENTLY.
 * This function passes SEVENTEEN message ids and 0x820 is THE ONLY SHIFTABLE ONE.
 * The other sixteen -- 0x81d, 0x81e, 0x821, 0x883, 0x886, 0x887, 0x888, 0x889,
 * 0x88a, 0x88b, 0x88c, 0x88d, 0x88e, 0x891, 0x892, 0x897 -- are all unshiftable,
 * and ALL SIXTEEN reproduce as plain literals in the correct pool slots in the
 * reference's exact pool order.  So the claim is "this one constant gcc CAN build
 * and the ROM chose not to", not "this id space wants symbols".  Same shape as
 * _MSG_b20 (Func_80a15f0) and _MSG_810..813 (batch 182).
 *
 * WITHHELD ANYWAY, because it does not COMPLETE the function -- 8 encodings remain,
 * about 5 after the symbol.  That is this tree's line and it is applied here against
 * the strongest evidence yet seen, which is the point: _MSG_d27 went in this same
 * batch on WEAKER evidence because it completed its file.  Evidence quality and
 * completion are separate tests.
 *
 * ================================================================
 * THE POOL WAS NOT THE BLOCKER -- contrary to the briefing's prior
 * ================================================================
 *
 * It fell out of the structure with no lever at all.  The reference has one interior
 * chunk of 6 words at 0x080bf91c-0x80bf934 (iwram_3001e74, 0x897, .Lc35bc, 0x820,
 * 0x81d, 0x821) and an end chunk of 13 at 0x080bfb70.  THE INTERIOR DUMP IS FORCED:
 * the first pool constant is loaded at 0x080bf686 and the next barrier after
 * `b .Lbf93a` is ~0x080bfb4e, 1224 bytes away and past the 1020 pool_range, so gcc
 * dumps at the last usable barrier.  Both chunks reproduced exactly and at the exact
 * byte offsets from candidate 4 onward.  No HImode-store lever was needed.
 *
 * ================================================================
 * THE RESIDUE -- one window, reference 0x080bf7f0-0x080bf7fc
 * ================================================================
 *
 *   ref                          ours
 *   ldr  r3, =.Lc35bc            ldr  r3, =.Lc35bc
 *   ldr  r1, [sp, #4]            ldr  r4, [r3, #4]
 *   ldr  r4, [r3, #4]            ldr  r3, [r3, #0]
 *   ldr  r3, [r3, #0]            str  r3, [sp, #12]
 *   str  r3, [sp, #12]           str  r4, [sp, #16]
 *   str  r4, [sp, #16]           ldr  r1, [sp, #4]
 *   ldrb r3, [r1]                ldrb r3, [r1]
 *
 * One instruction -- the reload of the spilled `f` pointer -- sits five slots later;
 * everything before and after is byte-identical.  The `ldr r1,[sp,#4] -> ldrb ->
 * cmp -> beq` chain has the longer critical path so it SHOULD win the ready list;
 * ours schedules it adjacent to its use.  sp+4 against sp+12/16 are disambiguable,
 * so the hoist is legal either way.  Diagnostic: -fno-schedule-insns2 moves the
 * whole function to 103 mismatched slots, confirming sched2 is the pass and that
 * suppressing it is not the cure.
 *
 * ================================================================
 * FIVE FINDINGS WORTH CARRYING, all measured
 * ================================================================
 *
 * 1. THE EPILOGUE POP REGISTER READS OFF THE FUNCTION'S OWN RETURN MODE.
 *    thumb_exit (arm.c ~8215) sets regs_available_for_popping from
 *    GET_MODE_SIZE (DECL_MODE (DECL_RESULT (decl))): VOIDmode gives r0|r1|r2 and
 *    picks r0, size <= 4 gives r1|r2 and picks r1.  SO A ROM EPILOGUE ENDING
 *    `pop {r1} / bx r1` IN AN INTERWORKING THUMB FUNCTION THAT SAVES HIGH REGISTERS
 *    IS DIRECT EVIDENCE THE FUNCTION RETURNS A VALUE, even with no `return`
 *    statement anywhere.  It is a READ, not a guess.  Compare
 *    src/non_matching/ovl_7fb4a8/2008860.c, where the same tell (a MISSING
 *    `mov r0, #0`) was worth 53 -> 30.
 *
 * 2. LIVE-RANGE INFLATION FROM VARIABLE REUSE ACROSS TWO HALVES OF A FUNCTION IS
 *    THE DOMINANT ALLOCATION BLOCKER AT THIS SIZE, AND DECLARATION ORDER IS INERT.
 *    Four separate splits (id, i, side, amt) took 146 -> 37, while SIX
 *    declaration-order permutations moved nothing.  One C variable is one pseudo
 *    whose live_length is the SUM of all its ranges, so reusing a counter in a
 *    second loop 250 instructions away silently halves its priority.  Direction
 *    matters both ways: splitting `amt` paid 18, splitting hp/mx in the same edit
 *    COST 18.
 *
 * 3. CROSSJUMPING MERGES IDENTICAL `p = arr;` TAILS, AND MOVING THE ASSIGNMENT ONE
 *    STATEMENT EARLIER IN ONE ARM DEFEATS IT -- 242 ENCODINGS IN ONE EDIT
 *    (388 -> 146), the largest lever in this function.  The ROM has three copies of
 *    `mov rX,sp / adds rX,#12 / str rX,[sp]`; gcc merged ours to two.  Writing the
 *    then-arm as `order[0]=2; sel=order; order[1]=1;` makes the tails differ.  Jump
 *    optimisation runs BEFORE sched2, so THE PRE-SCHEDULING STATEMENT ORDER IS WHAT
 *    CROSSJUMPING SEES, even though the ROM's scheduled output shows the store back
 *    in the middle.
 *
 * 4. A LOCAL AGGREGATE INITIALIZER'S CODE IS EMITTED AT ITS BLOCK'S ENTRY, NOT
 *    WHERE IT IS USED.  `int order[2] = {1,2}` at function scope put the rodata
 *    block copy in the prologue, ~370 bytes early.  An inner block placed it
 *    exactly where the ROM has it.
 *
 * 5. A COMPILER-GENERATED RODATA LABEL IS THE RIGHT SPELLING FOR SUCH AN
 *    INITIALIZER, AND IT FIXES SIZE.  Writing {1,2} inline emits 8 bytes of our own
 *    .rodata, which arm-none-eabi-size's text field counts -- that was the entire
 *    persistent "+8 bytes".  Referencing .Lc35bc via __asm__ and assigning a 2-int
 *    struct produces the reference's exact ldr/ldr/str/str block copy, the exact
 *    .Lc35bc reloc, and byte-exact size.  Element-wise assignment instead destroys
 *    the block copy (334, -16 bytes).
 *
 * Func_80bf5a8 in src/rom_b5000/rom_bbb0c_a_c_c_a_c_b.c is an EXACT-MATCHING
 * template for this function's first two loops.  Its "d = &rec->l; before
 * n = rec->l.count;" lever reproduced here (reversing it costs 41) but needed one
 * refinement this function exposes: `i = 0;` belongs BETWEEN them, worth 9.
 *
 * Forty probe variants are on file in scratch with their numbers.  Return-type
 * probes were worth 37 -> 17 in aggregate (Func_80bbabc, _SetDjinni,
 * Anim_MoveIntro, _ModifyHP, _ModifyPP as int, plus `int Func_80bf678(void)`),
 * while five other callees as int were exactly inert.
 *
 * NEXT: read .23.sched2's ready-list priorities for the one window.  The agent that
 * got here explicitly declined to claim a spelling without that, which is the right
 * call at 8 of 530.
 */
typedef unsigned char u8;

struct Flag {
    u8 v;
};

struct Djinni {
    u8 f0;
    u8 f1;
    u8 f2;
    signed char f3;
};

struct List {
    struct Djinni e[0x40];
    int count;
};

struct Rec {
    u8 pad0[8];
    struct List l;
};

extern u8 *iwram_3001e74;

extern struct Rec *_Func_8077330(int a);
extern int *GetBattleActor(int a);
extern u8 *_GetUnit(int id);
extern void Func_80bdfec(void);
extern void Func_80bd808(int a);
extern int Func_80bbabc(int a, int b);
extern void _PlaySound(int a);
extern void _Actor_SetAnim(int a, int b);
extern void _Actor_SetAnimSpeed(int a, int b);
extern int _SetDjinni(int a, int b, int c);
extern void _Func_807a3a8(int a, int b, int c);
extern void _CalcStats(int id);
extern int Anim_MoveIntro(int a, int b, int c, int d);
extern void Func_80be02c(void);
extern void Func_80c0774(int a, int b, int c);
extern int Func_80b6c08(int kind, unsigned short *buf);
extern int _ModifyHP(int a, int b);
extern int _ModifyPP(int a, int b);
extern void _Func_8019908(int a, int b);
extern void _Func_80175a0(int a);
extern void WaitTextPrompt(void);
extern void Func_80b78e4(int a, void *b);
extern void Func_80b7aac(int a);
extern int Func_80bf574(int a);
extern int Func_80bf250(int a);
extern int Func_80bf2b4(int a);
extern int Func_80bf318(int a);
extern int Func_80bf37c(int a);
extern int Func_80bf3bc(int a);
extern int Func_80bf400(int a);
extern int Func_80bf440(int a);
extern int Func_80bf484(int a);
extern int Func_80bf4c4(int a);
extern int Func_80bf524(int a);
extern int Func_80bf54c(int a);
extern int _MSG_820;
struct Pair { int a; int b; };
extern struct Pair Lc35bc __asm__(".Lc35bc");

int Func_80bf678(void)
{
    u8 *g;
    struct Flag *f;
    struct Rec *rec;
    struct List *d;
    struct Djinni *p;
    struct Djinni *q;
    u8 *v;
    u8 *u;
    u8 *r;
    u8 *w;
    int sides;
    int side;
    int i;
    int n;
    int m;
    int id;
    int e;
    int hp;
    int mx;
    int amt;
    int amt2;
    int uid;
    int j;
    int turn;

    g = iwram_3001e74;
    f = (struct Flag *)(g + 0x44);
    sides = (f->v != 0) + 1;
    for (side = 0; side < sides; side++) {
        rec = _Func_8077330(side);
        d = &rec->l;
        i = 0;
        n = rec->l.count;
        if (i < n) {
            p = d->e;
            do {
                if (p->f3 > 0) {
                    if (GetBattleActor(p->f2) != 0) {
                        v = _GetUnit(p->f2);
                        if (*(short *)(v + 0x38) != 0)
                            p->f3 = p->f3 - 1;
                    }
                }
                i++;
                p++;
            } while (i < d->count);
        }
        i = 0;
        if (i < d->count) {
            q = d->e;
            do {
                if (q->f3 == 0) {
                    id = q->f2;
                    if (GetBattleActor(id) != 0) {
                        Func_80bdfec();
                        Func_80bd808(0x1e);
                        Func_80bbabc(0, id);
                        Func_80bbabc(3, q->f0 * 20 + q->f1 + 0x12c);
                        Func_80bbabc(0xe, 0xaf);
                        Func_80bbabc(0xa, 0);
                        Func_80bbabc(4, 0x897);
                        Func_80bbabc(0xb, id);
                        _PlaySound(0xd4);
                        _Actor_SetAnim(*GetBattleActor(id), 3);
                        _Actor_SetAnimSpeed(*GetBattleActor(id), 0x20);
                        e = q->f0;
                        _SetDjinni(id, e, q->f1);
                        _Func_807a3a8(id, q->f0, q->f1);
                        _CalcStats(id);
                        Anim_MoveIntro(id, e, 3, 0);
                        Func_80be02c();
                    }
                } else {
                    q++;
                    i++;
                }
            } while (i < d->count);
        }
    }
    Func_80c0774(2, *(unsigned short *)(iwram_3001e74 + (0xc9 << 3)), 0);
    {
    struct Pair order;
    unsigned short buf[14];
    int *sel;
    order = Lc35bc;
    if (f->v != 0) {
        if (*(g + 0x50) == 0)
            goto inner_else;
        order.a = 2;
        sel = &order.a;
        order.b = 1;
    } else {
        sel = &order.a;
    }
    goto have_sel;
inner_else:
    sel = &order.a;
have_sel:
    ;
    for (turn = 0; turn < 2; turn++) {
        m = Func_80b6c08(sel[turn], buf);
        for (j = 0; j < m; j++) {
            uid = buf[j];
            u = _GetUnit(uid);
            r = u + (0xa2 << 1);
            if (*r != 0)
                *r = *r + 0xff;
            hp = *(short *)(u + 0x38);
            if (hp != 0) {
                w = u + 0x44;
                if (*w != 0) {
                    mx = *(short *)(u + 0x34);
                    if (hp != mx) {
                        amt = *w;
                        if (hp + amt > mx)
                            amt = mx - hp;
                        _ModifyHP(uid, amt);
                        _Func_8019908(uid, 1);
                        _Func_8019908(amt, 5);
                        if (*(short *)(u + 0x38) == *(short *)(u + 0x34))
                            _Func_80175a0((int)&_MSG_820);
                        else
                            _Func_80175a0(0x81d);
                        _PlaySound(0xaf);
                        WaitTextPrompt();
                    }
                }
                w = u + 0x45;
                if (*w != 0) {
                    hp = *(short *)(u + 0x3a);
                    mx = *(short *)(u + 0x36);
                    if (hp != mx) {
                        amt2 = *w;
                        if (hp + amt2 > mx)
                            amt2 = mx - hp;
                        _ModifyPP(uid, amt2);
                        _Func_8019908(uid, 1);
                        _Func_8019908(amt2, 5);
                        if (*(short *)(u + 0x3a) == *(short *)(u + 0x36))
                            _Func_80175a0(0x821);
                        else
                            _Func_80175a0(0x81e);
                        _PlaySound(0xaf);
                        WaitTextPrompt();
                    }
                }
            }
            if (Func_80bf574(uid) != 0) {
                Func_80b78e4(uid, GetBattleActor(uid));
                _Func_8019908(uid, 1);
                _Func_80175a0(0x889);
                WaitTextPrompt();
            }
            if (Func_80bf250(uid) != 0) {
                Func_80b78e4(uid, GetBattleActor(uid));
                _Func_8019908(uid, 1);
                _Func_80175a0(0x887);
                WaitTextPrompt();
            }
            if (Func_80bf2b4(uid) != 0) {
                Func_80b78e4(uid, GetBattleActor(uid));
                _Func_8019908(uid, 1);
                _Func_80175a0(0x888);
                WaitTextPrompt();
            }
            if (Func_80bf318(uid) != 0) {
                Func_80b78e4(uid, GetBattleActor(uid));
                _Func_8019908(uid, 1);
                _Func_80175a0(0x886);
                WaitTextPrompt();
            }
            if (Func_80bf37c(uid) != 0) {
                Func_80b78e4(uid, GetBattleActor(uid));
                _Func_8019908(uid, 1);
                _Func_80175a0(0x88b);
                WaitTextPrompt();
            }
            if (Func_80bf3bc(uid) != 0) {
                Func_80b78e4(uid, GetBattleActor(uid));
                _Func_8019908(uid, 1);
                _Func_80175a0(0x88a);
                WaitTextPrompt();
            }
            if (Func_80bf400(uid) != 0) {
                Func_80b78e4(uid, GetBattleActor(uid));
                _Func_8019908(uid, 1);
                _Func_80175a0(0x88e);
                WaitTextPrompt();
            }
            if (Func_80bf440(uid) != 0) {
                Func_80b78e4(uid, GetBattleActor(uid));
                _Func_8019908(uid, 1);
                Func_80b7aac(uid);
                _Func_80175a0(0x88d);
                WaitTextPrompt();
            }
            if (Func_80bf484(uid) != 0) {
                Func_80b78e4(uid, GetBattleActor(uid));
                _Func_8019908(uid, 1);
                Func_80b7aac(uid);
                _Func_80175a0(0x883);
                WaitTextPrompt();
            }
            if (Func_80bf4c4(uid) != 0) {
                Func_80b78e4(uid, GetBattleActor(uid));
                _Func_8019908(uid, 1);
                _Func_80175a0(0x88c);
                WaitTextPrompt();
            }
            if (Func_80bf524(uid) != 0) {
                Func_80b78e4(uid, GetBattleActor(uid));
                _Func_8019908(uid, 1);
                _Func_80175a0(0x891);
                WaitTextPrompt();
            }
            if (Func_80bf54c(uid) != 0) {
                Func_80b78e4(uid, GetBattleActor(uid));
                _Func_8019908(uid, 1);
                _Func_80175a0(0x892);
                WaitTextPrompt();
            }
        }
    }
    }
}
