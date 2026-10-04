/* OvlFunc_896_200c260 -- 0x0200c260  (asm/overlays/rom_78ef88/ovl_314_c_c_c_c_a.s)
 *
 * NON-MATCHING, 4 of 85 encodings.  MEASURED THIS BATCH, --func AND --whole.
 * PIN COUNT: 0 (tools/shimcount.py reports no shims).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_78ef88/200c260.c \
 *     asm/overlays/rom_78ef88/ovl_314_c_c_c_c_a.s --func OvlFunc_896_200c260
 *
 *   --func  : XX ENCODINGS differ in 4 place(s) (ref 85, ours 85), first at index 5
 *   --whole : OvlFunc_896_200c260  4 of 85 differ (ours 85), first at index 5
 *   relocations clean, no SIZE line, exact length.
 *
 * SPLIT: NONE NEEDED.  tools/datacheck.py is CLEAN and tools/split_s.py says
 * "holds only OvlFunc_896_200c260 and no data; convert it directly".
 *
 * ===== WHAT THE RESIDUE IS, AND IT IS ONE DECISION, NOT FOUR =====
 *
 * All four differing indices are REAL INSTRUCTIONS.  The function has exactly
 * ONE pool word -- index 84, the `.word 0` relocation placeholder for
 * gScript_881__0200cbe4 -- and it MATCHES.  So no rung below is blind here.
 *
 *     idx  REF                | OURS
 *      5   4682 mov sl, r0    | 2200 mov r2, #0
 *      6   2000 mov r0, #0    | 4682 mov sl, r0
 *      7   4680 mov r8, r0    | 2016 mov r0, #22
 *      8   2016 mov r0, #22   | 4690 mov r8, r2
 *
 * THE PARK'S "REGISTER-ROLE ROTATION" FRAMING IS REFUTED for this body.
 * Nothing rotates: 85 instructions against 85, one register wrong, and the
 * three transposed neighbours are a CONSEQUENCE of that one register.  (The
 * rest of the park header -- the one-variable lever, the deleted
 * __Func_8078948 prototype -- reproduces exactly; only the verdict was wrong.)
 *
 * `mov r8, #0` has no Thumb-1 encoding, so RELOAD splits it.  Read out of the
 * dumps (`-da`, production flags):
 *
 *   .17.lreg   (insn 18 (set (reg/v:SI 37) (const_int 0)))          pseudo 37
 *   .18.greg   (insn 206 (set (reg:SI 2 r2) (const_int 0)))   <-- NEW, reload
 *              (insn 18  (set (reg/v:SI 8 r8) (reg:SI 2 r2)))
 *
 * and greg's ORDER IS ALREADY THE ROM'S: insn 4 `mov sl,r0`, insn 206
 * `mov rX,#0`, insn 18 `mov r8,rX`, insn 21 `mov r0,#0x16`.  With rX = r0 the
 * r0 dependence pins all four insns in that order and sched2 cannot move them;
 * with rX = r2 the constant load is independent of the parameter save and
 * sched2 hoists it to the top.  CONFIRMED by flag: with -fno-schedule-insns2
 * the first difference MOVES to index 6 and becomes exactly the register
 * (ref `mov r0,#0` / ours `mov r2,#0`) -- the order is then right and only the
 * register is wrong.  (23 of 85 overall under that flag; a per-flag number,
 * not a production figure, and the flag is rejected below.)
 *
 * > So this is one reload-scratch-register choice, and it is the documented
 * > class: docs/elevation.md, "A RELOAD SCRATCH REGISTER IS ROUND ROBIN OVER A
 * > SET THE SOURCE CONTROLS".
 *
 * ===== THE DISCRIMINATOR THAT DOC ASKS FOR, MEASURED =====
 *
 * Count `Using reg N` in .18.greg.  This body: reg 3 once, reg 2 twice -- the
 * SPILL SET IS {r2, r3}, and r0 IS NOT IN IT.  Round robin over a two-element
 * set cannot ever emit the ROM's `mov r0, #0`, at any site, under any spelling
 * of the differing instruction.  Our fresh picks in stream order are
 * r2, r3, r2, r3, r3, r3 -- exactly spill_regs[i mod 2] over [2,3], so the
 * round-robin model is confirmed on this function.
 *
 * The ten MATCHING functions in this tree that emit `mov r0,#imm / mov rHI,r0`
 * all have r0 in a MULTI-register spill set where r0 is picked only 1-3 times
 * against dozens of r2/r3 picks (sets measured: {0,2,3} x4, {0,3}, {0,1,2},
 * {0,1,2,3} x3, {0,1,2,3,5}).  r0 is LAST in REG_ALLOC_ORDER {3,2,1,0,...}, so
 * find_reg reaches it only where r3, r2 and r1 are all unavailable -- i.e. only
 * under pressure this function does not have.  Its single multi-live site is a
 * 3-argument call, and none of its four reload sites can be made to need a
 * fourth low register.
 *
 * ONE MATCHING FUNCTION EMITS THE ROM'S EXACT THREE-INSTRUCTION SHAPE
 * (`mov rHI,r0 / mov r0,#imm / mov rHI,r0`): CheckLure in
 * src/rom_77000/rom_77320_a_c_a_b.c.  Its spill set is {0,2,3} and its FIRST
 * reload (insn 253, `r0 = 0` for `i = 0` into r8) takes r0.  r0 enters its set
 * from a reload deep inside a doubly-nested loop where r1, r2 and r3 are all
 * live.  That is the condition, and it is a function-shape condition, not a
 * spelling.
 *
 * ===== MEASURED, ALL OF IT =====
 *
 * tools/crossfire.py, 6 edits at depth 3 (32 subsets): FLAT.  Fifteen subsets
 * exactly inert at 4 (candidate prerequisites), nothing better, and every
 * mover a regression:
 *     decl order z first                                          4  inert
 *     g1 literal at the use                                       4  inert
 *     named local for the gfx selector                            4  inert
 *     named local for the +0x400 offset                           4  inert
 *     all ten pairs and triples of those four                      4  inert
 *     named pointer for the two byte stores                      61  RELOC
 *     z = 0 moved after the first call                           76  83 insns
 * Additional singles probed with the spill set read out each time:
 *     two locals (z + buf), buf-first or z-first        78, RELOC DIRTY, set {3}
 *     drop m (literal ~0x21)                            59, RELOC DIRTY
 *     drop m and g1                                     59, RELOC DIRTY
 *     z as unsigned char * instead of int                4  inert, set {2,3}
 *     actor/sprite as real structs (declaration lever)   4  inert, set {2,3}
 *     a seventh never-read parameter (device)            4  inert, set {2,3}
 *     unsigned short parameter                          79, 89 insns
 *     register int q0 __asm__("r0") feeding z (INSTRUMENT) 76, 83 insns, set {3}
 *
 * TWO CORRECTIONS TO THE RECORD.  (1) The park's "78" for the two-local
 * spelling is NOT A DISTANCE: that variant has DIRTY RELOCATIONS, which the
 * original measurement did not report.  (2) An r0 PIN on the zero does not even
 * reproduce the ROM's shape -- it regresses to 83 instructions, because the pin
 * forces a copy and gcc then rematerialises.  So this park is not one pin away
 * either; the pinned figure is WORSE than the pin-free one.
 *
 * DECLINING TO CLOSE, with the map replaced.  The residue is one reload scratch
 * pick; the spill set is {r2,r3} and r0 cannot enter it from any dimension
 * swept here.  The next move is NOT another spelling of indices 5-8 -- it is a
 * construct that forces a reload where r1, r2 and r3 are all busy, and nothing
 * in this function's shape supplies one.
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
