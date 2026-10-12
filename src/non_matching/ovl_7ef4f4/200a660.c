/* OvlFunc_965_200a660  --  asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c_c_c_c_c_c.s
 *
 * STILL NON-MATCHING.  THIS BODY'S OWN FIGURE: 4 of 32 encodings, DEVICE-FREE
 * AND PIN-FREE, down from the park's 19 of 32.  MEASURED batch 340 brief C with
 * tools/objcmp.py at production flags:
 *     ENCODINGS differ in 4 of 32   INSTRUCTIONS ref 28 / ours 28
 *     SIZE equal at 72 bytes        RELOCATIONS IDENTICAL
 *   so the figure IS a distance, not a phase reading.  The park's own 19 of 32
 *   was re-derived first and confirmed, with the same 28/28 counts.
 *   PINS 0, shims 0, devices 0 (tools/shimcount.py).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7ef4f4/200a660.c \
 *     asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c_c_c_c_c_c.s --func OvlFunc_965_200a660
 *
 * NOTE ON FLAGS, inherited and RE-CONFIRMED: tryc.py's wildcard rule gives this
 * path -O1 and that is the WRONG translation unit's row.  The figures here are
 * -O2.  objcmp/sweep_variants resolve it correctly on their own; only tryc needs
 * --cflags "-O2".  At -O1 the parked body reads 23 of 32 and this body is worse
 * still.
 *
 * ===== TWO SOURCE FACTS THE PARK HAD WRONG, BOTH PROVED IN THE COMPILER =====
 *
 * 1. THE FUNCTION IS NOT `void`.  IT RETURNS A SCALAR, AND THE EPILOGUE PROVES
 *    IT.  The ROM ends `pop {r1} / bx r1`.  A void thumb function cannot emit
 *    that.  `thumb_exit` (arm.c:8215; the result-mode block at arm.c:8288-8310)
 *    builds regs_available_for_popping from the RESULT MODE:
 *        size 0 and VOIDmode   ->  r0 | r1 | r2
 *        size <= 4             ->  r1 | r2
 *    the match loop at arm.c:8311-8316 then clears the lowest free bit per
 *    register still to pop, and the `if (available > 0)` line at arm.c:8318-8319
 *    keeps only what is left.  With {r0,r1,r2} that leaves r0 -- `pop {r0} / bx
 *    r0`, which is what the parked void body emitted.  With {r1,r2} it leaves r1
 *    -- `pop {r1} / bx r1`, the ROM.  So the ROM's result mode has size <= 4.
 *    The natural reading is that it returns the inner call's result, and that
 *    reading closes the epilogue exactly: 19 -> 17.
 *
 *    THIS IS A GENERAL TELL, not a fact about this function.  In a thumb
 *    interworking leaf-with-calls, `pop {r0} / bx r0` means VOID and
 *    `pop {r1} / bx r1` means a scalar return.  It is free to read off any
 *    reference .s before the C is written.
 *
 * 2. "NO REORDERING OF THE BODY MOVES THE BLOCK'S PSEUDO" IS FALSE.  The park
 *    says the stack block's pseudo "is created when gcc lowers the local array's
 *    address, which happens ahead of any statement -- so no reordering of the
 *    body moves it", and concluded the r5/r6 exchange was unreachable.  A NAMED
 *    POINTER TO THE ARRAY -- `int *p = t;` with every access through `p` --
 *    moves it, and the exchange disappears: the actor takes r5 and the block
 *    takes r6, exactly as the ROM has them.  17 -> 11, and with it the whole
 *    tail from `mov r3, r5` to `bx r1` becomes exact.
 *
 *    WHY.  Both pseudos are live across __Func_8011f54, so both need a
 *    call-saved register; REG_ALLOC_ORDER (arm.h:989-995) offers r4 -- call-USED
 *    in this tree under -fcall-used-r4 -- then r5, then r6, so whichever allocno
 *    `allocno_compare` (global.c:597-620) sorts FIRST takes r5.  The named
 *    pointer gives the block its own pseudo with its own reference count and its
 *    own live range instead of an addressof lowered at function entry, and that
 *    is enough to put the actor ahead of it in the sort.
 *
 * ===== WHAT THE REMAINING FOUR ENCODINGS ARE, AND THE COUPLING =====
 *
 * Two swapped PAIRS, and the registers in them are already right:
 *     ROM   ldr r1, [r5, #0x8]   ours  ldr r2, [r5, #0x10]
 *     ROM   ldr r2, [r5, #0x10]  ours  ldr r1, [r5, #0x8]
 *     ROM   str r1, [r6, #0x0]   ours  str r2, [r6, #0x8]
 *     ROM   str r2, [r6, #0x8]   ours  str r1, [r6, #0x0]
 * Everything else -- the prologue, the table index, the mask, the shift, both
 * adds, `mov r6, sp`, the f22 read, both calls, the epilogue -- is exact.
 *
 * THE TWO AXES ARE COUPLED AND ONE SOURCE CANNOT SATISFY BOTH.  Measured over
 * 110 bodies (c_v3b..c_v3f) crossing: which of a->f8 / a->f10 is a named local,
 * the order of those two statements, the order of the two sum statements, the
 * store order, and where the mask statement sits.
 *   * STORE ORDER FOLLOWS SOURCE ORDER.  The two stores are independent, equal
 *     priority, equal dependent count, so sched2 falls through to INSN_LUID
 *     (haifa-sched.c:4113-4116) and emits them in source order.  `p[0]` first
 *     then `p[2]` is therefore required.
 *   * BUT WITH `p[0]` FIRST THE TWO LOADS LAND IN THE OPPOSITE REGISTERS -- a->f8
 *     in r2 and a->f10 in r1, where the ROM has f8 in r1 and f10 in r2.  That
 *     body is 6 of 32 with the instruction ORDER exactly right and only the two
 *     register pairings wrong.
 *   * WITH `p[2]` FIRST and ONLY a->f10 named, the registers are right and the
 *     two pairs are merely transposed.  That is this body, 4 of 32.
 * So the frontier is: 4 of 32 with the order wrong, or 6 of 32 with the
 * registers wrong, and no spelling found reaches both at once.
 *
 * MEASURED AND INERT OR WORSE on the NEW body (all re-crossed, because a lever
 * that is inert on a park need not be inert on a changed body):
 *   all 24 declaration orders of {c0, c2, lo, hiv}        INERT, every one 6
 *   an anchor `do { } while (0);` at all 9 boundaries      best 6, base 4
 *   a bare `__asm__ volatile ("")` at all 9 boundaries     best 6, base 4
 *   an `__asm__ volatile ("" :: "i"(0))` at all 9          best 6, base 4
 *   -fno-schedule-insns2 (tools/flagcmp.py)                15 raw, much worse
 *   -O1                                                    21 raw, and one
 *                                                           instruction short
 *   -fno-gcse                                              11 raw, worse
 *   returning `short` instead of `int`                     26 of 32, dirty
 *   a non-void return that returns `v` rather than the call 39 of 32, dirty
 *   t[1] moved to any other position in the statement list 20, 25, 31, 31
 *   a second pointer `struct A *s = a;` for the actor      17 of 32
 *   a named local for the table index                      17 of 32
 *   a->f22 read into a local before the call               20 of 32
 *   the mask folded into its use (no named `hi`)           17 of 32
 *   BOTH CHEAP AXES CLOSED on the PARKED body by tools/autosweep.py: eight flag
 *   groups and seven anchor placements, all 19 of 32 except -O1 (23, worse) and
 *   -fno-schedule-insns2 (22, worse).
 *
 * WHAT WOULD BE NEW.  A lever that changes the two LOADS' register pairing
 * without changing their source order -- i.e. one that reaches local-alloc's
 * quantity numbering for two pseudos with identical reference counts and
 * identical live ranges -- or a reason the ROM's store order can differ from its
 * source order, which would mean one of those two stores is NOT an independent
 * insn in the ROM's RTL.  Do not spend another round on declaration order; all
 * twenty-four permutations are measured inert above.
 */
struct A {
    unsigned char pad00[6];
    unsigned short f6;
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[0xe];
    unsigned char f22;
};

extern int L2fd4[] __asm__(".L2fd4");
extern int __Func_8011f54(int a);
extern int OvlFunc_965_200806c(int *p, struct A *a);

int OvlFunc_965_200a660(struct A *a)
{
    int t[3];
    int *p = t;
    int v;
    int hi;
    int c2;
    int lo;
    int hiv;

    v = L2fd4[a->f6 >> 12];
    hi = 0xffff0000 & v;
    c2 = a->f10;
    lo = a->f8 + hi;
    hiv = c2 + (v << 16);
    p[2] = hiv;
    p[0] = lo;
    p[1] = __Func_8011f54(a->f22);
    return OvlFunc_965_200806c(p, a);
}
