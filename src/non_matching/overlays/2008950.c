/* OvlFunc_943_2008950 -- NOT MATCHING, a LENGTH gap: ref 52 instructions against ours 51 (-1).
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE and had no
 * figure; its own header named scratch/L8950.c, that file still
 * existed and it compiles.  70 of 133 prose parks were in that state --
 * work done, measured, and left in a GITIGNORED directory instead of installed.
 *
 * THE LENGTH-GAP CRITERION APPLIES, and it is this tree's most productive
 * targeting instrument -- four batches run on it closed 8 of 15, 7 of 25,
 * 8 of 25 and 10 of 32, and LOCATED the missing-or-extra instruction on
 * essentially every target.  Our side is 1 instruction(s) SHORTER, so the
 * search is which statement is missing -- see docs/elevation.md, 'OUR STREAM BEING
 * SHORTER IS A SIGNATURE'.
 * No encoding figure exists until the lengths agree.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/2008950.c \
 *     asm/overlays/rom_7c7b9c/ovl_30_c_a_a_a.s --func OvlFunc_943_2008950
 *
 * The original prose park is preserved below.
 */
extern int __GetFlag(int id);
extern unsigned char gScript_968__0200d508[];
extern unsigned char L4ef0[] __asm__(".L4ef0");
extern unsigned char L5028[] __asm__(".L5028");
extern unsigned char L4cf8[] __asm__(".L4cf8");
extern unsigned char L4ba8[] __asm__(".L4ba8");

unsigned char *OvlFunc_943_2008950(void)
{
    unsigned char *p;
    unsigned char *q;
    int v;
    int w;

    if (__GetFlag(0x93e) != 0)
        return gScript_968__0200d508;
    if (__GetFlag(0x927) != 0)
        return L4ef0;
    v = __GetFlag(0x928);
    if (v != 0)
        return L5028;
    if (__GetFlag(0x911) != 0) {
        if (__GetFlag(0x925) != 0) {
            p = L4cf8;
            q = p + (0xa7 << 1);
            *q = v;
            q = p + (0xd7 << 1);
            w = 2;
        } else if (__GetFlag(0x922) != 0) {
            p = L4cf8;
            q = p + (0xd7 << 1);
            w = 1;
        } else {
            return L4cf8;
        }
        *q = w;
        p += 0xe3 << 1;
        *p = w;
        return L4cf8;
    }
    return L4ba8;
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_943_2008950 -- 0x02008950, asm/overlays/rom_7c7b9c/ovl_30_c_a_a_a.s
 *
 * 65 vs 66 lines, 35 differing.  Candidate at scratch/L8950.c.
 * A flag dispatcher returning one of five script pointers.
 *
 * BLOCKER: gcc DERIVES a second table offset where the ROM builds both fresh.
 *      rom   mov r1,#0xa7 / lsl r1,#1 ... mov r3,#0xd7 / lsl r3,#1
 *      ours  mov r2,#0xa7 / lsl r2,#1 ... add r2, #0x60
 * 0xd7<<1 minus 0xa7<<1 is 0x60, and with the first value live in a register
 * gcc reaches the second with an `add`.
 *
 * This is the exact inverse of the case the symbol-base lever solves
 * (src/non_matching/overlays/20085a4.c, where the ROM derives and gcc will not).
 * Here gcc derives and the ROM does not, and there is no lever pointing that
 * way: naming the three offsets as separate locals in the dominating block
 * changes nothing, and neither does CSE_CFLAGS -- so the derivation is the main
 * -O2 CSE, not the rerun pass.
 *
 * Consistent with the Func_80a22f4 finding that the derived-constant machinery
 * has no per-operand granularity: it fires on whatever is in a register, and
 * the source cannot ask for it selectively in either direction.
 *
 * Worth keeping from the attempt: `v = __GetFlag(0x928); if (v) return ...;`
 * and then storing `v` later is right -- the ROM stores r5, the result register
 * of that call, as a byte it knows is zero on that path.
 *)

*/
#endif
