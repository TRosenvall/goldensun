/* Cluster Func_80d67dc extracted from goldensun/asm/rom_c9000/rom_d6504_a_c_c_c_c_a.s.
 *
 * Whole-file conversion: the piece holds one function and no data sections, so no split
 * is needed. No pins, no flags.
 *
 * Starts a screen transition: forces DISPCNT to mode 1, pokes an iwram halfword, calls
 * the transition entry with an id read from the actor table, DMA3-clears two regions
 * from one caller-owned zero word, waits a frame, then programs the blend registers and
 * the full DISPCNT value and posts a code into the physics vector.
 *
 * ================ BOTH BLOCKERS HAD LEGAL ANSWERS ALREADY IN THE TREE ================
 *
 * The park named two residues and rejected or mis-aimed each. Crossing them closes the
 * function with no new pin, no new header, and no fakematch row.
 *
 * ONE: THE SECOND DMA COUNT LOAD. The ROM loads the count from the pool once per
 * transfer; DMA3_SET binds the count as an INPUT, which promises gcc the register
 * survives the asm, so gcc commons the two loads and we came out one instruction short.
 * The park proposed widening DMA3_SET's clobber list to name the count register,
 * measured that tree-wide green, and then declined to ship it -- correctly, because
 * declaring an INPUT as clobbered is a fiction about what the asm does.
 *
 * It did not need a fiction. include/dma.h ALREADY CARRIES `DMA3_SET_RW`, promoted in
 * batch 323 for exactly this, which withdraws the same promise LEGALLY by making the
 * count an inline-asm output. Swapping the two calls over to it is worth the whole
 * length difference on its own and leaves only the store base. dma.h's own
 * DMA3_COPY16_RW note already records that the legal output form and an illegal clobber
 * score identically, so there was never a reason to prefer the clobber.
 *
 * TWO: THE STORE BASE -- `str r5, [r4]`, NOT `[sp]`. Both sides build `mov r4, sp`,
 * because the DMA source operand needs the address in a general register either way;
 * only the zero store's addressing differs. No pointer spelling can fix that, and the
 * reason is in cse: `insert` prefers a FIXED hard register as the head of an
 * equivalence class ahead of anything else (cse.c:1016-1036 -- a pseudo can only take
 * the head when the incumbent is NON-fixed), and `canon_reg` then rewrites the pseudo
 * inside the MEM address to that head (cse.c:2638-2651). The frame register is fixed,
 * so the store is canonicalised to the frame no matter how the pointer is named.
 *
 * WHAT WORKS IS TO KEEP CSE OUT OF THE MEM ALTOGETHER: declare the staging word
 * `volatile` and leave the POINTER PLAIN, casting the volatile away when taking its
 * address. The park tried `u32 *volatile q` -- the volatile on the other half -- found
 * it four instructions worse, and closed the dimension. A volatile POINTER forces the
 * pointer itself into a frame slot and reloads it; a volatile OBJECT with a plain
 * pointer leaves the pointer in a register and merely stops cse folding the store.
 *
 * THE SPELLING WAS ON DISK, in the header of the landed src/rom_9000/rom_c004_a.c,
 * whose generated output carries the identical window -- `mov r5,#0 / mov r4,sp /
 * str r5,[r4] / ldr r3,.L / mov r0,r4`. It was found by querying the generated half of
 * asm/ for a store through a register copy of the frame: 184 files tree-wide, which is
 * a reachability bound as well as a recipe. That is the labelled-corpus method doing
 * what it is for, after the spelling-level search behind the park had stalled.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern char *iwram_3001e74[];
extern u16 iwram_3001ad0[];
extern int gPhysVec[];
extern void _AnimTransitionIn(int a, int b, int c);

void Func_80d67dc(void)
{
    volatile u32 value;
    u32 *q;
    char *buf;
    char *base;

    buf = iwram_3001e74[0x7c / 4];
    base = iwram_3001e74[0];
    REG_DISPCNT = 1;
    iwram_3001ad0[3] = 0x20;
    _AnimTransitionIn(1, *(u16 *)(base + (0xc9 << 3)), 0x18);
    q = (u32 *)&value;
    *q = 0;
    DMA3_SET_RW(q, buf, 0x85001000);
    *q = 0;
    DMA3_SET_RW(q, (void *)0x6004000, 0x85001000);
    WaitFrames(1);
    REG_BLDALPHA = 0x100e;
    REG_BLDCNT = 0x3f46;
    REG_DISPCNT = 0x7741;
    gPhysVec[0x10 / 4] = 0x78;
}
