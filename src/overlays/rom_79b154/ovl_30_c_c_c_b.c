/* Cluster OvlFunc_907_2008fa0..OvlFunc_907_2008fa0, split out of
 * goldensun/asm/overlays/rom_79b154/ovl_30_c_c_c.s BY HAND -- code into this
 * file, data into ovl_30_c_c_c_c.s.
 *
 * Total .text for this TU = 120 bytes.
 *
 * WHY THE HAND SPLIT, AND WHY IT IS SAFE. The original .s held this function
 * followed by a `.section .data` carrying 21 `.incbin` blobs -- an actor
 * command array, scripts, and overlay data -- under 19 global labels that the
 * rest of the overlay references. tools/split_s.py refuses such a file, and
 * correctly: converting it wholesale deletes the data and the link fails.
 *
 * But the cut is CLEAN, and the linker script was already doing the work. The
 * function ends at `.func_end` and `.section .data` begins two lines later,
 * so there is exactly one boundary; and overlay.ld already placed this
 * object's (.text) and (.data) in two different regions, on separate lines.
 * The split is therefore: code to _b, data to _c, and point the two existing
 * linker lines at the two new objects. Verified byte-neutral BEFORE this C was
 * written, which is the only thing that makes it trustworthy.
 *
 * THIRTY-TWO single-function .s files in the tree carry a .data section like
 * this. Each is a function that is unreachable only because of file structure.
 *
 * The decompilation itself: each `else` arm needs its OWN block-scoped pair of
 * stack-argument locals. Sharing one pair across both else arms is 8
 * differing, bare literals everywhere is 6 -- both emit `mov r3 / str / mov r3
 * / str` where the ROM has `mov r3 / mov r2 / str / str`. The `==` arms keep
 * literals, because there the ROM reuses the already-computed shifted value as
 * the sixth argument rather than rebuilding it.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_907_2008fa0(void)
{
    unsigned char *a;

    a = __MapActor_GetActor(8);
    if (a == 0)
        return;
    if (*(int *)(a + 0x10) >> 20 == 6) {
        __Func_8010704(2, 0, 1, 1, 0xe, 6);
    } else {
        int e0 = 0xe;
        int f0 = 6;
        __Func_8010704(0, 0, 1, 1, e0, f0);
    }
    if (*(int *)(a + 0x10) >> 20 == 9) {
        __Func_8010704(2, 0, 1, 1, 0xe, 9);
    } else {
        int e1 = 0xe;
        int f1 = 9;
        __Func_8010704(1, 0, 1, 1, e1, f1);
    }
}
