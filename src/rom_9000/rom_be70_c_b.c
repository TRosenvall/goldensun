/* Func_800bf34  --  0x0800bf34
 *
 * Cut from the head of goldensun/asm/rom_9000/rom_be70_c.s; the second function
 * and the trailing .rodata stay in _c_c.s. The target has no literal pool of its
 * own, so its piece is pure text. Split verified byte-neutral before this landed.
 *
 * Steps a palette or gradient across all 128 entries in groups of four, one
 * frame per group, for every element of the caller's array.
 *
 * A sched2 RESIDUE OF TWO ADJACENT, MUTUALLY INDEPENDENT INSTRUCTIONS IS AN
 * INSN_LUID TIE-BREAK, AND LUID IS SOURCE ORDER. Read out of haifa-sched.c:
 * after priority, dependence class and dependence count all tie,
 * rank_for_schedule's last line returns the difference of the two insns'
 * original numbers, explicitly so the sort is stable. -fsched-verbose=6 prints
 * the proof -- the two instructions here had equal priority and equal cost, and
 * the ready list resolved on the lower number. So when the residue is two
 * adjacent independent instructions in the wrong order, it is not a scheduling
 * wall; it is the order you wrote them in.
 *
 * LEVER A. An instruction AFTER a call that touches only call-saved registers
 * has no dependence on the call, so sched2 hoists it back above the argument
 * setup. Writing the bookkeeping update AFTER the call gives it the higher
 * source number, and the pair comes out in the ROM's order with the update still
 * ahead of the branch. WHEN THE ROM PUTS A BOOKKEEPING UPDATE BETWEEN THE
 * ARGUMENT SETUP AND THE `bl`, WRITE IT AFTER THE CALL IN C.
 *
 * LEVER B. Loop-invariant motion always emits its hoists at the END of the
 * preheader -- read from loop.c, where move_movables inserts immediately before
 * the loop-start note. So EVERY PREHEADER STATEMENT YOU WRITE HAS A LOWER SOURCE
 * NUMBER THAN EVERY HOISTED INVARIANT. If the ROM emits a plain copy AFTER a
 * hoisted computation, no statement ordering can reach it while that computation
 * is still implicit -- the hoisting has to be written out as explicit named
 * locals. That is the mirror of the recorded delete-the-address-local rule: here
 * the fix was to ADD three locals gcc would otherwise have invented itself, and
 * to interleave a fourth between them.
 *
 * Flags were worthless -- six were swept and none moved it, while -fno-gcse is
 * far worse because it kills the hoisting entirely. The loop bound's spelling is
 * measured inert; both forms emit the same compare and branch.
 *
 * The neighbour finder was the whole match: it ranked a solved single-element
 * twin of this function top, with the same four-call group, the same step and
 * the same bound, and handed over both callee declarations verbatim. The first
 * candidate written off it landed at 4 differing. The caller-grep pinned arity
 * and the void return, and its call site -- passing an array of pointers --
 * settled the parameter type and the dereference form.
 */
extern void Func_800be70(unsigned int arg0, unsigned int arg1);
extern void WaitFrames(unsigned int nframes);

void Func_800bf34(unsigned int *arr, int count) {
    unsigned int i;
    unsigned int i1;
    unsigned int i2;
    unsigned int i3;
    unsigned int *p;
    int n;

    i = 0;
    do {
        if (count > 0) {
            i1 = i + 1;
            i2 = i + 2;
            p = arr;
            i3 = i + 3;
            n = count;
            do {
                Func_800be70(*p, i);
                Func_800be70(*p, i1);
                Func_800be70(*p, i2);
                Func_800be70(*p++, i3);
                n--;
            } while (n != 0);
        }
        WaitFrames(1);
        i += 4;
    } while (i <= 0x7f);
}
