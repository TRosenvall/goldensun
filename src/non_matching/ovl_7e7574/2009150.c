/* OvlFunc_959_2009150 -- NON-MATCHING at -O2: 40 of 180 differing (ours 182, ref 180).
 * NON-MATCHING, 173 encodings of 175.  NOT a distance under the tree's PRODUCTION flags (ref 400 bytes / 175 encodings against ours
 * 404 / 177), and that is what parkcheck measures.  `--align` 40 of 180.
 *
 * IT IS BYTE-EXACT UNDER `-fno-rerun-cse-after-loop`: 400 bytes, 175 encodings and 21
 * relocations identical.  THIS IS THE SECOND FUNCTION IN OVERLAY rom_7e7574 WITH THE IDENTICAL
 * BLOCKER AND THE IDENTICAL FIX -- its one-actor twin is src/non_matching/ovl_7e7574/2009528.c.
 * Two functions, one CSE_CFLAGS row: the owner decision should be put as a PAIR, not per
 * function, which is a materially better case than either alone.  No shims.
 *
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M`.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7e7574/2009150.c \
 *     asm/overlays/rom_7e7574/ovl_9dc_c_a_a_a_a_c_a.s --func OvlFunc_959_2009150
 * EXACT UNDER CSE_CFLAGS (-fno-rerun-cse-after-loop): 400 bytes, 175 encodings and
 * 21 relocations identical.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7e7574/2009150.c asm/overlays/rom_7e7574/ovl_9dc_c_a_a_a_a_c_a.s --func OvlFunc_959_2009150
 * (the exact result needs -fno-rerun-cse-after-loop added to objcmp's flags; a
 * wrapper that monkeypatches tryc.makefile_flags to add it, without touching
 * tools/ or the Makefile, is enough -- scratch_elev/b294/A/objcmp_cse.py.)
 *
 * THIS IS THE SECOND FUNCTION IN THIS OVERLAY WITH THE SAME BLOCKER AND THE SAME
 * FIX. src/non_matching/ovl_7e7574/2009528.c is its one-actor twin (127
 * instructions against this one's 168) and is parked on exactly this: at -O2 the
 * flag id 0x214 is commoned into r8 and costs an extra pushed register. Both are
 * exact under -fno-rerun-cse-after-loop. That is now two functions in one
 * overlay wanting one Makefile row, which is worth putting in front of the owner
 * as a pair rather than twice separately.
 *
 * THE MECHANISM, and it is narrower than "cse commons a constant" -- I traced it
 * to the PASS. `0x85 << 2` reaches five calls (__GetFlag x3, __SetFlag x2).
 * Argument precompute gives each site its own pseudo at expand (five
 * `(set (reg) (const_int 532))` in the 00.rtl dump; a thumb shiftable CONST_INT
 * costs 6, over calls.c:855's threshold of 2). cse1 leaves all five alone,
 * because `cse_main` calls `cse_basic_block (..., after_loop = 0)` and the path
 * stops at the conditional branch that separates them. cse2 -- the rerun after
 * loop -- passes `after_loop = 1`, which turns on `flag_cse_follow_jumps` and
 * `flag_cse_skip_blocks` path extension, joins the five into one pseudo live
 * across every call, and that pseudo takes r8. r8 then displaces the ROM's own
 * r8 and r10 quantities to r9 and r10, adds a third saved register to the
 * prologue, and the whole 40 is that one substitution plus its renumbering. The
 * count goes 180 -> 182 because the saved `lsl` at four sites does not pay for
 * the r8 setup and the extra push/pop.
 *
 * SO THE SOURCE CANNOT REACH IT. Two uses of the constant sit in the SAME source
 * statement pair with only a `cmp`/`bne` between them; nothing a C author writes
 * puts a multiply-referenced label between them, which is the only thing
 * `cse_end_of_basic_block` stops the path on. Confirmed by measurement, and it
 * reproduces the twin's finding independently: giving each of the five uses its
 * own named local scores 40 of 180 -- IDENTICAL, not merely close, to the
 * literal spelling. The twin recorded the same inertness over 15 combinations.
 *
 * EVERYTHING ELSE IN THE FUNCTION IS CLOSED. Under the flag it is byte-exact,
 * and three levers got it there from a first candidate at 50 of 180:
 *
 *  1. THE THREE HALFWORD STORES CROSS-JUMP INTO ONE `strh r3, [r2]`, and getting
 *     the register ROLES right is what merges them. Measured under the flag:
 *
 *       one shared `p`/`v` pair, pointer assigned first     13 (roles swapped)
 *       the same with the value assigned first              14
 *       only the value named                                14
 *       only the pointer named                              43
 *       shared `p`, a SEPARATE value local per site,
 *         value assigned first                               6
 *       shared `p`, a separate value local per site,
 *         pointer assigned first                           MATCH
 *       separate POINTER per site, shared value            20
 *
 *     The rule: the POINTER is one variable used at all three sites (so it is a
 *     global allocno and takes r2), each VALUE is its own block-scoped local (so
 *     local-alloc gives each r3, REG_ALLOC_ORDER's first choice), and within
 *     each site the pointer is assigned BEFORE the value. Assigning the value
 *     first lets cross-jumping swallow the `add r2, r8` into the shared tail as
 *     well -- the ROM keeps that add in each arm -- which is the same sinking
 *     the twin's note warns about, here with the direction identified: it is
 *     the ORDER of the two assignments that decides whether the add is in the
 *     tail. Swapping the declaration order of `p` and `v` is inert (13 either
 *     way); it is the assignment order that matters, not the declaration order.
 *
 *  2. EACH ARM OF THE gState CHAIN NEEDS ITS OWN `g = gState;` -- the twin's
 *     lever, and this function has three arms rather than two. The ROM reloads
 *     `=gState` fresh at the second and third tests while keeping the FIRST
 *     test's address in r5 across its own re-test; that is what per-arm locals
 *     plus one outer `g` produce.
 *
 *  3. The soft-float trio is the documented shape:
 *     `OvlFunc_common2_380 (OvlFunc_common2_28c (0x41610000, OvlFunc_common2_304 (x)))`
 *     with `long long` bit patterns (fixdfsi / subdf3 / floatsidf). No
 *     `__attribute__((const))` is needed here -- unlike
 *     src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_c_c_a_a.c, where it was the lever --
 *     because there is only one such expression and no argument constants
 *     competing for r0-r3.
 *
 * Structs and the iwram_3001e70[0] / [0x13] spelling are the twin's; `[0x13]`
 * is the +0x4c word, which is why the ROM reaches iwram_3001ebc through
 * `ldr r2, =iwram_3001e70 / ldr r2, [r2, #0x4c]` and not through its own symbol.
 *
 * SHIMS: none. No `register ... __asm__` pins, no `__asm__(".equ ...)` lines, no
 * inline asm of any kind.
 */
struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x5b - 0x14];
    unsigned char f5b;
};

struct S {
    unsigned char pad00[0x18];
    int f18;
    int f1c;
    int f20;
    int f24;
};

extern unsigned char gState[];
extern unsigned char *iwram_3001e70[];
extern volatile int iwram_3001e40;
extern struct Actor *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern int OvlFunc_959_2009108(void);
extern int OvlFunc_959_2009918(int slot);
extern unsigned int OvlFunc_959_20098e4(unsigned int slot);
extern long long OvlFunc_common2_304(int x);
extern long long OvlFunc_common2_28c(long long a, long long b);
extern int OvlFunc_common2_380(long long v);

void OvlFunc_959_2009150(void)
{
    struct Actor *a;
    struct Actor *c;
    struct S *s;
    unsigned char *b;
    unsigned char *g;
    unsigned short *p;

    a = __MapActor_GetActor(9);
    c = __MapActor_GetActor(0xa);
    s = (struct S *)(iwram_3001e70[0] + (0xb2 << 1));
    b = iwram_3001e70[0x13];
    if (iwram_3001e40 & 1) {
        s->f18 = 1;
        s->f1c = 1;
    } else {
        s->f18 = -1;
        s->f1c = -1;
    }
    if (__GetFlag(0x83 << 1) != 0 || *(short *)(b + (0xbf << 1)) != 0
        || *(short *)(b + (0xc0 << 1)) != 0) {
        a->f5b = 1;
        c->f5b = 1;
        return;
    }
    if (__GetFlag(0x85 << 2) != 0)
        return;
    a->f5b = 0;
    c->f5b = 0;
    if (__GetFlag(0x85 << 2) == 0 && a->f5b == 0)
        s->f20 = OvlFunc_common2_380(OvlFunc_common2_28c(0x41610000, OvlFunc_common2_304(a->f8)));
    if (OvlFunc_959_2009108() != 0)
        return;
    g = gState;
    if (*(short *)(g + (0x93 << 2)) != 0) {
        if (OvlFunc_959_2009918(9) != 0 && *(short *)(g + (0x93 << 2)) != 0) {
            int va;
            p = (unsigned short *)(b + (0xbf << 1));
            va = 0x2092;
            *p = va;
            return;
        }
        if (OvlFunc_959_2009918(0xa) != 0) {
            unsigned char *g2 = gState;
            int vb;
            if (*(short *)(g2 + (0x93 << 2)) == 0)
                goto rest;
            p = (unsigned short *)(b + (0xbf << 1));
            vb = 0x2092;
            *p = vb;
            return;
        }
        {
            unsigned char *g3 = gState;
            if (*(short *)(g3 + (0x93 << 2)) != 0)
                goto check;
        }
    }
rest:
    if (OvlFunc_959_20098e4(9) != 0) {
        __SetFlag(0x215);
        __SetFlag(0x85 << 2);
    }
    if (OvlFunc_959_20098e4(0xa) != 0) {
        __SetFlag(0x215);
        __SetFlag(0x85 << 2);
    }
check:
    if (__GetFlag(0x85 << 2) != 0) {
        int vc;
        p = (unsigned short *)(b + (0xc1 << 1));
        vc = 0x5b;
        *p = vc;
    }
}
