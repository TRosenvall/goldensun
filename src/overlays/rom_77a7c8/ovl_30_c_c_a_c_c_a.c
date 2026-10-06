/* OvlFunc_881_200b95c  --  0x0200b95c      EXACT
 *
 * From asm/overlays/rom_77a7c8/ovl_30_c_c_a_c_c_a.s, which holds this function
 * alone and no data -- converted directly, NO SPLIT (the park's note that the
 * file also holds OvlFunc_881_200b9fc is stale; that one was split out
 * earlier).  72 encodings, 160 bytes, 7 relocations.
 *
 * Every third frame, spawn a sparkle at one of the four diagonal neighbours of
 * the party leader's tile.
 *
 * VERDICT
 *   OK OvlFunc_881_200b95c -- 160 bytes, 72 encodings and 7 relocations identical
 * from tools/objcmp.py, reproduced on three consecutive runs.  datacheck.py
 * exits 0; shimcount.py exits 0 -- NO pins, no barriers, no device, no flag
 * group.  objcmp also prints the known benign line
 *   ~~ relocation _umodsi3_RAM / __umodsi3 is ONE symbol
 * which overlays/rom_77a7c8/overlay.ld resolves at link.
 *
 * ------------------------------------------------------------------ THE LEVER
 *
 * ONE EDIT against the park body: the SECOND argument, the constant -1, moved
 * into a named local assigned in the block DOMINATING the switch,
 *
 *     if (iwram_3001e40 % 3 == 0) {
 *         m = -1;                               <-- ADDED
 *         switch (__Random() * 4 >> 16) {
 *         case 0:
 *             __Func_80933f8((x << 16) - 0x10000, m, ...);
 *
 * and the four call sites left otherwise exactly as the park had them.
 *
 * THE PARK LOOKED AT THE RIGHT CALL AND NAMED THE WRONG ARGUMENT.  Its list
 * reads "NAMING THE ARGUMENT IS DECISIVELY WRONG HERE" on the strength of four
 * rows that all name the THIRD argument (24, 26, 47, 49 differing).  The
 * argument to name is the SECOND, and it has to be named in a DOMINATING block:
 * naming it inside each arm instead measures 10, WORSE than the park's 6.  So
 * the park's conclusion is the correct reading of its own rows and the wrong
 * generalisation -- "do not name an intermediate that is consumed immediately"
 * is about the arm-local position, not about naming.
 *
 * ---------------------------------------------------- WHY, READ IN sched2
 *
 * The residue decomposes into THREE RUNS OF TWO, one per arm, all the same
 * transposition, and case 1 agrees with the ROM already:
 *
 *     arm        ROM                            OURS (park body)
 *     case 0     lsl r2,r6,#16 / mov r1,#1      mov r1,#1 / lsl r2,r6,#16
 *     case 1     mov r1,#1 / lsl r2,r6,#16      SAME -- 0 differing
 *     case 2     lsl r2,r6,#16 / mov r1,#1      mov r1,#1 / lsl r2,r6,#16
 *     case 3     lsl r2,r6,#16 / mov r1,#1      mov r1,#1 / lsl r2,r6,#16
 *
 * Case 1 agrees because there the x-offset constant lives in r2
 * (mov r2,#0x80 / lsl r2,#9 / add r0,r2), so an anti-dependence forces the r2
 * fill late; in the other three arms that constant is in r3 and r2 is free.
 *
 * -fsched-verbose=5 on the park body, arm block (sched2 runs BEFORE jump2's
 * crossjumping, so each arm is still a complete block with its own call):
 *
 *     insn   what                  prio  dependents
 *     253    ldr r3,=0xffff0000     6    {call, 275, 87}
 *      83    lsl r0,r5,#16          5    {call, 87}
 *      87    add r0,r0,r3           4    {call, 275}
 *     275    mov r3,#0x80           4    {call, 276}
 *     276    lsl r3,r3,#9           3    {call, 103, 95}   <- 3 dependents
 *     273    mov r1,#1              3    {call, 274}       <- 2 dependents
 *      91    lsl r2,r6,#16          3    {call, 95}        <- 2 dependents
 *
 * rank_for_schedule orders by PRIORITY, then DEPENDENT COUNT, then INSN_LUID.
 * At t=5 insn 276 beats both 273 and 91 at equal priority on its THIRD
 * dependent -- so the dependent-count rung is live here, and it is visible
 * working.  273 and 91 then tie on priority AND on dependent count, so the
 * LUID decides, and 273 has the lower LUID.
 *
 * WHY, before sched2.  precompute_register_parameters (calls.c:805) copies an
 * argument into a pseudo when rtx_cost (value, SET) > 2, in ARGUMENT INDEX
 * ORDER.  arm_rtx_costs (config/arm/arm.c:2076-2083) under TARGET_THUMB gives
 * a CONST_INT in a SET cost 0 when (unsigned) INTVAL < 256 and
 * COSTS_N_INSNS (3) otherwise, so -1 (0xffffffff) is EXPENSIVE and IS
 * precomputed -- at index 1, i.e. BEFORE index 2's y expression is expanded.
 * That is the whole LUID order.  Writing -1 as anything else cannot help: every
 * spelling folds to the same CONST_INT.
 *
 * Giving the -1 a named local in a dominating block takes its def out of the
 * call's precompute stream altogether: args[1].value is then already a REG, the
 * copy_to_mode_reg is skipped, and the r1 fill is emitted by
 * load_register_parameters AFTER index 2's expansion.  mov r1,#1 then has the
 * HIGHER LUID and loses the tie, which is the ROM.  The mov/neg pair still
 * splits per arm and jump2 still crossjoins the neg into the shared tail.
 *
 * ------------------------------------------------- MEASURED, batch 329
 * Metric: tools/objcmp.py differing encodings, 72-encoding reference.
 *
 *   spelling                                                    differing
 *   ------------------------------------------------------------  -------
 *   SHIPPED: m = -1 before the switch, inside the % 3 guard             0
 *   m = -1 before the `if (iwram_3001e40 % 3 == 0)` instead            0
 *   PARK BODY (-1 inline at all four sites)                             6
 *   __Func_80933f8 with NO prototype                                    6
 *   __Func_80933f8 declared with a trailing ellipsis                    6
 *   __Func_80933f8 declared with an empty parameter list                6
 *   __Func_80933f8 declared to return int                               6
 *   m = -1 assigned INSIDE each arm, just before the call              10
 *
 * The four declaration rows are the no-prototype lever measured INERT on this
 * shape, which is worth recording: that lever is for a two-`mov` block where
 * r0 must go LAST, and here the argument that has to move is r2's shift, not
 * r0.  The park's own "NEXT: vary WHICH REGISTER the per-arm constant lands in"
 * was aimed at the right observation and the wrong variable -- the register the
 * per-arm constant takes is a consequence, not the lever.
 *
 * WHAT IS KEPT FROM THE PARK, unchanged and all of it right: the gState array
 * idiom, both ldrsh register-offset reads, iwram_3001e40 % 3 through the
 * overlay's aliased helper, the __Random() * 4 >> 16 switch value as an
 * UNSIGNED shift, the four diagonal offsets in 16.16, and the four arms
 * written out in full so gcc's crossjump pass builds the shared tail.
 */
extern unsigned char gState[];
extern unsigned char *__MapActor_GetActor(int slot);
extern unsigned int iwram_3001e40;
extern unsigned int __Random(void);
extern void __Func_80933f8(int a, int b, int c, int d);

void OvlFunc_881_200b95c(void)
{
    unsigned char *g;
    unsigned char *a;
    int x;
    int y;
    int m;

    g = gState;
    a = __MapActor_GetActor(*(int *)(g + (0xfa << 1)));
    x = *(short *)(a + 0xa);
    y = *(short *)(a + 0x12);
    if (iwram_3001e40 % 3 == 0) {
        m = -1;
        switch (__Random() * 4 >> 16) {
        case 0:
            __Func_80933f8((x << 16) - 0x10000, m, (y << 16) + (0x80 << 9), 1);
            break;
        case 1:
            __Func_80933f8((x << 16) + (0x80 << 9), m, (y << 16) - 0x10000, 1);
            break;
        case 2:
            __Func_80933f8((x << 16) + (0x80 << 9), m, (y << 16) + (0x80 << 9), 1);
            break;
        case 3:
            __Func_80933f8((x << 16) - 0x10000, m, (y << 16) - 0x10000, 1);
            break;
        }
    }
}
