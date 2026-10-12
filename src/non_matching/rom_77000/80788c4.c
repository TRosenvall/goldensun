/* Func_80788c4 (0x080788c4) -- asm/rom_77000/rom_78414_c_c_a_c_a_c_c_c.s
 *
 * NON-MATCHING, 8 of 62 encodings  (MEASURED, batch 338 C).  Was 14 of 62.
 *   SIZE EXACT: 132 bytes both sides, 62 encodings both sides, 57 instructions
 *   both sides, relocations identical.  THE FIGURE IS A DISTANCE -- batch 332
 *   D's claim that the lengths agree was re-verified at the head of batch 338
 *   and holds.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_77000/80788c4.c \
 *     asm/rom_77000/rom_78414_c_c_a_c_a_c_c_c.s --func Func_80788c4
 *
 * PINS: 0.  shimcount.py prints no rows.  No flag group, no fakematch row, no
 * device.  PIECE: tools/piece.py at the head of batch 338 -- this function is
 * the ONLY member of the reference .s, confirming batch 335 A's note and
 * RETIRING the older header line about Func_8078870 having to land first.
 *
 * ===========================================================================
 * WHAT BATCH 338 C CHANGED: 14 -> 8, AND THE COMPACTION LOOP IS NOW EXACT
 *
 * The 14 split 7/7 between the two loops, not 8/6 as the old header's (a)/(b)
 * guessed, and the whole of it was register fields -- the batch-335 addendum
 * was right and the older line (b) claiming "plus one opcode: the reference
 * brings its zero in through a word-sized pool load and we bring it in through
 * a halfword one" IS REFUTED.  Both sides emit `ldr rN, [pc, #16]` off the same
 * `.word 0x00000000`; only the destination register differed.  (The underlying
 * fact is still worth keeping: a HImode constant HAS no `mov Rd,#imm`
 * alternative in Thumb, so it always reaches its register through the minipool.
 * That is why a zero is pooled here at all, on both sides.)
 *
 * THE LEVER: REUSE `i`, THE COMPACTION COUNTER, AS THE ZERO-FILL COUNTER.
 *
 *     if (n <= 0xe) {
 *         p = base + n;
 *         i = 0xf - n;            <-- was a separate `k`
 *         do { i--; *p++ = 0; } while (i != 0);
 *     }
 *
 * This closes all seven of the compaction loop's differences -- indices 26-40
 * are now byte-identical, including the `src` / counter exchange the old header
 * called (a) and left as the next question.  IT IS NOT A LIFETIME TRICK IN THE
 * COMPACTION LOOP AT ALL; it is a priority change in `allocno_compare`
 * (global.c:598-620), measured from `-da`'s `.17.lreg` (`dump_flow_info`,
 * flow.c:5743-5756) and `.18.greg` (`dump_conflicts`, global.c:1862):
 *
 *   PARKED BODY   i   = reg 44, 7 refs / 22 insns -> pri 2*7/22*10000 =  6363
 *                 src = reg 36, 7 refs / 13 insns -> pri 2*7/13*10000 = 10769
 *                 dst = reg 37, 7 refs / 12 insns -> pri 2*7/12*10000 = 11666
 *     allocno_order `55 45 38 37 42 36 49 34 40 44 43 41 35 63 32`
 *     -- src and dst are allocated 4th and 6th and take r0/r1; i is 10th and
 *     is left r4, which is the exchange, because REG_ALLOC_ORDER reaches
 *     3, 2, 1, 0, 12, 14, 4, 5, ... (arm.h:989-995) and r4 is call-used here.
 *
 *   THIS BODY     i absorbs k's references and its live range spans both
 *     loops, so its priority rises past both cursors and it is allocated 4th:
 *     allocno_order `55 38 37 44 42 36 49 34 40 43 41 35 63 32` -- i takes r0,
 *     src is pushed to r4, and the reference's assignment falls out whole.
 *
 * `src` is live only over the FIRST block of the loop body (it dies at
 * `adds src, #2`), while the counter is live over all three, which is exactly
 * why the counter loses the sort on equal reference counts.  Worth knowing as
 * a shape: **a cursor that dies early in a loop body outranks the counter that
 * spans it, so the counter gets the late register unless something lengthens
 * the cursor or widens the counter.**
 *
 * ---------------------------------------------------------------------------
 * WHAT THE REMAINING EIGHT ARE -- one three-register rotation, 9 instructions
 *
 *   rom   lsl r3, r5, #1 / add r0, r3, r6 / ldr r2, =0 / mov r3, #15
 *           / sub r5, r3, r5 / sub r5, #1
 *           / strh r2, [r0] / add r0, #2 / cmp r5, #0 / bne
 *   ours  lsl r3, r5, #1 / add r2, r6, r3 / mov r3, #15 / sub r0, r3, r5
 *           / ldr r3, =0 / sub r0, #1
 *           / strh r3, [r2] / add r2, #2 / cmp r0, #0 / bne
 *
 * So: p is r0 in the reference and r2 here; the pooled zero is r2 there and r3
 * here; the counter is **r5 -- `n`'s own register** there and r0 here.  `n` is
 * r5 on both sides and dies at the subtract, so the reference's
 * `sub r5, r3, r5` writes the trip count back into the register it just freed.
 *
 * WHERE THE BOUND IS, WITH THE ARITHMETIC.  For `p` to be handed r0 the three
 * registers REG_ALLOC_ORDER reaches first (r3, r2, r1) must all already
 * conflict with it when `find_reg` runs.  r3 does: the literal 15 is
 * block-local, so local-alloc hands it r3 before global-alloc starts.  r2 does
 * NOT here, because the pooled zero (reg 63, 3 refs / 10 insns -> pri 1*3/10
 * *10000 = 1000) is allocated THIRTEENTH of fifteen, long after p (reg 38,
 * 7 refs / 9 insns -> 15555, allocated second).  For the zero to take r2 ahead
 * of p it would need pri > 15555, i.e. four references inside five insns --
 * it has three inside ten, and loading it inside the loop to raise the count
 * costs an instruction.  So **this residue is not reachable by changing the
 * zero**, and the open question is whether anything makes `p` sort late or
 * makes `dst`'s r1 conflict with it.
 *
 * ---------------------------------------------------------------------------
 * MEASURED IN BATCH 338 C, all 62 encodings and 57 instructions unless noted
 *
 *    8  THIS BODY
 *    8  `p = n + base;` (the add's operands written the other way)    INERT
 *    8  `*p = 0; p++;` split out of `*p++ = 0`                        INERT
 *    8  the zero-fill cursor taken from `dst` instead of a fresh `p`  INERT
 *   14  THE PARKED BODY (separate `k`)
 *   14  the zero-fill counter taken from `v` instead of `i`
 *   14  a `q = base;` copy inserted at each of five boundaries in the
 *       else branch -- cse2 removes all five, no allocno appears
 *   14  `p = (unsigned short *)((unsigned char *)base + n * 2)`
 *   14  `p = &base[n]`
 *   15  `i = 0xf - n;` written BEFORE `p = base + n;`
 *   15  `i` counter with the store and increment split
 *   16  the counter as `0xe - n` with a `>= 0` test
 *   18  the zero-fill counter taken from `n` IN PLACE (`n = 0xf - n`) --
 *       this gets the reference's `sub r5, r3, r5` but gives back the whole
 *       compaction loop, because i's priority falls to 6363 again.  THE TWO
 *       HALVES PULL AGAINST EACH OTHER THROUGH ONE SORT.
 *   18  ... the same with the store before the decrement
 *   18  ... the same plus `i = n;` kept alive in the tail
 *   18  the zero-fill as `for (i = n; i <= 0xe; i++) base[i] = 0;`  (58 insns)
 *   18  ... as a `while` (58 insns)
 *   19  ... as `for (k = n; ...)` (58 insns)
 *   19  `i = n; n = 0xf - n; p = base + i;` (58 insns)
 *   20  ... the index loop with no `if` guard (58 insns)
 *   21  the zero-fill cursor taken from `src` with `n` as the counter
 *   22  `p = base + n; i = n; do { i++; ... } while (i <= 0xe);` (56 insns)
 *   23  the index-loop zero-fill as a do/while (56 insns)
 *   30  the index-loop zero-fill with the guard inside (56 insns)
 *   42  the zero-fill cursor left as `dst` where it already stands -- drops
 *       the reference's `lsl`/`add`, 53 instructions, A DIFFERENT PROGRAM
 *       SHAPE and not a candidate
 *   45  the zero named in an `int` or an `unsigned short` local (56 insns)
 *   64  `off = n * 2` named and the cursor built from it (61 insns)
 *   65  `src++` moved to the bottom of the compaction body (62 insns)
 *
 * AUTOSWEEP (batch 338, recorded negatives on the PARKED body -- do not
 * re-screen): flags -fno-gcse, -fno-strict-aliasing, -fno-strength-reduce,
 * -fno-rerun-cse-after-loop, -fno-rerun-loop-opt all INERT at 14;
 * -fno-schedule-insns2 20, -O1 51, -ffixed-r7 65 with the length broken.
 * Anchor axis, 18 top-level boundaries: all inert at 14.
 *
 * ---------------------------------------------------------------------------
 * EVERYTHING BELOW IS CARRIED FORWARD FROM THE PARK AND STILL HOLDS
 *
 * THE ONE-MEMBER AGGREGATE `struct Held w` IS LOAD-BEARING and must not be
 * simplified to a declared halfword local.  simplify_comparison's ASHIFT case
 * (combine.c:10749-10774, the fit test at :10763-10765) folds a shift-against-
 * zero away whenever nonzero_bits of the operand fits the shifted mask, and no
 * SPELLING OF THE TEST defeats it (casts to unsigned short / short / signed
 * short, either cast as the whole condition, an explicit `<< 16`, that shift
 * compared against zero, a multiply by the same power of two, a low-16 mask --
 * all measured EXACTLY INERT).  What defeats it is THE TYPE OF THE HOLDER:
 * arm.h:597-606's PROMOTE_MODE widens every declared HImode local to a word
 * with a SIGN extension (TARGET_MMU_TRAPS is off here), and Thumb-1's
 * sign-extending halfword load has no immediate-offset form, so arm.md:3239
 * with :3289 emits a scratch move beside it -- a halfword local therefore lands
 * one instruction LONG, in five spellings.  explow.c:896-913's promote_mode
 * reaches PROMOTE_MODE only for the scalar type codes at :898-901; RECORD,
 * UNION and ARRAY fall through at :912 UNPROMOTED, so a one-member aggregate
 * is the only way to hold the halfword in a pseudo of its own width.  The
 * field must stay SIGNED: unsigned makes the widening pair a zero extension,
 * combine collapses it to a subreg copy, and the reference's `lsl` becomes a
 * register move (18 differing).  The aggregate is used as a HOLDER ONLY -- the
 * cursors stay halfword pointers, because the structure size boundary is a
 * word (arm.h:686-701) and making them `struct Held *` gives a stride of four
 * and a wrong program that still measures the right length.
 *
 * ALSO STILL RIGHT: the quantity decrement through the packed high bits, the
 * register-offset load and store at the slot's byte offset, the three return
 * values reaching one shared CalcStats tail through a single result variable,
 * the fifteen-iteration compaction with separate cursors, and the tail loop's
 * counted form.  Batch 332 D's 744-row declaration sweeps (24 pointer
 * permutations and 720 integer permutations, both EXACTLY FLAT) stay closed;
 * initialisation order is worth three (14 / 17 / 18 / 20) and is also closed.
 *
 * NEXT: `p`'s allocation order, per the bound above.  The compaction loop is
 * finished and should not be touched -- any change that shortens `i`'s live
 * range or its reference count gives it straight back.
 */
extern unsigned char *GetUnit(int who);
extern void CalcStats(int who);

struct Held { short v; };

int Func_80788c4(int who, int slot)
{
    unsigned char *u;
    unsigned short *base;
    unsigned short *src;
    unsigned short *dst;
    unsigned short *p;
    struct Held w;
    int off;
    int ret;
    int v;
    int n;
    int i;

    u = GetUnit(who);
    off = slot * 2 + 0xd8;
    v = *(unsigned short *)(u + off);
    ret = -1;
    if (v != 0) {
        if ((v & 0xf800) != 0) {
            *(unsigned short *)(u + off) = v - 0x800;
            ret = 1;
        } else {
            base = (unsigned short *)(u + 0xd8);
            *(unsigned short *)(u + off) = 0;
            n = 0;
            src = base;
            dst = base;
            i = 0xe;
            do {
                w.v = *src++;
                if (w.v != 0) {
                    *dst++ = w.v;
                    n++;
                }
                i--;
            } while (i >= 0);
            if (n <= 0xe) {
                p = base + n;
                i = 0xf - n;
                do {
                    i--;
                    *p++ = 0;
                } while (i != 0);
            }
            ret = 2;
        }
    }
    CalcStats(who);
    return ret;
}
