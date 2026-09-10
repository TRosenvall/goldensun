// fakematch
/* ovl_314_c_c_c_a_a_b.c  --  OvlFunc_925_200856c  --  0x0200856c
 *
 *   OK OvlFunc_925_200856c -- 804 bytes, 300 encodings and 94 relocations identical
 *
 * Re-measured SIX times (three on the 25-pin base, three on the shipped
 * 15-pin minimum).  objcmp prints no `(built with: ...)` line; tryc's
 * makefile_flags() on both the pre-split path src/overlays/rom_7b0400/
 * ovl_314_c_c_c_a_a.c and the post-split src/.../ovl_314_c_c_c_a_a_b.c is the
 * EMPTY SET -- `grep -n rom_7b0400 Makefile` returns NOTHING AT ALL, so no
 * wildcard and no explicit rule reach this directory and the generic
 * `asm/%.o: src/%.c` applies at the tree default -O2 -mthumb -mthumb-interwork
 * -fcall-used-r4.
 *
 * LANDING NEEDS A SPLIT.  `tools/asmfacts.py` on
 * asm/overlays/rom_7b0400/ovl_314_c_c_c_a_a.s says "2 functions  split first":
 * OvlFunc_925_200835c then OvlFunc_925_200856c, and only the second is solved
 * here.  Run
 *
 *     python3 tools/split_s.py asm/overlays/rom_7b0400/ovl_314_c_c_c_a_a.s \
 *             OvlFunc_925_200856c
 *
 * which leaves 200835c in ovl_314_c_c_c_a_a_a.s and the target in
 * ovl_314_c_c_c_a_a_b.s, and rewrites overlays/rom_7b0400/overlay.ld:27 --
 * the ONE line in the tree that names this .o on its full path -- into the two
 * lines in the same order.  VERIFY `make compare` IS GREEN AFTER THE SPLIT AND
 * BEFORE THIS .c LANDS.  The .s carries NO `.section`, `.data`, `.bss`,
 * `.lcomm`, `.word` or `.byte` line and NO `.global`; all six `.L` labels
 * (.L380 .L416 .L46c .L4da .L52e .L538 in 200835c, .L804 .L81c .L834 here) are
 * branch targets local to their own function, so the cut exports nothing.
 * FAKEMATCH: the name goes in fakematch.txt (register-pin idiom).
 *
 * ===================================================================
 * THE FINDING: A RELOAD SCRATCH REGISTER IS CHOSEN BY ROUND ROBIN OVER A SET
 * THE SOURCE CONTROLS, AND A NAMED-LOCAL STORE BLOCK EMPTIES THAT SET.
 * ===================================================================
 *
 * This function is `push {lr}` only, hi = 0, hiv = 0, 29 shared symbols, and
 * plain C opens `push {r5, r6, lr}` plus `mov r6, sl / mov r5, r8 / push` --
 * SEVEN callee-saved registers against the ROM's ZERO, 287 of 300 differing,
 * 20 bytes long.  That is the recorded commoning tell at its loudest, and
 * pinning all 25 sites that carry an argument fixes the push mask in ONE STEP
 * (287 -> 246, and `push {lr}` becomes encoding 0).  Two spelling fixes then
 * took it to 4:
 *
 *   - the `__Func_8093554()` result needs its OWN local (or no local at all);
 *     reusing the `r` that the three __MapActor_TravelTo blocks use costs a
 *     whole `adds r2, r0, #0` copy.  246 -> 10.
 *   - the iwram store block had to move the offset build in front of the
 *     pointer load.  10 -> 4.
 *
 * AND THEN IT STUCK AT 4 THROUGH 182 MEASURED PERTURBATIONS.  Ten variants of
 * the three TravelTo blocks (one local, three locals, named short temps, a
 * `short *` with q[5]/q[9], named offset constants, the two loads in the other
 * order, a hard pin on r1/r2, a hard pin on r0/r1/r2), a DROP-ONE pass over
 * all 25 pins, a WIDTH pass and a FULL ORDER-PERMUTATION pass over every
 * pinned site: 70 of those tie at 4 and NOT ONE goes below.  The residue was
 * always the same four encodings --
 *
 *     ROM   movs r2, #10 / ldrsh r1, [r0, r2]      (2nd and 3rd block)
 *     ours  movs r3, #10 / ldrsh r1, [r0, r3]
 *
 * -- with size, encoding count and all 94 relocations already identical.
 *
 * THE `ldrsh` OFFSET IS NOT AN ALLOCATED LOCAL, IT IS A RELOAD SCRATCH.  Thumb
 * `LDRSH` has no immediate-offset form, so `*(short *)(r + 0xa)` matches
 * `*thumb_extendhisi2_insn`, whose pattern carries `(clobber (match_scratch:SI
 * "=&l"))`.  Nothing in cse, loop, regmove or local-alloc picks that register;
 * RELOAD does, and reload's rule is this (gcc-2.96 reload1.c):
 *
 *   order_regs_for_reload()  ZEROES spill_cost[] for every insn (1527) and
 *       counts only pseudos LIVE AT THAT INSN (1511).  Two free low registers
 *       therefore TIE AT ZERO.
 *   find_reg()  breaks the tie with inv_reg_alloc_order (1629-1636).  ARM's
 *       REG_ALLOC_ORDER starts { 3, 2, 1, 0, ... }, so R3 WINS EVERY TIE.
 *   the winners become used_spill_regs, and spill_regs[] is that set SORTED
 *       ASCENDING (3527-3532).
 *   allocate_reload_reg()  then assigns the FINAL register per insn by
 *       ROUND ROBIN from last_spill_reg over spill_regs[] (5003-5013), a
 *       file-static that advances across the whole function.
 *
 * So when every reload in a function ties, used_spill_regs is the SINGLETON
 * {r3} and the round robin has one element: EVERY SCRATCH IN THE FUNCTION IS
 * r3.  The ROM's six are r3, r3, r2, r3, r2, r3 -- which is EXACTLY what the
 * round robin emits over the two-element set {r2, r3}.  The defect was never
 * "this insn wants r2".  It was that R2 WAS NOT IN THE SET.
 *
 * THE LEVER IS THE STORE BLOCK, AND IT IS 500 BYTES AWAY FROM THE DEFECT.
 * Written the neighbour's way through two named `unsigned int` locals --
 *
 *     r2 = 0xe0; r2 <<= 1; r3 = (unsigned int)iwram_3001ebc;
 *     r3 += r2;  r2 -= 0xc0;  *(int *)r3 = r2;
 *
 * -- the address arithmetic is ORDINARY INSNS and needs no reload: six reloads
 * in the function, `Using reg 3` six times, 4 differing.  Written as ONE
 * EXPRESSION --
 *
 *     *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x100;
 *
 * -- the 448-byte offset is out of range for thumb `str` (imm5 x 4, 0..124), so
 * reload has to materialise the address.  That is a SEVENTH reload, it is the
 * FIRST one in the function, r3 is busy there, find_reg answers `Using reg 2`,
 * and r2 joins spill_regs.  The six ldrsh scratches 500 bytes downstream
 * become r3, r3, r2, r3, r2, r3 and the function is EXACT.  The diff of the
 * two `.18.greg` dumps is one line:
 *
 *     named locals   6 x "Using reg 3 for reload 0"
 *     one expression 1 x "Using reg 2" (insn 150) + 6 x "Using reg 3"
 *
 * This is a NEW blocker class AND its cure, and both are mechanical:
 *
 *   > WHEN THE ONLY RESIDUE IS A RELOAD SCRATCH REGISTER, COUNT THE
 *   > `Using reg N` LINES IN `.18.greg`.  IF THEY ARE ALL ONE REGISTER WHILE
 *   > THE REFERENCE ALTERNATES TWO, THE SPILL-REGISTER SET IS TOO NARROW, AND
 *   > THE FIX IS TO ADD A RELOAD SOMEWHERE ELSE IN THE FUNCTION -- NOT TO
 *   > TOUCH THE SITE THAT IS WRONG.  A POINTER-PLUS-LARGE-OFFSET STORE OR LOAD
 *   > WRITTEN AS ONE EXPRESSION SUPPLIES ONE; THE SAME ADDRESS BUILT THROUGH
 *   > NAMED LOCALS SUPPLIES NONE.
 *
 * It also explains why the recorded "declaration order is inert" and the whole
 * pin ladder could not reach this: the defect is not in the argument window at
 * all, and no amount of work at the differing site can move it.
 *
 * -fno-schedule-insns2 REGRESSES 4 -> 94, so sched2 was already right and the
 * alias axis was correctly ruled out before any of this.  -fno-gcse and
 * -fno-cse-follow-jumps are both inert at 4.  No flag ships.
 *
 * PIN MINIMISATION: 25 candidates, 15 required, and forward and reverse
 * drop-to-fixpoint converge on the SAME FIFTEEN, so the set is unique here.
 * The ten that fall are 3, 8, 9, 14, 16, 18, 19, 31, 32 and 33 -- every one a
 * site whose only non-r0 argument is a bare `mov #imm8` or a single pooled r0,
 * which is the recorded inert-site rule holding 10 for 10.
 *
 * MEASURED WORSE / INERT (against 300 encodings / 804 bytes):
 *
 *   spelling                                                differing
 *   -----------------------------------------------------  ---------
 *   plain C, no pins                                             287  (+20 bytes)
 *   25 pins, store via locals, `r` reused at 8093554             246  (+20 bytes)
 *   25 pins, own local for 8093554, store ptr-first               10
 *   25 pins, own local, store offset-first                         4
 *   25 pins, store as one expression                               0
 *   store as `(0xe0 << 1) - 0xc0` instead of 0x100                 0  (ties)
 *   store with a `unsigned char *p` local for the address          4
 *   store written value-last (`*p = r2 - 0xc0`)                   11
 *   TravelTo with a hard r0/r1/r2 pin                             74  (+4 bytes)
 *   TravelTo with the two loads in the other order                12
 *   TravelTo: 3 locals / short temps / short* / named offsets      4  (all inert)
 *   drop-one over 25 pins                                  4 x10, 6 x4, 7 x2, 41..266
 *   width pass + full order-permutation pass (157 builds)    4 x70, 6, 7, 8 -- no 0
 *
 * Harness: scratch_elev/b257/a2 -- gen.py (site table, pin/width/order specs,
 * K1 store-block and K2 8093554 knobs), run.sh, cmp2.py, dis.py, sweep.py,
 * sweep2.py, minimise.py, NOTES.md.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_800fe9c(void);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
extern void __Func_8078144(void);
extern void __Func_8091200(int a, int b);
extern void __Func_8091220(int a, int b);
extern void __Func_8091254(int a);
extern void __Func_809259c(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern unsigned char *__Func_8093554(void);
extern unsigned char *iwram_3001ebc;

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")


void OvlFunc_925_200856c(void)
{
    unsigned char *r;

    __CutsceneStart();
    __Func_8078144();
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(1), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(2), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(3), 0);
    { PIN4; q0 = 0x98 << 17; q1 = -1; q2 = 0xf0 << 15; q3 = 0;
      __Func_80933f8(q0, q1, q2, q3); }
    __WaitFrames(1);
    __Func_800fe9c();
    __WaitFrames(1);
    __PlaySound(0x8d);
    { PIN3; q0 = 0xa0 << 11; q1 = 0xa0 << 11; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __PlaySound(0x121);
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x100;
    __MapTransitionIn();
    __WaitMapTransition();
    __Func_8012350();
    __CutsceneWait(0x1e);
    __Func_8093554()[0x55] = 0;
    { PIN2; q0 = 0xcccc; q1 = 0x1999;
      __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0x80 << 18; q1 = 0xffe80000; q2 = 0xa0 << 16; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    { PIN2; q0 = 0x80 << 9; q1 = 0;
      __Func_8091220(q0, q1); }
    __Func_8091200(0x10005, 0);
    __Func_8091254(0x32);
    __CutsceneWait(0x32);
    __Func_8091200(0x7fff, 0);
    __Func_8091254(0x1e);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0; q1 = 0xfc << 17; q2 = 0xa8 << 16;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x84 << 18; q2 = 0x90 << 16;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xf4 << 17; q2 = 0x90 << 16;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x80 << 18; q2 = 0x98 << 16;
      __MapActor_SetPos(q0, q1, q2); }
    __MapActor_SetAnim(0, 0x13);
    __MapActor_SetAnim(1, 0x13);
    __MapActor_SetAnim(2, 0x13);
    __MapActor_SetAnim(3, 0x13);
    __CutsceneWait(0xa);
    __Func_8091200(0x80 << 9, 0);
    __Func_8091254(0x1e);
    __CutsceneWait(0x1e);
    __CutsceneWait(0x50);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 1);
    __MapActor_SetAnim(0, 1);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(0, 4);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_809259c(0, 2);
    __CutsceneWait(0x3c);
    __Actor_SetSpriteFlags(__MapActor_GetActor(1), 1);
    __MapActor_SetAnim(1, 1);
    __CutsceneWait(0x14);
    __Func_8092adc(1, 0x80 << 6, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 1; q1 = 0xc0 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __Func_8092adc(1, 0, 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(2), 1);
    __MapActor_SetAnim(2, 1);
    __Func_8092adc(1, 0xc0 << 7, 0);
    __CutsceneWait(0x28);
    __Actor_SetSpriteFlags(__MapActor_GetActor(3), 1);
    __MapActor_SetAnim(3, 1);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x14);
    { PIN3; q0 = 1; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(1, 2);
    __MapActor_SetAnim(2, 2);
    __MapActor_SetAnim(3, 2);
    r = __MapActor_GetActor(0);
    if (r != 0)
        __MapActor_TravelTo(1, *(short *)(r + 0xa), *(short *)(r + 0x12));
    r = __MapActor_GetActor(0);
    if (r != 0)
        __MapActor_TravelTo(2, *(short *)(r + 0xa), *(short *)(r + 0x12));
    r = __MapActor_GetActor(0);
    if (r != 0)
        __MapActor_TravelTo(3, *(short *)(r + 0xa), *(short *)(r + 0x12));
    __MapActor_WaitMovement(3);
    __MapActor_SetPos(3, 0, 0);
    __MapActor_WaitMovement(2);
    __MapActor_SetPos(2, 0, 0);
    __MapActor_WaitMovement(1);
    __MapActor_SetPos(1, 0, 0);
    __CutsceneEnd();
}
