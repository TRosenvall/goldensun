/* DrawMsgGlyph -- NON-MATCHING.
 * 33 of 197 encodings differ (ref 197, ours 197).  INSTRUCTION COUNTS NOW
 * EQUAL at 176/176, size 436/436, pool words and pads identical.  Batch 345
 * brief C took this from 84 of 197 (gap -1 instruction) to 33 of 197 at a
 * CLOSED gap; aligned residue 149 -> 164 of 197 equal (75.6% -> 83.2%),
 * 59 -> 33 differing, 28 -> 27 hunks.  PINS 0.
 *
 * Verify with (the PARK path, this file):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/80178b0.c \
 *     asm/rom_15000/rom_178b0_a.s --func DrawMsgGlyph
 * Aligned view (the number to move, objcmp's is positional):
 *   ... python3 tools/aligncmp.py src/non_matching/rom_15000/80178b0.c \
 *     asm/rom_15000/rom_178b0_a.s DrawMsgGlyph -v
 * ONE function, no data sections -- converts whole when it lands, no split.
 *
 * ===================================================================
 * BATCH 345: THE -1 WAS TWO SITES, +2 AND -1.  BOTH ARE CLOSED.  THE WHOLE
 * REMAINING RESIDUE IS ONE ALLOCNO PAIR.
 * ===================================================================
 *
 * PER-REGION SPLIT of the parked body, off an index-aligned dump:
 *
 *   prologue + first glyph + blit blocks 1 and 2 (idx 0..123)   124 : 124   0
 *   SITE A  `v = v + 1` narrowing            (ref idx 124..130)   7 :   5  ref +2
 *   SITE B  `p = buf + w` in the 4th block   (ref idx 132..134)   3 :   4  ours +1
 *   join + the 32-word nibble loop nest + epilogue               55 :  55   0
 *
 * A net gap of -1 was NOT one site.  The loop nest the park says matches does
 * match; the gap is one +2 in the second glyph's `if` arm and one -1 in its
 * `else` arm.
 *
 * SITE B -- `p = buf; p += w;` IN THE ELSE ARM ONLY.  The ROM has
 * `add r5,sp,#4 / add r5,r8`, the TWO-OPERAND destructive Thumb alternative
 * (`add Rd,Hs`), whose constraint is `%0` against a high register and which
 * therefore needs operands 0 and 1 in the same hard register after reload.
 * From the single expression `p = buf + w` gcc emits the three-operand form
 * `add r3,sp,#4 / mov r1,r8 / adds r6,r3,r1`.  Two statements make `p`'s
 * pseudo the destination of its own increment and the destructive alternative
 * matches.  THE `if` ARM MUST KEEP THE ONE-EXPRESSION FORM: the ROM uses the
 * three-operand shape there.  The ROM's two arms are spelled differently and
 * that asymmetry is load-bearing.
 * Effect alone: 175 -> 174 instructions, figure 84 -> 98, aligned 149 -> 143.
 * A WORSE FIGURE AT A WORSE GAP, and the site is nevertheless fixed -- do not
 * re-reject it on the figure.
 *
 * SITE A -- THE ROM DISTRIBUTES THE SHIFT OVER THE `+1`.  `combine.c`'s
 * `(ashift (plus foo C) N) -> (plus (ashift foo N) C')` rewrite (the PLUS case
 * of simplify_shift_const, gated on merge_outer_ops succeeding) gives the ROM
 * `mov r0,sl / movs r2,#128 / lsls r3,r0,#16 / lsls r2,r2,#9 / adds r3,r3,r2 /
 * asrs r3,r3,#16`; ours was the undistributed `adds r3,#1 / lsls / asrs`.
 * NINE spellings swept and EIGHT ARE BIT-IDENTICAL to `v = v + 1`: `v++`,
 * `v += 1`, `v = (short)(v+1)`, `v = v - -1`, `v = (unsigned short)v + 1`, and
 * an int temp (that one differs only in hunk count).  The park's "four
 * spellings are all identical or worse" is confirmed and extended to nine.
 * What works is the distributed form WITHOUT AN OUTER CAST:
 *     v = (((int)v) << 16) + 0x10000 >> 16;
 * The park records "the distributed form written out" as inert; the spelling it
 * must have used is `v = (short)((((int)v) << 16) + 0x10000) >> 16;`, measured
 * here at 172 instructions (gap -4) and 105 differing -- the outer `(short)`
 * cast is the whole difference, because it re-narrows before the assignment's
 * own narrowing and combine folds the pair away.
 * 175 -> 176 instructions (GAP CLOSED), figure 98 -> 47.
 * LABELLED PASS-3 DEBT: this line is a matching artifact, not plausible
 * human source.  It is NOT a device -- no fictitious symbol, no .equ, no
 * union, no volatile, no inline asm; shimcount reports 0.
 *
 * THE LOOP POINTER MUST BE ITS OWN LOCAL -- THE PARK'S NEGATIVE HERE IS DEAD.
 * Park: "Giving the loop pointer its OWN local instead of reusing the blit
 * blocks' pointer was measured and is WORSE (84 -> 95), so the shared name is
 * not the cause."  That was measured on a body one instruction short, with the
 * saturating positional figure.  Re-measured on the gap-closed body:
 *     shared `p`     176 insns   47 differing   aligned-eq 155
 *     separate `q`   176 insns   33 differing   aligned-eq 164
 * THE ROM PUTS THE LOOP POINTER IN r1 -- caller-saved, and the loop nest has no
 * calls -- and the blit pointer in r5/r6, callee-saved.  Two storage classes
 * means two variables.  With `q` the whole five-pseudo nest matches the ROM
 * exactly (`q` r1, `z` r0, `k` r4, `j` r5, `i` r6), which the park had recorded
 * as "rotated by one position in REG_ALLOC_ORDER".
 *
 * WHAT IS LEFT: ONE ALLOCNO PAIR, AND EVERY ONE OF THE 33 IS IT.
 *
 *     ROM   ctx (iwram_3001e8c) -> r6     g (glyph pointer) -> r7
 *     ours  ctx                 -> r7     g                 -> r6
 *
 * with two consequences: the 4th blit block's `p` (ROM r5, ours r7) and its
 * function pointer (ROM r6, ours r5), which is the only remaining reason the
 * relocations read `_call_via_r5` where the ROM reads `_call_via_r6`.
 * NO INSTRUCTION IS MISSING, SPARE, OR OUT OF ORDER ANYWHERE IN THE FUNCTION.
 * `allocno_compare` (global.c:597-619) is
 * floor_log2(n_refs)*n_refs/live_length*10000*size, ties on ascending allocno
 * number; `g` has ~14 references to `ctx`'s ~5 so `g` out-prioritises `ctx` and
 * takes r6 (REG_ALLOC_ORDER is 3,2,1,0,12,14,4,5,6,7,... at arm.h:988-993).
 * The ROM has the reverse.
 *
 * MEASURED AND INERT OR WORSE on the gap-closed body (figure, positional):
 *   declaration order `g` before `ctx`                 33  (BIT-IDENTICAL)
 *   `ctx` declared last of the pointers                33  (BIT-IDENTICAL)
 *   the second glyph pointer split into its own `g2`    89
 *   BOTH CHEAP AXES ARE CLOSED (tools/autosweep.py, base 33):
 *     flag axis   -O1 133, -fno-gcse 33, -fno-schedule-insns2 93,
 *                 -ffixed-r7 200, -fno-strict-aliasing 33,
 *                 -fno-strength-reduce 33, -fno-rerun-cse-after-loop 33,
 *                 -fno-rerun-loop-opt 33
 *     anchor axis 47 of 55 boundaries measured, all inert or worse, best 33
 *
 * NEXT: route 3 or route 5 of the batch-345 allocno table -- add a definition
 * so `g`'s range splits WITHOUT becoming two names (route 1, splitting `g`, is
 * now a measured negative at 89), or lengthen `ctx` so its own hard-register
 * preference is pruned (global.c:1630-1642, :897-909, :1016).  Declaration
 * order is exactly inert here, twice over.
 *
 * ===================================================================
 * THE ORIGINAL PARK HEADER, CARRIED FORWARD UNCHANGED.  Its four levers are
 * all still in the body; its loop-pointer negative and its "four spellings"
 * row are superseded above.
 * ===================================================================
 * DrawMsgGlyph -- NON-MATCHING, 84 encodings of 197 against the tree reference.
 * SIZE EXACT (436 bytes both), ENCODING COUNT EXACT (197 = 197), FRAME EXACT
 * (sub sp,#0x184).  Relocations differ (see below).  NO SHIMS.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_15000/80178b0.c \
 *     asm/rom_15000/rom_178b0_a.s
 * ONE function, no data sections -- converts whole when it lands, no split.
 *
 * THE WHOLE 32-WORD NIBBLE-PACKING LOOP NEST MATCHES INSTRUCTION FOR INSTRUCTION,
 * and so do both `Func_8000984` blocks of the first glyph and the third block of
 * the second.  Ladder: 178 -> 183 -> 189 -> 149 -> 145 -> 135 -> 84.
 *
 * FOUR LEVERS, each measured:
 *
 * 1. FOUR SEPARATE FUNCTION-POINTER LOCALS, ONE PER BLOCK.  The ROM reloads
 *    `ldr r5,=Func_8000984` (and once `ldr r6,=...`) at the head of each of the
 *    four blit blocks.  ONE pointer local assigned at the top of the function gets
 *    a callee-saved register (r8) and steals it from `w`; ONE local reassigned at
 *    the head of each block is rematerialised at EVERY CALL (+4 instructions,
 *    444 bytes).  FOUR DISTINCT LOCALS, each assigned in its own block, give four
 *    short-lived allocnos that each win a caller-saved register -- which is what
 *    the ROM has, and it is why the ROM uses r5 in three blocks and r6 in the
 *    fourth.  189 -> 149 differing, 201 -> 197 encodings.
 *    **Read the ROM's per-block register: a different register per block means a
 *    different variable per block, not a rematerialised one.**
 * 2. `unsigned int` FOR THE CHARACTER CODE, so `Data_32224 + (c - 0x20) * 0x20`
 *    does NOT fold into `(Data_32224 - 0x400) + c * 0x20`.  Signed `int c` folds
 *    and costs the ROM's `mov r3,r5 / sub r3,#0x20` pair.  Same fact the sibling
 *    src/rom_15000/rom_178b0_b.c records from the other direction.
 * 3. `unsigned short` FOR THE HIGH-HALF PARAMETER, as its OWN variable, separate
 *    from the `short` the second glyph table yields.  `arg` (unsigned short) gives
 *    the ROM's `lsl #16 / lsr #16 / cmp #0` test and lets the masked value be
 *    reused as the `* 0x20` index; a single reused variable makes gcc test the
 *    register directly (2 instructions instead of 4) and fold the mask into the
 *    index shift.  Splitting it also hands r10 to both halves -- they do not
 *    conflict, so global.c gives them the same hard register, exactly as the ROM.
 * 4. ASSIGN `n` BEFORE `m` IN BOTH ARMS of the mode branch.  135 -> 84, the single
 *    biggest step.  The ROM's `mov r11,r3` sits AT THE JOIN, not in either arm --
 *    gcc's cross-jumping merges the identical trailing copy only when both arms
 *    END with it, which requires the other variable's copy to come first in both.
 *    An intermediate `t` temp (`t = 8 / t = *(u16*)...; m = t;`) does NOT do it:
 *    gcc coalesces `t` into `m` and puts the copy back in each arm.
 *
 * NAMED BLOCKER: register allocation, in two places, plus one combine difference.
 *
 *   PASS 18 (global.c / local-alloc).  The fourth blit block swaps the two
 *   short-lived allocnos: the ROM has the `buf + w` pointer in r5 and the function
 *   pointer in r6, gcc the reverse.  That also changes `add r5,sp,#4 / add r5,r8`
 *   (2 insns, destructive add of a high register) into `add r3,sp,#4 / mov r1,r8 /
 *   add r6,r3,r1` (3).  It is the ONLY reason the relocations differ: the block's
 *   four calls are `_call_via_r5` where the ROM has `_call_via_r6`.
 *   The loop nest's five pseudos are also rotated by one position in
 *   REG_ALLOC_ORDER (ROM p r1, z r0, k r4, j r5, i r6; gcc z r1, k r0, j r4, i r5,
 *   p r6).  Giving the loop pointer its OWN local instead of reusing the blit
 *   blocks' pointer was measured and is WORSE (84 -> 95), so the shared name is
 *   not the cause.
 *
 *   PASS 12 (combine).  `v = v + 1` on a `short` gives `add r3,#1 / lsl #16 /
 *   asr #16`; the ROM distributes the shift -- `lsl r3,#16 / mov r2,#0x80 /
 *   lsl r2,#9 / add r3,r2 / asr r3,#16` -- which is one instruction MORE and is
 *   what simplify_shift_const does to (ashift (plus X C) N).  Four spellings
 *   (`v++`, `v += 1`, `int v` with an explicit `(short)` cast, and the distributed
 *   form written out) are all identical or worse.
 *
 * The remaining small residue is scheduling: the prologue's `lsl r1,#1` lands one
 * slot early, two reload temps use r1 where the ROM uses r0, and one argument fill
 * wants `mov r2,r11` BETWEEN `mov r1,r6` and `add r1,#0x21` (the descending-fill
 * shape docs/elevation.md records).
 */
extern unsigned char *iwram_3001e8c;
extern unsigned char Data_32224[];
extern unsigned char L31e24[] __asm__(".L31e24");
extern void Func_80008d4(void *p, int n);
extern void Func_8000984(void *src, void *dst, int n);

int DrawMsgGlyph(unsigned int id, int *out)
{
    unsigned char buf[0x180];
    unsigned char *ctx;
    unsigned char *g;
    unsigned char *p;
    unsigned char *q;
    int w;
    int m;
    int n;
    int i;
    int j;
    int k;
    int z;
    int t;
    unsigned short arg;
    short v;
    int (*clr)(void *, int);
    int (*blit)(void *, void *, int);
    int (*b2)(void *, void *, int);
    int (*b3)(void *, void *, int);
    int (*b4)(void *, void *, int);

    ctx = iwram_3001e8c;
    arg = (unsigned short)(((int)(id << 8)) >> 16);
    id &= 0xff;
    clr = (int (*)(void *, int))Func_80008d4;
    clr(buf, 0x180);
    if (ctx[0xea4] != 0) {
        n = 0;
        m = 8;
    } else {
        n = 1;
        m = *(unsigned short *)(ctx + 0xeae);
    }
    g = Data_32224 + (id - 0x20) * 0x20;
    w = *(unsigned short *)g;
    g += 2;
    if (*(unsigned short *)(ctx + 0xeac) == 1) {
        blit = (int (*)(void *, void *, int))Func_8000984;
        blit(g, buf + 0x31, n);
        blit(g, buf + 0x32, n);
        blit(g, buf + 0x20, m);
        blit(g, buf + 0x21, m);
        w += 1;
    } else {
        b2 = (int (*)(void *, void *, int))Func_8000984;
        b2(g, buf + 0x31, n);
        b2(g, buf + 0x20, m);
    }
    if (arg != 0) {
        g = L31e24 + arg * 0x20;
        v = *(short *)g;
        g += 2;
        if (*(unsigned short *)(ctx + 0xeac) == 1) {
            p = buf + w;
            b3 = (int (*)(void *, void *, int))Func_8000984;
            b3(g, p + 0x31, n);
            b3(g, p + 0x32, n);
            b3(g, p + 0x20, m);
            b3(g, p + 0x21, m);
            v = (((int)v) << 16) + 0x10000 >> 16;
        } else {
            p = buf;
            p += w;
            b4 = (int (*)(void *, void *, int))Func_8000984;
            b4(g, p + 0x31, n);
            b4(g, p + 0x20, m);
        }
        w += (unsigned short)v;
    }
    q = buf + 7;
    for (i = 0; i <= 1; i++) {
        for (j = 0; j <= 1; j++) {
            for (k = 0; k <= 7; k++) {
                t = 0;
                for (z = 7; z >= 0; z--) {
                    t = (t << 4) + *q;
                    q--;
                }
                *out++ = t;
                q += 0x18;
            }
            q -= 0x78;
        }
        q += 0x70;
    }
    return w;
}
