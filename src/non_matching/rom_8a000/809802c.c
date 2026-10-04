/* Field_Move (0x0809802c) -- STILL PARKED at 2 of 26.  batch 321, brief A,
 * target 1.  NOT A LANDING.  The park's closure is CONFIRMED, not refuted.
 *
 * ref:     asm/rom_8a000/rom_97b54_a_c_a_a_a_c_a_c_a.s
 * install: src/rom_8a000/rom_97b54_a_c_a_a_a_c_a_c_a.c  (if ever landed)
 * park:    src/non_matching/rom_8a000/809802c.c  -- KEEP IT, this body is its body
 *
 * MEASURED THIS BATCH, both forms:
 *   --func   XX 2 of 26 differ (ref 26, ours 26), first at index 3
 *   --whole  XX Field_Move  2 of 26 differ (ours 26), first at index 3 -- clean,
 *            no relocation dirt, so the figure IS a distance.
 *   PINS 0.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_8a000/809802c.c \
 *     asm/rom_8a000/rom_97b54_a_c_a_a_a_c_a_c_a.s --func Field_Move
 *   (NOTE: the park's own title line names rom_97b54_a_c_a_a.s, which is NOT the
 *   reference -- the path in its Verify recipe is the one that works.)
 *
 * The residue, decided from the encodings rather than inherited: index 3 is ref
 * b083 `sub sp,#0xc` against ours 691d `ldr r5,[r3,#0x10]`.  An adjacent
 * transposition in sched2, exactly as the park says.  The park's batch-271
 * closure computes the ready list and shows `sub sp` losing by EXACTLY ONE
 * priority unit for any source that loads the caster from memory, with a tie
 * also losing on the LUID tiebreak.  I did not re-sweep it; the brief's own
 * instruction in that park is "DO NOT SWEEP THIS AGAIN", and it is right.
 *
 * WHAT IS NEW, AND IT IS A NEGATIVE WITH ITS REASON ATTACHED:
 *
 * This batch closed the other two sched2 targets in this brief (Func_8095938 and
 * Func_8095fcc) with ONE lever -- an alias-set-0 union member access, which both
 * restores a memory anti-dependence and raises a rival chain's sched2 priority.
 * THAT LEVER CANNOT REACH THIS FUNCTION, and the reason is structural rather
 * than empirical, so it should not be retried:
 *
 *   The lever works by giving an instruction NEW SUCCESSORS through the memory
 *   dependence graph, which raises its priority.  Here the instruction that must
 *   win is `sub sp, #0xc`, and the park already establishes that THE TWELVE BYTES
 *   ARE NEVER ACCESSED -- nothing is stored to the frame and nothing takes its
 *   address.  An instruction with no memory successors cannot be given any by
 *   changing an alias set, and giving it a real one means emitting a store, which
 *   takes the function to 27 instructions against the ROM's 26.  So the only two
 *   knobs sched2 has -- priority and LUID -- are both out of reach here for the
 *   same reason the park gives: nothing in C orders anything against the frame
 *   adjustment.
 *
 * ALSO CHECKED THIS BATCH: tools/dupfuncs.py reports 8 duplicate groups covering
 * 16 functions tree-wide and NONE of this brief's four targets is in any of them.
 * Field_Move (26 encodings) and Field_Halt (191) are NOT a near-duplicate pair,
 * so there is no body to port between them -- what transfers between the
 * rom_8a000 field functions is the LEVER, not the body.
 *
 * NEXT: a compiler-side reading, not a spelling.  Unchanged from batch 271.
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

