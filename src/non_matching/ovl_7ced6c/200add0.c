/* OvlFunc_946_200add0  [overlays/rom_7ced6c]
 *
 * Source asm: goldensun/asm/overlays/rom_7ced6c/
 *             ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_c_c_c_c_c_c_c.s
 *
 * BLOCKER CLASS: callee-saved register coin flip. 80 lines against 80, and
 * ALL THIRTEEN differing lines are the same two-register rename:
 *
 *     rom    a in r6, w in r5
 *     ours   a in r5, w in r6
 *
 * Nothing else differs -- same block order, same branch senses, same
 * instruction shapes, same stack layout, same prologue and epilogue.
 *
 * ITS THREE SIBLINGS IN THE SAME CHUNK ARE ELEVATED, from this same source
 * shape: OvlFunc_946_200ab80, _200ac4c and _200ad0c all match byte for byte,
 * two of them on the first screen. So the shape is not in question here; this
 * is purely which of two registers the allocator picks.
 *
 * WHY THIS ONE AND NOT THE OTHERS. The siblings each read THREE actor values
 * and spend three callee-saved registers (r5, r6, r7); this one reads TWO and
 * spends two. With three live values gcc and the original compiler agree. With
 * two, gcc gives the higher-priority pseudo the FIRST available callee-saved
 * register and the ROM gives it the second. That is a much sharper statement
 * of the coin flip than the corpus had: the disagreement appears only when
 * there is slack in the allocation, and disappears under pressure.
 *
 * THE ALLOCATION IS NOT DRIVEN BY STATEMENT ORDER, which was tested directly.
 * Swapping the two computations moves which VALUE is computed first but leaves
 * `a` in r5 either way -- the registers stay put and the two `ldr` offsets
 * swap instead, for the same 13 differing. So the batch-152 birth-order lever
 * does not apply: it moves a pointer's materialisation, not a value's
 * allocation priority.
 *
 * MEASURED:
 *   call in every arm (the sibling shape)                    13   (best)
 *   call hoisted after the join, argument assigned per arm   66   (9 short)
 *   both call arguments assigned per arm, call after join    66
 *   declaring the two values in the opposite order           13   (no effect)
 *   swapping the two computations                            13   (redistributed)
 *   naming the actor pointer in a local                      70   (2 long)
 *   separate local for the tail's recomputed field           13   (no effect)
 */
extern unsigned char *__MapActor_GetActor(unsigned int slot);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_946_2009774(int a, int b, int c);

void OvlFunc_946_200add0(void)
{
    int a;
    int b;
    int c;

    a = *(int *)(__MapActor_GetActor(0x10) + 8) >> 20;
    b = *(int *)(__MapActor_GetActor(0x10) + 0x10) >> 20;
    c = *(int *)(__MapActor_GetActor(9) + 0x10) >> 20;
    if (a == 6) {
        if ((unsigned int)(c - 9) <= 2)
            OvlFunc_946_2009774(0x10, 0x20, 0);
        else
            OvlFunc_946_2009774(0x10, 0x70, 0);
    } else if (a == 8) {
        if ((unsigned int)(c - 9) <= 2)
            return;
        OvlFunc_946_2009774(0x10, 0x50, 0);
    } else if (a == 9) {
        OvlFunc_946_2009774(0x10, 0x40, 0);
    } else if (a == 0xc) {
        OvlFunc_946_2009774(0x10, 0x10, 0);
    } else if (a == 0xd) {
        return;
    }
    __WaitFrames(2);
    c = *(int *)(__MapActor_GetActor(0x10) + 8) >> 20;
    b = b - 1;
    __Func_8010704(a, b, 1, 3, c, b);
    __Func_8010704(0, 0, 1, 3, a, b);
}
