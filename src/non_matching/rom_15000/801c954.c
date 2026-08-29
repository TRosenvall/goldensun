/* Func_801c954 (CloseMenuScreen) -- NON-MATCHING.
 * Blocker class: CONSTANT CSE ACROSS A CALL -- the SECOND counterexample to
 * the documented remedy, which is why this park is worth reading.
 *
 * 41 lines against the ROM's 44, 40 differing. Three lines short, and the
 * three are real work the ROM does and we do not:
 *
 *     rom    ldr r2, =0xff4 / add r3, r5, r2 / ldr r0, [r3] / bl CloseUIBox
 *            / ldr r3, =0xff4 / add r6, r5, r3
 *     ours   ...one computation of s + 0xff4, reused after the call
 *
 * The ROM takes TWO pool entries for 0xff4 and computes the address twice,
 * once for the call argument and once for the loop pointer. gcc computes it
 * once and keeps it across the call.
 *
 * WHY THIS IS A COUNTEREXAMPLE. docs/elevation.md says recovering the ROM's
 * rebuild needs a control-flow boundary between the two uses AND
 * -fno-rerun-cse-after-loop. Here the boundary is a CALL, which is as strong
 * a boundary as exists, and the flag changes nothing: 40 differing with it,
 * 40 without, 40 at -O1.
 *
 * src/non_matching/ovl_7b2078/2008388.c is the first counterexample and its
 * boundary is a `beq`. Two independent cases now show the stated remedy is a
 * NECESSARY condition and not a sufficient one, so tools/blocked_cse.py's
 * count remains a population rather than a worklist.
 *
 * Tried: naming the offset in a local and re-assigning it before the second
 * use -- the "name the OFFSET, not the base" lever, and exactly the shape the
 * ROM has. gcc CSEs the two assignments into one and the result is WORSE, 40
 * lines against 41 and 42 differing. The lever moves which value is named; it
 * cannot ask for a value to be materialised twice.
 */
extern char *iwram_3001e9c;
extern void CloseUIBox(int a, int b);
extern void WaitFrames(int n);
extern int Func_8017394(int a);
extern void Func_8003f3c(int n);
extern void gfree(int n);

void Func_801c954(void)
{
    char *s;
    int *w;
    int off;

    s = iwram_3001e9c;
    CloseUIBox(*(int *)(s + 0xff4), 0);
    w = (int *)(s + 0xff4);
    while (Func_8017394(*w) == 0)
        WaitFrames(1);
    if (*(unsigned short *)(s + 0x46) != 0)
        Func_8003f3c(*(unsigned short *)(s + 0x48));
    off = 0x352;
    if (*(unsigned short *)(s + off) != 0) {
        off += 2;
        Func_8003f3c(*(unsigned short *)(s + off));
    }
    gfree(0x13);
}
