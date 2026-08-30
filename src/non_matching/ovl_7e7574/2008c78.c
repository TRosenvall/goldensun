/*
 * OvlFunc_959_2008c78 -- asm/overlays/rom_7e7574/ovl_9dc_a_c_c_a_a_a_a_b.s
 * SPLIT OUT this round; byte-neutral, verified.
 *
 * BLOCKER: argument emission interleave. 10 lines against 10, THREE differing.
 *
 *      rom   mov r1,#0xf8 / mov r2,#0xbc / mov r0,#0xc / lsl r1,#16 / lsl r2,#17
 *      ours  mov r1,#0xf8 / mov r2,#0xbc / lsl r1,#16 / lsl r2,#17 / mov r0,#0xc
 *
 * The ROM emits the slot argument between the two constant builds and their
 * shifts; we finish the shifts first.
 *
 * TRIED AND REJECTED, all three byte-identical to the literal form: naming both
 * shifted arguments; naming the slot; naming all three.
 *
 * This is the clearest possible instance of the documented condition. The
 * interleave IS reachable -- 27 matching functions emit it, 16 of them pushing
 * only lr -- but in every one the named locals compete with six or more other
 * live values, and gcc rematerialises rather than allocating. This function has
 * a two-instruction body and nothing else live at all, so a named local always
 * wins a register and the rematerialisation never happens.
 *
 * Chosen deliberately as a function with ZERO pool loads, to isolate the
 * interleave from the pool-ordering problems that dominate the other small
 * parks. It isolates cleanly, and confirms the condition is register pressure.
 */
extern void __MapActor_SetPos(int slot, int x, int y);
extern void OvlFunc_959_2008b4c(void);

void OvlFunc_959_2008c78(void)
{
    __MapActor_SetPos(0xc, 0xf8 << 16, 0xbc << 17);
    OvlFunc_959_2008b4c();
}
