/* YesNoMenu  --  0x08028df4, asm/rom_15000/rom_23178_a_a_c_c_a.s
 *
 * BYTE-IDENTICAL.  96 bytes, 42 encodings and 6 relocations identical,
 * instructions 36/36.  Pin-free, device-free, no per-file flag.
 *
 * Verify with (the INSTALLED path, after the split below):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_15000/rom_23178_a_a_c_c_a_b.c \
 *     asm/rom_15000/rom_23178_a_a_c_c_a_b.s --func YesNoMenu
 *
 * SPLIT REQUIRED -- the .s holds two functions and the other is still parked
 * (YesNoMenu2, 32 of 35, blocked on an owner decision about a const.sym entry
 * for 0x24).  tools/split_s.py asm/rom_15000/rom_23178_a_a_c_c_a.s YesNoMenu:
 *     would write asm/rom_15000/rom_23178_a_a_c_c_a_b.s  (1 function, 50 lines)
 *     would write asm/rom_15000/rom_23178_a_a_c_c_a_c.s  (1 function, 39 lines)
 *     would REMOVE asm/rom_15000/rom_23178_a_a_c_c_a.s
 *     would rewrite stage1.ld
 * tools/datacheck.py on the .s prints NOTHING -- no data, no exports, so the
 * split is code/code and `exports` is empty.  tools/piece.py reports no
 * non-function symbols.
 *
 * WHAT THE PARK CLAIMED, AND WHAT SURVIVED.
 * The park (batch 338 recovery of scratch/Cyesno.c) called this a FIVE-WAY
 * REGISTER PERMUTATION and concluded "nothing in the source chooses which of
 * five equally-long live ranges gets the high register".  The OBSERVATION was
 * exactly right and the VERDICT was wrong.
 *
 * Re-measured: ref 36 instructions / 96 bytes against ours 34 / 92, i.e. -2,
 * and the -2 is TWO SITES of -1, both on `k`:
 * (16-bit instructions, `bl` excluded, so the columns sum to 36 and 34)
 *     prologue + parameter copies   ROM 10  ours  9   -1
 *       the ROM pays `mov r3,#0` + `mov r8,r3` where we pay `mov r7,#0`
 *     if (c == 0) c = 3;            ROM  3  ours  3    0
 *     if (a != 0) k = 0x11;         ROM  4  ours  3   -1
 *       the ROM pays `mov r3,#0x11` + `mov r8,r3`
 *     2x AddMenuBarOption + the Func_8028808 argument setup
 *                                   ROM  5  ours  5    0
 *     Func_8028574 setup + result copy  ROM 2  ours 2   0
 *     Func_802851c + the == -1 test ROM  5  ours  5    0
 *     epilogue (with the high-reg restore)  ROM 7 ours 7  0
 * `k` in the ROM is in r8, a HIGH register, which takes no immediate, so each
 * assignment costs a low `mov` plus a `mov r8, r3`.  We put it in r7 and pay
 * one instruction each time.  All of that reproduced.
 *
 * THE LEVER: there are only FIVE long-lived values in the original, not six.
 * The fourth parameter `d` is REUSED to hold Func_8028574's result, so one
 * allocno covers both ranges.  That changes `allocno_compare`'s sort
 * (global.c:600-618, floor_log2(n_refs)*n_refs/live_length*10000*size, no
 * frequency term, ties on ascending allocno) and therefore which value is 4th
 * in line for the callee-saved sequence r5, r6, r7, r8, r10 that
 * REG_ALLOC_ORDER (arm.h:989-994 = 3,2,1,0,12,14,4,5,6,7,8,10,...) hands out
 * after the call-clobbered registers are ruled out.  Measured from
 * `.18.greg`, park body:
 *     ;; 6 regs to allocate: 37 34 32 36 33 35
 *     32 in 6  33 in 8  34 in 5  35 in 10  36 in 7  37 in 5
 *   i.e. r(37) r5, c(34) r5, a(32) r6, k(36) r7, b(33) r8, d(35) r10.
 * With `d` reused there are five allocnos and the order becomes
 * c, d, a, k, b -> r5, r6, r7, r8, r10, with the result sharing d's r6.  That
 * is the ROM, instruction for instruction.
 *
 * This is the lever the PIECE-MATE park already named.  rom_28e54.c
 * (YesNoMenu2) says, in its batch-335 block: "reusing an incoming PARAMETER as
 * the later value is the cheap way to make a pseudo span them.  That is the
 * edit to try there."  It is already in YesNoMenu2's own shipped body
 * (`c = Func_8028574(c);`).  Reading the piece-mate was worth one `cat`.
 *
 * MEASURED NEGATIVES on the park body (all re-measured, ndiff of 42):
 *   autosweep FLAG axis, all eight groups: -O1 35, -fno-gcse 34,
 *     -fno-schedule-insns2 35, -ffixed-r7 43 (LONGER), -fno-strict-aliasing 34,
 *     -fno-strength-reduce 34, -fno-rerun-cse-after-loop 34,
 *     -fno-rerun-loop-opt 34.  Nothing helps; CLOSED.
 *   autosweep ANCHOR axis, 11 of 11 boundaries: all inert.  CLOSED.
 *   the park's own three probes (declaration order, `k = 0` moved down) are
 *     confirmed inert and are declaration-order probes of the kind
 *     rom_28e54.c warned against.
 */
extern void Func_80284dc(void);
extern void AddMenuBarOption(int n);
extern void Func_8028808(int a, int b, int c);
extern int Func_8028574(int n);
extern void Func_802851c(void);

int YesNoMenu(int a, int b, int c, int d)
{
    int k;

    k = 0;
    Func_80284dc();
    if (c == 0)
        c = 3;
    if (a != 0)
        k = 0x11;
    AddMenuBarOption(5);
    AddMenuBarOption(6);
    Func_8028808(k, c, b);
    d = Func_8028574(d);
    Func_802851c();
    if (d == -1)
        d = 1;
    return d;
}
