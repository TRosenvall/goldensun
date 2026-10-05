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
