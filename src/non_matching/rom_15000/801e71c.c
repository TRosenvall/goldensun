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
