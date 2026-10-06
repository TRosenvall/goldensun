/* OvlFunc_896_200c260 -- 0x0200c260
 *
 * STILL NON-MATCHING, **3 differing encodings of 85** (ref 85 / ours 85, first
 * differing index 6).  WAS 4 of 85.  Sizes equal, relocations clean, no
 * INSTRUCTION COUNT line and no POOL WORD COUNT line.  PIN-FREE, SHIM-FREE,
 * FLAG-FREE, DEVICE-FREE.
 *
 * Verify with: python3 tools/objcmp.py src/non_matching/ovl_78ef88/200c260.c asm/overlays/rom_78ef88/ovl_314_c_c_c_c_a.s --func OvlFunc_896_200c260
 *
 * SPLIT: NONE NEEDED.  grep -c func_start = 1, tools/datacheck.py CLEAN.
 *
 * ===== WHAT CHANGED: ONE do { } while (0) AT THE TOP, 4 -> 3 =====
 *
 * The park's four differing encodings were indices 5-8 and it read them as ONE
 * reload-scratch-register choice with three transposed neighbours.  Half of
 * that is right.  They are TWO causes, and one of them is a scheduling hoist
 * that a source-level barrier removes:
 *
 *     idx  REF              | BEFORE (4)         | NOW (3)
 *      5   mov sl, r0       | mov r2, #0      XX | mov sl, r0      ok
 *      6   mov r0, #0       | mov sl, r0      XX | mov r2, #0      XX
 *      7   mov r8, r0       | mov r0, #22     XX | mov r0, #22     XX
 *      8   mov r0, #22      | mov r8, r2      XX | mov r8, r2      XX
 *
 * `do { } while (0)` as the first statement (haifa's two total barriers) stops
 * sched2 hoisting the independent `mov r2, #0` above the parameter save, so the
 * parameter save lands first, as the ROM has it.  `__asm__ volatile ("")` in the
 * same place is byte-identical at 3.  The park had ALREADY SEEN this effect and
 * attributed it to a flag it then rejected -- "with -fno-schedule-insns2 the
 * first difference MOVES to index 6 ... the order is then right and only the
 * register is wrong".  It is reachable WITHOUT the flag, from source.
 *
 * ===== THE PARK'S BOUND: ITS EVIDENCE IS REFUTED, THE BOUND IS NOT =====
 *
 * The park closes on this discriminator:
 *
 *     "Count `Using reg N` in .18.greg.  This body: reg 3 once, reg 2 twice --
 *      the SPILL SET IS {r2, r3}, and r0 IS NOT IN IT.  Round robin over a
 *      two-element set cannot ever emit the ROM's `mov r0, #0`, at any site,
 *      under any spelling."
 *
 * THAT MEASUREMENT IS INVALID, and the invalidity is already documented:
 * *the register find_reg prints is not necessarily the one emitted.*  Measured
 * on this body:
 *
 *     .18.greg    Spilling for insn 30. / Using reg 3 for reload 0
 *     .19.flow2   (insn 219 (set (reg:SI 2 r2) (const_int 0)))
 *                 (insn 30  (set (reg/v:SI 8 r8) ...))
 *
 * greg PRINTS 3 and the EMITTED register is 2, at the one site the whole bound
 * rests on.  So the park's "fresh picks in stream order are r2, r3, r2, r3, r3,
 * r3 -- exactly spill_regs[i mod 2] over [2,3]" mixes printed with emitted
 * values, and "the round-robin model is confirmed on this function" does not
 * follow.  The bound is NOT thereby broken -- I did not reach r0 -- but it is
 * no longer evidenced, and anyone continuing here should count FREE REGISTERS
 * at insn 30 in .19.flow2 rather than greg's printf.
 *
 * ===== MODULE-MATE EVIDENCE, WHICH CONTRADICTS THE PARK'S "CONDITION" =====
 *
 * The park concluded the condition for r0 is REGISTER PRESSURE -- "a construct
 * that forces a reload where r1, r2 and r3 are all busy" -- from CheckLure in
 * src/rom_77000/rom_77320_a_c_a_b.c, whose r0 enters the set from a reload deep
 * inside a doubly-nested loop.  Scanning all 454 LANDED SIBLINGS of this
 * function's upstream module (overlays/ovl_314.s, via tools/upstream_module.py)
 * for the ROM's exact `mov r0, #imm / mov rHI, r0` shape finds TWO, and NEITHER
 * has any pressure:
 *
 *   asm/overlays/rom_7eaf28/ovl_314_c_c_c_c_a.s  (twice, both in the prologue)
 *       push {r5,r6,r7,lr} / mov r7,r8 / push {r7} / mov r0,#0 / mov r8,r0 /
 *       ldr r0,.L73 / bl __GetFlag
 *   asm/overlays/rom_7a5214/ovl_314_c_c_c_c_b.s
 *       push {r7} / ldr r7,.L19 / ldr r5,[r7] / mov r0,#0 / mov fp,r0
 *
 * Both are `void` functions -- no incoming parameter, so r0 is free at entry
 * and the FIRST reload in the function takes it.  That is a far cheaper
 * condition than a doubly-nested loop, and it is NOT satisfiable here: this
 * function takes `int item` in r0.  The ROM nonetheless gets r0, after saving
 * the parameter to sl, so the real question is narrower than either account:
 * why is r0 excluded at insn 30 in OUR build when the parameter save at insn 4
 * already freed it.  That is the question to take to pass 3.
 *
 * ===== MEASURED THIS BATCH, ALL AGAINST THE 3-DIFFERING BASE =====
 *
 * Every one of the park's recorded inert rows is STILL EXACTLY INERT crossed
 * with the barrier -- they are not missing prerequisites:
 *
 *     z as unsigned char * instead of int                         3  inert
 *     declaration order, z first                                  3  inert
 *     g1 literal at the use                                       3  inert
 *     named local for the gfx selector o[0x1c]                    3  inert
 *     named local for the (0x80 << 3) offset                      3  inert
 *     declz-first x g1-literal (crossed)                          3  inert
 *     `int it = item;` as the first statement                     3  inert
 *     `int it = item;` WITHOUT the barrier                        4  (so the
 *                                        barrier is the whole 4 -> 3)
 * and the regressions:
 *     m as a literal at the use                                  58  RELOCDIFF
 *     m-literal x g1-literal                                     58  RELOCDIFF
 *     two variables: `z` for the zero, `unsigned char *buf` for
 *       the galloc result (properly typed, 4 spellings)           78  RELOCDIFF
 *     `z = 0` moved after call 1 / 2 / 3 / to just before the
 *       byte stores                                       75-76  RELOCDIFF,
 *                                                         4 BYTES SHORTER
 *     INSTRUMENT (device): register int q0 __asm__("r0") feeding z
 *                                                         76  RELOCDIFF, -4 bytes
 *
 * CORRECTION CARRIED FORWARD: the park's own note that its recorded "78" for
 * the two-local spelling "is NOT A DISTANCE: that variant has DIRTY
 * RELOCATIONS" is CONFIRMED, and it survives giving `buf` its proper pointer
 * type -- so the two-variable dimension is genuinely closed, not merely
 * mis-measured.  And the r0 pin still does not reproduce the ROM's shape even
 * on the better base: the pinned figure is WORSE than the pin-free one.
 *
 * DECLINING TO CLOSE at 3.  Remaining cause, NAMED: the reload scratch register
 * for `mov r8, #0` at insn 30 is r2 where the ROM has r0, and the two insns
 * that carry it plus sched2's hoist of `mov r0, #22` past it are the three
 * encodings.  Not a spelling of indices 6-8.
 *
 * ========== BATCH 329: THE PASS-3 QUESTION IS ANSWERED.  IT IS REG_ALLOC_ORDER ==========
 *
 * RE-DERIVED AS FOUND: 3 differing encodings of 85, first differing index 6,
 * counts equal, relocations clean.  The greg-versus-flow2 correction above is
 * reproduced exactly on this body: .18.greg prints
 *     Spilling for insn 30. / Using reg 3 for reload 0
 * and .19.flow2 emits `(insn 219 (set (reg:SI 2 r2) (const_int 0)))` ahead of
 * insn 30.  Printed 3, emitted 2, same insn.
 *
 * THE QUESTION THIS PARK LEFT FOR PASS 3 -- "why is r0 excluded at insn 30 when
 * the parameter save at insn 4 already freed it" -- HAS AN ANSWER, and it is
 * neither account recorded above.  r0 is not excluded because it is live and not
 * because the function takes a parameter.  It is FOURTH IN LINE.
 *
 * (1) order_regs_for_reload (reload1.c:1525-1537) builds, per insn,
 *         bad_spill_regs = fixed_reg_set
 *                        | hardregs (chain->live_throughout)
 *                        | hardregs (chain->dead_or_set)
 *     r0 is in none of those at insn 30.
 * (2) find_reg (reload1.c:1616-1655) then scans regno 0 upward, scoring each
 *     candidate `spill_cost[regno]` -- zero for a register holding no live
 *     pseudo -- and breaks an EQUAL-COST tie with
 *         inv_reg_alloc_order[regno] < inv_reg_alloc_order[best_reg]
 *     config/arm/arm.h:989-995 defines
 *         REG_ALLOC_ORDER = { 3, 2, 1, 0, 12, 14, 4, 5, 6, 7, 8, 10, 9, 11, ... }
 *     so among equally free low registers find_reg takes r3 FIRST, then r2, then
 *     r1, and r0 LAST.  That is the whole reason greg prints 3.
 * (3) The pick sets used_spill_regs_local (reload1.c:1686), which is ORed into
 *     the function-global used_spill_regs (reload1.c:1754).
 * (4) reload1.c:3529-3532 rebuilds spill_regs[] from that global set in
 *     ASCENDING REGISTER ORDER, and allocate_reload_reg (reload1.c:5003-5013)
 *     starts its round robin at last_spill_reg + 1, which is 0 for the first
 *     reload in the function (last_spill_reg is -1 at reload1.c:821).
 *     THE FUNCTION'S FIRST RELOAD THEREFORE GETS THE LOWEST-NUMBERED MEMBER OF
 *     THE GLOBAL SPILL SET.  Ours is {2,3}, so insn 30 emits r2.  If r0 were in
 *     the set it would be spill_regs[0] and insn 30 would emit r0 -- the ROM.
 *     So the park's round-robin model is right and its membership evidence was
 *     the only broken part.
 *
 * CONTROL, and it refutes BOTH accounts recorded above.  The two landed
 * siblings this park cites were dumped with -da:
 *     src/overlays/rom_7eaf28/ovl_314_c_c_c_c_a.c   .18.greg:
 *         Spilling for insn 10.  / Using reg 3 for reload 0
 *         ... Using reg 0 for reload 0   at insn 317 AND at insn 319
 *     src/overlays/rom_7a5214/ovl_314_c_c_c_c_b.c   .18.greg:
 *         Using reg 3, then Using reg 2, then Using reg 0 for reload 0 at insn 131
 * In BOTH, the FIRST reload reserves r3 exactly as ours does, and r0 is reserved
 * only at an insn DEEP IN THE FUNCTION.  Their prologue `mov r0, #0 / mov rHI, r0`
 * is then produced by step (4), not by r0 being free at entry.  So:
 *   - "both are void, so r0 is free at entry and the FIRST reload takes it" is
 *     FALSE as stated -- their first reload takes r3;
 *   - "the condition is register pressure" is RIGHT IN SUBSTANCE and wrong in
 *     scale: what is required is ONE insn at which r3, r2 AND r1 are all in
 *     bad_spill_regs, not a doubly-nested loop.
 *
 * THE ACTION, stated so it can be executed: make SOME insn anywhere in this
 * function need a reload while r1, r2 and r3 are all live across or set at that
 * insn.  Nothing written at indices 6-8 can do it; the lever is elsewhere in the
 * body.  The screen is cheap and needs no objcmp run -- compile with -da and
 * grep the .18.greg dump for a line reading "Using reg 0".  That is a boolean
 * test on exactly the condition, and it costs one compile per candidate.
 *
 * STILL AT 3 pin-free, device-free, flag-free.  Declining to close, with the
 * cause now named to the line and the next test made cheap.
 */
extern unsigned char gScript_881__0200cbe4[];
extern unsigned char *__CreateActor(int a);
extern int __CheckPartyItem(int item);
extern int __CheckItem(int a, int item);
extern void __Actor_SetScript(void *a, void *s);
extern unsigned char *__galloc_iwram(int a, int b);
extern void __gfree(int a);
extern void __LoadItemIcon(int id);
extern void __UploadSpriteGFX(int a, int b, void *p);
extern void __PlaySound(int id);
extern void __Func_808f140(void *a, int b);
extern void __GiveItemTo(int a, int b);
extern void __DeleteActor(void *a);
extern void __MapActor_SetAnim(int a, int b);

int OvlFunc_896_200c260(int item)
{
    unsigned char *act;
    unsigned char *o;
    int who;
    int has;
    int z;
    int g1;
    int m;

    do { } while (0);
    g1 = 0xc1 << 3;
    m = -0x21;
    z = 0;
    act = __CreateActor(0x16);
    who = __CheckPartyItem(0xe0);
    has = __CheckItem(who, 0xe0);
    if (act == 0)
        return who;
    __Actor_SetScript(act, gScript_881__0200cbe4);
    o = *(unsigned char **)(act + 0x50);
    o[0x26] = z;
    o[0x27] = z;
    o[5] &= m;
    o[9] &= 0xf;
    *(int *)(act + 0x28) = 0xa0 << 10;
    *(int *)(act + 0x48) = 0x80 << 7;
    z = (int)__galloc_iwram(0x11, g1);
    __LoadItemIcon(item);
    __UploadSpriteGFX(o[0x1c], 0x80, (unsigned char *)z + (0x80 << 3));
    __gfree(0x11);
    __PlaySound(0x53);
    __Func_808f140(act, 3);
    __Func_8078948(who, has);
    __GiveItemTo(who, item);
    __DeleteActor(act);
    __MapActor_SetAnim(0, 1);
    return who;
}
