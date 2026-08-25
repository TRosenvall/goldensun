/* Cluster Func_8021c34..Func_8021c34 extracted from goldensun/asm/rom_15000/rom_20198_c_c_c_c_a.s.
 *
 * Slotted between rom_20198_c_c_c_c_a_a.o and the rest of stage1.ld.
 *
 * Creates a UI box, draws one string into it, and RETURNS THE BOX. The return
 * is read off the epilogue: `mov r0, r5 / pop {r1} / bx r1` -- r1 rather than
 * r0 as the epilogue scratch means r0 is live across it. Written void, the
 * box would be created and dropped.
 *
 * The string is a `.L` label in a SIBLING .s, which already exports it with
 * `.global .L37300`. No new export was needed here -- worth stating because
 * every other `.L` reference in this tree did need one.
 */
extern void *CreateUIBox(int a, int b, int c, int d, int e);
extern void UIDrawText(unsigned char *s, void *box, int x, int y);
extern unsigned char L37300[] __asm__(".L37300");

void *Func_8021c34(void)
{
    void *box;

    box = CreateUIBox(0, 0, 6, 4, 6);
    UIDrawText(L37300, box, 0, 0);
    return box;
}
