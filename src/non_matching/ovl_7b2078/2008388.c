/* OvlFunc_926_2008388 -- NON-MATCHING.  Blocker class: CONSTANT CSE ACROSS A
 * CALL -- and it is a COUNTEREXAMPLE to the documented remedy, which is the
 * reason this park is worth reading.
 *
 * 60 lines against the ROM's 59, 25 differing.  The first 23 lines are exact,
 * including the area comparison, so the `_AREA_3c` half of this function is
 * finished -- this came out of the "209 functions are elevatable NOW" pool in
 * HANDOFF.md and that part of it worked.
 *
 * THE ONE CAUSE
 *
 *     rom   ldr r0, =0x895 / bl __GetFlag   ...   ldr r0, =L4998 / ldr r1, =0x895
 *     ours  ldr r5, =0x895 / mov r0, r5 / bl __GetFlag  ...  (r5 reused)
 *
 * 0x895 is a save-bit id: tested with __GetFlag, and then -- only if the test
 * passes -- stored into four halfword fields of the table at .L4998.  The ROM
 * takes TWO pool entries for it.  gcc-2.96 hoists it into r5, keeps it live
 * across the call, and every later register assignment shifts by one.
 *
 * WHY THIS IS A COUNTEREXAMPLE
 *
 * docs/elevation.md, "Pool-constant CSE: the complete rule", says recovering
 * the ROM's reload needs BOTH a control-flow boundary between the two uses AND
 * -fno-rerun-cse-after-loop, and that with no boundary nothing reaches it.
 *
 * Here BOTH preconditions are met.  The first use dominates the second, and
 * there is a real boundary between them -- the `beq` around the whole store
 * block.  The flag still does not move it.  So the rule states a necessary
 * condition, not a sufficient one, and a boundary plus the flag is not a
 * recipe.  tools/blocked_cse.py counts 585 functions in this class on the
 * strength of the boundary test; that count is a population, not a worklist.
 *
 * Tried, all 25 differing and most byte-identical to each other:
 *   - the literal 0x895 in all five places
 *   - the stored value in a named local inside the guarded block, which is the
 *     lever that took ovl_7b8cb0/200807c from 15 differing to 6
 *   - --no-rerun-cse, and --cflags with each of -fno-gcse,
 *     -fno-cse-follow-jumps, -fno-expensive-optimizations, -fno-force-mem
 *
 * CHECKED AND RULED OUT: that the stored 0x895 is a symbol rather than a
 * literal, which would explain two pool entries naturally.  It is in none of
 * area.sym, message.sym, const.sym or file_table.sym.  If a save-bit namespace
 * is ever given a .sym file, re-try this first -- a `_FLAG_895` address would
 * pool separately from the integer and the CSE could not fire.
 */
extern unsigned char gState[];
extern int _AREA_3c;
extern unsigned char L48f0[] __asm__(".L48f0");
extern unsigned char L4ae8[] __asm__(".L4ae8");
extern unsigned char L4998[] __asm__(".L4998");
extern int __GetFlag(int id);
extern void __Func_808b868(unsigned char *p);

unsigned char *OvlFunc_926_2008388(void)
{
    unsigned char *g;
    unsigned char *p;

    g = gState;
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_3c))
        return L48f0;
    if (*(short *)(g + (0xe1 << 1)) == 3)
        return L4ae8;
    if (__GetFlag(0x895)) {
        p = L4998;
        *(short *)(p + 0x7a) = 0x895;
        *(short *)(p + 0xaa) = 0x895;
        *(int *)(p + 0xc8) = 0x90 << 17;
        *(int *)(p + 0xd0) = 0xf8 << 16;
        *(short *)(p + (0x85 << 1)) = 0x895;
        *(short *)(p + 0x122) = 0x895;
    }
    __Func_808b868(L4998);
    return L4998;
}
