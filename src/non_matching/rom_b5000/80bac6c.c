/*
 * ### PROGRESS, batch 261 -- LOOP 3 IS NOW EXACT, preheader and body.
 *
 * Verified baseline first: 33 differing of 57 ENCODINGS, ref 124 bytes against
 * ours 128, first diff at index 18. The note's "34 differing" counts LINES, not
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
 * The total does not fall (35) because everything downstream is displaced by
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
 * 62 lines against the ROM's 62, 34 differing, with the first twenty-three
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
 * difference from instruction 16 to 24 and took 36 differing to 34.
 *
 * WHAT IS LEFT is loop 2. The ROM keeps an index and REBUILDS the byte offset
 * every iteration -- `lsl r3, r1, #1 / mov r2, r3 / add r2, #0x64` -- against a
 * base of `g + 2` held in r0. gcc strength-reduces that to a walking pointer
 * starting at `g + 0x66` and hoists the 0xfe the ROM reloads inside the loop.
 *
 * MEASURED against it:
 *   the documented `goto`-loop lever, which disables loop optimisation
 *     entirely and is the standard remedy for exactly this shape:
 *     67 lines, 51 differing -- MUCH worse, and worse on its own as well as
 *     combined, so it is not a case of the rewrite being spoiled by something
 *     else. Applied to loop 2 alone it still costs five lines.
 *   named intermediates for the shift and the add (`t = i * 2;
 *     off = t + 0x64;`), which is the copy-then-modify shape the ROM's
 *     `mov r2, r3` suggests, together with an unsigned up-counting loop 3:
 *     61 lines, 38 differing.
 *
 * So this is a counter-example to the `goto`-loop lever rather than a candidate
 * for it: the ROM does NOT hoist, but rewriting the loop as a `goto` costs more
 * than the hoisting does. The distinguishing feature against Func_8090584,
 * where the lever took 95 differing to 3, is that there the hoisted values were
 * many (a pointer, two masks, a base and three store values) and here it is one
 * constant and one induction variable.
 *
 * FLAGS MEASURED, and none of them is the answer:
 *   -fno-strength-reduce                     65 lines, 51 differing from insn 0
 *   -fno-move-all-movables                   62 lines, 34 -- byte-identical to
 *                                            no flag at all, so the 0xfe hoist
 *                                            is NOT move-all-movables
 *   both together                            65 lines, 51
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
    off = 0xbb << 2;
    i = 0;
    do {
        if (*(short *)(off + (int)g) == id)
            *(short *)(off + (int)g) = 0xff;
        i++;
        off += 0x10;
    } while (i <= 0x13);
}
