/* CheckSpecialExits (0x0808ba38 area) -- NON-MATCHING, 10 of 106.
 * Blocker class: RELOAD's spill-register ordering -- not reachable from source.
 *
 * asm/rom_8a000/rom_8ba38_a_a_a_c_a_c_a_a.s (2 functions, so landing needs a split).
 *
 * EVERYTHING STRUCTURAL IS CORRECT: the `__start_overlay[11]` indirect call via _call_via_r0,
 * the 8-short box record, the `for(;;) { g = ...; if (x0 == -1) break; ... }` shape that makes
 * gcc duplicate the header at the loop bottom, the seven-term `&&` chain, both spill slots at
 * the ROM's offsets, and EVERY hard-register assignment -- e->r5, x0->r6, y1->r7, y0->r8,
 * z1->r9, x1->r10, z0->r11, `val` spilled to sp+4, `g` to sp+0.
 *
 * PATH: 27 -> 13 by `break` rather than `return` (which is what duplicates the header),
 * declaring `g` after `val` (spill slots follow declaration order -> pseudo number -> descending
 * sp offset), and reading `y1` last. 13 -> 10 by SWAPPING THE z0/z1 DECLARATIONS: `.17.lreg`
 * shows both as "used 4 times across 19 insns" -- an EXACT priority tie -- and `allocno_compare`
 * breaks ties by pseudo number, which is declaration order. That is the recorded
 * declaration-order tie-break, used deliberately after confirming from the dump that it IS a tie
 * rather than a strict win.
 *
 * THE REMAINING 10 ARE reload's CHOICE, AND NO SOURCE SPELLING REACHES THEM. The four remaining
 * loads are emitted in the ROM's order with the ROM's structure; only the scratch registers
 * rotate:
 *
 *                       ROM                 ours
 *     offset regs       r2, r2, r3, r3      r1, r3, r2, r1
 *     value regs        r1, r1, r0, r7      r3, r2, r0, r7
 *     str ..,[sp,#4]    last in block       immediately after its load
 *
 * The first three field loads match register for register; divergence starts at the fourth.
 * BOTH the offset and the destination of every `ldrsh` are RELOAD registers -- Thumb has no
 * immediate-offset `ldrsh`, and a high-register destination needs an output reload -- so the
 * choice is made by `order_regs_for_reload`, not by anything a source expression names.
 *
 * SIXTEEN SPELLINGS MEASURED, ALL 10 OR WORSE: all six permutations of the last three field
 * reads (10,10,10,10,11,10); `cond` named before the call (10), after `val` (10), first of all
 * (19); the `&&` chain parenthesised into pairs (10); three declaration positions for `val`
 * (10,10,10); the record read through a `short *` with indices (10); `y1` before `val` (10).
 *
 * NEXT: nothing source-level. This is a FOURTH allocation entry point beyond the three recorded
 * in docs/elevation.md -- reload's own register ordering, below local_alloc.
 *
 * NOTE ON SELECTION: this was the smallest of its round's four targets at 103 instructions and
 * the only one under the old 120 cut-off, and it is the one that stalled -- because its
 * stem-mates in rom_8ba38_* are 20-byte stubs with nothing transferable. The other three all had
 * a real neighbour and all landed. Size predicted nothing; the neighbour predicted everything.
 */
struct Box {
    short x0;
    short y0;
    short z0;
    short x1;
    short y1;
    short z1;
    short cond;
    short val;
};

extern unsigned int __start_overlay[];
extern unsigned char iwram_3001ebc[];
extern int Func_808d428(int cond);
extern void _PlaySound(int id);
extern void Func_8091660(void);

void CheckSpecialExits(int x, int y, int z);

void CheckSpecialExits(int x, int y, int z)
{
    struct Box *(*fp)(void);
    struct Box *e;
    int x0;
    int y0;
    int z1;
    int x1;
    int y1;
    int z0;
    int val;
    unsigned char *g;

    fp = (struct Box *(*)(void))__start_overlay[11];
    e = fp();
    if (e == 0)
        return;
    for (;;) {
        g = *(unsigned char **)iwram_3001ebc;
        x0 = e->x0;
        if (x0 == -1)
            break;
        y0 = e->y0;
        z0 = e->z0;
        x1 = e->x1;
        z1 = e->z1;
        val = e->val;
        y1 = e->y1;
        if (Func_808d428(e->cond) != 0
            && y >= (y0 << 16) && y < (y1 << 16)
            && x >= (x0 << 16) && x < (x1 << 16)
            && z >= (z0 << 16) && z < (z1 << 16)) {
            *(unsigned short *)(g + 0xb8 * 2) = val;
            _PlaySound(0x7b);
            Func_8091660();
            break;
        }
        e++;
    }
}
