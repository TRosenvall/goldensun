/* OvlFunc_948_2009ac8  --  0x02009ac8
 *
 * MATCHING.  objcmp: 152 bytes, 65 encodings and 10 relocations identical,
 * on --func AND on --whole.  PIN-FREE, SHIM-FREE, NO asm.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7d30e0/ovl_30_c_c_a_a_c_c_a_c_c_a_a.c \
 *     asm/overlays/rom_7d30e0/ovl_30_c_c_a_a_c_c_a_c_c_a_a.s --whole
 *
 * SPLIT SHAPE: none needed.  split_s.py --dry-run says the reference "holds only
 * OvlFunc_948_2009ac8 and no data; convert it directly".  datacheck.py: no data.
 * shimcount.py: 0.
 *
 * Decides which of six behaviour routines to run for the actor, from its
 * position field divided down to a room index.
 *
 * ===== WHY THE LAST ARM IS WRITTEN OUT THREE TIMES, AND WHY IT COSTS NOTHING =====
 *
 * The park spelled it `d == 0x27 || d == 0x26 || d == 0x25` and lost two
 * instructions to fold:
 *
 *     rom    cmp r5,#0x27 / beq / cmp r5,#0x26 / beq / cmp r5,#0x25 / bne   SIX
 *     ours   mov r3,r5 / sub r3,#0x25 / cmp r3,#0x2 / bhi                   FOUR
 *
 * `fold_range_test` (fold-const.c:3757, called from the TRUTH_ORIF_EXPR arm of
 * `fold` at fold-const.c:6031) turns adjacent singleton ranges over one
 * expression into `(unsigned)(X - low) <= span`.  Its own comment at :3203 gives
 * this exact example.  It fires whenever `operand_equal_p (lhs, rhs, 0)` holds on
 * the two `make_range` subjects and `merge_ranges` succeeds, so NO spelling of a
 * single boolean expression over one variable avoids it -- `||` of `==`, `&&` of
 * `!=`, and `>= && <=` all reduce to the same range check.  This is the FRONT
 * END, not an RTL pass; no pass flag and no register pressure bears on it.
 *
 * Three separate `else if`s never build a TRUTH_ORIF_EXPR, so fold never sees a
 * range.  jump.c's cross-jumping then merges the three identical tails and the
 * conditional-jump inversion turns each `bne next / b arm` into `beq arm`, which
 * is the ROM's six instructions exactly.  THE DUPLICATION IS FREE.
 *
 * > GENERALISABLE LEVER, new: when the reference tests each constant separately
 * > (`cmp`/`beq` per value) and you have a range test (`sub`/`cmp`/`bhi`), the
 * > original had SEPARATE STATEMENTS, not one `||` chain -- and when the arms are
 * > identical, writing the arm once per value costs nothing because cross-jumping
 * > puts it back.  fold-const.c:3757.
 *
 * ===== THE PARK'S BLOCKER WAS AN ARITHMETIC ERROR, AND A CROSSING FIXED IT =====
 *
 * The park claimed "68 lines against 71 -- THREE SHORT, and the three are
 * `push {r6}` / `mov r6, #0` / `pop {r6}`", and called it the same register
 * allocator blocker as Func_80b0070.  THAT COUNT IS WRONG.  `push {r5,r6,r14}`
 * and `push {r5,r14}` are ONE instruction each, and both sides emit exactly one
 * `mov rN,#0` -- the ROM's before the branch chain, ours at the use.  The zero
 * cost ZERO instructions of length; it was a position difference only, and the
 * whole two-instruction gap was the range test above.
 *
 * `z = 0;` before the branch -- which the park had already written and recorded
 * as "not enough on its own" -- is in this body UNCHANGED, and the zero takes r6
 * by itself once the lengths agree.  It was a prerequisite, not a dead end.
 * Func_80b0070 should be re-measured on its own arithmetic before anyone spends
 * more on the "allocator declines a callee-saved register for a rebuildable
 * value" class; this specimen has left it.
 *
 * KEPT FROM THE PARK, both still load-bearing: both actor fields loaded into
 * named locals BEFORE the sign test that begins the signed division (worth 8),
 * and `/ 0x100000` written as a signed division, whose `if (x < 0) x += 0xfffff`
 * fixup is gcc's expansion and never appears in the source.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void OvlFunc_948_20099e8(void);
extern void OvlFunc_948_2009a9c(void);
extern void OvlFunc_948_2009a48(void);
extern void OvlFunc_948_2009a70(void);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_948_2009ac8(void)
{
    unsigned char *p;
    int d;
    int z;
    int n;
    int a;
    int b;

    p = __MapActor_GetActor(8);
    a = *(int *)(p + 8);
    b = *(int *)(p + 0xc);
    d = a / 0x100000;
    if (b == 0)
        __MapActor_GetActor(8)[0x23] = 2;
    OvlFunc_948_20099e8();
    z = 0;
    __MapActor_GetActor(8)[0x55] = 3;
    if (d == 0x28) {
        OvlFunc_948_2009a9c();
    } else if (d == 0x2a) {
        OvlFunc_948_2009a48();
    } else if (d == 0x29) {
        OvlFunc_948_2009a70();
    } else if (d == 0x27) {
        n = 0x2a;
        __Func_8010704(0x3d, 0x24, 1, 1, d, n);
        __MapActor_GetActor(8)[0x55] = z;
        *(int *)(__MapActor_GetActor(8) + 0xc) = 0x80 << 14;
    } else if (d == 0x26) {
        n = 0x2a;
        __Func_8010704(0x3d, 0x24, 1, 1, d, n);
        __MapActor_GetActor(8)[0x55] = z;
        *(int *)(__MapActor_GetActor(8) + 0xc) = 0x80 << 14;
    } else if (d == 0x25) {
        n = 0x2a;
        __Func_8010704(0x3d, 0x24, 1, 1, d, n);
        __MapActor_GetActor(8)[0x55] = z;
        *(int *)(__MapActor_GetActor(8) + 0xc) = 0x80 << 14;
    }
}
