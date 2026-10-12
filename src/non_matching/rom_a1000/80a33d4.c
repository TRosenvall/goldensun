/* Func_80a33d4 -- NON-MATCHING, 16 differing STREAM encodings of 65.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/K33d4.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 65 / ours 65 stream encodings; 61/61 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a33d4.c \
 *     asm/rom_a1000/rom_a1814_c_a_c_c_a_c_c_c_c_c.s --func Func_80a33d4
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
extern int _Func_801eb64(int a, int b, int c, int d, int e);

void Func_80a33d4(unsigned char *a, int b)
{
    int i;
    int *p;

    p = (int *)(a + 0x48);
    for (i = 0; i <= 7; i++)
        *p++ = _Func_801eb64(2, i, b, 0xf8, 0xa8);
    p = (int *)(a + 0x68);
    for (i = 8; i <= 0xf; i++)
        *p++ = _Func_801eb64(2, i, b, 0x80 << 1, 0xa8);
    p = (int *)(a + 0x88);
    for (i = 0x10; i <= 0x1f; i++)
        *p++ = _Func_801eb64(2, i, b, 0x80 << 1, 0xa8);
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* Func_80a33d4 -- 0x080a33d4, asm/rom_a1000/rom_a1814_c_a_c_c_a_c_c_c_c.s
 *
 * 67 of 67 lines, 16 differing.  Candidate: scratch/K33d4.c.
 *
 * Three near-identical counted loops, each filling a run of words in the caller's
 * structure from _Func_801eb64.  The loop bodies, the `stmia r6!, {r0}`
 * store-with-writeback, the bounds (0..7, 8..0xf, 0x10..0x1f) and the two
 * different fourth arguments (0xf8, then 0x80 << 1 twice) all reproduce.
 *
 * BLOCKER: prologue ordering of the fifth argument.  0xa8 is passed on the stack
 * and the ROM materialises it EARLY, before the destination pointer is derived:
 *
 *      rom   mov r8,r0 / mov r3,#0xa8 / mov r6,r8 / sub sp,#4 / mov r7,r1 /
 *            mov r5,#0 / mov r10,r3 / add r6,#0x48
 *      ours  mov r8,r0 / mov r6,r8 / mov r3,#0xa8 / sub sp,#4 / mov r7,r1 /
 *            add r6,#0x48 / mov r5,#0 / mov r10,r3
 *
 * Same instructions, and the same three-instruction setup repeated before each
 * loop; only the interleaving with the pointer derivation differs.
 *
 * NAMING IT MAKES IT WORSE, and in an instructive way.  A single named local for
 * 0xa8 is hoisted out of all three loops and the function comes out FOUR lines
 * SHORT (63 against 67, 45 differing) -- the ROM re-materialises the constant
 * before every loop and gcc will not.  Assigning it three times, once before
 * each loop, gives the identical 63/45 because gcc commons the three
 * assignments straight back together.  So the "name a repeated stack argument"
 * lever inverts here: the ROM's repetition is the thing that cannot be asked for.
 *
 * TRIED, all 16 except where noted: one named local (63/45); three named locals
 * (63/45); CSE, GCSE, STRENGTH, -fno-schedule-insns.  SCHED2 and O1 are worse
 * (30).
 *)

*/
#endif
