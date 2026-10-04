/* Func_8096ddc -- NON-MATCHING, 11 of 146.  ref 146 / ours 146, LENGTH EXACT,
 * relocations identical, first diff at index 31.  PRODUCTION FLAGS.
 * RE-MEASURED batch 322 brief E with tools/objcmp.py: 11 stands, and all 11 are
 * real instructions -- indices 0-30, 40-80 and 83-145 agree, and both
 * mid-function pools (a HImode 0 and a HImode 0xfffffc00) reproduce.
 *
 * asm/rom_8a000/rom_96cdc_a_a_c_c.s holds only this function; tools/datacheck.py
 * is silent on it.  Whole file, no split, no exports beyond Func_8096ddc.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/8096ddc.c \
 *     asm/rom_8a000/rom_96cdc_a_a_c_c.s --func Func_8096ddc
 *
 * ***********************************************************************
 * *** BATCH 322: THE 11 ARE **THREE** CAUSES, 5 + 4 + 2, AND ONLY ONE OF
 * *** THEM IS THE r2/r3 BLOCKER.  TWO PREVIOUS HEADERS GOT THE
 * *** DECOMPOSITION WRONG IN OPPOSITE DIRECTIONS.
 * ***********************************************************************
 * The instrument is one pin and one compile: `register unsigned char *q
 * __asm__("r3")` and nothing else reads 6 of 146 at exact count (the figure
 * batch 315 recorded, reproduced).  Reading WHICH SIX, which nobody had done:
 *
 *   idx 35 36 37 39   ref `ldr r5,[r0,#0x50] / strb r2,[r3] / adds r3,#15
 *                      ... ldr r1,[pc,#16]`
 *                     ours the same four insns with `ldr r5,[r0,#0x50]` LAST
 *   idx 81 82         the negs/strh pair, still swapped
 *
 * So with the walk pointer forced into r3 the ENTIRE 31-34/38 window closes and
 * the tail pair DOES NOT.  Therefore:
 *   - batch 316's "ONE FACT WITH TWO SYMPTOMS, which is also why the flag
 *     closes both at once" is REFUTED.  Closing the r2/r3 fact leaves the tail
 *     at 2.  A flag that closes both closes two things.
 *   - batch 315's "TWO INDEPENDENT PARTS" was right in kind and short by one.
 *     There are THREE: (A) the q/zero r2-r3 swap, 5 encodings; (B) the
 *     placement of `s = o->f50`'s load, 4 encodings; (C) the negs/strh tie, 2.
 *   - (B) had never been separated from (A) at all.  It is pure sched2: moving
 *     the `s = o->f50;` statement to three different source positions is
 *     EXACTLY INERT at 6 (see the measured list).
 *
 * ===== (A) THE r2/r3 SWAP: THE PARK NAMED THE WRONG PSEUDO =====
 * Batch 316's "next step, named and narrow" was to find a spelling of the
 * `o->f55` byte store whose zero does not create a second DEAD QImode pseudo.
 * The dead pseudo is real -- .17.lreg insn 78 is
 *     (set (reg:QI 52) (const_int 0))   REG_UNUSED
 * i.e. a set whose result is never read, which local-alloc still gives a
 * quantity (2 refs / 2 insns) and still hands r3.  **BUT IT IS NOT THE
 * BLOCKER.**  The REAL zero is pseudo 54, insn 80, `(set (reg:SI 54)
 * (const_int 0))`, used as `(subreg:QI (reg 54))` by insn 84 and
 * `(subreg:HI (reg 54))` by insn 98 -- the sharing the park wanted.  Its range
 * is insns 80-98.  The walk pointer (pseudo 40) lives 73-84 and 87-98.  **54
 * OVERLAPS ALL OF q AND ALSO TAKES r3** (disposition `54 in 3`).  Deleting 52
 * would free nothing: 54 would still be in r3 and q would still be pushed to
 * r2.  Do not spend another round on the QImode pseudo.
 *
 * ===== WHY q LOSES r3, CORRECTED AT THE SOURCE =====
 * q is in `;; 12 regs to allocate` -- a GLOBAL allocno -- and globals are
 * allocated AFTER every local quantity, so q can only ever have what the
 * locals left.  The park explains that exclusion with the STACK_REG gate:
 * `reg_class_size[STACK_REG] == 1` so `CLASS_LIKELY_SPILLED_P` is true and
 * local-alloc.c:362-368 refuses the pseudo.  **READ THE WHOLE TEST
 * (local-alloc.c:360-366):**
 *     if (REG_BASIC_BLOCK (i) >= 0 && REG_N_DEATHS (i) == 1
 *         && (reg_alternate_class (i) == NO_REGS
 *             || ! CLASS_LIKELY_SPILLED_P (reg_preferred_class (i))))
 *       reg_qty[i] = -2;            // eligible
 * `.17.lreg` says q is "used 8 times across 7 insns in block 2; set 2 times;
 * **dies in 2 places**".  *** REG_N_DEATHS IS 2, SO THE SECOND CONJUNCT FAILS
 * AND THE CLASS_LIKELY_SPILLED_P TEST IS NEVER EVALUATED. ***  Every STACK_REG
 * line in this park is true about the macro and about `reg_preferred_class`,
 * and NONE of it is the reason q is global here.
 *
 * q dies twice because of the rebase the park itself identified: cse rewrites
 * `q += 0xf` to `o + 0x64`, so the second set does not READ q and q becomes two
 * disjoint ranges.  (The emitted `adds r3,#15` is then put BACK by
 * reload_cse_move2add's SECOND transform, reload1.c:8920-8970 -- the
 * `(set REGX REGY)(set REGX (plus REGX A))` ... `(set REGX (plus REGX B))`
 * form collapsing to `B-A`.  Same pass that landed Func_8091254 this batch.)
 *
 * ===== THE TARGET, NOW QUANTITATIVE =====
 * If q were local-eligible it would be allocated FIRST in block 2 and take r3,
 * and the zero would fall to r2 -- the ROM.  `.18.greg` says 12 regs to
 * allocate, so block 2's locals are ranked by local-alloc's QTY_CMP_PRI
 * (local-alloc.c:1496).  *** NOTE: that macro DOES contain floor_log2, exactly
 * like global.c's allocno_compare; the two differ only in the denominator
 * (`qty[].death - qty[].birth` in half-luids vs `REG_LIVE_LENGTH`).  A brief
 * in this batch circulated the opposite and it is false -- see FINDINGS.md. ***
 * Using the .17.lreg spans as a proxy for death-birth:
 *     q  (40)  8 refs /  7  ->  floor_log2(8)*8/7  = 3.43   <-- would win
 *     f14 temp (50) 4 / 4   ->  2*4/4              = 2.00
 *     0x1999   (61) 6 / 6   ->  2*6/6              = 2.00
 *     zero     (54) 6 / 10  ->  2*6/10             = 1.20
 *     dead QI  (52) 2 / 2   ->  1*2/2              = 1.00
 * So the whole of (A) reduces to ONE question: **make the walk pointer die
 * once.**  Its preferred class must then also not be likely-spilled, and the
 * park's own isolate gives the rule -- +0x54 and +0x64 read BASE_REGS, +0x55
 * and +0x65 read STACK_REG -- with BASE_REGS being `{ 0x00020FF }`
 * (arm.h:1040), nine registers, hence NOT likely spilled.  A single-death
 * pointer born at 0x55 is still refused; one born at 0x64 is not.
 *
 * ===== STRIKE "FIX ONE AND THE OTHER FOLLOWS" -- STILL STRUCK =====
 * Batch 316 struck it between this park and ovl_784360/200a440.c on measured
 * evidence (same flag, 95 of 146 first-diff-120 here against 125 of 136
 * first-diff-7 there).  Nothing in this batch disturbs that.  THEY SHARE A
 * MECHANISM, NOT A FIX.  Family: 8096ddc, 200a440, ovl_787e04/200968c.c,
 * ovl_791794/200aeb0.c, and the parked 200dd68 / 200c41c.  tools/dupfuncs.py
 * makes this a duplicate of OvlFunc_896_200a440; the flag separates them.
 *
 * ===== STILL TRUE AND CORRECTLY CITED (re-checked, not re-derived) =====
 * REG_ALLOC_ORDER `{3,2,1,0,12,14,4,5,...}` (arm.h:989-995) -- r3 tried first.
 * The STACK_REG/BASE_REGS preference rule and its four line citations
 * (arm.md:496, arm.h:1095, regclass.c:1459-1462, local-alloc.c:362-368) --
 * right about what they say, just not the operative conjunct here.
 * -fno-expensive-optimizations: 95 of 146, FIRST DIFF AT INDEX 120, indices
 * 0-119 exact, so it closes both residue windows and exposes a further defect
 * from 120 on.  It NAMES THE PASS and bounds the search; it is not a flag row.
 * The idx 81-82 analysis: insn 239 vs 509, priorities tie at 12, the CLASS rung
 * ties at 3 via rank_for_schedule's `insn_cost == 1` escape, dependent count
 * ties at 2, INSN_LUID decides; and the dependence is a REGISTER
 * ANTI-DEPENDENCE on r3, not a memory one, so no alias set can reach it.
 * Lever 5 (callee return types int vs void): EXHAUSTED, 15 variants all 11.
 *
 * ===== MEASURED THIS BATCH.  Exact count 146 unless a flag is shown. =====
 * On the production body (BASE 11):
 *    6  INSTRUMENT `register unsigned char *q __asm__("r3")`  (see above)
 *    7  that pin + the pointer re-based to o+0x54 with a [+1] byte store
 *   11  adding `int z;` to the declarations and not using it (inert)
 *   13  `((struct H1 *)q)->v = zero` -- the HImode store from the existing
 *       `unsigned short zero` instead of a literal
 *   15  that pin + the pointer re-based to o+0x64 with a [-0xf] byte store
 *   17  second address as its own expression `((struct H1 *)(o+0x64))->v = 0`
 *   17  pointer born at o+0x54, byte store at [+1], `q += 0x10`
 *   18  pointer born at o+0x64, byte store at [-0xf]
 *   20  `((struct B1 *)q)->v = zero` (the QImode store from `zero`)
 *   24  MEM -- `zero = 0;` hoisted above the walk.  A MEM flag at this
 *       distance is a WRONG PROGRAM: `zero` is re-read per iteration.
 *  114-134 RELOC/COUNT -- every form that introduces an `int z` carrier for the
 *       walk zeros (four spellings) lands at 141-148 insns, because the carrier
 *       is loop-invariant and loop.c hoists it into the preheader.  This
 *       reproduces the park's recorded 22/148 and is why Field_Whirlwind's
 *       lever 4 (an int carrier plus a named pointer for the FIRST store only,
 *       src/rom_8a000/rom_9a44c_c_c_a_a.c) DOES NOT TRANSFER here: that loop is
 *       49 real insns and clears move_movables' threshold at lifetime 3, this
 *       one does not.
 * On the pinned instrument (BASE 6), i.e. figures ABOUT causes (B) and (C):
 *    6  moving `s = o->f50;` to any of three later source positions -- EXACTLY
 *       INERT.  (B) is a sched2 placement, immune to statement order.
 *    6  an alias-set-0 union member (`union UZ { unsigned char b; unsigned
 *       short h; }`) on the f55 byte store -- EXACTLY INERT
 *    6  the same union on the f64 halfword store -- EXACTLY INERT
 *    6  a union read inserted beside `s->f08 = ...` -- EXACTLY INERT
 *       *** So Field_Whirlwind's LEVER 5 is inert on this function in all three
 *       positions.  That is consistent with the park's own finding that the
 *       tail dependence is a REGISTER anti-dependence: there is no memory
 *       anti-dependence for alias set 0 to restore. ***
 *
 * ===== NEXT STEP, NAMED AND NARROW (replaces batch 316's) =====
 * (A) Make the walk pointer a SINGLE-DEATH pseudo whose preferred class is not
 *     likely-spilled.  Two sub-questions, both cheap: can cse be stopped from
 *     re-basing `q += 0xf` onto `o`, and is there a spelling born at 0x64
 *     (BASE_REGS) that still emits `strb r2,[r3,#0]` at 0x55?
 * (B) is sched2 and has never been attacked on its own; it is now isolated by
 *     the pin, so measure it against the pinned base, not the production one.
 * (C) is decided at INSN_LUID with everything above it tied; per the brief's
 *     batch-321 counterexample the CLASS rung is live in general, but here it
 *     ties, so the lever is whatever changes the LUID of insn 239 or 509.
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
