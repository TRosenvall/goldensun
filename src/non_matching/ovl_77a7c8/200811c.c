/* OvlFunc_881_200811c -- NOT MATCHING. 2 of 16 lines, same length.
 *
 * Source asm: goldensun/asm/overlays/rom_77a7c8/ovl_30_a_a_a.s
 *
 * Blocker class: register allocation, and nothing else.
 *
 *     rom    mov r4, #0x0 / ldrsh r3, [r2, r4]
 *     ours   mov r1, #0x0 / ldrsh r3, [r2, r1]
 *
 * Every other instruction matches. Thumb `ldrsh` has no immediate-offset form,
 * so the zero has to live in a register; the ROM puts it in r4 and gcc puts it
 * in r1.
 *
 * WHY gcc PICKS r1: the halfword is loaded into r1 on the very next
 * instruction, and the zero is dead by then, so reusing r1 is free. The ROM
 * kept the two apart. There is no source-level handle on that -- the zero has
 * exactly one use and cannot be made to live longer without adding an
 * instruction.
 *
 * A FALSE LEAD, CHECKED AND DROPPED: r4 is callee-saved and the ROM's prologue
 * is `push {lr}` alone, so this looked like a function clobbering a saved
 * register without saving it -- which would have said something about the
 * original build. It says nothing: 826 of 2779 unelevated functions do the
 * same. It is ordinary codegen here, not a signal.
 *
 * TWO THINGS THAT WERE WRONG AND ARE NOW RIGHT, so they are not re-tried:
 *
 *   The fall-through arm is the INCREMENT, not the delete. `if (t > 0) delete;
 *   else increment;` compiles to `ble` and swaps the arms.
 *
 *   The increment is on an int. Reading the halfword into an `unsigned short`
 *   makes gcc re-truncate with `lsl #16 / lsr #16` before the add -- two extra
 *   instructions the ROM does not have.
 *
 * Flags do not help: -O1, -fno-schedule-insns2 and -fno-rerun-cse-after-loop
 * all give the identical 2-line diff.
 */
extern void __DeleteActor(void *a);

void OvlFunc_881_200811c(void *actor)
{
    unsigned char *p;
    unsigned int off;
    short t;
    int u;

    p = (unsigned char *)actor + 0x64;
    off = 0;
    t = *(short *)(p + off);
    u = *(unsigned short *)p;
    if (t <= 0)
        *(unsigned short *)p = u + 1;
    else
        __DeleteActor(actor);
}
