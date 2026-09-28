/* OvlFunc_957_200b644 -- 0x0200b644, asm/overlays/rom_7e3e08/ovl_30_c_c_c_c_b_a.s.
 * NON-MATCHING, 388 encodings of 425.  NOT a distance (ref 1004 bytes / 425 encodings against ours 1012 / 429).  READ `--align`: 102 of 426.
 *
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M`.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7e3e08/200b644.c \
 *     asm/overlays/rom_7e3e08/ovl_30_c_c_c_c_b_a.s --func OvlFunc_957_200b644
 * PARKED, batch 294 brief H.  403 instructions.
 *
 * WHOLE-FILE CONVERSION, confirmed not assumed: `grep -ci func_start` on the
 * reference is 1 and `python3 tools/datacheck.py <ref>` exits 0 with no EXPORTS
 * line.  No split, no linker-script change, no export.
 *
 * PARKED AT 102 of 426 (tools/tryc.py --align).  objcmp --whole says
 *     XX OvlFunc_957_200b644   388 of 425 differ (ours 429), first at index 3
 *     XX SIZE  ref 1004 bytes, ours 1012
 * NOT a true distance: 429 encodings against 425 and 8 bytes over.  The two
 * numbers are 4x apart because 429/425 makes objcmp's positional count nearly
 * meaningless here -- exactly the trap the brief names.  --align is the number
 * to rank against.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/tryc.py src/non_matching/ovl_7e3e08/200b644.c \
 *     asm/overlays/rom_7e3e08/ovl_30_c_c_c_c_b_a.s --align
 *
 * SHIMS: ZERO in both reported classes.  No `register ... __asm__` of my own and
 * no `__asm__(".equ ...")`.  The one `__asm__` in the file is the `.L3f6c`
 * asm-NAME binding for a data label in a sibling object (already `.global` in
 * asm/overlays/rom_7e3e08/ovl_30_c_c_c_c_b_c_dat.s), the tree's established
 * convention -- src/non_matching/ovl_7e3e08/2008f94.c and
 * src/overlays/rom_7e3e08/ovl_30_c_c_c_b.c both use it.  `DMA3_CLEAR` does carry
 * `register ... __asm__` declarations, but inside the shipped include/dma.h that
 * landed files already use; nothing was added here.
 *
 * NO .sym ENTRY IS PROPOSED and one was measured and rejected -- see THE 0x201
 * CHAIN below.  The six area ids the function compares against are already in
 * area.sym (_AREA_92, _AREA_93, _AREA_94, _AREA_95, _AREA_96, _AREA_97) and are
 * REQUIRED, with an unusually clean internal control (below).
 *
 * ========================= WHAT THE FUNCTION IS =========================
 * A per-area entry hook for overlay 957, five area tests deep.  It is the third
 * member of a family the tree has already matched: OvlFunc_957_200b598
 * (src/overlays/rom_7e3e08/ovl_30_c_c_c_b.c) and OvlFunc_957_2008a00
 * (ovl_30_c_c_a_a_a_b.c) both dispatch on the same gState halfword at +0x1C0
 * against the same _AREA_9x symbols, and OvlFunc_957_2008c2c
 * (ovl_30_c_c_a_c_c_c_c_c_c_a_b.c) already contains the five-call
 * __Func_8092950 run this function extends to ten.  Reading those three first is
 * worth more than anything else here.
 *
 * Block A remaps the SUB-AREA halfword at gState+0x1C2 from the AREA halfword at
 * gState+0x1C0 (0x93->0xa, 0x94->0x14, 0x95->0x1e, 0x96->0x28, 0x97->0x32), but
 * only when the sub-area is 0.  Then SetFlag(0x200) / ClearFlag(0x201), then four
 * independent area tests (_AREA_92, _AREA_93, _AREA_95, _AREA_97) each with its
 * own actor setup, then a final GetFlag(0x200) fork.  Returns 0.
 *
 * ===================== THE _AREA_ SYMBOLS ARE PROVEN =====================
 * The usual tell (gcc never pools a value one 8-bit `mov` can build, and the ROM
 * writes `ldr r3, =0x93`) is here, and this function adds a SECOND, independent
 * proof that does not depend on the pooling argument at all.
 *
 * Block A's tests 2..5 sign-extend a zero-extended load and compare:
 *     lsl r3, r1, #16 / asr r3, #16 / cmp r3, r2
 * With a LITERAL on the right, combine's simplify_comparison folds the `asr`
 * into the constant and emits `lsl r3, r1, #16 / cmp r3, =0x940000` -- a
 * different instruction stream.  It cannot do that to a SYMBOL_REF, so the
 * `asr` survives.  The same function shows the folded form 200 instructions
 * later, at the `(signed char) == 2` test, where the value IS a literal:
 *     mov r0, #0x80 / lsl r3, r2, #24 / lsl r0, #18 / cmp r3, r0
 * i.e. `cmp r3, 2<<24`.  One function, both branches of the rule, one page
 * apart.  That is in-function control of the kind the brief asks for.
 *
 * ============== SIX THINGS THAT WERE LOAD-BEARING, MEASURED ==============
 * Each number is a single drop against the file it was measured in; the chain
 * ran 274 -> 156 -> 147 -> 140 -> 132 -> 120 -> 107 -> 103 -> 102.
 *
 * 1. THE gState BASE MUST BE A DIFFERENT LOCAL IN BLOCK A THAN IN BLOCK B.
 *    140 -> 132.  The ROM has `ldr r2, =gState` at entry and `ldr r6, =gState`
 *    again after the two flag calls.  With ONE local `base` assigned twice there
 *    is one front-end pseudo and nothing to common or split, so gcc keeps it in
 *    one callee-saved register across everything.  Two locals (`ba`, `bb`) give
 *    two pseudos, and cse1 does not join them because the merge point after
 *    block A ends the cse path.  This is not a CSE lever -- it is a
 *    ONE-VARIABLE-PER-REGION lever, and the existing docs do not name it.
 *
 * 2. THE OFFSET LOCALS MUST BE SHARED, NOT PER-SITE, AND THERE MUST BE EXACTLY
 *    TWO.  Per-site locals (o1..o11, one per offset expression) measure 125
 *    against 102 -- a 23-instruction REGRESSION.  What matters is that the
 *    0xe0<<1 sites and the 0xe1<<1 sites use DIFFERENT locals from each other:
 *    sharing one local across both values lets `reload_cse_move2add` derive
 *    0x1c2 from 0x1c0 with `add rN, #2` / `sub rN, #2`, because move2add chains
 *    per HARD REGISTER and one local means one register.  Two locals put the two
 *    constants in different hard registers and the chain cannot form.  That is
 *    batch 293's correction #3 ("which register the derivation runs through")
 *    used as a POSITIVE lever rather than as evidence, which is new.
 *    Two residual move2add chains survive at 102 and are pure fallout of the
 *    register assignment, not of the spelling: `add r1, #0x2` where the ROM has
 *    `mov r2, #0xe1 / lsl r2, #1`, and `sub r0, #0xb9` where the ROM has
 *    `ldr r0, =0x109`.
 *
 * 3. THE SPELLED-OUT gState OFFSET IS STILL REQUIRED and the pointer form
 *    differs between the two uses.  `ob = 0xe1; ob <<= 1; ob += ba;` gives the
 *    ROM's two-address `add r0, r2`; `area = (unsigned short *)(ba + oa)` gives
 *    its three-address `add r0, r2, r3`.  Writing either inline folds the sum
 *    into a single `ldr =gState+0x1c2` (the rule recorded in
 *    src/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_c_a_c_c.c).
 *
 * 4. THE FIVE SUB-AREA STORES NEED `int` CARRIERS.  Without them the HImode
 *    store of a literal goes through `force_const_mem` and prints
 *    `ldr r3, =0xa` where the ROM has `mov r3, #0xa`; five sites, and it also
 *    rotates the pool.  `k = 0xa; *sub = k;` fixes all five.  This is the
 *    recorded HImode-literal rule, firing in the "int carrier" direction.
 *
 * 5. THE SIX-ARGUMENT CALLS NEED THEIR TWO STACK ARGUMENTS AS LIVE LOCALS.
 *    132 -> 120, three sites.  The ROM materialises both stack values BEFORE
 *    storing either -- `mov r3, #0x49 / mov r2, #0x11 / str r3, [sp] /
 *    str r2, [sp, #4]` -- which needs two registers live at once.  Passing the
 *    literals directly gives `mov r3, #0x49 / str r3, [sp] / mov r3, #0x11 /
 *    str r3, [sp, #4]`: same length, one register, wrong order.  `s5 = 0x49;
 *    s6 = 0x11; __Func_8010704(..., s5, s6);` restores it.  A `register`
 *    pin cannot reach this -- the two values are STACK arguments, so there is no
 *    hard register to name, which is why the two-live-locals form matters.
 *    Recorded because docs/elevation.md's split-constant-interleave section is
 *    about register arguments only.
 *
 * 6. `a->f23` MUST GO THROUGH ITS OWN `unsigned char *`.  132 -> 119, three
 *    sites.  Offset 0x23 is past thumb `strb`'s 5-bit immediate, so an address
 *    temporary is unavoidable; the ROM reuses the actor pointer's own register
 *    (`add r0, #0x23 / strb r5, [r0]`), ours copies it first (`mov r3, r0 /
 *    add r3, #0x23 / strb r5, [r3]`).  `p = (unsigned char *)a + 0x23; *p = 2;`
 *    lets the actor pointer die into the address.
 *
 * `volatile` ON THE SPRITE POINTER MEMBER, 103 -> 102, and it is the honest
 * spelling of a real fact rather than a trick: the ROM RELOADS `[r0, #0x50]`
 * three times in a row across stores to `spr->f9` and `spr->f26`, and gcc's
 * alias analysis proves those stores cannot touch the pointer, so it loads once.
 * Writing the three reads out explicitly is INERT (measured); untyped
 * `unsigned char *` arithmetic for the whole actor/sprite family is a
 * 28-instruction REGRESSION (160 against 132).  `struct Sprite *volatile spr`
 * is what reproduces the reloads.  It is worth one instruction and should be
 * revisited if a cheaper reading of the reloads turns up.
 *
 * =========== MEASURED AND REJECTED (all single drops, all recorded) ===========
 *   spelling                                                       --align
 *   -------------------------------------------------------------- -------
 *   baseline (natural C, one `base`, one `off`, no carriers)            274
 *   block A written as v/u variables with explicit re-reads            147
 *   block A tests 2..5 re-reading `*area` directly (KEPT)               140
 *   block 1's store through `short *` to break the alias set           140 (inert)
 *   block 1's store as `*(unsigned short *)((char *)area + 2)`         121
 *   `ewram_2001001` as a `signed char[]` with the double read          130 -> 102 combined
 *   `s->f9 = (s->f9 & ~0xd) | 4` with an `int` temp                    134 (worse)
 *   `signed char f9` field                                            102 (inert with volatile)
 *   `s->f9 = 0xc | s->f9` (operand order)                             102 (inert)
 *   `*p = t | 2` / `*p = 2 | *p`                                       103 / 102 (inert)
 *   loop's zero as a named `h = 0x80 << 9` carrier                     134 (worse)
 *   the running `q` pointer for the p[0x55]/p[0x59] pair               compile error, retry
 *   a per-site local for every gState offset                           125 (worse)
 *   a per-site local for every `g` pointer                             125 (worse)
 *   `-fno-cse-skip-blocks`                                             99  (flag, not a fix)
 *   `-fno-gcse`                                                        124 (worse)
 *   `-fno-cse-follow-jumps` / `-fno-expensive-optimizations`           102 (inert)
 *
 * ================== THE 0x201 CHAIN, AND WHY IT IS NOT THE BLOCKER ==================
 * The ROM materialises 0x201 three times (`ldr r0, =0x201` before __ClearFlag
 * and before both __SetFlag calls).  gcc materialises it twice and copies from a
 * callee-saved register at the middle site, which costs one extra instruction,
 * two replaced ones and r5.
 *
 * I LOCATED THE PASS EXACTLY.  It is cse1, not gcse.  `gcse.c`'s
 * `want_to_gcse_p` returns 0 for CONST_INT so gcse never sees it, and `-fno-gcse`
 * is a 22-instruction REGRESSION.  The `.03.cse` dump shows expand emitting three
 * separate pseudos (81, ?, 112) each `(set (reg) (const_int 513))`, and cse1
 * rewriting the MIDDLE one to `(set (reg:SI 0 r0) (reg:SI 81))` with a
 * REG_EQUAL note, while pseudo 112 -- the third site -- keeps its own constant.
 * The reason only the middle site is reached is `cse_end_of_basic_block`
 * (cse.c:6534): the run from site 1 walks forward to the first CODE_LABEL, and
 * the `if (__GetFlag(0x109) == 0)` guard's label satisfies the SKIP_BLOCKS
 * branch at cse.c:6637 -- `LABEL_NUSES == 1` and the insn before the label is
 * not itself a CODE_LABEL -- so the run jumps over it and swallows site 2.  The
 * third site sits after a label with two uses and starts a fresh run.
 * `-fno-cse-skip-blocks` confirms it: 102 -> 99, exactly the three instructions.
 *
 * AND IT IS WORTH ONLY TWO.  I probed the symbol route -- a `_CONST_201` shim on
 * the FIRST site only, so site 1 carries a SYMBOL_REF and cannot be commoned
 * with the two literals -- and it measures 100 against 102.  On the third site
 * it is inert.  So the documented-unresolved "repeated flag id" class
 * (docs/elevation.md's `OvlFunc_952_200be40` note) IS present here and IS NOT
 * the blocker, and no const.sym entry is justified: it neither completes the
 * function nor buys more than two instructions.  Recording the negative result
 * matters as much as the positive one -- the next agent should not spend the
 * batch on it.
 *
 * ================== THE ACTUAL BLOCKER, AND WHAT IS LEFT ==================
 * At 102 the structure is right end to end: frame, block order, every branch
 * polarity, the ten-call __Func_8092950 run, the actor loop, the DMA3_CLEAR, the
 * `(t << 16) / 5` through the overlay's `__divsi3 = _divsi3_RAM` alias, the
 * 0x204 store rebuilt as `add r3, #0x44` off 0x1c0 by move2add, and the return.
 * What remains is REGISTER ALLOCATION: roughly fifteen sites where the ROM and
 * gcc pick a different low register for the same instruction, in two clusters
 * that cascade from the front (block A's r1/r2 scratch alternation) and from the
 * three-actor `str r5, [r0, #0x6c]` run, where gcc inserts `mov r1, r0` because
 * the pointer has to survive the next call's argument setup.  There is no
 * single arithmetic argument that says it is unreachable, and no lever in
 * docs/elevation.md that has moved it.
 *
 * TWO CONCRETE THINGS LEFT TO TRY, in order:
 *   (a) BLOCK A'S ONE EXTRA `ldrh r1, [r0]`.  The ROM reloads the area halfword
 *       at the end of the 0x94, 0x95 and 0x96 store blocks and NOT at the end of
 *       the 0x93 block, even though all four stores go through the same r12.  We
 *       reload after all four.  This single instruction is what flips the whole
 *       r1/r2 alternation that follows, so it is worth more than its size.  I
 *       could not find a spelling: the alias-set route (`short *` for one store)
 *       is inert, the constant-offset route (`(char *)area + 2`) costs 19, and
 *       an explicit re-read makes gcc keep `u << 16` live instead of `u` and
 *       costs four `lsl`s.  A gcse/PRE dump read (`.07.gcse`) on the four store
 *       blocks is the next step and I did not do it.
 *   (b) `ewram_2001001[1]` WANTS A REGISTER INDEX.  The ROM has
 *       `mov r0, #1 / ldrsb r0, [r5, r0]`; a literal index gives
 *       `ldrb r5, [r5, #1] / lsl #24 / asr #24`, three instructions for two.
 *       Thumb has no signed load with an immediate offset, so index 0 naturally
 *       materialises a zero register (`mov r3, #0 / ldrsb r3, [r5, r3]`, which
 *       we already match) while index 1 falls back to ldrb+shifts.  A variable
 *       index is constant-propagated away.  Unsolved, worth 3.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;

extern int _AREA_92;
extern int _AREA_93;
extern int _AREA_94;
extern int _AREA_95;
extern int _AREA_96;
extern int _AREA_97;

extern unsigned char *L3f6c __asm__(".L3f6c");
extern unsigned char ewram_2001000[];
extern signed char ewram_2001001[];
extern unsigned char ewram_2001004[];
extern unsigned char *iwram_3001ebc;

struct Sprite {
    unsigned char pad[9];
    unsigned char f9;
    unsigned char pad0a[0x14];
    unsigned short f1e;
    unsigned char pad20[6];
    unsigned char f26;
};

struct Actor {
    unsigned char pad[0x18];
    int f18;
    int f1c;
    unsigned char pad20[3];
    unsigned char f23;
    unsigned char pad24[0x2c];
    struct Sprite *volatile spr;
    unsigned char pad54[0x18];
    void *f6c;
};

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8010788(int a, int b, int c, int d, int e, int f);
extern void __Func_8092950(int a, int b);
extern void OvlFunc_957_20088c0(int slot);
extern void OvlFunc_957_2008b30(void);
extern void OvlFunc_957_2008f6c(int a);
extern void OvlFunc_957_200b610(struct Actor *a);

int OvlFunc_957_200b644(void)
{
    unsigned int ba;
    unsigned int bb;
    unsigned int oa;
    unsigned int ob;
    int k;
    unsigned short *sub;
    unsigned short *area;
    int v;
    unsigned int u;
    unsigned char *g;
    unsigned char *w;
    struct Actor *a;
    struct Sprite *s;
    unsigned char *p;
    int i;
    int t;
    int c;
    unsigned int d;
    int h;
    int s5;
    int s6;

    ba = (unsigned int)&gState;
    ob = 0xe1;
    ob <<= 1;
    ob += ba;
    sub = (unsigned short *)ob;
    if (*(short *)sub == 0) {
        oa = 0xe0;
        oa <<= 1;
        area = (unsigned short *)(ba + oa);
        v = *(short *)area;
        u = *area;
        if (v == (int)(&_AREA_93)) {
            k = 0xa;
            *sub = k;
        }
        if ((short)*area == (int)(&_AREA_94)) {
            k = 0x14;
            *sub = k;
        }
        if ((short)*area == (int)(&_AREA_95)) {
            k = 0x1e;
            *sub = k;
        }
        if ((short)*area == (int)(&_AREA_96)) {
            k = 0x28;
            *sub = k;
        }
        if ((short)*area == (int)(&_AREA_97)) {
            k = 0x32;
            *sub = k;
        }
    }
    __SetFlag(0x80 << 2);
    __ClearFlag(0x201);
    bb = (unsigned int)&gState;
    oa = 0xe0;
    oa <<= 1;
    g = (unsigned char *)bb + oa;
    if (*(short *)g == (int)(&_AREA_92)) {
        ob = 0xe1;
        ob <<= 1;
        g = (unsigned char *)bb + ob;
        if (*(short *)g == 1) {
            if (__GetFlag(0x109) == 0)
                ewram_2001004[0] = 0;
            __SetFlag(0x201);
        }
        ob = 0xe1;
        ob <<= 1;
        g = (unsigned char *)bb + ob;
        if (*(short *)g == 2) {
            if (__GetFlag(0x109) == 0)
                ewram_2001004[0] = 5;
            __SetFlag(0x201);
        }
    }
    oa = 0xe0;
    oa <<= 1;
    g = (unsigned char *)bb + oa;
    if (*(short *)g == (int)(&_AREA_93)) {
        if (__GetFlag(0x962) != 0) {
            __MapActor_SetPos(8, 0, 0);
        } else {
            a = __MapActor_GetActor(8);
            s = a->spr;
            s->f9 = (s->f9 & ~0xd) | 4;
            a->spr->f26 = 2;
            a->spr->f1e = 0x80 << 7;
        }
    }
    oa = 0xe0;
    oa <<= 1;
    g = (unsigned char *)bb + oa;
    if (*(short *)g == (int)(&_AREA_95)) {
        __ClearFlag(0x80 << 2);
        OvlFunc_957_20088c0(8);
        OvlFunc_957_20088c0(9);
        OvlFunc_957_20088c0(0xa);
        if (__GetFlag(0x211) != 0) {
            __MapActor_SetAnim(0xb, 5);
            s5 = 0x49;
            s6 = 0x11;
            __Func_8010704(0x4c, 0x10, 1, 1, s5, s6);
        } else {
            a = __MapActor_GetActor(0xb);
            p = (unsigned char *)a + 0x23;
            *p |= 2;
        }
        a = __MapActor_GetActor(0xb);
        __Actor_SetSpriteFlags(a, 0);
        if (__GetFlag(0x212) != 0)
{
            s5 = 0x20;
            s6 = 0x14;
            __Func_8010704(0x1e, 0x14, 1, 1, s5, s6);
        }
        bb = (unsigned int)&gState;
    }
    oa = 0xe0;
    oa <<= 1;
    g = (unsigned char *)bb + oa;
    if (*(short *)g == (int)(&_AREA_97)) {
        __ClearFlag(0x80 << 2);
        OvlFunc_957_20088c0(8);
        OvlFunc_957_20088c0(9);
        OvlFunc_957_20088c0(0xa);
        a = __MapActor_GetActor(8);
        a->f6c = OvlFunc_957_200b610;
        a = __MapActor_GetActor(9);
        a->f6c = OvlFunc_957_200b610;
        a = __MapActor_GetActor(0xa);
        a->f6c = OvlFunc_957_200b610;
        ob = 0xe1;
        ob <<= 1;
        g = (unsigned char *)bb + ob;
        if (*(short *)g == 0x34) {
            DMA3_CLEAR(L3f6c, 0xc);
            if (__GetFlag(0x109) == 0) {
                ewram_2001000[0] = 0;
                ewram_2001000[1] = 0;
                ewram_2001000[2] = 4;
            }
        }
        c = ewram_2001001[0];
        d = ((unsigned char *)ewram_2001001)[0];
        if (c == 0x63) {
            s5 = 0x1e;
            s6 = 0x37;
            __Func_8010788(0x29, 0x37, 3, 2, s5, s6);
            s5 = 0x1f;
            s6 = 8;
            __Func_8010704(0x2a, 8, 1, 1, s5, s6);
            d = ((unsigned char *)ewram_2001001)[0];
        }
        if ((signed char)d == 2) {
            t = ewram_2001001[1];
            OvlFunc_957_2008f6c(((t << 16) / 5) + (0x80 << 7));
        }
        i = 0;
        do {
            t = i + 0xb;
            a = __MapActor_GetActor(t);
            p = (unsigned char *)a;
            p += 0x55;
            *p = 0;
            p += 4;
            *p = 0;
            a->f18 = 0x80 << 9;
            a->f1c = 0x80 << 9;
            a = __MapActor_GetActor(t);
            i++;
            __Actor_SetSpriteFlags(a, 0);
            __MapActor_SetAnim(t, i);
        } while (i <= 4);
        __Func_8092950(0xb, 1);
        __Func_8092950(0xc, 4);
        __Func_8092950(0xd, 0xb);
        __Func_8092950(0xe, 2);
        __Func_8092950(0xf, 3);
        __Func_8092950(0x10, 6);
        __Func_8092950(0x11, 6);
        __Func_8092950(0x12, 6);
        __Func_8092950(0x13, 6);
        __Func_8092950(0x14, 6);
        a = __MapActor_GetActor(0x10);
        s = a->spr;
        s->f9 |= 0xc;
        a = __MapActor_GetActor(0x14);
        s = a->spr;
        s->f9 |= 0xc;
        a = __MapActor_GetActor(0x10);
        p = (unsigned char *)a + 0x23;
        *p = 2;
        a = __MapActor_GetActor(0x14);
        p = (unsigned char *)a + 0x23;
        *p = 2;
        a = __MapActor_GetActor(0x10);
        __Actor_SetSpriteFlags(a, 0);
        a = __MapActor_GetActor(0x14);
        __Actor_SetSpriteFlags(a, 0);
    }
    if (__GetFlag(0x80 << 2) != 0) {
        OvlFunc_957_2008b30();
    } else {
        w = iwram_3001ebc;
        oa = 0xe0;
        oa <<= 1;
        p = w + oa;
        *(int *)p = 0x204;
        ob = 0xe4;
        ob <<= 1;
        p = w + ob;
        *(int *)p = 0x18;
    }
    return 0;
}
