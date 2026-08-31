/* Func_80119cc -- asm/rom_9000/rom_11568_c_c_a_c_c.s
 *
 * BLOCKER: LICM out of the interpreter loop. 86 of 87, two short.
 *
 * A blend-effect script interpreter: a wait counter, a 0xffff restart opcode,
 * an 0xfe00 jump opcode, a 0x3000 opcode that writes REG_BLDCNT, and a default
 * that writes REG_BLDALPHA or REG_BLDY and then takes a wait value.
 *
 * The high differing count is misleading -- it is ONE decision at the top,
 * taken before the loop, that shifts every instruction after it:
 *
 *     rom   push {r14}          computes base+0x103 and 0xffff INSIDE the loop
 *     ours  push {r5, r14}      hoists both into r12 and r5 ahead of the loop
 *
 * Both are loop-invariant, gcc lifts them, and lifting costs a callee-saved
 * register the ROM does not spend. This is the same class as
 * src/non_matching/rom_9000/80110e0.c, where gcc hoists a loop-invariant base
 * that the ROM rematerialises every iteration -- and there three separate
 * formulations all folded to the same hoisted output.
 *
 * TWO THINGS WERE READ CORRECTLY off the disassembly and should be kept if
 * this is retried:
 *
 *   - the ROM copies the wait counter before testing it (`ldrh r2,[r4,#8] /
 *     mov r3, r2 / cmp r3, #0`), which by the copy-then-modify tell means two
 *     live names, not one expression;
 *   - REG_BLDALPHA and REG_BLDY are selected into ONE register and stored
 *     through once (`ldr r3,=A / b / ldr r3,=B / strh r1,[r3]`), so the source
 *     picks a `vu16 *` and stores once. Writing two separate stores would give
 *     two `strh` and is wrong.
 *
 * Neither was the blocker. Not pursued further because the hoist is the whole
 * residue and the sibling park already records three failed formulations
 * against it.
 */
#include "gba/io.h"

extern int iwram_3001e70;

void Func_80119cc(void)
{
    char *base;
    char *s;
    unsigned short *pc;
    unsigned int op;
    vu16 *reg;
    unsigned short wait;

    base = (char *)iwram_3001e70;
    s = base + 0xd8;
    if (*(int *)s == 0)
        return;
    if (*(unsigned short *)(s + 0xa) != 0)
        return;
    for (;;) {
        wait = *(unsigned short *)(s + 8);
        if (wait != 0) {
            *(unsigned short *)(s + 8) = wait - 1;
            return;
        }
        pc = *(unsigned short **)(s + 4);
        op = *pc;
        pc++;
        if (op == 0xffff) {
            *(int *)(s + 4) = *(int *)s;
            continue;
        }
        if ((op & (0xff << 8)) == (0xfe << 8)) {
            if ((op & 0xff) == 0xff)
                return;
            *(int *)(s + 4) = *(int *)s + ((op & 0xff) << 2);
            continue;
        }
        if ((op & (0xf0 << 8)) == (0xc0 << 6)) {
            REG_BLDCNT = op;
            *(unsigned char *)(base + 0x103) = op;
            *(int *)(s + 4) = *(int *)(s + 4) + 2;
            continue;
        }
        if ((*(unsigned char *)(base + 0x103) & 0xc0) == 0x40)
            reg = &REG_BLDALPHA;
        else
            reg = &REG_BLDY;
        *reg = op;
        *(unsigned short *)(s + 8) = *pc;
        *(int *)(s + 4) = *(int *)(s + 4) + 4;
    }
}
