/* Func_8093054 (ShowMessageWithPrompt) -- NON-MATCHING.
 *
 * NON-MATCHING, 39 of 44 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 44, ours 40) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   RELOCATIONS differ, but THE SAME SYMBOLS AT A SHIFTED OFFSET -- which
 *   this project treats as a CONSEQUENCE of the length difference, not a
 *   separate blocker.  Re-classified in batch 322; the figure IS a distance.
 *   SIZE ref 100 bytes, ours 92.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_8a000/8093054.c \
 *     asm/rom_8a000/rom_92950_c_a_c_c.s --func Func_8093054
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Blocker class: CROSS-BRANCH CSE of a duplicated block.
 * 40 lines against the ROM's 43 -- THREE SHORT -- 37 differing.
 *
 * The function has two arms that do the same two things in opposite order:
 * one increments the message id at iwram_3001ebc + 0x1d8 and then shows the
 * line, the other shows the line and then increments. The ROM writes the
 * increment out IN FULL in both arms -- eight instructions each, including a
 * fresh `ldr r3, =iwram_3001ebc / ldr r2, [r3]` and a fresh `mov r3, #0xec /
 * lsl r3, #1`. gcc hoists the shared address computation above the branch and
 * both arms use it, which is where the three missing lines go.
 *
 * The stream being SHORT is the tell, and it is the same reading as
 * Func_80a1bdc and OvlFunc_957_2008f10: the source is not carrying what the
 * ROM carries. Here, though, the missing thing is not a value -- it is the
 * DUPLICATION itself, and C has no way to ask for an expression to be
 * evaluated twice when the compiler can prove once is enough.
 *
 * Tried, none reaching the duplication:
 *   - the increment written out inline in both arms:            37 differing
 *   - each arm with its own named `p` and `off` locals:         37, identical
 *   - two separate `return choice;` statements so the arms have
 *     no join for gcc to hoist to:                              41 differing
 *     and one line LONGER, so it recovered a line and spent it elsewhere
 *
 * A second, smaller difference is unexplained: the ROM saves its parameters as
 * `mov r6, r1 / mov r5, r0`, taking the SECOND argument first, and gcc takes
 * the first. Nothing tried moved that either, and it is probably downstream of
 * the register assignment the hoist forces.
 */
extern unsigned char gState[];
extern char *iwram_3001ebc;
extern void Func_8092c40(void);
extern int Func_8091c7c(int a, int b);
extern void ActorMessage(int a, int b);

int Func_8093054(int slot, int flags)
{
    int choice;
    unsigned short *m;

    Func_8092c40();
    choice = Func_8091c7c(*(int *)(gState + (0xfa << 1)), 0);
    if (choice != 0) {
        m = (unsigned short *)(iwram_3001ebc + (0xec << 1));
        *m = *m + 1;
        ActorMessage(slot, flags);
    } else {
        ActorMessage(slot, flags);
        m = (unsigned short *)(iwram_3001ebc + (0xec << 1));
        *m = *m + 1;
    }
    return choice;
}
