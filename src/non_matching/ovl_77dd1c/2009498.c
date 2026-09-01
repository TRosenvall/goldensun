/* OvlFunc_882_2009498 (0x02009498) -- NON-MATCHING.
 * Blocker class: register pressure -- the ROM spends r8, gcc will not.
 *
 * 47 lines against the ROM's 53. Five straight-line __Func_8010704 calls whose
 * two stack arguments are shared across sites; the ROM holds three of those
 * constants in callee-saved registers (0x47 in r8 across calls 1 and 4, 0x1a
 * then 0x1b in r5, 0x46 in r6) and our stream keeps none of them past r5/r6.
 * The six missing lines are the r8 save/restore pair and the copies that go
 * with it.
 *
 * The C below already models the ROM's register reuse literally -- one local
 * per register, reassigned where the ROM reassigns -- which is what produced
 * the r5/r6 pair correctly. It does not create demand for a THIRD callee-saved
 * register, and nothing in the recorded levers does: docs/elevation.md's note
 * on the dead-callee-saved-register shape says the same thing from the other
 * side, that the prologue is bookkeeping for a value the C has no way to
 * demand.
 *
 * NOT COPIED FROM ITS EXEMPLAR. tools/fuzzy_solved.py offered
 * OvlFunc_882_2008ec4 at ratio 0.838 and that file is marked `// fakematch` --
 * it forces its allocation with inline-asm register pins. Copying it would
 * have produced a match and buried a hack here. The C below is written from
 * the disassembly.
 *
 * NEXT: this wants whatever eventually creates demand for a specific
 * callee-saved register from source. It is a poor specimen for that work --
 * OvlFunc_957_2008f10 is a better one, being one register short with an
 * otherwise exact body.
 */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_882_2009498(void)
{
    int s;
    int t;
    int u;

    u = 0x47;
    s = 0x1a;
    __Func_8010704(0x1d, 0x14, 1, 1, s, u);
    t = 0x46;
    __Func_8010704(0x1d, 0x14, 1, 1, s, t);
    s = 0x1b;
    __Func_8010704(0x1d, 0x14, 1, 1, s, t);
    __Func_8010704(0x1c, 0x15, 1, 1, 0x1c, u);
    __Func_8010704(0x1c, 0x16, 1, 1, s, 0x48);
}
