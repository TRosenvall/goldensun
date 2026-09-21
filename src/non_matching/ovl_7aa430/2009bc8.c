/* OvlFunc_923_2009bc8 -- NOT MATCHING
 * OvlFunc_924_200d158 -- the same function in a different overlay, differing
 *                        only in the script symbol, so this park covers two.
 *
 * OvlFunc_907_2008f3c -- a THIRD member, batch 99, same blocker at 7 of 46.
 *
 * Source asm: goldensun/asm/overlays/rom_7aa430/ovl_1a3c_a_a_a.s
 *             goldensun/asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_c_a.s
 *             goldensun/asm/overlays/rom_79b154/ovl_30_c_c.s
 * Best screen: 7 differing of 39, streams the same length.
 *
 * OvlFunc_907_2008f3c is the same routine with one extra masked byte at the
 * sprite's +5 (`mask & s->f5 | 4`, the named-int form). It reaches EXACTLY the
 * same 7 differing positions with exactly the same spelling, which is a useful
 * confirmation: the residue is the two pointer chains and nothing to do with
 * what the sprite branch does afterwards. Two further spellings were measured
 * on it and both are worse -- naming the 1 and 2 as `unsigned char` (11), and
 * storing +0x22 before +0x55 (10) -- matching what the other two members do.
 *
 * BLOCKER CLASS: register allocation, r2 against r3, on two pointer chains.
 *
 * TWO LEVERS GOT IT FROM 26 TO 7 and both are new, so they are the reason this
 * is worth reading:
 *
 *   TWO POINTERS MUST BOTH BE LIVE, OR GCC DERIVES ONE FROM THE OTHER. The
 *   function writes a byte at +0x55 and two more at +0x22 and +0x23. Writing
 *   the first as a struct field and the others through a walked pointer gives
 *
 *       mov r2, r5 / add r2, #0x55 / strb / sub r2, #0x33 / strb ...
 *
 *   -- ONE register walked backwards, because 0x55 - 0x33 is 0x22 and gcc will
 *   find that. The ROM keeps two independent chains (`mov r3, r5 / add r3,
 *   #0x55` and `mov r2, r5 / add r2, #0x22`). Naming both as separate pointer
 *   locals is not enough on its own; they have to be COMPUTED BEFORE the first
 *   store, so that both are live across it. 26 differing to 10.
 *
 *   THE STORE ORDER THEN DECIDES WHICH CHAIN IS WHICH. With both pointers
 *   computed up front, storing to +0x55 first gives the ROM's order; storing to
 *   +0x22 first swaps the two chains. 10 to 7.
 *
 * What remains is the r2/r3 exchange itself -- the ROM puts the +0x55 chain in
 * r3 and the +0x22 chain in r2, and we have them the other way round. That is
 * the same four-member class documented in docs/elevation.md.
 *
 * WORTH TRYING NEXT: batch 97 found that for
 * src/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_a_a_c_b.c the exchange was reachable
 * through the TYPE of a named constant -- `unsigned char two = 2` matched where
 * `int two` did not. Nothing here is a named constant of that shape, but the
 * three stored values (0, 1, 2) are all candidates.
 *
 * The shared zero IS forced: it is written to +0x55 and again to the sprite's
 * +0x26 after a call, so it crosses a call and a bare literal would be rebuilt.
 * That is batch 95's discriminator and it is satisfied below.
 */

/* ===================== BATCH 278 UPDATE (below) =====================
 * The header above is RETAINED IN FULL and its two levers are still required -- the
 * batch-278 candidate below keeps both. What changed is the diagnosis of the residue
 * that remained, which is no longer "the r2/r3 exchange" at all.
 *
 * AND NOTE THE THIRD MEMBER RECORDED ABOVE: OvlFunc_907_2008f3c in
 * asm/overlays/rom_79b154/ovl_30_c_c.s reaches the SAME 7 differing positions with the
 * same spelling. tools/dupfuncs.py does NOT report it in this group, because it has one
 * extra masked byte at the sprite's +5 and so is not byte-identical -- so dupfuncs
 * UNDERCOUNTS what a single solution converts. Solving this closes three functions, not
 * two. Worth remembering when reading that tool's group sizes.
 * ================================================================== */

/* OvlFunc_923_2009bc8 (0x02009bc8) -- NON-MATCHING, 7 of 39. Ties the batch-99 park.
 * Blocker class: NEW -- A DEAD INSN LEFT BEHIND BY combine STILL TAKES A HARD REGISTER.
 *
 * asm/overlays/rom_7aa430/ovl_1a3c_a_a_a.s (2 functions, so landing needs a split).
 * TWIN: OvlFunc_924_200d158 in asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_c_a.s -- 39 vs 39 lines,
 * ONE line differs (the per-overlay script symbol). Solving this converts both.
 *
 * ===== THIS IS NO LONGER "AN r2/r3 EXCHANGE". IT IS ONE DEAD INSTRUCTION. =====
 *
 * `-da` traces it end to end:
 *   .00.rtl   expand emits `(set (reg:QI T) (subreg:QI (reg:SI zero)))` before the f55 store.
 *   .03.cse   rewrites it to `(set (reg:QI T) (const_int 0))`.
 *   .13.combine substitutes the subreg straight into the store and leaves insn T REG_UNUSED.
 *   ...and GCC-2.96 RUNS NO FLOW PASS BETWEEN combine AND ALLOCATION, so the dead insn is
 *   STILL THERE at .17.lreg.
 *
 * `local_alloc` then hands it r3 (first in ARM's REG_ALLOC_ORDER 3,2,1,0,...), and `.18.greg`
 * shows the hard conflict on BOTH address pseudos:
 *
 *     ;; 42 conflicts: 33 34 35 36 37 42 3 13      <- 42 is the dead QImode zero
 *     ;; 36 conflicts: 33 34 35 36 37 42 3 13      <- 36 is &n->f55, forced off r3 by it
 *
 * The constants 1 and 2, which also want r3, do NOT conflict with the f55 pointer. So if that
 * one dead insn were gone, `&n->f55` would take r3 and the function would be EXACT -- the
 * `mov r3,#1` that sched2 hoists above the store could not move, because `strb r7,[r3]` would
 * anti-depend on it.
 *
 * THE RESIDUE IS THEREFORE DOWNSTREAM OF A gcc-2.96 ARTEFACT, not of the source, unless some
 * spelling stops combine folding the subreg into the store. None was found.
 *
 * HOW TO DIAGNOSE THIS CLASS ELSEWHERE: an address pseudo that cannot get r3 for no visible
 * reason. Grep `.17.lreg` for REG_UNUSED and cross-check the `conflicts` lines in `.18.greg`.
 * This is a THIRD distinct allocation entry point beyond the priority formula and the
 * local_alloc-claims-first case -- here the competitor is not a real value at all.
 *
 * MEASURED (39-instruction stream unless noted): park baseline 7; `q` before f55 plus a plain
 * field store 7; int/unsigned/char/unsigned int for the zero 7; the zero assigned first 7; a
 * literal at f55 with a named zero at f26 7; `(unsigned char *)n + 0x55` 7; early-return style
 * 7; BOTH ZEROS LITERAL 10 (loses r7 entirely, which confirms the shared zero is forced);
 * `*q=1` before `*r=zero` 8; two separate pointers 11; `s` loaded after Actor_SetScript 12;
 * named `one`/`two` 13.
 *
 * TWO LOWER COUNTS THAT ARE WORSE OBJECTS, recorded so they are not chased: f22/f23 as plain
 * struct fields scores 6 but is 38 instructions using the `sub r2,#0x33` derived chain, and
 * "q computed after the f55 store" is 6 at 38 instructions. Count-is-not-a-difference.
 *
 * FLAGS, all inert or worse: -fno-gcse 7, -fno-strict-aliasing 7, -fno-rerun-cse-after-loop 7,
 * -fno-cse-follow-jumps 7, -fno-cse-skip-blocks 7, -fno-strength-reduce 7, -fno-defer-pop 7,
 * -fno-thread-jumps 7, -fno-delete-null-pointer-checks 7, -fno-schedule-insns2 9,
 * -fno-expensive-optimizations 9, -O1 9, -ffixed-r7 14.
 *
 * NEXT: a spelling that stops combine folding the subreg. Nothing else is left.
 */
struct Spr {
    unsigned char pad00[9];
    unsigned char f9;
    unsigned char pad0a[0x26 - 0xa];
    unsigned char f26;
};

struct A {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x22 - 0x14];
    unsigned char f22;
    unsigned char pad23[0x50 - 0x23];
    struct Spr *f50;
    unsigned char pad54[1];
    unsigned char f55;
};

extern unsigned char gScript_923__0200a7b8[];
extern unsigned char gScript_924__0200de08[];
extern struct A *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetScript(struct A *a, unsigned char *s);
extern void __Sprite_SetAnim(struct Spr *s, int n);

void OvlFunc_923_2009bc8(struct A *src)
{
    struct A *n;
    struct Spr *s;
    unsigned char *q;
    unsigned char *r;
    unsigned char zero;

    n = __CreateActor(0x18, src->f8, src->fc, src->f10);
    if (n != 0) {
        s = n->f50;
        __Actor_SetScript(n, gScript_923__0200a7b8);
        r = &n->f55;
        q = &n->f22;
        zero = 0;
        *r = zero;
        *q = 1;
        q += 1;
        *q = 2;
        if (s != 0) {
            __Sprite_SetAnim(s, 2);
            s->f26 = zero;
            s->f9 |= 0xc;
        }
    }
}

