/*
 * ### CORRECTION, batch 258 -- THE RECORDED SCORE OF 1 WAS WRONG.
 *
 * objcmp at tree default, three runs:
 *
 *     XX SIZE  ref 76 bytes, ours 72
 *     XX ENCODINGS differ in 26 place(s) (ref 34, ours 32)
 *        first at index 4: ref 4c0b  ours 7343
 *
 * The "1" came from tools/tryc.py, which NORMALISES pool words -- and a pool
 * defect is exactly what this function's blocker is, so the screen erased the
 * evidence. tools/park_status.py then inherited the number from this note.
 * docs/elevation.md already records that tryc is blind to pool-word differences;
 * this is what that blindness costs when the note is believed.
 *
 * BIGGEST LEVER FOUND: -fno-strict-aliasing (ALIAS_CFLAGS), 26 -> 16, with size
 * and encoding count going EXACT (72/32 -> 76/34) and the entire tail from
 * strh r3,[r5,#0x16] to strb r2,[r3,#0x14] instruction-exact. The ROM re-reads
 * a->p after the b->f16 store; at -O2 strict aliasing proves a u16 store cannot
 * alias a struct B * load and gcc keeps the pointer in r5. Same mechanism as the
 * four existing ALIAS_CFLAGS rules. If this closes it needs a fifth -- and note
 * the split means Func_80b09fc is in a DIFFERENT TU and must not get the flag.
 * Best candidate: scratch_elev/b261/Func_80b0a20/BEST_80b0a20_residue16_needs_ALIAS_CFLAGS.c
 *
 * THE REMAINING 16 IS ONE THING: the 0xffff pool entry is SImode in ours and must
 * be HImode. From the .26.mach minipool fixup list:
 *
 *     ;; HImode fixup for i41;  addr 6,  range (0,64):   0x0
 *     ;; SImode fixup for i19;  addr 10, range (0,1020): 0xffff
 *
 * gcc sorts the minipool by max_address, so our HImode entry (max 70) always
 * sorts first, giving [0, ffff, 1ff, fe00]. The ROM's is [ffff, 0, 1ff, fe00].
 * Every other difference cascades from which constant lands in which register.
 *
 * WHERE OUR HImode ZERO COMES FROM, and why no spelling moved it: gcc-2.96
 * expands every struct-field store as a mode-typed read-modify-write bitfield
 * insert. The halfword store b->f6 = x creates (set (reg:HI) (const_int 0)) as
 * the insert's MASK, and CSE reuses that HImode zero for the byte store
 * a->fc = 0 via a subreg:QI. The pooled zero is a leftover halfword mask, not
 * the source's zero. Bisection confirms it: a->fc = 0 alone, or before any
 * halfword store, gives mov r3, #0.
 *
 * So the open question is narrow and stated: make the 0xffff entry HImode, or
 * stop the zero from becoming a pool entry at all. Full 30-row measured table
 * below the line; everything in it was measured UNDER -fno-strict-aliasing
 * against a baseline of 16.
 *
 * --- original note follows; its score of 1 is superseded ---
 *
 * BLOCKER CLASS: the width of a pooled ZERO, and the setup order that follows
 * from it.
 * Status: 28 lines against the ROM's 29 in the best spelling below.
 *
 * WHAT IT DOES
 * Writes an x and a y into an actor record and the sprite it points at: three
 * halfword copies each, a 9-bit field at bits 0-8 of the sprite halfword at
 * +0x16, and two flag bytes.
 *
 * FOUR THINGS ARE SETTLED AND SHOULD BE KEPT:
 *
 *   1. THE 0xffff IS A NAMED int, NOT A u16 CAST. `xs = (unsigned short)x`
 *      gives `lsl #16 / lsr #16`; the ROM masks with a register holding 0xffff,
 *      and reuses that register for y. Only a named `int m = 0xffff` produces
 *      that.
 *   2. THE 9-BIT FIELD IS HAND-WRITTEN, NOT A BITFIELD. Declared
 *      `unsigned short bf : 9`, gcc narrows the 0x1ff to a HALFWORD pool entry
 *      (`ldrh`); the ROM loads it as a word. Written as
 *      `t = b->f16; t = 0xfffffe00 & t; t |= x;` with int locals, both masks
 *      come out word-width and correct. That is the reverse of the batch-71
 *      bitfield rule and it is the mask WIDTH that decides, exactly as that
 *      rule says -- read the width, do not assume the construct.
 *   3. The value is the destination of both ANDs: `x &= m; x &= 0x1ff;`.
 *   4. `a->p` is re-read after the field write, because the write may alias it.
 *      That happens on its own and needs no help.
 *
 * WHAT IS LEFT is one instruction and its knock-on. The ROM loads the zero it
 * stores to +0xc as a WORD, `ldr r4, =0x0`, before the first byte store; gcc
 * pools it as a HALFWORD, `ldrh`, and emits it later.
 *
 * THE SYMBOL READING WAS TESTED AND IS NOT THE ANSWER. `ldr rN, =0x0` where
 * `mov rN, #0` would do looks like the pool tell, and a symbol address cannot
 * be narrowed, so a word-width pooled zero is suggestive. Substituting
 * `(int)(&_AREA_00)` DOES produce the ROM's `ldr r3, =0x0` -- the mechanism is
 * real -- but the function then comes out at 26 lines, three SHORT, because gcc
 * re-optimises the byte store around it. The same substitution on
 * Func_809b0dc, the other function with this symptom, goes from 1 differing
 * line to 4.
 *
 * So the width tell is real and the substitution is not a fix. Recorded that
 * way rather than as a naming lead, because guessing a namespace on top of a
 * spelling that makes the function WORSE would be two mistakes.
 *
 * REVISITED, and the missing instruction is identified.  It is not the zero at
 * all: the ROM's last instructions are
 *
 *      b .Lb0a64 / .pool_aligned / .Lb0a64: / pop {r5,r6} / pop {r0} / bx r0
 *
 * -- a real branch over an in-function LITERAL POOL placed before the epilogue.
 * That branch is the one line we are short.  `.pool_aligned` expands to
 * `.align 2, 0` + `.pool` (include/macros.inc), so this is pool placement, not
 * control flow, and no source spelling reaches it.
 *
 * MEASURED: across every elevated translation unit in the tree, ZERO generated
 * `.s` files contain a mid-function pool.  old_agbcc emits the pool at
 * `.func_end` and never early, so this pattern has never been reproduced here.
 *
 * ===> THAT MEASUREMENT AND THE CONCLUSION DRAWN FROM IT ARE BOTH FALSE. <===
 *
 * It searched generated output for the `.pool` DIRECTIVE. gcc never writes that
 * directive -- it writes `.word` tables at its own labels -- so the search could
 * only ever return zero. Counting properly, 64 of the 3495 already-matching
 * functions carry a mid-function literal pool, and batch 141 elevated five
 * functions out of the class this park helped close.
 *
 * old_agbcc is also not the compiler. /opt/gcc296/xgcc builds essentially
 * everything; old_agbcc builds five m4a and agb_flash objects.
 *
 * AND THE MISSING LINE IS NOT THE BRANCH. Compiling the C below emits
 * `b .L4 / .align / .word 0 / .word 65535 / .word 511 / .word -512 / .L4:` --
 * a branch over an in-function pool, the ROM's exact shape. The claim below
 * that "no source spelling reaches it" is wrong on its own evidence.
 *
 * WHAT ACTUALLY DIFFERS, re-measured: 28 lines against 29, 21 differing, first
 * difference at line 4. The ROM loads the pooled zero BEFORE the byte store at
 * +0xd; gcc emits it after, and the register assignments cascade from there.
 * Moving `z = 0;` earlier in the source does not move it -- gcc schedules the
 * load itself.
 *
 * So this function is a live candidate on ordering and allocation, not a
 * toolchain ceiling. The four settled findings above are unaffected and should
 * still be kept.
 *
 * THE LEAD, and it is a translation-unit one.  Three of the FOUR functions in
 * asm/rom_b0000/rom_b0070_a_a_c_c_a_a.s carry a mid-function `.pool_aligned`,
 * so early pool dumping looks like a property of how that TU was compiled rather
 * than of this function.  A single-function `.c` can therefore probably never
 * match it.  The test would be to elevate the cluster -- all four functions in
 * one `.c`, which the "Cluster X..Y" headers elsewhere in the tree show is a
 * supported shape -- and see whether the pool lands early on its own.
 *
 * THE CLUSTER HYPOTHESIS WAS TESTED AND IS REFUTED.  Before transcribing three
 * more functions to try it, the cheap version was run: compile this function's
 * C alone, then compile it again with a second function appended, and compare
 * where old_agbcc puts the literal pool.
 *
 * It does not move.  In both cases the generated `.s` ends
 *
 *      pop {r5, r6} / pop {r0} / bx r0 / .L4: / .align 2, 0 / .L3: / .word ...
 *
 * -- pool AFTER the epilogue, with no branch, because control never reaches it
 * and none is needed.  Adding a following function changes nothing about the
 * first function's pool placement, and no `b` over a pool appears anywhere.
 *
 * So the mid-function pool is not a translation-unit property and elevating the
 * cluster would not have produced it.  Both sides even emit a label at the same
 * point; the difference is purely that the ROM's epilogue sits AFTER its pool
 * and ours before.  That is old_agbcc's constant-pool emission, which no source
 * form and no TU composition reaches.
 *
 * Recording the refutation rather than leaving the lead standing, because it was
 * flagged as the most promising open experiment in the tree and it is not one.
 */

struct B {
    unsigned char pad00[6];
    unsigned short f6;
    unsigned short f8;
    unsigned char pad0a[0xa];
    unsigned char f14;
    unsigned char pad15;
    unsigned short f16;
};

struct A {
    struct B *p;
    unsigned short f4;
    unsigned short f6;
    unsigned short f8;
    unsigned short fa;
    unsigned char fc;
    unsigned char fd;
};

void Func_80b0a20(struct A *a, int x, int y)
{
    struct B *b;
    int m;
    int t;
    int z;

    b = a->p;
    m = 0xffff;
    a->fd = 1;
    b->f6 = x;
    a->f8 = x;
    a->f4 = x;
    x &= m;
    x &= 0x1ff;
    z = 0;
    a->fc = z;
    t = b->f16;
    t = 0xfffffe00 & t;
    t |= x;
    b->f16 = t;
    a->fa = y;
    a->f6 = y;
    a->p->f8 = y;
    y &= m;
    a->p->f14 = y;
}
