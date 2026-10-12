/* Func_807977c  --  asm/rom_77000/rom_79460_c_c_a_a_a_c.s
 *
 * MATCHING -- 0 of 42.  BYTE-IDENTICAL: 88 bytes, 42 encodings and 2
 * relocations identical.  Instruction counts 39/39.  Measured batch 342,
 * brief B.  Pin-free, device-free.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_77000/rom_79460_c_c_a_a_a_c.c \
 *     asm/rom_77000/rom_79460_c_c_a_a_a_c.s --func Func_807977c
 *
 * SPLIT SHAPE: none.  `tools/split_s.py ... --dry-run` reports
 * "holds only Func_807977c and no data; convert it directly, no split needed".
 * PINS: 0.
 *
 * THE PARK'S FIGURE WAS STALE AND ITS NEXT STEP WAS RIGHT BUT NOT SUFFICIENT.
 * It read "42 lines against 42, 23 differing"; objcmp at the head of batch 342
 * measures 22 of 42 (stream), instructions 39/39, relocations clean.  Its
 * "NOT YET TRIED" line -- name the end pointer in a local before the loop --
 * is INERT ON ITS OWN (22, byte-identical to the park) and is nevertheless a
 * REQUIRED PREREQUISITE.  Three independent source changes were needed and
 * only the full triple is worth anything:
 *
 *   step                                                          figure
 *   park body                                                     22 of 42
 *   + `end = p + 0xf;` named before the loop                      22  (inert)
 *   + `one = 1;` named, the shift written `one << b`              13
 *   + `p++` moved ABOVE the `if`, not below it                     2
 *   + `n = 0;` placed BEFORE `one = 1;`                            0
 *
 * `one` alone, without `end`, is 18; `one` BEFORE `end` in source order is 20.
 * So the pair is crossed in both directions: order matters and neither half
 * carries the other.
 *
 * WHY `p++` ABOVE THE `if` IS WORTH ELEVEN ENCODINGS.  It is not a scheduling
 * lever; it is an allocno-priority lever, and it works by SHORTENING `b`'s
 * live range relative to `dest`'s rather than by lengthening anything.  With
 * `p++` below the `if`, `.18.greg` prints
 *
 *     ;; 6 regs to allocate: 36 32 33 35 37 34
 *     32 in 6  33 in 7  34 in 10  35 in 1  36 in 5  37 in 8
 *
 * -- allocno 36 (`b`) is sorted FIRST by allocno_compare (global.c:600-618,
 * floor_log2(n_refs)*n_refs/live_length*10000*size) and therefore takes r5,
 * the first callee-saved register REG_ALLOC_ORDER offers once r4 is excluded
 * (arm.h:983-994 gives 3,2,1,0,12,14,4,5,6,... and the build is
 * -fcall-used-r4, so a value crossing the `bl` starts at r5).  Allocno 32
 * (`dest`) then gets r6.  The ROM is the other way round: dest r5, b r6, and
 * every other assignment -- p r7, n r1 spilled to [sp], one r8, end r10 --
 * already agreed.  So the whole 13 was ONE EXCHANGE AT THE TOP OF ONE SORT.
 *
 * The arithmetic says dest can never outscore b: refs are loop-depth
 * quantised (flow.c:5115-5116, +loop_depth+1 per reference), b's three
 * references are all at depth 1 while dest's definition and its final store
 * are at depth 0, and dest is live across the whole function where b is live
 * across one iteration.  That is the bound batch 341 would have written.  It is
 * also beside the point: moving `p++` above the `if` puts the pointer bump in
 * the SAME basic block as the load, which closes b's range before the branch
 * and reorders the two allocnos without anyone's priority having to win --
 * global.c:617-619 breaks a tie on allocno NUMBER, and 32 < 36.
 *
 * WHY `n = 0;` BEFORE `one = 1;` IS WORTH THE LAST TWO.  At 2 of 42 every
 * register matched and the only disagreement was a prologue pair in the other
 * order: the ROM emits `mov r1, #0` (n) then `mov r8, r3` (the reload copy of
 * the constant 1 into a high register), ours the reverse.  sched2 tie-breaks on
 * INSN_LUID (haifa-sched.c:4097-4110 and below), and the reload-generated
 * `mov r8, r3` is numbered above the original stream, so source order of the
 * two initialisations is the only handle.  THREE setup orders are
 * byte-identical -- `n` immediately before `one`, `n` first of all four, and
 * `n` between `p` and `end` -- so what the compiler reads is "n = 0 precedes
 * one = 1", not n's absolute position.
 *
 * AUTOSWEPT AT FULL DEPTH on the park body, batch 342: flag axis all eight
 * groups (-O1 37, -fno-schedule-insns2 25, -ffixed-r7 42, -fno-rerun-cse-after-
 * loop 38, -fno-rerun-loop-opt 41, the other three inert at 22) and the anchor
 * axis 11 of 11 boundaries, all inert.  Neither cheap axis reaches this.
 *
 * THE PARK'S OTHER NEGATIVES ARE CONFIRMED, and one of its readings is
 * promoted: `for (p = L84a8c; p <= L84a8c + 0xf; p++)` is still worse (40
 * differing), and the park's conclusion -- "textual order of the two
 * expressions is NOT what picks the hoist order" -- is correct about the HOIST.
 * The hoist order was never the question; the allocno sort was, and both
 * invariants end up in the ROM's own high registers (r8 and r10) as soon as
 * they are named.  Also measured inert at 13 against the end+one body: `b`
 * declared inside the loop block, `unsigned char b`, `*dest++ = b`, a named
 * copy of `b`, a named `dest + 1` computed before the store, the call result
 * in a local, four declaration orders.  Worse: a named `m = one << b` and a
 * `while` loop both go to 36 with a size change and dirty relocations.
 */
extern unsigned char L84a8c[] __asm__(".L84a8c");
extern int *Func_8077330(int n);

int Func_807977c(unsigned char *dest)
{
    unsigned char *p;
    unsigned char *end;
    int n;
    int b;
    int one;

    p = L84a8c;
    end = p + 0xf;
    n = 0;
    one = 1;
    do {
        b = *p;
        p++;
        if (*Func_8077330(0) & (one << b)) {
            *dest = b;
            n++;
            dest++;
        }
    } while (p <= end);
    *dest = 0x20;
    return n;
}
