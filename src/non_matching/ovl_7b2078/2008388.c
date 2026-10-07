/* OvlFunc_926_2008388 -- NON-MATCHING.  Blocker class: CONSTANT CSE ACROSS A
 *
 * NON-MATCHING, 19 of 62 encodings  (RE-MEASURED, batch 333 brief B).
 *   COUNTS AGREE: ref 62, ours 62, instructions fifty-three against
 *   fifty-three, so this figure IS a distance.  The earlier twenty-six was
 *   measured on a stream one instruction long.
 *   THE RELOCATION WARNING THAT USED TO STAND HERE IS WITHDRAWN: the only
 *   symbol difference is `_AREA_3c`, and area.sym line seventy-five already
 *   carries `_AREA_3c = 0x3c`.  An absolute assignment resolves at link time
 *   and emits no bytes, so objcmp on a single function always shows it as a
 *   relocation against a zero word.  That is the same artefact Func_8091eb0's
 *   park documents for its own device symbol, not a blocker.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7b2078/2008388.c \
 *     asm/overlays/rom_7b2078/ovl_314_c_c_a_a_a.s --func OvlFunc_926_2008388
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
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

 * ========================================================================
 * BATCH 333 BRIEF B -- TWENTY-SIX DOWN TO NINETEEN, LENGTHS NOW EQUAL, AND
 * THE PARK'S "TWO POOL ENTRIES" OBSERVATION IS A MISREADING OF THE .s
 * ========================================================================
 *
 * DIRECTION, STATED BACK: this park was OURS LONGER, by one instruction.  So
 * the search was for a value we keep that the original build did not, never
 * for a statement to add.
 *
 * THE LEVER IS ONE MOVED STATEMENT.  `p = L4998;` hoisted ABOVE the
 * `if (__GetFlag(...))` aligns the lengths and takes the figure to nineteen.
 * Mechanism, and it is already written down in this tree:
 * src/rom_c9000/rom_d82b0_b.c's header records "A POOL LOAD HOISTED ABOVE A
 * CALL IS A REGISTER-ROTATION LEVER", because lengthening the pool pseudo's
 * live range changes which pseudo wins the local-alloc race.  Same effect
 * here, in a different overlay, on a different constant.
 *
 * REFUTED, AND IT IS THE REASON THIS PARK READ AS A COUNTEREXAMPLE.  The park
 * above says "The ROM takes TWO pool entries for it".  It does not.
 * `ldr r0, =0x895` and `ldr r1, =0x895` in a HAND-WRITTEN .s are two loads of
 * ONE pool word -- the assembler's `=` syntax deduplicates identical literals.
 * Counted both ways: the reference and our candidate each have exactly SIX
 * pool words (gState, the area value, the two returned tables, the save-bit id
 * and the stored-to table) and each shows eight thirty-two-bit stream entries,
 * of which two are the calls.  Corpus check, over all four and a half thousand
 * generated .s files in asm/: NOT ONE has a duplicated numeric literal-pool
 * word.  The single apparent hit is a `.rodata` array, not a pool.
 *   >> So there is no "two pool entries" phenomenon to reproduce.  The ROM
 *   >> simply RELOADS one pool word twice, which is ordinary pool-constant
 *   >> reload, and the park's claim that this function refutes the documented
 *   >> remedy rests on the misreading.  The boundary-plus-flag observation
 *   >> still stands on its own evidence; the counterexample framing does not.
 *
 * WHY THE EXTRA INSTRUCTION WAS THREE STATEMENTS AWAY FROM ITS CAUSE.  The
 * save-bit id hoisted into a callee-saved register costs a copy into the
 * argument register, and saves the reference's reload inside the guarded block
 * -- those cancel.  The surplus showed up at the LAST TWO STORES instead: the
 * reference holds the table base in the FIRST argument register, so the only
 * free low registers at that point are the two the offsets need, and
 * reload_cse_move2add is forced to increment the offset register in place
 * (`add <off>, #0x18`).  In ours the first argument register is free, the
 * second offset is born in it, and a different register cannot be incremented
 * from the first -- so the offset is rebuilt with a shift pair.  One cause,
 * surfacing as a lost move2add well downstream of the register it came from.
 *
 * MEASURED EXACTLY INERT -- all nineteen with the counts equal, CROSSED WITH
 * THE HOIST (which is why they are listed again; the park's rejected rows had
 * been measured without it): the stored value in a named local inside the
 * guarded block; a second pointer local advanced in place for the last pair;
 * the flag-test argument passed through a named local; the tail call and the
 * return taken through the hoisted pointer rather than the table name.
 * MEASURED AS A DIFFERENT PROGRAM, recorded so it is not re-found and
 * believed: ANY running-offset spelling (`off = 0x85 << 1; ... off += 0x18;`)
 * OVERSHOOTS to fifty or fifty-one instructions, because `*(short *)(p + off)`
 * with a VARIABLE offset is a legal Thumb register-plus-register address, so
 * gcc drops the separate address computation the reference performs.  A
 * CONSTANT offset is what makes gcc materialise the address first.  That is a
 * real asymmetry between a named offset and a literal one, and it is the
 * opposite of the usual direction.
 * MEASURED WORSE: the tail taken through a second, separate pointer local
 * (twenty-six, one instruction long again).
 *
 * NEXT: the residue is the extra callee-saved register plus the lost move2add,
 * and they are ONE question -- get the table base into the first argument
 * register inside the guarded block and both follow.  Do not vary the stores.
 *
 * THIS PIECE HOLDS ONLY THIS FUNCTION, so a landing here is installable alone.
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
    p = L4998;
    if (__GetFlag(0x895)) {
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
