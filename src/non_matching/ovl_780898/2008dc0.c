/* OvlFunc_883_2008dc0  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_a_c_c_a_c_c.s
 * Best screen: 2 instructions in disagreeing regions, of 16 (streams same length).
 *
 * BLOCKER CLASS: argument-setup ordering.
 *
 * THIS FILE STANDS FOR A GROUP OF SEVEN.  The same four-call shape, differing
 * only in constants, appears as:
 *
 *      OvlFunc_883_2008dc0   ovl_30_c_c_c_a_a_a_a_c_c_a_c_c.s
 *      OvlFunc_883_2008e54   ovl_30_c_c_c_a_a_a_c_c_c_a_a.s
 *      OvlFunc_883_2008e84   ovl_30_c_c_c_a_a_a_c_c_c_a_a.s
 *      OvlFunc_883_2008f5c   ovl_30_c_c_c_a_a_a_c_c_c_a_a.s
 *      OvlFunc_883_2008f8c   ovl_30_c_c_c_a_a_a_c_c_c_a_a.s
 *      OvlFunc_884_200881c   rom_784360/ovl_30_c_a_a_a_c_c_c_c_c_a.s
 *      OvlFunc_884_20088ac   rom_784360/ovl_30_c_a_a_a_c_c_c_c_c_c_c.s
 *
 * Six of them have the identical instruction shape
 * `mov r1, #K / mov r0, #0 / lsl r1, #1 / ldr r2, =P`; this one has the
 * operands transposed (`mov r2, #0xd2 / mov r0, #0 / ldr r1, =0x101 /
 * lsl r2, #1`) and is otherwise the same.  Two were screened and both land on
 * exactly 2 of 16 with the same single cause.  ONE FIX WOULD TAKE ALL SEVEN.
 *
 * THE CAUSE
 *
 *      rom   mov r2, #0xd2 / mov r0, #0x0  / ldr r1, =0x101 / lsl r2, #0x1
 *      ours  mov r2, #0xd2 / ldr r1, =0x101 / lsl r2, #0x1  / mov r0, #0x0
 *
 * The ROM materialises the FIRST argument second, between the base of the
 * shifted argument and the pool load. gcc sinks it to last, because a
 * dependency-free `mov` of a small constant has nothing to hold it in place.
 *
 * THE MECHANISM, read out of the compiler source in the build image
 * (/opt/camelot-gcc/gcc-2.96/gcc/). This is a COMPILER DIFFERENCE and is NOT
 * REACHABLE FROM C.
 *
 *   calls.c:805  precompute_register_parameters() walks the arguments in order
 *                and, for each one, copies its value into a pseudo BEFORE any
 *                hard register is loaded, when
 *
 *                    rtx_cost (args[i].value, SET) > 2
 *                 && SMALL_REGISTER_CLASSES && reg_parm_seen
 *
 *                `reg_parm_seen` is set for argument i before argument i is
 *                tested, so it is already 1 on the very first register
 *                argument. The condition therefore reduces to the cost test.
 *
 *   arm.h:1061   #define SMALL_REGISTER_CLASSES  TARGET_THUMB   -- always 1 here.
 *
 *   arm.c:2042   In Thumb, ASHIFT / PLUS / MINUS / NEG / NOT / COMPARE all cost
 *                COSTS_N_INSNS(1) = 4, and a constant needing synthesis or a
 *                literal-pool load costs more than 2 as well.
 *
 *   calls.c:1684 load_register_parameters() then loops FORWARD, 0..num_actuals.
 *                LOAD_ARGS_REVERSED is not defined anywhere in this tree.
 *
 * So every "expensive" argument is hoisted ahead of the register loads, and a
 * cheap `mov rN, #imm` is emitted afterwards -- last, if the expensive ones
 * came before it in the list. The ROM's compiler did not do this: its stream is
 * plain forward load order with the constant synthesis left in place, and only
 * then scheduled.
 *
 * THE PREDICTIVE RULE, and it has been tested
 *
 *   A call MISORDERS when its argument list mixes CHEAP constants with TWO OR
 *   MORE EXPENSIVE values and a cheap one is not last. A call whose arguments
 *   are all cheap constants matches.
 *
 *   Confirmed on OvlFunc_921_20099bc, which has one call of each kind:
 *   __MapActor_SetSpeed(0, 0x20000, 0x1999) misorders, __Func_8092158(0, 0xe8,
 *   0xcc) is byte-identical. Both predictions held on the first screen.
 *   It also explains the successes: OvlFunc_946_2009624 and
 *   OvlFunc_932_200aa10 matched this batch and last, and every one of their
 *   calls passes only cheap constants.
 *
 * WHAT WAS TRIED BEFORE THE DIAGNOSIS, all byte-identical to each other:
 *   the literal 0 passed directly; a local assigned 0 at the top of the
 *   function; the shift as its own statement; the shift folded into the
 *   initialiser; the FINISHED constant 0x1a4 passed instead of `k << 1`
 *   (gcc synthesises it and precomputes the synthesis, so this changes
 *   nothing); the declaration lever.
 *
 * Flags, all byte-identical to the default: -fno-peephole, -fno-caller-saves,
 * -fomit-frame-pointer, -fno-regmove, -fno-gcse, -fno-cse-follow-jumps,
 * -fno-force-mem, -fno-expensive-optimizations.
 *
 * CORRECTION to an earlier claim in this file's history: `-fno-schedule-insns`
 * was never a real test. arm.c:634 FORCE-DISABLES flag_schedule_insns whenever
 * TARGET_THUMB is set, without a warning, "since it's on by default in -O2".
 * The first scheduler never runs for any file in this project. Only
 * -fno-schedule-insns2 does anything, and turning it off is worse here (4 of
 * 16) and worse everywhere else it has been tried.
 *
 * -O1 is also worse (4 of 16), so the -O2 path is the right one.
 */
extern unsigned char L7544[] __asm__(".L7544");
extern void __PlaySound(int id);
extern void __Func_8010560(void *p, int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_8091e9c(int a);

void OvlFunc_883_2008dc0(void)
{
    int k;

    __PlaySound(0xbc);
    __Func_8010560(L7544, 0x2d, 0xb);
    k = 0xd2;
    __Func_809218c(0, 0x101, k << 1);
    __Func_8091e9c(0xb);
}
