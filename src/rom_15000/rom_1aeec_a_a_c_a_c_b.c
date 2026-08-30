/* Cluster Func_801b398..Func_801b398 extracted from
 * goldensun/asm/rom_15000/rom_1aeec_a_a_c_a_c.s.
 *
 * A modal input loop, and it turns entirely on WHICH loop-invariant address gcc
 * is allowed to hoist.  The ROM hoists exactly one of the two and recomputes the
 * other every iteration, and reproducing that took a different lever for each.
 *
 *   THE POLLED HALFWORD ADDRESS MUST NOT BE HOISTED.  The ROM rebuilds
 *   `mov r2, #0xe8 / lsl r2, #2 / add r3, r5, r2` inside the loop body every
 *   pass.  Written as `do { ... } while (cond)` gcc recognises a loop, lifts the
 *   address into a register of its own and needs a FIFTH callee-saved register
 *   for it -- 67 lines against 61, with `mov r7, r8 / push {r7}` in the
 *   prologue.  Writing the loop with an explicit `goto top;` defeats the loop
 *   recognition and the address goes back inside: 67/66 -> 62/46.  This is the
 *   goto lever recorded in batch 145 as defeating check_dbra_loop, working here
 *   against loop-invariant motion instead.
 *
 *   THE gKeyPress ADDRESS MUST BE.  The ROM loads `ldr r7, =gKeyPress` ONCE
 *   before the loop and pushes r7 to keep it.  With the global named directly
 *   that load sits inside the loop; a `volatile unsigned int *k = &gKeyPress;`
 *   assigned before the loop puts it where the ROM has it.  62/46 -> exact.
 *
 * So one address wants a goto to keep it in, and the other wants a pointer local
 * to lift it out, in the same loop.  Read which the ROM does for each before
 * reaching for either.
 *
 * Both globals are `volatile`: gKeyRepeat is masked twice with no call between
 * the reads, and gKeyPress likewise on the fall-through path, so without it gcc
 * common-subexpressions each pair into one load.  Screened: neither volatile,
 * and each alone -- all three are 66-68 lines, so the volatile is necessary but
 * nowhere near sufficient; the hoisting is what dominates.
 */
extern unsigned char *iwram_3001e98;
extern volatile unsigned int gKeyPress;
extern volatile unsigned int gKeyRepeat;

extern void Func_801b9ec(unsigned char *p, int n);
extern void WaitFrames(int n);
extern void Func_801b664(unsigned char *p);
extern void Func_801b810(unsigned char *p);
extern int Func_801be80(unsigned char *p);

int Func_801b398(int a)
{
    unsigned char *p;
    volatile unsigned int *k;

    p = iwram_3001e98;
    Func_801b9ec(p, 0);
    k = &gKeyPress;
top:
    WaitFrames(1);
    if (*(unsigned short *)(p + (0xe8 << 2)) != 0)
        goto top;
    if (a != 0x3e7) {
        if (gKeyRepeat & 0x10) {
            Func_801b664(p);
        } else if (gKeyRepeat & 0x20) {
            Func_801b810(p);
        } else if (*k & 1) {
            return Func_801be80(p);
        }
    }
    if (a == 0)
        goto top;
    if ((*k & 2) == 0)
        goto top;
    return -1;
}
