/* Func_80f7f30 -- 0x080f7f30 -- asm/rom_f6000/rom_f6008_c_c_c.s
 *
 * NON-MATCHING, 9 differing encodings of 33  (MEASURED, batch 329 brief J;
 * was 10 of 33).  Counts agree (ref 33, ours 33), 31 instruction lines against
 * 31, and the two 0x4404 pool loads share one word so POOL WORD agrees.
 * First differing index 2.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_f6000/80f7f30.c asm/rom_f6000/rom_f6008_c_c_c.s --func Func_80f7f30
 *
 * ----- everything below to the next ===== heading is the INHERITED park text,
 * ----- kept verbatim; its figure line and recipe are superseded by the two
 * ----- above, and its numbered rule 1 is REFUTED further down.
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 *
 * BLOCKER: register-offset loads where the ROM builds addresses.
 * 10 of 31, LENGTH EXACT.
 *
 * Copies `count` bytes from a fixed source into dst at a running index held in
 * memory, bumping that index per byte and re-reading the count each pass.
 *
 * TWO ORDERING FIXES took it from 28 differing to 10, and both were read off
 * the ROM rather than guessed:
 *
 *   1. THE OFFSET IS LOADED BEFORE THE BASE. The ROM has `ldr r1, =0x4404 /
 *      ldr r2, [r3] / add r3, r2, r1`, and keeps 0x4404 live to rebuild the
 *      count pointer inside the loop body. Assigning `o = 0x4404` before the
 *      base pointer reproduces that; the other order costs the length.
 *   2. THE COUNTER IS INITIALISED BEFORE THE TEST. `mov r0, #0` precedes the
 *      `cmp`, so `i = 0;` belongs before the `if`, not inside it.
 *
 * WHAT REMAINS is the addressing form at the two `base + o` sites:
 *
 *     rom    add r3, r2, r1 / ldr r3, [r3, #0x0]     (address built, then load)
 *     ours   ldr r3, [r2, r3]                        (register-offset load)
 *
 * MEASURED, none of them move it:
 *   a named pointer local for the test site
 *     (`t = (int *)(base + o); if (*t != 0)`)     30 lines, 24 differ  WORSE
 *   index-first form at the test site
 *     (`*(int *)(o + (int)base)`)                 31 lines, 10 differ
 *   index-first at both sites                     31 lines, 10 differ
 *
 * The last two are byte-identical to the baseline, so gcc canonicalises
 * `o + (int)base` and `base + o` to one rtx here. That bounds the operand-order
 * lever, which DID decide this exact choice on Func_80b9a70 and
 * OvlFunc_916_2008be4: it works when the two operands are a base and a
 * COMPUTED index, and does nothing when the index is a loop-invariant constant
 * gcc has already hoisted into a register.
 *
 * The named-pointer result is the sharper negative -- naming the address is the
 * documented way to force `add` + immediate load, and here it costs a line and
 * fourteen differences, because the named pointer is then shared with the
 * in-loop count pointer that the ROM rebuilds separately.
extern int ewram_2004c00;

void Func_80f7f30(unsigned char *dst)
{
    char *base;
    int *pos;
    int *cnt;
    unsigned char *src;
    int i;
    int o;
    int off;

    o = 0x4404;
    base = (char *)ewram_2004c00;
    i = 0;
    if (*(int *)(base + o) != 0) {
        off = 0x443c;
        pos = (int *)(base + off);
        off -= 0x34;
        cnt = (int *)(base + o);
        src = (unsigned char *)(base + off);
        do {
            dst[*pos] = *src;
            *pos = *pos + 1;
            i++;
            src++;
        } while (i != *cnt);
    }
}
 * ===== THE PARK'S RULE 1 IS REFUTED: BASE BEFORE OFFSET IS BETTER. =====
 *
 * The park recorded "THE OFFSET IS LOADED BEFORE THE BASE ... the other order
 * costs the length."  It does not.  Assigning `base` first and `o` second keeps
 * the stream at 31 lines and reads 9 of 33 where offset-first reads 10.  One
 * encoding, but it is the park's stated prerequisite that was wrong, and the
 * row was never retested after the body changed around it.
 *
 * ===== ALL NINE REMAINING ARE ONE DECISION, AND IT IS COUPLED. =====
 *
 *   rom   ldr r3,=0x2004c00 / ldr r1,=0x4404 / ldr r2,[r3] / add r3,r2,r1
 *         / ldr r3,[r3,#0x0] / mov r6,r0 / mov r0,#0 / cmp / beq / ldr r3,=0x443c
 *   ours  ldr r2,=0x2004c00 / ldr r3,=0x4404 / ldr r2,[r2] / ldr r3,[r2,r3]
 *         / mov r6,r0 / mov r0,#0 / cmp / beq / ldr r3,=0x443c / ldr r1,=0x4404
 *
 * We save one instruction with a REGISTER-OFFSET load and pay it straight back
 * with a second `ldr =0x4404` after the branch, because the reg-offset load
 * consumes `o` as a dying operand and reuses its register as the destination.
 * Everything from index 5 on is that one swap shifting the stream.
 *
 * TWO FACTS READ IN THE COMPILER, both new:
 *
 * 1. THE ADDRESSING FORM IS THE EXPANDER'S, NOT COMBINE'S.  In `.01.sibling`
 *    the test load is already
 *      (insn 23 (set (reg:SI 41) (mem:SI (plus:SI (reg/v:SI 33) (reg/v:SI 38)))))
 *    -- the plus is inside the MEM from expand, because `o` is a pseudo.  Write
 *    the offset as the LITERAL `*(int *)(base + 0x4404)` and the address cannot
 *    be a reg+reg MEM, so reload materialises it and gcc emits the ROM's
 *    `add r0,r2,r3 / ldr r3,[r0,#0x0]` exactly.
 *
 * 2. WHAT THEN COSTS THE LINE IS GCSE, NOT CSE1.  With the literal at the test
 *    site the stream comes out 30 lines, 24 differ, because the test's address
 *    gets shared with `cnt`.  `-fno-gcse` on that same body gives 31 lines and
 *    5 differ -- right length, and the residue is only the pool-load order, the
 *    register naming of the add pair, and `mov r5,r1` where the ROM has
 *    `add r5,r2,r1`.  So gcse (pass 07) is the pass that shares the two
 *    `base + 0x4404` adds; -fno-cse-follow-jumps, -fno-cse-skip-blocks and
 *    --no-rerun-cse all leave it at 24.
 *
 * THE COUPLING, stated so the next reader does not re-derive it: the ROM
 * computes `base + 0x4404` TWICE, once for the entry test and once for `cnt`.
 * gcc will give us the ROM's add+load form (literal offset) or two independent
 * adds (variable offset), never both, because whichever form puts an explicit
 * `(plus reg X)` in the entry block makes `cnt`'s identical plus redundant and
 * gcse removes it.  Closing this needs a source construct that makes the two
 * sums non-identical to gcse at zero instruction cost, or a flag.
 *
 * MEASURED (all 31 lines unless noted):
 *   base before offset                                      9  THIS BODY
 *   offset before base (the park's rule 1)                  10
 *   `i = 0` moved inside the if                             12
 *   `cnt` assigned after `src` instead of before            13
 *   literal at cnt, variable at the test                    10
 *   literal at the test, variable at cnt           30 lines, 24
 *   literal at both sites                          30 lines, 24
 *   literal at the test, `o` assigned inside the if 30 lines, 24
 *   count read inline in the do-while condition    29 lines, 30
 *   ditto with literals                            30 lines, 24
 *   `cnt` derived from `pos` (`(char *)pos - 0x38`) 35 lines, 26
 *   `*(int *)&base[o]` at the test                          10, BYTE-IDENTICAL
 *       to the baseline -- the C front end folds `&arr[i]` to `arr + i`, so it
 *       is the same tree and cannot reach a different expand path.  That bounds
 *       the ARRAY_REF/EXPAND_SUM lever (expr.c:7340) for this shape.
 *   `o + (int)base` index-first, at one site and at both   10, byte-identical
 *       (the park's own rows, reproduced: gcc canonicalises the sum)
extern int ewram_2004c00;

void Func_80f7f30(unsigned char *dst)
{
    char *base;
    int *pos;
    int *cnt;
    unsigned char *src;
    int i;
    int o;
    int off;

    base = (char *)ewram_2004c00;
    o = 0x4404;
    i = 0;
    if (*(int *)(base + o) != 0) {
        off = 0x443c;
        pos = (int *)(base + off);
        off -= 0x34;
        cnt = (int *)(base + o);
        src = (unsigned char *)(base + off);
        do {
            dst[*pos] = *src;
            *pos = *pos + 1;
            i++;
            src++;
        } while (i != *cnt);
    }
}
 */
extern int ewram_2004c00;

void Func_80f7f30(unsigned char *dst)
{
    char *base;
    int *pos;
    int *cnt;
    unsigned char *src;
    int i;
    int o;
    int off;

    base = (char *)ewram_2004c00;
    o = 0x4404;
    i = 0;
    if (*(int *)(base + o) != 0) {
        off = 0x443c;
        pos = (int *)(base + off);
        off -= 0x34;
        cnt = (int *)(base + o);
        src = (unsigned char *)(base + off);
        do {
            dst[*pos] = *src;
            *pos = *pos + 1;
            i++;
            src++;
        } while (i != *cnt);
    }
}
