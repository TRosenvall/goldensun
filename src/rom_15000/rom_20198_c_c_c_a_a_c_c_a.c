/* Func_80217a4 -- 0x080217a4  (asm/rom_15000/rom_20198_c_c_c_a_a_c_c_a.s)
 *
 * MATCHING.  objcmp --func and --whole both green:
 *   OK whole file -- 164 bytes, 74 encodings and 3 relocations identical
 * The park read 28 of 74, ref 164 bytes against ours 160.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_15000/rom_20198_c_c_c_a_a_c_c_a.c \
 *     asm/rom_15000/rom_20198_c_c_c_a_a_c_c_a.s --whole
 *
 * SPLIT SHAPE: none.  The .s holds this function alone (one
 * .thumb_func_start), so datacheck/split_s are not needed.  PINS: 0.  No flag
 * group.
 *
 * The 28 decomposed into FOUR runs with three unrelated causes.
 *
 * 1. THE STRUCT IS BITFIELDS, and that closes three of the four runs at once.
 *    The park wrote the object as "unsigned char *o" with hand-written masks.
 *    But "& -0x3f" is ~(0x1f << 1), and "& 0xfffffe00" / "& 0x1ff" are ~0x1ff
 *    and 0x1ff -- those are store_bit_field masks, and the halfword unit at
 *    0x16 spans bytes 0x16-0x17, so BOTH fields live in ONE 16-bit bitfield
 *    unit (:9 at bit 0, :5 at bit 9, :2).  Written that way the value-first
 *    mask order, the register roles in the halfword block, AND the function
 *    SIZE all fall out together: 28 -> 5.
 *
 *    The size is the interesting part.  The reference is 164 bytes and dumps
 *    its literal pool BEFORE the epilogue with a branch over it; ours was 160
 *    and dumped after the return.  base.c.26.mach (push_minipool_fix,
 *    arm.c:5380) showed all seven of our fixes as SImode, range 1020.  The :9
 *    store is HImode, so the 0x1ff fix becomes a HImode fix: *thumb_movhi_insn
 *    alternative 1 takes the "mn" constraint and carries pool_range 64
 *    (arm.md:4318, the attr at 4352) against pool_range 1020 for
 *    *thumb_movsi_insn.  fix->forwards is get_attr_pool_range (insn)
 *    (arm.c:5370), so its max_address is 108+64 = 172 -- below the
 *    post-return barrier address of 180.  arm_reorg then breaks at that
 *    barrier with last_barrier still NULL (arm.c:5488) and calls
 *    create_fix_barrier (arm.c:5571), which inserts a barrier and a jump
 *    around the pool: +1 insn, +1 align nop, +4 bytes = exactly 164.  And
 *    add_minipool_forward_ref sorts the HImode entry to the pool HEAD, which
 *    is why the reference pool reads 0x1ff, iwram_3001800, .L37230,
 *    0xffff0000, 0xffff, 0xfff0, 0xfffffe00 in that order.
 *
 * 2. THE TABLE READ IS A SHIFTED BYTE OFFSET, NOT AN ARRAY_REF.  5 -> 4.
 *    One encoding: ldr r1,[r1,r3] against ldr r1,[r3,r1].  expr.c:7340, in the
 *    both_summands tail of expand PLUS_EXPR, says "Put a constant term last
 *    and put a multiplication first" and does
 *        if (CONSTANT_P (op0) || GET_CODE (op1) == MULT)
 *          temp = op1, op1 = op0, op0 = temp;
 *    An ARRAY_REF scaling comes back from expand_expr under EXPAND_SUM as a
 *    MULT rtx, so base and index get swapped and the mem prints
 *    [index, base].  A shift expression comes back as a REG, the swap does not
 *    fire, and the operand order is the one in the ROM.
 *
 * 3. THE BYTE RMW AT 0x15 NEEDS AN int TEMP, not |=.  4 -> 0.
 *    combine.c:3492 canonicalises a commutative op, and its third clause
 *    swaps when XEXP (x, 0) is a SUBREG whose SUBREG_REG has RTX class o while
 *    XEXP (x, 1) does NOT have class o.  With "o->u15 |= 3" on a char field
 *    BOTH ior operands are (subreg:SI (reg:QI)), and the RTX class of a SUBREG
 *    is x, not o -- so the clause fires and the load/constant order flips.
 *    The two-address orr then ties its destination to the pseudo holding the
 *    CONSTANT; local-alloc gives the constant r3 and the load r2; and the
 *    ldrb r2,[r5,#0x15] picks up an OUTPUT dependence on the later
 *    ldrh ...,[r5,#6] (also r2).  That lifts its sched2 dependent count to 3
 *    against 2 for the ior, and rank_for_schedule (haifa-sched.c:4100-4110)
 *    prefers the larger dependent count at equal priority 17 -- hoisting the
 *    load above the 0x17 store and taking the register roles with it.  An int
 *    temp makes the second ior operand a plain const_int, whose RTX class IS
 *    o, so the clause does not fire and the whole cascade unwinds.
 *
 * Measured inert or worse, all against the bitfield body:  L37230[idx],
 * L37230 + idx, &L37230[idx] and *(t + idx) all read 7 (worse); idx[t] reads 5
 * (inert).  For the 0x15 store, "o->u15 = o->u15 | 3", "3 | o->u15",
 * "(unsigned char)(o->u15 | 3)" and "|= 3u" all read 4 (inert) -- the int temp
 * is the whole lever, NOT the operand order written in the source.
 */
typedef struct {
    unsigned int a : 16;
    unsigned int b : 16;
    unsigned int c : 16;
    unsigned int d : 16;
} P;

typedef struct {
    unsigned char pad0[6];      /* 0x00 */
    unsigned short u6;          /* 0x06 */
    unsigned char u8;           /* 0x08 */
    unsigned char pad9[6];      /* 0x09 */
    unsigned char uf;           /* 0x0f */
    unsigned char pad10[4];     /* 0x10 */
    unsigned char u14;          /* 0x14 */
    unsigned char u15;          /* 0x15 */
    unsigned short b9 : 9;      /* 0x16 bits 0-8  */
    unsigned short b5 : 5;      /* 0x17 bits 1-5  */
    unsigned short b2 : 2;
} O;

extern unsigned int iwram_3001800;
extern int L37230[] __asm__(".L37230");
extern int Func_8003d28(P *p);

void Func_80217a4(O *o)
{
    P p;
    int *t;
    int idx;
    int v;
    int res;
    int h;
    int k15;

    t = L37230;
    idx = (iwram_3001800 >> 1) & 7;
    v = *(int *)((char *)t + (idx << 2)) / 256;
    if (o == 0)
        return;
    p.a = v;
    p.b = v;
    p.c = 0;
    res = Func_8003d28(&p);
    o->b5 = res;
    k15 = o->u15;
    o->u15 = k15 | 3;
    h = o->u6 + 0xfff0;
    o->b9 = h;
    o->u14 = o->u8 + 0xf0;
    o->uf = 0xfc;
}
