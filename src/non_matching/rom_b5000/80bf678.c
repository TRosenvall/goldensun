/* Func_80bf678 -- NON-MATCHING, 504 encodings of 530, size 1328 against the ROM's 1324 (+4).
 *
 * READ THIS FIRST: THIS IS AN EARLY-STAGE TRANSCRIPTION, NOT AN ANALYSED PARK.
 * No blocker class has been isolated. It is committed because the structural
 * reading is worth more than re-doing it from scratch, and because an honest
 * record of how far a target actually got is better than silence about it.
 * Do not read the difference count as a near miss and do not quote it as a
 * measured plateau -- nothing here has been driven to a plateau.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_b5000/80bf678.c \
 *     asm/rom_b5000/rom_bbb0c_c.s --func Func_80bf678
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
typedef unsigned char u8;

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
extern void Func_80bbabc(int a, int b);
extern void _PlaySound(int a);
extern void _Actor_SetAnim(int a, int b);
extern void _Actor_SetAnimSpeed(int a, int b);
extern void _SetDjinni(int a, int b, int c);
extern void _Func_807a3a8(int a, int b, int c);
extern void _CalcStats(int id);
extern void Anim_MoveIntro(int a, int b, int c, int d);
extern void Func_80be02c(void);
extern void Func_80c0774(int a, int b, int c);
extern int Func_80b6c08(int kind, unsigned short *buf);
extern void _ModifyHP(int a, int b);
extern void _ModifyPP(int a, int b);
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

void Func_80bf678(void)
{
    int order[2] = {1, 2};
    unsigned short buf[14];
    u8 *g;
    u8 *f;
    int *sel;
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
    int id;
    int e;
    int hp;
    int mx;
    int amt;

    g = iwram_3001e74;
    f = g + 0x44;
    sides = (*f != 0) + 1;
    for (side = 0; side < sides; side++) {
        rec = _Func_8077330(side);
        d = &rec->l;
        n = rec->l.count;
        i = 0;
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
    if (*f != 0) {
        if (*(g + 0x50) != 0) {
            order[0] = 2;
            order[1] = 1;
            sel = order;
        } else {
            sel = order;
        }
    } else {
        sel = order;
    }
    for (side = 0; side < 2; side++) {
        n = Func_80b6c08(sel[side], buf);
        for (i = 0; i < n; i++) {
            id = buf[i];
            u = _GetUnit(id);
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
                        _ModifyHP(id, amt);
                        _Func_8019908(id, 1);
                        _Func_8019908(amt, 5);
                        if (*(short *)(u + 0x38) == *(short *)(u + 0x34))
                            _Func_80175a0(0x820);
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
                        amt = *w;
                        if (hp + amt > mx)
                            amt = mx - hp;
                        _ModifyPP(id, amt);
                        _Func_8019908(id, 1);
                        _Func_8019908(amt, 5);
                        if (*(short *)(u + 0x3a) == *(short *)(u + 0x36))
                            _Func_80175a0(0x821);
                        else
                            _Func_80175a0(0x81e);
                        _PlaySound(0xaf);
                        WaitTextPrompt();
                    }
                }
            }
            if (Func_80bf574(id) != 0) {
                Func_80b78e4(id, GetBattleActor(id));
                _Func_8019908(id, 1);
                _Func_80175a0(0x889);
                WaitTextPrompt();
            }
            if (Func_80bf250(id) != 0) {
                Func_80b78e4(id, GetBattleActor(id));
                _Func_8019908(id, 1);
                _Func_80175a0(0x887);
                WaitTextPrompt();
            }
            if (Func_80bf2b4(id) != 0) {
                Func_80b78e4(id, GetBattleActor(id));
                _Func_8019908(id, 1);
                _Func_80175a0(0x888);
                WaitTextPrompt();
            }
            if (Func_80bf318(id) != 0) {
                Func_80b78e4(id, GetBattleActor(id));
                _Func_8019908(id, 1);
                _Func_80175a0(0x886);
                WaitTextPrompt();
            }
            if (Func_80bf37c(id) != 0) {
                Func_80b78e4(id, GetBattleActor(id));
                _Func_8019908(id, 1);
                _Func_80175a0(0x88b);
                WaitTextPrompt();
            }
            if (Func_80bf3bc(id) != 0) {
                Func_80b78e4(id, GetBattleActor(id));
                _Func_8019908(id, 1);
                _Func_80175a0(0x88a);
                WaitTextPrompt();
            }
            if (Func_80bf400(id) != 0) {
                Func_80b78e4(id, GetBattleActor(id));
                _Func_8019908(id, 1);
                _Func_80175a0(0x88e);
                WaitTextPrompt();
            }
            if (Func_80bf440(id) != 0) {
                Func_80b78e4(id, GetBattleActor(id));
                _Func_8019908(id, 1);
                Func_80b7aac(id);
                _Func_80175a0(0x88d);
                WaitTextPrompt();
            }
            if (Func_80bf484(id) != 0) {
                Func_80b78e4(id, GetBattleActor(id));
                _Func_8019908(id, 1);
                Func_80b7aac(id);
                _Func_80175a0(0x883);
                WaitTextPrompt();
            }
            if (Func_80bf4c4(id) != 0) {
                Func_80b78e4(id, GetBattleActor(id));
                _Func_8019908(id, 1);
                _Func_80175a0(0x88c);
                WaitTextPrompt();
            }
            if (Func_80bf524(id) != 0) {
                Func_80b78e4(id, GetBattleActor(id));
                _Func_8019908(id, 1);
                _Func_80175a0(0x891);
                WaitTextPrompt();
            }
            if (Func_80bf54c(id) != 0) {
                Func_80b78e4(id, GetBattleActor(id));
                _Func_8019908(id, 1);
                _Func_80175a0(0x892);
                WaitTextPrompt();
            }
        }
    }
}
