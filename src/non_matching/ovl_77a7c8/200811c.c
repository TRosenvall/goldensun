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
  *
 * ============== BATCH 205: 2 DOWN TO 1, AND A MATCH REJECTED ==============
 *
 * THE PIN REACHES THIS, BUT ONLY WITH A SECOND USE. Pinning the offset local to
 * r4 changes nothing on its own -- gcc constant-propagates `off = 0` into the
 * address, the local is dead before allocation, and the pin has nothing to bind:
 *
 *     register unsigned int off __asm__("r4");   initialised or assigned later
 *                                                  2 differing, byte-identical
 *                                                  to the unpinned form
 *     the same with `volatile` added              18 lines against 16, 15 differing
 *
 * Give `off` a SECOND use and the pin takes: `mov r4, #0 / ldrsh r3, [r2, r4]`
 * matches the ROM exactly, and the count falls to 1. The remaining line is the
 * second access, which then also comes out register-offset:
 *
 *     rom    ldrh r1, [r2, #0x0]
 *     ours   ldrh r1, [r2, r4]
 *
 * That is not a defect in the lever, it is the encodings talking. Thumb `ldrsh`
 * has NO immediate-offset form, so its zero must live in a register; `ldrh`
 * does have one, so the ROM uses it. The ROM therefore wants the offset live
 * for the FIRST access and not the second, and every spelling that keeps it
 * live for one keeps it live for both.
 *
 * A MATCHING FORM EXISTS AND IS DELIBERATELY NOT USED. Writing the second
 * access as `*(unsigned short *)(p + off - off)` matches at 16 of 16. It is
 * rejected: `off - off` is not a plausible spelling of anything, it exists only
 * to hold a register live while yielding the immediate form, and it is exactly
 * what src/non_matching/ovl_780898/2008fec.c called "inventing code to fit
 * output" when it rejected the same kind of move.
 *
 * The distinction worth keeping: a register PIN is a declared fakematch, listed
 * in fakematch.txt and readable as scaffolding. A fabricated expression is a
 * fakematch disguised as ordinary C, and the next reader has no way to tell.
 * One is honest about what it is; the other is not.
 *
 * NEXT: what keeps a register live for one access and not the next, without a
 * fabricated use. Nothing in docs/elevation.md addresses that, and this park
 * is now a clean statement of the question.
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
