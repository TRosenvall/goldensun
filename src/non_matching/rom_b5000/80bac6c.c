/* Func_80bac6c -- asm/rom_b5000/rom_b9b30_c_a_c_c.s
 *
 * NON-MATCHING, 35 of 57 encodings  (MEASURED, batch 333).
 *   READ THE COUNT BEFORE THE FIGURE.  The stream is now 55 encodings against
 *   the reference's 57, so this positional figure STILL MEASURES MISALIGNMENT.
 *   It ROSE from thirty-three while the function got materially closer: the
 *   aligned residue (tools/aligncmp.py) fell from twenty-two differing in ten
 *   hunks to 12 differing in 5 hunks, and aligned-equal rose from thirty-nine
 *   to 45 of 57.  The stream used to be one encoding LONG and is now two SHORT,
 *   which is why the positional count moved the other way.
 *   SIZE ref 124 bytes, ours 120.  Instructions ref 51, ours 49.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b5000/80bac6c.c \
 *     asm/rom_b5000/rom_b9b30_c_a_c_c.s --func Func_80bac6c
 *   and then, because the figure above is phase:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/rom_b5000/80bac6c.c \
 *     asm/rom_b5000/rom_b9b30_c_a_c_c.s Func_80bac6c -v
 *
 * === BATCH 333: LOOP 3 IS NOW BYTE-EXACT, AND THE ONLY RESIDUE IS LOOP 2. ===
 *
 * THE PARK FILE WAS BEHIND ITS OWN HEADER.  The batch-261 progress note below
 * described three fixes that close loop 3 and named a candidate in a scratch
 * directory that no longer exists; the installed body never carried them.  All
 * three are now IN the body and all three were re-measured:
 *
 *   1. an UNSIGNED up-counter stops check_dbra_loop reversing loop 3, giving
 *      the ROM's `add r1, #1 / cmp r1, #0x13 / bls` where a signed `int` gave
 *      `mov r1, #0x13 / sub r1, #1 / cmp r1, #0 / bge` plus a `b`;
 *   2. the stored 0xff ROUTED THROUGH AN INT LOCAL (`n = 0xff;`) gives the
 *      ROM's `mov r0, #0xff`.  Written as a bare literal into a halfword
 *      lvalue it POOLS -- our stream carried `ldr r0, [pc, #20]` and a
 *      `.word 0x000000ff`.  That is the HImode-literal rule firing on OUR side,
 *      and it is the open item (a) in the note below ANSWERED: 0xff here is NOT
 *      a const.sym entry, it is an int intermediate.  Nothing to add to .sym.
 *   3. the preheader written `k = 0; n = 0xff; off = 0xbb << 2;` WITH THE
 *      OFFSET LAST reproduces `mov r2, #0xbb / mov r1, #0 / mov r0, #0xff /
 *      lsl r2, #2`.
 *
 * Together: twenty-two differing in ten hunks -> 12 in 5, and indices 37
 * onwards (the whole of loop 3 and the epilogue) are exact.
 *
 * THE RESIDUE IS LOOP 2 AND NOTHING ELSE.  Every remaining hunk is inside it,
 * plus one `b` whose offset moves with the pool:
 *
 *     rom    mov r1,#0 / add r0,r5,#2 | lsl r3,r1,#1 / mov r2,r3 / add r2,#0x64
 *            / ldrsh r3,[r0,r2] ... ldr r3,=0xfe / strh r3,[r0,r2] ... add r1,#1
 *     ours   add r2,r5,#0 / ldr r1,=0xfe / add r2,#0x66 / mov r0,#0
 *            / ldrsh r3,[r2,r0] ... strh r1,[r2,#0] ... add r2,#2
 *
 * i.e. gcc eliminated the index and walks a pointer from g+0x66, hoisting the
 * 0xfe the ROM reloads inside the loop; the ROM keeps the index in r1, holds
 * g+2 in r0 and REBUILDS the byte offset every iteration.
 *
 * MEASURED AGAINST LOOP 2 IN BATCH 333, with loop 3 already closed so the
 * comparison is clean (aligned differing, of 57):
 *   `(i << 1) + 0x64` instead of `i * 2 + 0x64`              12  -- INERT
 *   `((short *)(q + 0x64))[i]`, array decay                  12  -- INERT
 *   named `t = i * 2; off2 = t + 0x64;`                      15  -- WORSE
 *   named with the copy-then-modify split
 *     `off2 = t; off2 += 0x64;`                              15  -- WORSE
 *   `while (1)` with the offset named                        15  -- WORSE
 *   THE GOTO-LOOP LEVER on loop 2, offset named              49  -- MUCH WORSE
 *   THE GOTO-LOOP LEVER on loop 2, offset inline             51  -- MUCH WORSE
 *
 * So the park's verdict on the goto-loop lever is CONFIRMED a second time, now
 * against a clean baseline: it is not the remedy here.  And the named-
 * intermediate shape the ROM's `mov r2, r3` suggests is a REGRESSION, because
 * gcc strength-reduces the named offset too -- it emits `mov r2,#0x64 /
 * ldrsh r3,[r2,r1] / add r2,#2`, which is the SAME giv wearing a different
 * base.  The copy the ROM shows is not reachable while the giv exists.
 *
 * WHAT A FUTURE ATTEMPT SHOULD DO: the question is not how to spell the
 * address, it is how to stop loop.c replacing the biv at all in ONE loop while
 * leaving loops 1 and 3 reduced.  -fno-strength-reduce is file-wide and costs
 * the other two loops (recorded below).  Nothing source-level has reached it in
 * three batches.
 *
 * --- THE PRE-BATCH-333 RECORD FOLLOWS; ITS NUMBERS ARE SUPERSEDED ---
 *
 * ### PROGRESS, batch 261 -- LOOP 3 IS NOW EXACT, preheader and body.
 *
 * Verified baseline first: thirty-three differing of fifty-seven ENCODINGS, ref 124 bytes against
 * ours 128, first diff at index eighteen. The note's "thirty-four differing" counts LINES, not
 * encodings -- worth knowing before comparing numbers below.
 *
 * Candidate with loop 3 closed: scratch_elev/b264/Func_80babdc/nb_v4_loop3_off_last.c
 * Three things got it there, all recorded in docs/elevation.md:
 *   * the HImode-literal rule applied to the 0xff store gives the ROM's mov r0,#0xff;
 *   * an UNSIGNED up-counter stops check_dbra_loop reversing the loop, giving the
 *     ROM's cmp r1,#0x13 / bls;
 *   * the sched2 LUID tie-break orders the preheader -- u2 = 0; n = 0xff;
 *     off = 0xbb << 2; with the OFFSET LAST reproduces
 *     mov r2,#0xbb / mov r1,#0 / mov r0,#0xff / lsl r2,#2.
 *
 * The total did not fall (thirty-five) because everything downstream is displaced by
 * loop 1's pool. That is a cascade, not a regression.
 *
 * TWO OPEN ITEMS for whoever takes this next:
 *   (a) The ROM word-loads 0xfe in BOTH loop 1 (ldr r1, .Lbac94) and loop 2
 *       (ldr r3, =0xfe) where we emit ldrh. const.sym's header names the
 *       halfword-expression case as the one exception to the pooled-small-value
 *       tell -- but that exception describes OUR output, not the ROM's, so the
 *       symbol tell stands and 0xfe here looks like a const.sym entry. objcmp
 *       CANNOT settle this: a symbol adds an R_ARM_ABS32 the reference .s lacks.
 *       Only make compare can.
 *   (b) Loop 2's strength reduction is this park's own blocker and is untouched.
 *
 * Its file-mate Func_80babdc was elevated in batch 261, so this function now sits
 * alone in asm/rom_b5000/rom_b9b30_c_a_c_c.s -- closing it converts that file
 * WHOLE with no further split.
 *
 * --- original note follows ---
 * Blocker class: loop strength reduction gcc performs and the ROM does not.
 *
 * sixty-two lines against the ROM's sixty-two, thirty-four differing, with the first twenty-three
 * instructions exact -- the GetUnit call, the byte clear, and the whole of the
 * first search loop including its `b` into the middle.
 *
 * THE OPERAND-ORDER LEVER LANDED and is worth recording because this function
 * needs BOTH spellings. `LDRSH Rd, [Rn, Rm]` encodes the two registers in fixed
 * positions, and docs/elevation.md gives the rule: `base + off` puts the base
 * first, `off + (int)base` puts the offset first. Loops 1 and 3 here are
 * offset-first (`ldrsh r3, [r2, r5]` with r2 the offset) and loop 2 is
 * base-first (`ldrsh r3, [r0, r2]`). Writing loops 1 and 3 as
 * `*(short *)(off + (int)g)` and leaving loop 2 as `q + off` moved the first
 * difference from instruction sixteen to twenty-four and took thirty-six differing to thirty-four.
 *
 * WHAT IS LEFT is loop 2. The ROM keeps an index and REBUILDS the byte offset
 * every iteration -- `lsl r3, r1, #1 / mov r2, r3 / add r2, #0x64` -- against a
 * base of `g + 2` held in r0. gcc strength-reduces that to a walking pointer
 * starting at `g + 0x66` and hoists the 0xfe the ROM reloads inside the loop.
 *
 * MEASURED against it:
 *   the documented `goto`-loop lever, which disables loop optimisation
 *     entirely and is the standard remedy for exactly this shape:
 *     sixty-seven lines, fifty-one differing -- MUCH worse, and worse on its own as well as
 *     combined, so it is not a case of the rewrite being spoiled by something
 *     else. Applied to loop 2 alone it still costs five lines.
 *   named intermediates for the shift and the add (`t = i * 2;
 *     off = t + 0x64;`), which is the copy-then-modify shape the ROM's
 *     `mov r2, r3` suggests, together with an unsigned up-counting loop 3:
 *     sixty-one lines, thirty-eight differing.
 *
 * So this is a counter-example to the `goto`-loop lever rather than a candidate
 * for it: the ROM does NOT hoist, but rewriting the loop as a `goto` costs more
 * than the hoisting does. The distinguishing feature against Func_8090584,
 * where the lever took ninety-five differing to three, is that there the hoisted values were
 * many (a pointer, two masks, a base and three store values) and here it is one
 * constant and one induction variable.
 *
 * FLAGS MEASURED, and none of them is the answer:
 *   -fno-strength-reduce                     sixty-five lines, fifty-one differing from insn zero
 *   -fno-move-all-movables                   sixty-two lines, thirty-four -- byte-identical to
 *                                            no flag at all, so the 0xfe hoist
 *                                            is NOT move-all-movables
 *   both together                            sixty-five lines, fifty-one
 *
 * -fno-strength-reduce costs three lines here, which matches the recorded
 * "one extra callee-saved register" price and confirms why the goto-loop lever
 * is described as subsuming it more cheaply -- except that here the goto
 * rewrite is dearer still.
 *
 * NEXT: nothing measured is close. The useful framing is that loop 2 needs
 * gcc's induction-variable pass off for ONE loop, and this compiler offers no
 * way to say that which costs less than the transformation is worth.
 */
extern unsigned char *iwram_3001e74;
extern unsigned char *_GetUnit(int id);
extern void Func_80c1ebc(int id);

void Func_80bac6c(int id)
{
    unsigned char *g;
    unsigned char *u;
    unsigned char *q;
    int off;
    int i;
    int v;
    int t;
    int off2;
    int n;
    unsigned int k;
    g = iwram_3001e74;
    u = _GetUnit(id);
    u[0x95 << 1] = 0;
    off = 0x58;
    for (;;) {
        v = *(short *)(off + (int)g);
        if (v == id) {
            *(short *)(off + (int)g) = 0xfe;
            goto found;
        }
        if (v == 0xff)
            break;
        off += 2;
    }
    i = 0;
    q = g + 2;
    for (;;) {
        v = *(short *)(q + (i * 2 + 0x64));
        if (v == id) {
            *(short *)(q + (i * 2 + 0x64)) = 0xfe;
            goto found;
        }
        i++;
        if (v == 0xff)
            return;
    }
found:
    Func_80c1ebc(id);
    k = 0;
    n = 0xff;
    off = 0xbb << 2;
    do {
        if (*(short *)(off + (int)g) == id)
            *(short *)(off + (int)g) = n;
        k++;
        off += 0x10;
    } while (k <= 0x13);
}
