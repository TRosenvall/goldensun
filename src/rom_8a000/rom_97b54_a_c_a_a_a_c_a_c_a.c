/* Field_Move -- MATCHES.  0 differing encodings of 26 (STREAM), 68 bytes, 26
 * encodings and 8 relocations identical; `--whole` also clean.  Instructions
 * 17/17.  PINS 0 -- tools/shimcount.py reports `empty asm : 1 (NOT treated as a
 * fakematch in this tree)`, the same standing as the four barriers in the landed
 * src/rom_c0/rom_5cf8_a_a_c_a_c.c, which carries no fakematch row either.
 * DEVICE-FREE, no flag group, no split (the .s holds this function alone and
 * tools/datacheck.py prints nothing for it).
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_8a000/rom_97b54_a_c_a_a_a_c_a_c_a.c asm/rom_8a000/rom_97b54_a_c_a_a_a_c_a_c_a.s --whole
 *
 * ================================================================
 * WHAT CLOSED IT: ONE LINE, AND ITS PLACEMENT IS THE WHOLE RESULT
 * ================================================================
 *     a = *(void **)(iwram_3001f30 + 0x10);
 *     __asm__ volatile ("");          <- NEW, immediately AFTER the load
 *     Func_8097384();
 *
 * The park below was at two, and its residue was the adjacent transposition at
 * indices 3 and 4: the ROM issues `sub sp,#0xc` (b083) there and we issued
 * `ldr r5,[r3,#0x10]` (691d).  Crossed placements, all at 26 encodings / 17
 * instructions:
 *     barrier AFTER the load                 *** 0, byte-identical ***
 *     no barrier (the park body)                 2, first idx 3
 *     barrier BEFORE the load (top of body)      3, first idx 1   WORSE
 *     barrier on BOTH sides of the load          3, first idx 1   WORSE
 * A barrier before the load is not merely inert, it is worse -- it drags the
 * first difference from index 3 up to index 1 -- so the placement is a real
 * one-slot target and not a "sprinkle a barrier" result.
 *
 * ================================================================
 * THE PARK'S BOUND REPRODUCED IN EVERY STATEMENT AND WAS STILL WRONG
 * ================================================================
 * This is batch 335's finding, second instance: CHECK THE PRECONDITION OF THE
 * COMPARISON, NOT ITS ARITHMETIC.  The park closed a PRIORITY argument at both
 * ends and declared "2 is the floor at 26".  Priority never got to decide.
 *
 * Its observation (a) is exactly right and I re-ran it: `grep -c
 * "stack_pointer\|frame_pointer" haifa-sched.c` is ZERO in this build, so the
 * scheduler has no special case for a write of sp.  Its INFERENCE -- "its
 * dependents are exactly the sp-referencing insns, and bb0 is six instructions
 * and not one references sp, so a dependent of sub sp is necessarily a 27th
 * instruction" -- is what fails, because an ASM_INPUT becomes a dependent of
 * `sub sp` WITHOUT REFERENCING sp IN ANY SOURCE EXPRESSION:
 *
 *     haifa-sched.c:3568  case ASM_INPUT:
 *     haifa-sched.c:3582      int max_reg = max_reg_num ();
 *     haifa-sched.c:3583      for (i = 0; i < max_reg; i++)
 *     haifa-sched.c:3589        for (u = deps->reg_last_sets[i]; u; u = XEXP (u, 1))
 *     haifa-sched.c:3590          add_dependence (insn, XEXP (u, 0), 0);
 *     haifa-sched.c:3595      reg_pending_sets_all = 1;
 *     haifa-sched.c:3597      flush_pending_lists (deps, insn, 0);
 *
 * :3589-3590 walk EVERY register, hard and pseudo, and make the asm depend on
 * that register's last set -- the stack pointer's included.  :3595 then makes
 * everything after the asm depend on it.  So one zero-byte insn hands `sub sp`
 * the forward dependent the park proved no C expression could give it, and it
 * costs nothing in the output.
 *
 * > A BARE `__asm__ volatile ("")` IS A TWO-SIDED SCHEDULING BARRIER THAT
 * > DEPENDS ON EVERY REGISTER'S LAST SET, SO IT CAN GIVE A *PROLOGUE* INSN A
 * > FORWARD DEPENDENT.  A BOUND THAT REASONS ABOUT WHICH *SOURCE EXPRESSIONS*
 * > CAN REFERENCE A REGISTER DOES NOT COVER IT.
 *
 * Fourteen parks in the tree bound a sched2 residue by arguing that nothing can
 * depend on, or be depended on by, a particular insn.  This is the second to
 * fall to one line.
 * -- scratch_elev/b336/E
 *
 * ============ THE PARK THIS REPLACES, CARRIED FORWARD VERBATIM ===============
 * p1 -- Field_Move -- PARK at 2 differing encodings of 26.  PIN-FREE, DEVICE-FREE.
 *
 * Figure I measured (not inherited): 2 differing encodings of 26 (ref 26, ours 26),
 * first differing index 3 -- ref b083 (sub sp,#0xc) against ours 691d
 * (ldr r5,[r3,#0x10]).  No SIZE line, no INSTRUCTION COUNT line, no RELOCATIONS
 * line, so the figure IS a distance.  PINS 0 (no inline asm, no register
 * declarations anywhere in the body).
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/809802c.c asm/rom_8a000/rom_97b54_a_c_a_a_a_c_a_c_a.s --func Field_Move
 *
 * SPLIT SHAPE: unchanged from the park -- install path if it ever lands is
 * src/rom_8a000/rom_97b54_a_c_a_a_a_c_a_c_a.c.  No split run recorded as needed.
 *
 * THE RESIDUE IS one adjacent transposition, indices 3 and 4, and nothing else.
 * One run, one cause: sched2 issues insn 14 (the load) at t=4 ahead of insn 77
 * (sub sp), priority 37 against 35.
 *
 * WHAT I ADDED TO THE PARK -- the ceiling argument is now closed at both ends.
 *   (a) haifa-sched.c in this build has ZERO occurrences of stack_pointer or
 *       frame_pointer, so there is no special case giving a write of sp a memory
 *       dependence; its dependents are exactly the sp-referencing insns.
 *   (b) The reference must be in bb0.  INSTRUMENT, labelled: the park's frame
 *       store moved into the `if (r != 0)` body instead of bb0 leaves sub sp
 *       exactly where it was and costs 2 instructions (18 of 28).
 *   (c) The other side is pinned too: priority(14) is 37 rather than 36 only
 *       because of the cost-2 edge `ldr r5` -> `mov r0,r5`; arm_adjust_cost
 *       returns 1 only when the CONSUMER is a CALL_INSN and insn 19 is a mov, so
 *       that edge cannot be reduced, and deleting insn 19 is a 25-insn body.
 * bb0 in the ROM is six instructions and not one references sp, so a dependent
 * of sub sp is necessarily a 27th instruction.  2 is the floor at 26.
 *
 * MEASURED THIS ROUND (the park's 13 frame-object rows deliberately NOT re-swept):
 *   EXACTLY INERT at 2 -- `if (r)` for `if (r != 0)`; int-typed handles throughout.
 *   WORSE -- tail calls swapped, 4 and RELOCATIONS differ.
 */
extern char *iwram_3001f30;
extern void Func_8097384(void);
extern void *Func_8098070(void *a);
extern void Func_8098184(void);
extern void _Actor_SetAnim(void *a, int n);
extern void WaitFrames(int n);
extern void Func_809748c(void);
extern void Func_80981b0(void *a);

void Field_Move(void)
{
    char buf[12];
    void *a;
    void *r;

    a = *(void **)(iwram_3001f30 + 0x10);
    __asm__ volatile ("");
    Func_8097384();
    r = Func_8098070(a);
    Func_8098184();
    if (r != 0) {
        _Actor_SetAnim(r, 4);
        WaitFrames(0x1e);
    }
    Func_809748c();
    Func_80981b0(r);
}

