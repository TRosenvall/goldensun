/* OvlFunc_888_2008848 -- 0x02008848  (asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_a_c_a.s)
 *
 * NON-MATCHING, 37 of 32 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 32, ours 38) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   RELOCATIONS differ, but THE SAME SYMBOLS AT A SHIFTED OFFSET -- which
 *   this project treats as a CONSEQUENCE of the length difference, not a
 *   separate blocker.  Re-classified in batch 322; the figure IS a distance.
 *   SIZE ref 68 bytes, ours 80.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7892c8/2008848.c \
 *     asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_a_c_a_a.s --func OvlFunc_888_2008848
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 *
 * BLOCKER: PRE hoisting of a repeated cheap constant. 36 of 31, six lines long.
 *
 * The function merges a two-bit field from one sprite into another, twice. The
 * mask 0xc is used at both sites and the ROM rebuilds it in a scratch register
 * each time (`mov r2, #0xc` then `mov r3, #0xc`); gcc hoists it into r6, which
 * pushes the parameter out to r8 and costs the save/restore pair plus the moves
 * -- six lines, and every register downstream shifts.
 *
 * Not reachable. Measured, all identical at 36 differing and 37 lines:
 *   separate `int` locals per use site for the 0xc  (the documented lever for
 *     repeated constants, which works only when the uses are CLOSE -- here they
 *     are ~15 instructions and a call apart)
 *   the -0xd mask created at its first use rather than at the top
 *   separate locals for the two blocks' pointers
 *   --no-rerun-cse
 *   -fno-gcse
 *
 * SELECTED BY tools/aliastell.py AND A FALSE POSITIVE, which is why the
 * detector now excludes it. The ROM re-reads e[0x50] after a byte store, but
 * there is a CALL between them: gcc cannot assume a pointed-to field survives a
 * call, so it reloads regardless and the re-read says nothing about aliasing.
 * -fno-strict-aliasing changes nothing here. The detector now clears its
 * pending set at every `bl`, which took its output from 48 candidates to 11.
 */
extern void *__MapActor_GetActor(int slot);

int OvlFunc_888_2008848(unsigned char *e)
{
    unsigned char *s;
    unsigned char *d;
    int m;

    m = -0xd;
    s = *(unsigned char **)((char *)__MapActor_GetActor(0) + 0x50);
    d = *(unsigned char **)(e + 0x50);
    d[9] = (d[9] & m) | (s[9] & 0xc);
    s = *(unsigned char **)((char *)__MapActor_GetActor(0) + 0x50);
    d = *(unsigned char **)(e + 0x50);
    d[0x15] = (d[0x15] & m) | (s[9] & 0xc);
    return 0;
}
