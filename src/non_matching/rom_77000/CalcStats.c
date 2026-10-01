/*
 * CalcStats  --  asm/rom_77000/rom_77320_a_c_a_a.s  @ 0x08077428
 *
 * NON-MATCHING, 834 of 927  (objcmp, PRODUCTION FLAGS, --func CalcStats)
 *
 *   SIZE   EXACT.   ref 2024 bytes, ours 2024 bytes.
 *   COUNT  EXACT.   ref 864 instructions, ours 864 instructions.
 *   FRAME  EXACT.   `sub sp, #4` / `add sp, #4`; prologue and epilogue both
 *                   byte-identical -- push {r5,r6,r7,lr} + mov r7,r10 /
 *                   mov r6,r8 / push {r6,r7}, i.e. the SAME TWO high
 *                   registers (r8 and r10) and no third.
 *   RELOC  COUNT and SYMBOL MULTISET EXACT: 69 relocations each, same
 *                   symbols in the same order (Func_8004970, GetUnit,
 *                   GetItemInfo x2, GetDjinniInfo, GetClassInfo, __divsi3
 *                   x14, GetFlag x4, free, plus 27 + 8 + 6 jump-table
 *                   .text words). objcmp still prints RELOCATIONS differ,
 *                   and the whole of that difference is WHERE two literal
 *                   pool dumps land: the reference dumps one pool inside
 *                   switch 1 (at +0x2c8, the `.pool_aligned` after case 25)
 *                   and two near the tail; ours dumps two inside switch 2
 *                   and one at the very end. Nothing is missing and nothing
 *                   is extra.
 *   aligncmp.py:    ref 927 encodings, ours 927; aligned-equal 586 (63.2%
 *                   of ref); differing/ins/del 446 in 140 hunks.
 *   shimcount.py:   0 shims. No `register ... __asm__`, no pins, no macros.
 *
 * Both axes are exact and so is the frame, which by docs/elevation.md's own
 * ladder is rung three -- so the figure that matters here is aligncmp's
 * 63.2%, not objcmp's 834: objcmp compares encodings index by index, and
 * with locally reordered pairs throughout the count saturates.
 *
 * Verify with:
 *   cd /Users/timothyrosenvall/gs_project/goldensun
 *   docker run --rm -v "$PWD:/work" -w /work goldensun-build sh -c \
 *     'python3 tools/objcmp.py src/non_matching/rom_77000/CalcStats.c \
 *        asm/rom_77000/rom_77320_a_c_a_a.s --func CalcStats'
 *   docker run --rm -v "$PWD:/work" -w /work goldensun-build sh -c \
 *     'python3 tools/aligncmp.py src/non_matching/rom_77000/CalcStats.c \
 *        asm/rom_77000/rom_77320_a_c_a_a.s CalcStats'
 *   python3 tools/shimcount.py src/non_matching/rom_77000/CalcStats.c
 *
 * SPLIT SHAPE: none needed. asm/rom_77000/rom_77320_a_c_a_a.s holds exactly
 * ONE .thumb_func_start, so this file lands as
 * src/rom_77000/rom_77320_a_c_a_a.c against the generic `src/%.c` Makefile
 * rule -- no per-file flag group applies to this stem (checked: not in any
 * O1_CFLAGS / CSE_CFLAGS / GCSE_CFLAGS / ALIAS_CFLAGS / SCHED2_CFLAGS /
 * STRENGTH_CFLAGS / FIXEDR7_CFLAGS rule), and tryc.py reports no extra
 * flags. Its immediate landed neighbour is src/rom_77000/rom_77320_a_c_a_b.c
 * (CheckLure), which is the oracle this reconstruction was written against:
 * the bare `unsigned char *` + hand-computed-offset house style and the
 * `*(unsigned short *)(off + (int)u)` register-offset idiom both come from
 * there.
 *
 * TRIAGE: the brief called this the OUTLIER, and the measurement agrees.
 * 17 high-register mentions in 864 instructions (13 x r8, 4 x r10, no r9, no
 * r11), against 81/102/115/96 for the other four targets in this brief. The
 * three frame greps:
 *   1. `sub sp, #imm`          -> ONE hit, `sub sp, #4`. A 4-byte frame.
 *   2. `mov rX, sp` + `add rX` -> ZERO hits. No aggregate on the stack.
 *   3. `add rX, sp` + a LOAD   -> ZERO hits of that form, but `str r4, [sp]`
 *      and `ldr r4, [sp]` bracket the GetDjinniInfo call, and the LOAD BACK
 *      is what makes that one word a SPILL and not argument staging. So the
 *      spill-slot map is a single scalar at sp+0: the Djinn element index,
 *      spilled because `-fcall-used-r4` is in the production flags, so r4 is
 *      call-clobbered and the three remaining call-saved low registers are
 *      already s (r6), u (r7) and the inner counter (r5).
 * So: a straight-line-population function, and the brief's prediction held
 * twice over -- cse1 commoning repeated constants across calls is visible in
 * the reference (one `ldr r2, =0x3e7` serving THREE clamps at +0x8, +0xc and
 * +0x10, and `mov r12, r2` parking 200 in ip for the four-iteration clamp
 * loop), and the allocation levers were weak: every register-level win below
 * came from changing WHICH PSEUDO EXISTS, not from scoping one.
 *
 * WHAT THE FUNCTION DOES. r0 = combatant id. Allocates a 0x60-byte scratch
 * with Func_8004970 and uses it as a working stat block: the eight scalars
 * at +0x00..+0x24, four pairs at +0x28..+0x44, and three fields the code
 * writes and reads back rather than keeping in registers -- the current item
 * record at +0x58, the current modifier kind at +0x48 and its amount at
 * +0x54. It seeds the block from the unit, re-derives HP/PP ratios, walks the
 * 15 equipment slots twice (additive modifiers first, then percentage ones),
 * walks the 4 x 20 Djinn bitmask, applies the class multipliers, applies the
 * three elemental-power bytes, clamps everything, writes the derived values
 * back into the unit at +0x3c..+0x4f, and frees the scratch.
 *
 * ---------------------------------------------------------------------------
 * LEVERS THAT PAID, IN ORDER, WITH FIGURES
 * (metric: `shape` = diff lines after mapping every rNN to R over the
 * normalised instruction streams, so it is blind to allocation and sees
 * structure; `strict` = the same diff keeping register names; `insns` =
 * our instruction count against the reference's 864.)
 *
 *  0. First draft, written straight off the disassembly:
 *     insns 856, strict 606, shape 282. Three high registers against the
 *     reference's two, so the prologue was wrong at instruction 2 and the
 *     whole stream was shifted.
 *
 *  1. GCSE/PRE WAS INVENTING THE THIRD HIGH REGISTER -- and the fix is to
 *     stop the expression existing. `s + 0x28` appears THREE times in the
 *     reference (the seed loop, the clamp loop and the write-back loop) and
 *     is NEVER commoned there. Spelled as an explicit pointer
 *     (`q = s + 0x28`) in two places, gcse's pre_insert_copies hoists it
 *     into one pseudo live across the entire function: `mov r9, r2` after
 *     the first `add r2, #0x28` and `mov r2, r9` at the write-back loop,
 *     which costs a third call-saved register plus one push and one pop
 *     slot. Re-spelling both loops as
 *         for (k = 0; k < 4; k++) { q = s + (0x28 + k * 8); ... }
 *     puts the SAME pointer back -- loop.c's strength reduction builds the
 *     identical `add r2, #0x28` preheader insn and `add r2, #8` latch -- but
 *     builds it AFTER gcse has run, so gcse never sees `s + 0x28` at all.
 *     The inner parenthesis matters: `s + 0x28 + k * 8` is folded to
 *     `(s + 0x28) + k*8`, which puts `s + 0x28` back in the loop body for
 *     gcse to find, and ALSO makes combine_givs normalise the giv base to
 *     `s` with immediate displacements 0x28/0x2c -- the reference's base is
 *     `s + 0x28` with displacements 0 and 4.
 *     PROLOGUE AND EPILOGUE BECAME BYTE-EXACT. shape 282 -> 240.
 *
 *  2. A SIGNED DIVISION BY A POWER OF TWO KEEPS ITS COPY ONLY IF THE
 *     DIVIDEND PSEUDO HAS MORE THAN ONE SET. expand_divmod emits
 *     `tem = copy(x); if (x >= 0) goto L; tem += 2^k - 1; L: q = tem >> k`,
 *     and the reference keeps that copy (`mov r2, r3` then the THREE-operand
 *     `add r2, r3, r1`) in the HP/PP guard at the top while coalescing it
 *     away in the identical division at the tail. Writing the product into a
 *     named local that is ASSIGNED TWICE --
 *         v = *(short *)(u + 0x34) * *(short *)(u + 0x14);
 *         a = v / 0x4000;              -- and then v is set again
 *     -- reproduced the copy on the first pair exactly. It does NOT
 *     reproduce it on the second, where that same `v` has no later set; that
 *     asymmetry is the one instruction the reference has and we do not in
 *     that block, and it is the cleanest demonstration in this function that
 *     the coalescer, not the expander, decides.
 *
 *  3. THE ABSOLUTE-DIFFERENCE GUARD IS A CONDITIONAL EXPRESSION, NOT TWO
 *     COMPARISONS. The reference's
 *         cmp r2, #0 / blt L1 / cmp r2, #1 / bgt RESET / b CONT
 *         L1: sub r3, r1, r0 / cmp r3, #1 / bgt RESET
 *     is `(a - b < 0 ? b - a : a - b) > 1`: the `> 1` test is DUPLICATED
 *     into both arms because gcc cannot sink it past the conditional, and
 *     `a - b` is commoned into r2 while `b - a` gets its own `sub`. Two
 *     separate `||`-ed comparisons give a different shape. The second pair's
 *     branch sense is `ble` rather than `bgt` purely because it is the last
 *     test before the shared reset block, so a `goto reset` / `goto noreset`
 *     pair reproduces both senses.
 *
 *  4. `(1 << d) & mask` MUST BE SPLIT ACROSS TWO STATEMENTS or fold rewrites
 *     the whole test. Written inline, gcc's fold turns the bit test into
 *     `(mask >> d) & 1` -- our output read `mov r3, r10 / asr r3, r5 /
 *     mov r2, #1 / and r3, r2` against the reference's `mov r3, #1 /
 *     lsl r3, r5 / mov r2, r10 / and r3, r2`. Assigning the shift to its own
 *     local first (`v = 1 << d; t = v & mask;`) hides the pattern from fold,
 *     because the AND's operand is then a VAR_DECL. shape 240 -> 235.
 *
 *  5. AN `&=` WHOSE RESULT IS ONLY STORED TO A BYTE GETS NARROWED, AND THE
 *     NARROW CONSTANT IS CHEAPER. `u[0x130] = t & -10` compiled to
 *     `mov r3, #0xf6` (one instruction, QImode) against the reference's
 *     `mov r3, #0xa / neg r3, r3` (two, SImode). Routing the result through
 *     a second int local -- `v = t & -10; u[0x130] = v;` -- kept it in
 *     SImode and restored the neg pair. shape 235 -> 234, strict 604 -> 580.
 *     The sibling `& -4` block fifty instructions earlier never needed this,
 *     because THERE the AND's result has three further uses, which keeps it
 *     wide on its own. That contrast is the discriminator: narrowing is
 *     blocked by an extra USE, and when there is none you have to add a
 *     second variable to block it.
 *
 *  6. A ONE-SET OFFSET LOCAL IS NOT A BARRIER -- A LOOP-INVARIANT ARRAY
 *     SUBSCRIPT IS. The Djinn mask walk reads `*(int *)(u + 0x108 + e * 4)`.
 *     Spelled as an explicit `int *mp` advanced by `mp++`, gcse hoists the
 *     base the same way it did in lever 1; spelled as
 *     `*(int *)(u + (0x108 + e * 4))` loop.c builds the same walking pointer
 *     after gcse. shape 232.
 *
 *  7. `(signed char)` OF AN `unsigned char *` DEREFERENCE FOLDS INTO
 *     `ldrsb` ONLY THROUGH AN INDIRECT_REF; THROUGH A SUBSCRIPT IT DOES NOT.
 *     The two elemental loops in the tail read a signed byte in two
 *     DIFFERENT ways in the reference, and the difference is not cosmetic:
 *     at +0x12c it is `ldrb r3, [r0] / lsl r3, #24 / asr r3, #24` (three
 *     instructions, so the byte was loaded UNSIGNED and then cast), while at
 *     +0x137 it is `mov r3, #0 / ldrsb r3, [r0, r3]` (two, a real signed
 *     load -- and the `mov #0` is ISA-forced, Thumb-1 `ldrsb` having no
 *     immediate-offset form). Writing `(signed char)*p` gave us the ldrsb
 *     form in BOTH places; writing `(signed char)p[0]` for the first and
 *     `*(signed char *)(u + 0x137)` for the second gave each its own.
 *     strict 526 -> 432, the single largest strict gain in the whole
 *     reconstruction.
 *
 *  8. THREE INDUCTION VARIABLES, NOT TWO, IS WHAT STOPS check_dbra_loop
 *     REVERSING THE CLAMP LOOP -- and reversal is observable. The reference's
 *     four-iteration clamp loop counts UP (`mov r5, #0 / add r5, #1 /
 *     cmp r5, #3 / ble`) and carries THREE pointers stepping by 8: r2 over
 *     +0x28, r1 over +0x2c, and r4 as a bare OFFSET used by exactly one of
 *     the four stores (`str r3, [r6, r4]`, register-offset, where the other
 *     three go through r1/r2). Written with subscripts throughout, gcc
 *     reversed the loop to `mov r4, #3 / sub r4, #1 / cmp r4, #0 / bge` and
 *     built only two givs. Writing two explicit pointers AND keeping the
 *     fourth access in the `*(int *)(off + (int)s)` offset form -- the
 *     CheckLure idiom from next door -- gave three induction variables, and
 *     the loop stopped being reversed. insns 856 -> 862, shape 168 -> 160.
 *
 *  9. REUSING ONE COUNTER ACROSS DISTANT LOOPS COSTS TWO MOVES, BECAUSE
 *     live_length IS THE DENOMINATOR OF global.c's PRIORITY. The tail
 *     `* 20` loop shared `off` and `k` with the loop above it; giving it its
 *     own `off2` and `n` removed two `mov rX, rY` copies and took the
 *     instruction count to EXACTLY 864. insns 862 -> 864.
 *     The same lever read the other way is why the two item loops' indices
 *     are still swapped against the reference (ours i in r8 / j in r5, the
 *     reference's i in r5 / j in r8): the inner index sits in a doubly
 *     nested loop, so its freq term is large and its priority beats the
 *     outer index's, and it takes r5. Moving `j` onto the write-back and
 *     clamp loops as well -- lengthening its live range to lower its
 *     priority -- recovered 20 strict lines (568 -> 548) and then a
 *     further 6 (446 -> 440) by giving it the seed loop as well, but did
 *     NOT flip the pair. Initialising `j` at its declaration, which the batch-310
 *     note says forces rematerialisation, did nothing here: the `for (j = 0;`
 *     re-initialisation makes the declaration's store dead and it is
 *     removed before global.c ever sees it. That is a NEW limit on that
 *     lever and it is worth recording -- initialise-at-declaration only
 *     lowers a pseudo's priority if the declaration's value is the one the
 *     body actually uses.
 *
 * 10. OPERAND ORDER OF `mul` IS THE SECOND OPERAND, NOT THE FIRST. Thumb's
 *     `mul rd, rm` is destructive, so gcc moves one operand into the result
 *     register first. For `A * B` it is B that is moved: `cls[8] * s[0]`
 *     emitted `mov r0, r3` with r3 = s[0]; the reference has `mov r0, r2`
 *     with r2 = cls[8]. Every one of the twenty multiplies in this function
 *     had to be written with the factor the reference moves LAST:
 *     `*(int *)(s + 0) * cls[8]`, `*(int *)(s + 0) * *(int *)(s + 0x54)`,
 *     `*(int *)(s + 8) * ((signed char)u[0x133] + 8)`.
 *
 * 11. THINGS THAT DID NOT PAY, so nobody repeats them. Reversing the operand
 *     order of the six Djinn accumulations (`byte + acc` for `acc += byte`)
 *     moved nothing structurally and cost 30 strict lines. Same for the six
 *     class multiplies (588) and the eight percentage multiplies (594): all
 *     three load-order differences are sched2's, not the source's. Writing
 *     the item address with explicit parentheses
 *     (`*(unsigned short *)(u + (0xd8 + i * 2))`) was worse (shape 245).
 *     Materialising the modifier record pointer into its own local before
 *     the +1 read changed nothing. An extra `case 0: break;` to pull the
 *     switch minimum down to 0 changed nothing at all -- see the blocker.
 *
 * ---------------------------------------------------------------------------
 * THE SWITCH MATERIAL, since this function has three dispatches.
 *
 * All three are TABLES in the reference, so none of them is a Case B site
 * and screen_missing_case has nothing to find here:
 *   switch 1 (additive modifiers): 27 `.word` entries, guard `cmp #0x1a` /
 *     `bls`. count 23ish, range 26, 26 <= 10*23 -> table.
 *   switch 2 (percentage modifiers): 8 entries, guard `sub #7` / `cmp #7` /
 *     `bhi`. The `sub #7` IS the minimum-subtraction, so the lowest case is
 *     7 and there is nothing below it.
 *   switch 3 (the class flag lookup): 6 entries, guard `cmp #5` / `bhi`,
 *     with entry 4 pointing at the default -- so `case 4:` is genuinely
 *     absent and cases 0,1,2,3,5 are the five that exist. count 5, range 5,
 *     5 <= 50 -> table, which is why it is a table and not the tree the
 *     threshold of 5 would otherwise allow.
 * Two adjacent cases were duplicated arms as the brief requires: cases 23
 * and 24 write `u[0x142]` and `u[0x143]` and the reference cross-jumps their
 * common tail (`add r3, r2 / strb r3, [r1]` at a shared label), which is
 * exactly what writing them as two separate arms produces. Cases 1 and 5 of
 * switch 3 both call GetFlag(0x112) and the reference does NOT merge them --
 * case 5's body sits after a `.pool_aligned` dump and falls through to the
 * join rather than branching to it, and jump.c's cross_jump does not
 * consider that pair. Writing them as two independent arms reproduces that.
 *
 * ---------------------------------------------------------------------------
 * THE BLOCKER, attributed.
 *
 * Primary: **gcc's local register allocator, local-alloc.c / global.c**, not
 * any missing statement. Size, instruction count, frame, prologue, epilogue,
 * relocation count and relocation symbol order are all exact; what is left
 * is 446 encodings in 140 hunks, and sampling them shows essentially all of
 * it is (a) which hard register a pseudo got and (b) sched2's choice of
 * order between two independent loads in a pair. The one allocation decision
 * that is clearly WRONG rather than merely different is the item loops'
 * index pair: the reference gives the OUTER index the call-saved low
 * register r5 and exiles the INNER index to r8, paying `mov r1, r8` on entry
 * to the body, `mov r1, #1 / add r8, r1` to increment it and `mov r2, r8` to
 * compare it -- four extra moves it did not have to pay. Our build makes the
 * cheaper choice. global.c's priority is
 * `floor_log2(n_refs) * n_refs * freq / live_length`, and the inner index's
 * `freq` is multiplied by the inner loop's nesting, so for it to LOSE to the
 * outer index its live_length must be much the larger of the two in the
 * reference's source. Every spelling tried here either left the pair as it
 * is or made the function worse; lengthening the inner index's range by
 * sharing it with the tail loops moved 20 strict lines in the right
 * direction without flipping the allocation, which says the lever is the
 * right one and the magnitude is not yet enough.
 *
 * Secondary, and genuinely separate: **expand_divmod's dividend copy** (see
 * lever 2) accounts for one instruction in the HP/PP tail block, and
 * **combine's narrowing of a byte store** is why the HImode `0x4000` store
 * pair reads `ldr r3, =0x4000` here and `mov r3, #0x80 / lsl r3, #7` in the
 * reference -- the reference's value is an SImode constant, ours a HImode
 * one, and no spelling tried (int temp, two-step shift, unsigned short
 * lvalue) moved it. Those two plus the two `ldrb`+`lsl`+`asr` pairs in the
 * modifier reads are the only places where the instruction MIX differs
 * rather than the register names, and they cancel against each other, which
 * is exactly the "exact count hiding two cancelling defects" trap the brief
 * warns about -- hence the relocation-sequence check above, which is clean.
 *
 * NOT a blocker, checked and excluded: there is no missing switch case
 * (relocation counts prove all three tables are the reference's length), no
 * missing frame slot (one 4-byte spill, found and reproduced), no missing
 * aggregate (grep 2 is empty), and no shim is needed.
 */
extern unsigned char *Func_8004970(int size);
extern void free(void *p);
extern unsigned char *GetUnit(int id);
extern unsigned char *GetItemInfo(int item);
extern unsigned char *GetDjinniInfo(int element, int index);
extern unsigned char *GetClassInfo(int cls);
extern int GetFlag(int id);

void CalcStats(int id)
{
    unsigned char *s;
    unsigned char *u;
    unsigned char *p;
    unsigned char *q;
    unsigned char *cls;
    int j = 0;
    int i, k, n, e, d, off2;
    int t, a, b, off, v, old, cap, mask, flag;

    s = Func_8004970(0x60);
    u = GetUnit(id);
    *(int *)(s + 0x00) = *(short *)(u + 0x10);
    *(int *)(s + 0x04) = *(short *)(u + 0x12);
    *(int *)(s + 0x08) = *(unsigned short *)(u + 0x18);
    *(int *)(s + 0x0c) = *(unsigned short *)(u + 0x1a);
    *(int *)(s + 0x10) = *(unsigned short *)(u + 0x1c);
    *(int *)(s + 0x18) = u[0x1e];
    *(int *)(s + 0x1c) = u[0x1f] & 0xf;
    *(int *)(s + 0x20) = u[0x20];
    *(int *)(s + 0x24) = u[0x21];
    for (j = 0; j < 4; j++) {
        p = u + (0x24 + j * 4);
        q = s + (0x28 + j * 8);
        *(int *)q = *(short *)p;
        *(int *)(q + 4) = *(short *)(p + 2);
    }

    v = *(short *)(u + 0x34) * *(short *)(u + 0x14);
    a = v / 0x4000;
    b = *(short *)(u + 0x38);
    if ((a - b < 0 ? b - a : a - b) > 1)
        goto reset;
    v = *(short *)(u + 0x36) * *(short *)(u + 0x16);
    a = v / 0x4000;
    b = *(short *)(u + 0x3a);
    if ((a - b < 0 ? b - a : a - b) <= 1)
        goto noreset;
reset:
    *(unsigned short *)(u + 0x14) = 0x4000;
    *(unsigned short *)(u + 0x16) = 0x4000;
    *(unsigned short *)(u + 0x38) = *(unsigned short *)(u + 0x34);
    *(unsigned short *)(u + 0x3a) = *(unsigned short *)(u + 0x36);
noreset:

    t = u[0x130] & -4;
    u[0x130] = t;
    if ((t & 4) != 0)
        u[0x130] = t | 1;
    if (u[0x144] != 0)
        *(int *)(s + 0x1c) += 1;
    u[0x142] = 0;
    u[0x143] = 0;
    if (u[0x129] != 0) {
        for (i = 0; i <= 0xe; i++) {
            off = 0xd8 + i * 2;
            if ((*(unsigned short *)(off + (int)u) & 0x200) != 0) {
                *(unsigned char **)(s + 0x58) =
                    GetItemInfo(*(unsigned short *)(off + (int)u));
                if (((*(unsigned char **)(s + 0x58))[3] & 1) != 0)
                    u[0x130] |= 3;
                *(int *)(s + 0x08) +=
                    *(short *)(*(unsigned char **)(s + 0x58) + 8);
                *(int *)(s + 0x0c) +=
                    *(signed char *)(*(unsigned char **)(s + 0x58) + 0xa);
                for (j = 0; j <= 3; j++) {
                    b = 0x18 + j * 4;
                    *(int *)(s + 0x48) = (*(unsigned char **)(s + 0x58))[b];
                    *(int *)(s + 0x54) =
                        *(signed char *)(*(unsigned char **)(s + 0x58) + b + 1);
                    switch (*(int *)(s + 0x48)) {
                    case 0: break;
                    case 1: *(int *)(s + 0x00) += *(int *)(s + 0x54); break;
                    case 2: *(int *)(s + 0x20) += *(int *)(s + 0x54); break;
                    case 3: *(int *)(s + 0x04) += *(int *)(s + 0x54); break;
                    case 4: *(int *)(s + 0x24) += *(int *)(s + 0x54); break;
                    case 5: *(int *)(s + 0x10) += *(int *)(s + 0x54); break;
                    case 6: *(int *)(s + 0x18) += *(int *)(s + 0x54); break;
                    case 15: *(int *)(s + 0x28) += *(int *)(s + 0x54); break;
                    case 16: *(int *)(s + 0x30) += *(int *)(s + 0x54); break;
                    case 17: *(int *)(s + 0x38) += *(int *)(s + 0x54); break;
                    case 18: *(int *)(s + 0x40) += *(int *)(s + 0x54); break;
                    case 19: *(int *)(s + 0x2c) += *(int *)(s + 0x54); break;
                    case 20: *(int *)(s + 0x34) += *(int *)(s + 0x54); break;
                    case 21: *(int *)(s + 0x3c) += *(int *)(s + 0x54); break;
                    case 22: *(int *)(s + 0x44) += *(int *)(s + 0x54); break;
                    case 23: u[0x142] += *(int *)(s + 0x54); break;
                    case 24: u[0x143] += *(int *)(s + 0x54); break;
                    case 25: u[0x130] |= 8; break;
                    case 26: *(int *)(s + 0x1c) += *(int *)(s + 0x54); break;
                    }
                }
            }
        }
        t = u[0x130];
        if ((t & 8) != 0) {
            v = t & -10;
            u[0x130] = v;
        }
        for (e = 0; e <= 3; e++) {
            mask = *(int *)(u + (0x108 + e * 4));
            for (d = 0; d <= 0x13; d++) {
                v = 1 << d;
                t = v & mask;
                if (t != 0) {
                    p = GetDjinniInfo(e, d);
                    *(int *)(s + 0x00) += *(signed char *)(p + 4);
                    *(int *)(s + 0x04) += *(signed char *)(p + 5);
                    *(int *)(s + 0x08) += *(signed char *)(p + 6);
                    *(int *)(s + 0x0c) += *(signed char *)(p + 7);
                    *(int *)(s + 0x10) += *(signed char *)(p + 8);
                    *(int *)(s + 0x18) += *(signed char *)(p + 9);
                }
            }
        }
        cls = GetClassInfo(u[0x129]);
        *(int *)(s + 0x00) = *(int *)(s + 0x00) * cls[8] / 10;
        *(int *)(s + 0x04) = *(int *)(s + 0x04) * cls[9] / 10;
        *(int *)(s + 0x08) = *(int *)(s + 0x08) * cls[0xa] / 10;
        *(int *)(s + 0x0c) = *(int *)(s + 0x0c) * cls[0xb] / 10;
        *(int *)(s + 0x10) = *(int *)(s + 0x10) * cls[0xc] / 10;
        *(int *)(s + 0x18) = *(int *)(s + 0x18) * cls[0xd] / 10;
        for (i = 0; i <= 0xe; i++) {
            off = 0xd8 + i * 2;
            if ((*(unsigned short *)(off + (int)u) & 0x200) != 0) {
                *(unsigned char **)(s + 0x58) =
                    GetItemInfo(*(unsigned short *)(off + (int)u));
                for (j = 0; j <= 3; j++) {
                    b = 0x18 + j * 4;
                    *(int *)(s + 0x48) = (*(unsigned char **)(s + 0x58))[b];
                    *(int *)(s + 0x54) =
                        *(signed char *)(*(unsigned char **)(s + 0x58) + b + 1);
                    switch (*(int *)(s + 0x48)) {
                    case 7:
                        *(int *)(s + 0x00) = *(int *)(s + 0x00) * *(int *)(s + 0x54) / 10;
                        break;
                    case 8:
                        *(int *)(s + 0x20) = *(int *)(s + 0x20) * *(int *)(s + 0x54) / 10;
                        break;
                    case 9:
                        *(int *)(s + 0x04) = *(int *)(s + 0x04) * *(int *)(s + 0x54) / 10;
                        break;
                    case 10:
                        *(int *)(s + 0x24) = *(int *)(s + 0x24) * *(int *)(s + 0x54) / 10;
                        break;
                    case 11:
                        *(int *)(s + 0x08) = *(int *)(s + 0x08) * *(int *)(s + 0x54) / 10;
                        break;
                    case 12:
                        *(int *)(s + 0x0c) = *(int *)(s + 0x0c) * *(int *)(s + 0x54) / 10;
                        break;
                    case 13:
                        *(int *)(s + 0x10) = *(int *)(s + 0x10) * *(int *)(s + 0x54) / 10;
                        break;
                    case 14:
                        *(int *)(s + 0x18) = *(int *)(s + 0x18) * *(int *)(s + 0x54) / 10;
                        break;
                    }
                }
            }
        }
    }
    *(int *)(s + 0x08) = *(int *)(s + 0x08) * ((signed char)u[0x133] + 8) / 8;
    *(int *)(s + 0x0c) = *(int *)(s + 0x0c) * ((signed char)u[0x135] + 8) / 8;
    *(int *)(s + 0x10) = *(int *)(s + 0x10) * ((signed char)u[0x147] + 8) / 8;
    off = 0x28;
    k = 3;
    p = u + 0x12c;
    do {
        t = (signed char)p[0];
        *(int *)(off + (int)s) += (t * t + t) * 5;
        k--;
        p++;
        off += 8;
    } while (k >= 0);
    off2 = 0x2c;
    n = 3;
    do {
        *(int *)(off2 + (int)s) += *(signed char *)(u + 0x137) * 20;
        n--;
        off2 += 8;
    } while (n >= 0);
    if (u[0x129] != 0) {
        flag = 0;
        switch (u[0x128]) {
        case 0: flag = GetFlag(0x88 * 2); break;
        case 1: flag = GetFlag(0x89 * 2); break;
        case 2: flag = GetFlag(0x113); break;
        case 3: flag = GetFlag(0x111); break;
        case 5: flag = GetFlag(0x89 * 2); break;
        }
        if (flag != 0)
            *(int *)(s + 0x24) += 4;
    }
    if (*(int *)(s + 0x08) < 0) *(int *)(s + 0x08) = 0;
    if (*(int *)(s + 0x08) > 0x3e7) *(int *)(s + 0x08) = 0x3e7;
    if (*(int *)(s + 0x0c) < 0) *(int *)(s + 0x0c) = 0;
    if (*(int *)(s + 0x0c) > 0x3e7) *(int *)(s + 0x0c) = 0x3e7;
    if (*(int *)(s + 0x10) < 0) *(int *)(s + 0x10) = 0;
    if (*(int *)(s + 0x10) > 0x3e7) *(int *)(s + 0x10) = 0x3e7;
    if (*(int *)(s + 0x18) < 0) *(int *)(s + 0x18) = 0;
    if (*(int *)(s + 0x18) > 0x63) *(int *)(s + 0x18) = 0x63;
    if (*(int *)(s + 0x1c) < 0) *(int *)(s + 0x1c) = 0;
    if (*(int *)(s + 0x1c) > 2) *(int *)(s + 0x1c) = 2;
    if (*(int *)(s + 0x20) < 0) *(int *)(s + 0x20) = 0;
    if (*(int *)(s + 0x20) > 0x2710) *(int *)(s + 0x20) = 0x2710;
    if (*(int *)(s + 0x24) < 0) *(int *)(s + 0x24) = 0;
    if (*(int *)(s + 0x24) > 0xc8) *(int *)(s + 0x24) = 0xc8;
    p = s + 0x28;
    q = s + 0x2c;
    off = 0x2c;
    for (j = 0; j <= 3; j++) {
        if (*(int *)p < 0) *(int *)p = 0;
        if (*(int *)p > 0xc8) *(int *)p = 0xc8;
        if (*(int *)q < 0) *(int *)q = 0;
        if (*(int *)(off + (int)s) > 0xc8) *(int *)(off + (int)s) = 0xc8;
        p += 8;
        q += 8;
        off += 8;
    }
    *(short *)(u + 0x3c) = *(int *)(s + 0x08);
    *(short *)(u + 0x3e) = *(int *)(s + 0x0c);
    *(short *)(u + 0x40) = *(int *)(s + 0x10);
    u[0x42] = *(int *)(s + 0x18);
    u[0x43] = *(int *)(s + 0x1c);
    u[0x44] = *(int *)(s + 0x20);
    u[0x45] = *(int *)(s + 0x24);
    for (j = 0; j < 4; j++) {
        p = u + (0x48 + j * 4);
        q = s + (0x28 + j * 8);
        *(short *)p = *(int *)q;
        *(short *)(p + 2) = *(int *)(q + 4);
    }
    cap = u[0x129] == 0 ? 0x270f : 0x7cf;
    old = *(short *)(u + 0x34);
    if (*(int *)(s + 0x00) < 0) *(int *)(s + 0x00) = 0;
    if (*(int *)(s + 0x00) > cap) *(int *)(s + 0x00) = cap;
    *(short *)(u + 0x34) = *(int *)(s + 0x00);
    if (old != *(short *)(u + 0x34)) {
        t = *(short *)(u + 0x14) * *(int *)(s + 0x00) / 0x4000;
        if (t < 0) t = 0;
        if (t > cap) t = cap;
        if (*(short *)(u + 0x38) != 0 && t == 0) t = 1;
        *(short *)(u + 0x38) = t;
    }
    old = *(short *)(u + 0x36);
    if (*(int *)(s + 0x04) < 0) *(int *)(s + 0x04) = 0;
    if (*(int *)(s + 0x04) > cap) *(int *)(s + 0x04) = cap;
    *(short *)(u + 0x36) = *(int *)(s + 0x04);
    if (old != *(short *)(u + 0x36)) {
        t = *(short *)(u + 0x16) * *(int *)(s + 0x04) / 0x4000;
        if (t < 0) t = 0;
        if (t > cap) t = cap;
        if (*(short *)(u + 0x3a) != 0 && t == 0) t = 1;
        *(short *)(u + 0x3a) = t;
    }
    free(s);
}
