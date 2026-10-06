/* p1 -- Field_Move -- PARK at 2 differing encodings of 26.  PIN-FREE, DEVICE-FREE.
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

