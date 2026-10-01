/* Func_8096ddc -- NON-MATCHING, 11 encodings of 146 differ (ref 146 / ours 146,
 * LENGTH EXACT, relocations identical, first diff at index 31).  PRODUCTION-FLAG
 * FIGURE, re-measured in batch 316 with tools/objcmp.py itself: the figure stands.
 * NOT POOL-INFLATED -- all 11 are real instructions (batch 316).
 *
 * asm/rom_8a000/rom_96cdc_a_a_c_c.s holds only this function and
 * tools/datacheck.py is silent on it -- whole file, no split, no exports beyond
 * Func_8096ddc.  Two mid-function pools (a HImode 0 and a HImode 0xfffffc00)
 * both reproduce.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/8096ddc.c \
 *     asm/rom_8a000/rom_96cdc_a_a_c_c.s --func Func_8096ddc
 *
 * ***********************************************************************
 * *** BATCH 316: THIS PARK IS OPEN.  "NO FLAG ROUTE" IS FALSE, AND THE
 * *** IMPOSSIBILITY ARGUMENT IS NOT ESTABLISHED.
 * ***********************************************************************
 * Batches 295 and 315 both declared the residue unreachable from source, with
 * "NO FLAG ROUTE. -fno-expensive-optimizations ... Measured: --align 53 -> 94,
 * i.e. much worse ... Nothing reaches local-alloc's eligibility test."
 * THAT IS A FIGURE, NOT A READING AT THE SITE.  Read at the instruction:
 *     -fno-expensive-optimizations:  95 of 146, FIRST DIFF AT INDEX 120.
 * *** INDICES 0-119 ARE ALL EXACT.  BOTH RESIDUE WINDOWS -- the r2/r3 swap at
 * idx 31-39 AND the negs/strh tie at idx 81-82 -- ARE CLOSED BY THE FLAG. ***
 * The 95 is a SECOND, INDEPENDENT defect behind the first: from index 120 the
 * candidate is about five instructions SHORT and the tail shifts.  So the flag
 * reaches the blocker, and the right conclusion is the opposite of the recorded
 * one.  (-fno-expensive-optimizations is NOT proposed as a flag row; it NAMES
 * THE PASS and bounds the search.)
 *
 * AND THE PARK'S MECHANISM FOR HOW THE FLAG WOULD WORK IS ALSO WRONG, WHICH IS
 * WHAT MAKES THE NEXT STEP NARROW.  The prediction was that the flag changes
 * `reg_preferred_class` / `reg_alternate_class` and so satisfies
 * local-alloc.c:362-368.  From the two .17.lreg dumps:
 *     baseline                      -fno-expensive-optimizations
 *     Register 40 pref STACK_REG    Register 40 pref STACK_REG   <-- UNCHANGED
 *     Register 52 pref LO_REGS      *** Register 52 ABSENT ***
 *     Register 54 pref LO_REGS      Register 54 pref LO_REGS
 * The walk pointer's `pref STACK_REG` DOES NOT MOVE.  What the flag does is
 * *** STOP THE DEAD QImode ZERO (reg 52) EXISTING AS A PSEUDO AT ALL. ***  With
 * reg 52 gone, no local quantity competes for r3 inside the walk pointer's live
 * range and the allocation falls into the ROM's shape.
 * So the ORIGINAL diagnosis -- "the dead QImode zero steals an address register"
 * -- was the right target all along.  The STACK_REG gate is correct and
 * correctly cited, but it is a reason the WALK POINTER IS GLOBAL, not a reason
 * the residue is unreachable.  The impossibility argument conflated the two.
 *
 * *** THE NEXT STEP, NAMED AND NARROW: find a source spelling of the `o->f55`
 * byte store whose zero does NOT create a second, DEAD QImode pseudo, while the
 * SImode zero stays SHARED between the strb and the strh (the sharing is what
 * produces the ROM's single `mov r2, #0`).  The recorded bitfield sweep -- eight
 * spellings and all four combinations of the ZB/ZH carrier member types -- varied
 * the CARRIER MEMBER'S TYPE.  It never varied whether a separate dead zero pseudo
 * is created.  Those are different questions and only the second one matters. ***
 *
 * ===== "FIX ONE AND THE OTHER FOLLOWS" IS WRONG.  STRIKE IT. =====
 * This park and src/non_matching/ovl_784360/200a440.c (OvlFunc_884_200a440, 12
 * of 136) assert they are the same function with the same gate and that fixing
 * either fixes both.  Under -fno-expensive-optimizations:
 *     8096ddc   95 of 146, FIRST DIFF AT 120 -- residue entirely closed
 *     200a440  125 of 136, FIRST DIFF AT 7   -- global perturbation from the top
 * Same flag, opposite behaviour.  200a440 carries the extra `__PlaySound` and
 * the `w` base pointer, and its own header already records that the r3 pin which
 * isolates the residue here costs four instructions there.  *** THEY SHARE A
 * MECHANISM, NOT A FIX. ***  The family is 8096ddc, 200a440,
 * ovl_787e04/200968c.c (12 of 133), ovl_791794/200aeb0.c (12 of 148) and the
 * parked 200dd68 / 200c41c -- and note tools/dupfuncs.py (re-run batch 316) makes
 * OvlFunc_883_200dd68 a BYTE DUPLICATE of OvlFunc_881_200c058
 * (asm/overlays/rom_77a7c8/ovl_30_c_c_c_c_c_c.s), which is not parked with them.
 *
 * ===== THE TAIL TIE (idx 81-82): LAST BATCH'S REPLACEMENT REASONING IS ALSO WRONG
 * Batch 315 rewrote this as "decided at rank_for_schedule's CLASS rung, not its
 * dependent-count rung -- so the alias-set lever cannot reach it".  IT IS NOT THE
 * CLASS RUNG.  From .23.sched2 (-fsched-verbose=6):
 *      insn  prio  dep  INSN_DEPEND
 *       508    13    3  509              (mov r1,#0x21 -- last scheduled, t=101)
 *       239    12    3  291 242          (strh r3,[r5,#8])
 *       509    12    2  497 244          (neg r1,r1)
 * At t=102 the ready list is `509 239` and 239 is taken.  WHICH TWO COMPETE:
 * **239 against 509**.  Priority ties at 12.  The CLASS rung TIES AT 3 -- and this
 * is the subtlety the rewrite missed: 509 IS in INSN_DEPEND(508), but
 * rank_for_schedule's class test is `if (link == 0 || insn_cost (...) == 1)
 * tmp_class = 3`, and insn_cost(508, link, 509) IS 1 (the dump says "insn 509
 * into queue with cost=1"), so *** THE `insn_cost == 1` ESCAPE PUTS A
 * DATA-DEPENDENT INSN IN CLASS 3 ANYWAY. ***  Dependent count then ties at 2, and
 * INSN_LUID decides -- 239 is a pre-reload insn, 509 is created late.
 * THE CONCLUSION ("the alias-set lever cannot reach it") SURVIVES, FOR A BETTER
 * REASON.  From .19.flow2, insn 239 stores `(mem/s:HI (plus (reg/v 5 r5) 8) 23)`
 * and insn 242 loads `(mem/s:QI (plus (reg/v 5 r5) 5) 23)` -- SAME ALIAS SET 23,
 * disjoint offsets.  The dependence 239 -> 242 that would have to be removed is
 * NOT a memory dependence at all: it is a REGISTER ANTI-DEPENDENCE ON r3 (239
 * reads r3, 242 writes it).  Alias sets are irrelevant in both directions.
 * *** AND THAT MAKES THE TAIL 2 ENCODINGS DOWNSTREAM OF THE SAME r2/r3 BLOCKER
 * AS THE OTHER 9.  Batch 315 called the residue "TWO INDEPENDENT PARTS"; it is
 * ONE FACT WITH TWO SYMPTOMS, which is also why the flag closes both at once. ***
 *
 * ===== STILL TRUE AND CORRECTLY CITED =====
 * REG_ALLOC_ORDER is `{3, 2, 1, 0, 12, 14, ...}` (config/arm/arm.h:989, verified
 * batch 316) -- r3 IS TRIED FIRST, which is why the first quantity allocated in a
 * range takes r3.
 * THE STACK_REG GATE.  An address pseudo set by `(set (reg) (plus (reg)
 * (const_int N)))` and used as a memory base gets `pref STACK_REG` when N is not
 * a valid `add rd, sp, #imm` operand and `pref BASE_REGS` when it is.  The cause
 * is `*thumb_addsi3` alternative 6 (`!k`/`!k`/`!O`, config/arm/arm.md:496) with
 * CONST_OK_FOR_THUMB_LETTER's `O` (arm.h:1095) and regclass.c:1459-1462 giving
 * `!` ZERO COST while the LO_REGS alternative pays a copy_cost; 0x54 also matches
 * `M`, which drags the pref to BASE_REGS, and 0x55 does not.  All four line
 * citations were re-checked in batch 316 and all four are right.
 * reg_class_size[STACK_REG] == 1, so CLASS_LIKELY_SPILLED_P is TRUE and
 * local-alloc.c:362-368 refuses the pseudo even when it dies exactly once.
 * Measured on a four-line isolate: +0x55 STACK_REG, +0x54 BASE_REGS, +0x64
 * BASE_REGS, +0x65 STACK_REG -- the CONSTANT, not the store's mode.
 * The `add r3,#0xf` second death comes from *** cse ***, which re-bases
 * `q += 0xf` to `o + 0x64` (absent in 00.rtl/02.jump, present from 03.cse), not
 * from move2add.
 *
 * ===== CORRECTION TO A RECORDED DIAGNOSTIC, carried forward from batch 315 =====
 * The `register unsigned char *q __asm__("r3")` pin reading 11 -> 6, and the
 * 5-versus-6 split of the residue built on it, are a property of that one pin on
 * this one function: on 200968c the same pin reads 128 of 133 and +12 bytes,
 * because r3 is call-clobbered and the pin sits in a loop with four calls.  DO
 * NOT QUOTE THE SPLIT AS FACT.
 *
 * ===== LEVER 5 (callee return type): EXHAUSTED =====
 * Every subset of the four `extern void` declarations (_Sprite_SetAnim,
 * Func_8003f3c, Func_8096d84, Func_8096d2c) switched to `int`, 15 variants, ALL
 * exactly 11 at dsize 0 (batch 315).  Func_8003f3c IS declared `extern int`
 * elsewhere in this tree, so the lever's precondition was present.
 *
 * ===== MEASURED NEGATIVES, ALL AT THE BASELINE OF 11 =====
 * A second address local instead of the chain, `q2 = q + 0xf`, and
 * `((struct H1 *)(q + 0xf))` / `&q[0xf]` all 17 -- and in every one the zero DOES
 * move to r2 and the f64 address DOES get r3, leaving only the f55 address in r1,
 * which is the cleanest statement of the blocker.  Inert at 11: all four
 * bitfield-carrier combinations, a `struct B1 *` walk pointer, BLK-u8 +
 * PLAIN-u16.  Worse: plain-u8 + BLK-u16 23; `((struct H1 *)q)->v = zero` 13;
 * `((struct B1/H1 *)q)->v = zero` 144 encodings; the HI store written first with
 * `q -= 0xf` 86; a function-scope `int z` 24; BLKmode stores of 0 for both
 * `s->f26` and `s->f28[0x16]` 12; a plain `int z = 0` in the walk 22 (and 148 if
 * declared in the loop, loop.c hoisting it).
 * *** CAUTION: all of those are figures at 11, and batch 316 broke this park's
 * closure by reading a probe the park had already run.  None of them bears on the
 * dead-QImode-pseudo question above, which is the one that matters. ***
 */
struct Sprite {
    unsigned char pad00[5];
    unsigned char c0 : 5;
    unsigned char c5 : 1;
    unsigned char c6 : 2;
    unsigned char pad06[1];
    unsigned char d0 : 6;
    unsigned char d6 : 2;
    unsigned short f08 : 10;
    unsigned short b10 : 2;
    unsigned short b12 : 4;
    unsigned char pad0a[0x1c - 0x0a];
    unsigned char f1c;
    unsigned char f1d;
    unsigned char pad1e[0x26 - 0x1e];
    unsigned char f26;
    unsigned char pad27;
    unsigned char *f28;
};

struct Actor {
    void *f00;
    unsigned char pad04[2];
    unsigned short f06;
    int f08;
    int f0c;
    int f10;
    int f14;
    int f18;
    int f1c;
    unsigned char pad20[3];
    unsigned char f23;
    unsigned char pad24[0x50 - 0x24];
    struct Sprite *f50;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[0x64 - 0x56];
    unsigned short f64;
    unsigned char pad66[2];
    struct Actor *f68;
    void *f6c;
};

struct B1 { unsigned char v; unsigned char pad[11]; };
struct H1 { unsigned short v; unsigned char pad[10]; };
struct SpriteSlot {
    unsigned short f0;
    unsigned short f2;
};

extern unsigned char *iwram_3001f30;
extern struct SpriteSlot gSpriteSlots[];
extern struct Actor *_CreateActor(int id, int x, int y, int z);
extern void _Sprite_SetAnim(struct Sprite *s, int n);
extern void Func_8003f3c(int n);
extern void Func_8096d84(void);
extern void Func_8096d2c(void);

void Func_8096ddc(struct Actor *e)
{
    struct Actor *arr[2];
    struct Actor *c;
    struct Actor *o;
    struct Sprite *s;
    unsigned char *m;
    unsigned short zero;
    int i;
    unsigned char *q;

    m = iwram_3001f30;
    c = *(struct Actor **)(m + 0x10);
    for (i = 0; i <= 1; i++) {
        o = _CreateActor(0x1a, e->f08, e->f0c, e->f10);
        arr[i] = o;
        if (o == 0)
            continue;
        o->f14 = e->f14;
        s = o->f50;
        q = (unsigned char *)o + 0x55;
        ((struct B1 *)q)->v = 0;
        q += 0xf;
        ((struct H1 *)q)->v = 0;
        o->f68 = e;
        o->f1c = 0x1999;
        zero = 0;
        o->f18 = 0x1999;
        if (s == 0)
            continue;
        _Sprite_SetAnim(s, 0);
        s->f26 = zero;
        Func_8003f3c(s->f1c);
        s->f1c = *(unsigned short *)(m + 0x46);
        s->f1d |= 1;
        s->f08 = gSpriteSlots[s->f1c].f2 >> 5;
        s->c5 = 0;
        s->c6 = 1;
        s->d6 = 2;
        s->f28[0x16] = zero;
    }
    arr[0]->f6c = Func_8096d84;
    arr[0]->f50->b10 = 0;
    arr[1]->f6c = Func_8096d2c;
    arr[1]->f50->b10 = c->f50->b10;
    arr[1]->f23 = 2;
}
