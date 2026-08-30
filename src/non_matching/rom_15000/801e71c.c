/*
 * SetTextColor (SetTextInk) -- asm/rom_15000/rom_1de5c_c_a_b.s
 * SPLIT OUT this round; byte-neutral, verified.
 *
 * BLOCKER: pool-load order. 8 lines against 8, 5 differing. The ROM loads the
 * mask before dereferencing the iwram pointer; we load the offset first.
 *
 * TWO THINGS ESTABLISHED, and the first is a mechanism worth reusing:
 *
 *   THE MASK 0xf IS A SYMBOL, and the tell is the LOAD WIDTH. The ROM has
 *   `ldr r2, .L1e72c` -- a WORD pool load of 0xf. A plain literal gives
 *   `ldrh r3, .L3`, a HALFWORD load, because the AND feeds a halfword store and
 *   gcc narrows it. That is exactly the near-miss const.sym's _CONST_1f entry
 *   describes. Referencing a symbol inline -- `c & (int)&_CONST_f` -- restores
 *   the word load. NO const.sym ENTRY WAS ADDED: that file's bar wants a symbol
 *   that completes a function, and this one still differs by five.
 *
 *   The store needs an EXPLICIT ADDRESS. `*(short *)(p + off) = c` gives
 *   `strh r3, [r1, r2]`, a register-offset store; the ROM computes `add r3, r2`
 *   and stores with no offset. Assigning `p = p + off;` first reproduces it and
 *   took the function from 7 lines to the correct 8.
 *
 * TRIED for the remaining order: the AND moved above the pointer load (5
 * differing, the version below); the offset assigned first (6). Neither reaches
 * the ROM's sequence of iwram address, mask, dereference, and.
  *
 * ROUND 2: down to FIVE differing, and the residue is now two facts.
 *
 *      rom   ldr r3,=iwram / ldr r2,=0xf / ldr r3,[r3] / and r0, r2
 *              / ldr r2,=0xeae / add r3, r2 / strh r0, [r3]
 *      ours  ldr r3,.L3 / ldr r2,.L3+4 / ldr r3,[r3] / ldr r1,.L3+8
 *              / and r2, r2, r0 / add r3, r3, r1 / strh r2, [r3]
 *
 *   1. The 0xeae pool load is emitted before the AND; the ROM emits it after.
 *   2. The AND writes to r2 (the mask's register); the ROM writes to r0 (the
 *      parameter's). Both operands are dead afterwards, so the allocator is
 *      free either way and picks the other one.
 *
 * whodoesthis.py finds 14 matching functions with `and r0, r0, rN`, so the
 * ROM's destination is reachable -- but in the one read (Func_801c46c) the mask
 * is a `mov`-built literal in a fresh register, not a pool load. With a pool
 * load gcc appears to prefer the loaded register as destination.
 *
 * TRIED AND REJECTED, all 5 or 6 differing: `c &= mask` compound assignment;
 * the AND before and after the pointer load; the offset folded into the pointer
 * arithmetic; `p += 0xeae`; the mask named in a local and assigned in three
 * different positions.
 *
 * A way to make the mask's register NOT dead after the AND would force r0 as
 * the destination, but every such spelling adds an instruction.
*/
extern unsigned char *iwram_3001e8c;
extern int _CONST_f;

void SetTextColor(int c)
{
    unsigned char *p;
    int off;

    c = c & (int)&_CONST_f;
    p = iwram_3001e8c;
    off = 0xeae;
    p = p + off;
    *(short *)p = c;
}
