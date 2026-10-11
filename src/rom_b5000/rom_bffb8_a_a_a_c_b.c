/* Func_80c0130 -- MATCHING, landed in batch 337 (installed after the batch, on
 * a measurement that dissolved the owner decision it was held for).
 *
 * 6 of 37 -> 0.  84 bytes, 37 encodings and 1 relocation identical; 31/31
 * 16-bit instructions, in phase, so the 6 was a true distance.
 *
 * HOW THE HOLD WAS LIFTED.  Brief B reached 0 with TWO bare
 * `__asm__ volatile ("")` barriers -- one inside a new include/dma.h helper and
 * one at the call site -- and correctly escalated the first as an owner
 * decision, because a scheduling DEVICE in a SHARED HEADER would be inherited
 * by every future caller of DMA0_SET_OFS.  That was a new class for this tree.
 *
 * Brief A, finishing later in the same batch, established that for a purely
 * SCHEDULING residue a `do { } while (0)` anchor has the same reach as a bare
 * barrier, is stronger in the compiler (haifa-sched.c:3744-3757 depends on every
 * register's last uses, sets and clobbers, then calls flush_pending_lists, so it
 * orders memory too), emits nothing, and is NOT counted by shimcount.  The
 * substitution was then measured in all four combinations:
 *
 *     barrier (header) / barrier (call site)    84 bytes, 37 enc, 1 reloc  OK
 *     anchor  (header) / barrier (call site)    84 bytes, 37 enc, 1 reloc  OK
 *     barrier (header) / anchor  (call site)    84 bytes, 37 enc, 1 reloc  OK
 *     anchor  (header) / anchor  (call site)    84 bytes, 37 enc, 1 reloc  OK
 *
 * So the anchor substitutes in BOTH positions, include/dma.h carries no device,
 * and this file carries none either.  shimcount on the inlined control printed
 * `empty asm : 2`; on the anchor version it prints no device rows at all.
 * THE DECISION DISSOLVED RATHER THAN BEING RULED ON -- which is the general
 * lesson: when a hold is about a DEVICE, check whether a device-free construct
 * reaches the same place before asking for a ruling.
 *
 * The remaining four register pins live in include/dma.h's helper, not here, and
 * are that file's house pattern: `stmia r3!, {r0, r1, r2}` requires those exact
 * registers, exactly as DMA0_SET does.
 *
 * WHY THE HELPER AT ALL, and why it is inert alone: the lever is WHERE the
 * `base + off` add happens.  With DMA0_SET the caller computes the address, so
 * the add is evaluated with the call's arguments and lands at a lower INSN_LUID
 * than the body's operand setups; taking the offset as an argument moves it
 * inside the body.  On its own that is INERT -- all 24 declaration permutations
 * read the same figure -- because sched2's class rung (haifa-sched.c:4069-4094)
 * sits ABOVE INSN_LUID.  The ladder: no anchor either side 6, call site only 5,
 * header only 3, both 0.
 *
 * A FOURTH CLASS-RUNG DECISION the park never reached, recorded because it
 * stands independently: even granting the park's stated escape at 3, the ROM's
 * next insn is ANTI-DEPENDENT on the `strh`, so insn_cost returns 0
 * (haifa-sched.c:3105-3123 with arm.c:2424-2427) and the candidate is class 2
 * against class 3 -- unreachable a second time, for a different reason than the
 * one the park had recorded.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char iwram_3001f00[];

void Func_80c0130(void)
{
    unsigned char *a;
    unsigned char *b;
    unsigned char *p;
    int n;
    int h;

    a = *(unsigned char **)iwram_3001f00;
    if (*(int *)(a + 8) != 2)
        return;
    b = *(unsigned char **)(iwram_3001f00 - 0x88);
    n = *(int *)b;
    p = b + ((n * 5) << 6);
    h = *(unsigned short *)(p + 0x20);
    REG_BG2CNT = h;
    do { } while (0);   /* anchor, not a barrier -- see the header */
    DMA0_SET_OFS(p, 0x22, (void *)&REG_BG2CNT, 0xa2600001);
    DMA3_SET(b + 0x10, (void *)&REG_BG2PA, 0x84000004);
}
