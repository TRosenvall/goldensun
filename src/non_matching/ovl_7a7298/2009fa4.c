/* OvlFunc_921_2009fa4 -- NON-MATCHING, 1 ENCODING OF 199.  Size identical (452
 * both), relocations identical, 201 instructions.  THE CLOSEST PARK IN THE TREE,
 * and as of batch 315 ITS SISTER src/non_matching/ovl_7a8c8c/200a094.c IS ALSO
 * AT 1 WITH THE IDENTICAL RESIDUE -- THEY ARE THE SAME FUNCTION IN TWO OVERLAYS
 * AND MUST BE WORKED AS ONE.  Whatever lands one lands both.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7a7298/2009fa4.c \
 *     asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_c_c_c.s
 *   XX ENCODINGS differ in 1 place(s) (ref 199, ours 199)
 *      first at index 142: ref aa02  ours 1c2a
 * The reference holds ONE function, so no --func is needed and no TEXT split.
 *
 * THE RESIDUE, one instruction at the loop latch's second __vec3_translate:
 *     ref   add  r2, sp, #8      (aa02)
 *     ours  adds r2, r5, #0      (1c2a)
 *
 * `.global .L2430` IS STILL A BUILD PREREQUISITE (the one-line, zero-byte export
 * in that .s's .data section; batch 314 confirmed it does not change the figure).
 * SHIMS: 6 register pins (tools/shimcount.py); a pinned landing needs a
 * fakematch.txt row.
 *
 * NOT POOL-INFLATED, CHECKED EXPLICITLY IN BATCH 315.  Every differing index was
 * listed, not just the first.  Production (--func-equivalent) has exactly ONE,
 * `aa02` vs `1c2a`, a real instruction.  A WHOLE-OBJECT sweep reads 2 because it
 * also sees `idx 194 ref .word 0x40 / ours .word 0x00` -- THE .L2430 POOL WORD.
 * That is why every sweep figure in this header prints one higher than
 * production, and it is the brief's pool-word trap caught in the act.
 *
 * ============== THE BLOCKER, RE-DERIVED IN BATCH 315.  IT IS TWO BLOCKERS. ==============
 *
 * THE PARK'S "gcse (PRE)" IS RIGHT AT THE DUMP LEVEL and the aggregate -fno-gcse
 * figure (71 of 199, +4 bytes) does NOT refute it -- gcse_main runs cprop and
 * PRE in one loop, so the flag removes a pass group the whole function depends
 * on.  THE DISCRIMINATING PROBE IS THE INSTRUCTION, NOT THE FIGURE.  Read that
 * way, `-fno-gcse` emits at the step site
 *       add r5,sp,#8 / mov r0,#128 / ldr r1,[sp,#4] / mov r2,r5 / lsl r0,#13
 * -- the `(plus sfp -12)` SURVIVES, so gcse is confirmed, AND A SECOND BLOCKER
 * IS EXPOSED BEHIND IT.
 *
 *   STAGE 1, cse1 -- SOURCE-DEFEATABLE.  The step block has two predecessors, so
 *   cse_main starts a fresh path there and the address temp is the FIRST
 *   occurrence of `(plus sfp -12)` in it; cse1 then rewrites the block's OWN
 *   post-call v[] MEM addresses to use that temp (visible at .03.cse: insns
 *   376/380 read `(mem (reg 118))`).  Its live range therefore crosses the call,
 *   it must take a CALL-SAVED register, and the r2 fill is forced to be a copy.
 *   Pseudos are NOT invalidated by cse's invalidate_for_call, which is why the
 *   intervening `bl` does not break this.
 *
 *   STAGE 2, gcse -- NOT YET DEFEATED.  With stage 1 removed, PRE still commons
 *   the step occurrence with the dominating one; its reaching_reg then has TWO
 *   uses, becomes a real quantity, and gets SPILLED -> `ldr r2, [sp]`.
 *
 * THE LANDING MECHANISM IS NOT RELOAD, IT IS ONE PASS EARLIER.  local-alloc.c
 * update_equiv_regs substitutes a single-set/single-use pseudo's REG_EQUIV into
 * its one use and deletes the set.  cse1 already attaches
 * `REG_EQUAL (plus (reg 25 sfp) (const_int -12))` to the r2 fill (.03.cse insn
 * 352), so once the temp is single-use the plus reaches the hard-r2 fill insn
 * and `add r2, sp, #8` is emitted.  THIS IS WHY EVERY RELOAD-LEVEL PROBE WAS
 * INERT, and it is the batch-315 brief's "blocker one pass earlier" in its
 * clearest form.
 *
 * ================ PROOF THAT THE ROM'S INSTRUCTION IS REACHABLE ================
 * docs/repro-2009fa4-vp/A1_vp_r5.c routes EVERY v[] element access through a
 * pointer pinned to the ROM's own r5 (`register int *vp __asm__("r5");
 * vp = v;` set once after __GetFieldActor) and leaves both __vec3_translate
 * calls passing the array `v`.  Compiled -fno-gcse it emits
 *       .L8:  mov r0,#128 / add r2, sp, #8 / ldr r1,[sp,#4] / lsl r0,#13 / bl
 * -- ENCODING aa02, THE ROM'S INSTRUCTION, with only `add r2` and `ldr r1`
 * swapped against the ROM.  A1 is NOT a landing (186 of 199 and +8 bytes in
 * production, because the r5 pin moves the whole spill map, and `ldr r2,[sp]`
 * with gcse on).  IT IS THE EXISTENCE PROOF, and it also explains why this
 * park's recorded `int *vp` negative (74 of 199) was MISREAD: the shape was
 * right and was measured with the second blocker still in place.
 *
 * ========== WHY A HARD-REGISTER DEST CANNOT BE USED TO ESCAPE gcse ==========
 * Three facts in the shipped compiler, and together they kill a whole family of
 * attempts:
 *   gcse.c:1855-1866  hash_scan_set enters a set in the expression hash table
 *                     only when its REG dest has `regno >= FIRST_PSEUDO_REGISTER`.
 *                     A HARD-REGISTER DEST IS NEVER A PRE CANDIDATE.
 *   expr.c:5896-5898  at -O2 (`! cse_not_expected`) expand_expr DISCARDS any
 *                     `target` that is not a pseudo, so an expression can never
 *                     be expanded straight into a hard register.
 *   expr.c ADDR_EXPR  `op0 = force_operand (XEXP (op0, 0), target)` is
 *                     unconditional, so `&local` as a call argument ALWAYS lands
 *                     in a pseudo whatever the pin says.
 * MEASURED, exactly as those predict -- all INERT:
 *   `register int *q2 __asm__("r2"); q2 = v;`                      INERT
 *   `register int *q2 __asm__("r2"); q2 = &v[0];`                  INERT
 *   `register int q2 __asm__("r2");  q2 = (int)&v[0];`             INERT
 *
 * ======= FLAG PROBES READ AT THE INSTRUCTION, NOT THE FIGURE (all `mov r2,r5`) =======
 *   production / -fno-gcse / -Os / -fno-gcse -fno-cse-follow-jumps /
 *   -fno-gcse -fno-rerun-cse-after-loop.
 * -Os is the interesting one: gcse_main swaps PRE for one_classic_gcse_pass when
 * optimize_size, and the instruction is UNCHANGED -- so this is global CSE
 * generally, not PRE specifically, and no flag rung exists.
 * `-fno-cse-follow-jumps` remains INERT.
 *
 * ============== BATCH 315 NEGATIVES, RE-MEASURED FROM THIS BASELINE ==============
 * (sweep figures are whole-object and print one higher; BASE = 2 == production 1)
 *   `__asm__ volatile ("")` before the q2 statement                 2  INERT
 *   `{ int *t2 = v; q2 = (int)t2; }`                                2  INERT
 *   `__asm__ volatile ("")` after `q0 <<= 13`                       3  WORSE
 *   `int *vp` unpinned for every element access                     72  WORSE
 *   `register int *vp __asm__("r5")` for every element access      186  WORSE, +8
 *   the same, with the step call passing `vp`                      186  WORSE
 * ALL NINE `extern void` callees swept to `extern int`:
 *   OvlFunc_921_2009f24, __Actor_WaitMovement, __CutsceneEnd,
 *   __CutsceneStart, __WaitFrames                                   2  INERT
 *   __vec3_translate 4, __Actor_SetAnim 4, __Actor_SetAnimSpeed 4,
 *   __Actor_TravelTo 9                                              WORSE
 * So the batch-315 callee-return-type lever does not pay here either.
 * THE BRIEF'S ALIAS-SET DEPENDENT-COUNT LEVER STILL CANNOT REACH THIS PARK:
 * the residue is not a scheduling tie.  The buffer/pointer split (75-76 of 199,
 * first diff at index 11) stands as recorded -- do not re-derive it.
 *
 * NEXT, AND IT IS NOW A SHARP QUESTION RATHER THAN "nothing found".  Two things
 * must hold at once:
 *   (a) remove stage 1 WITHOUT moving the frame -- the r5 pin is what costs 186,
 *       because it reorders the spill slots; a shape that takes the step block's
 *       v[] MEMs off the `(plus sfp -12)` expression while leaving the slot map
 *       alone is the open half that IS reachable;
 *   (b) a dominating occurrence gcse will not hash.  Hard to see how: the ROM
 *       itself computes `add r5, sp, #8` in a dominating block, and expand is
 *       forced to put that address in a pseudo because Thumb rejects a
 *       negative-offset `(plus vsv -12)` as a MEM address, so the set is always
 *       hash_scan_set-eligible.  IF (b) IS GENUINELY IMPOSSIBLE THIS PARK CLOSES
 *       -- but say so with the citation, not with a count of attempts.
 *
 * WHAT CLOSED THE OTHER 198, unchanged and all still load-bearing:
 *  1. `if (dir << 16 == (int)0xffff0000)` written out as the shift -- worth 49 of
 *     55, and the other 48 were eleven r2/r3 scratch permutations that were
 *     symptoms.
 *  2. Explicit labels and gotos for the ROM's block order -- worth 27.
 *  3. `register int lim __asm__("r11")` for the 0x80000 threshold -- fixed the
 *     frame size, 101 -> 82.
 *  4. The pooled halfword zero and the byte-flag OR, both via r3 carriers.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned int gKeyHeld;
extern short L2430[] __asm__(".L2430");

extern unsigned char *__GetFieldActor(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern int __Func_8012038(int a, int b, int c);
extern int __Func_8011f54(int a, int b, int c);
extern void __vec3_translate(int len, int dir, int *v);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __Actor_SetAnimSpeed(unsigned char *a, int n);
extern void __Actor_WaitMovement(unsigned char *a);
extern void OvlFunc_921_2009f24(void);

void OvlFunc_921_2009fa4(void)
{
    unsigned char *a;
    unsigned char *b22;
    unsigned char *f;
    int v[3];
    unsigned int base;
    unsigned int off;
    int dir;
    int first;
    int t;
    int px;
    int pz;
    register int lim __asm__("r11");

    base = (unsigned int)&gState;
    off = 0xfa;
    off <<= 1;
    base += off;
    a = __GetFieldActor(*(int *)base);
top:
    dir = L2430[(gKeyHeld >> 4) & 0xf];
    if (dir << 16 == (int)0xffff0000)
        return;
    __CutsceneStart();
    lim = 0x80 << 12;
    v[0] = (*(int *)(a + 8) & 0xfff00000) + lim;
    v[1] = *(int *)(a + 0xc);
    v[2] = (*(int *)(a + 0x10) & 0xfff00000) + lim;
    pz = v[2];
    px = v[0];
    b22 = a + 0x22;
    first = __Func_8012038(*b22, px, pz);
    __vec3_translate(0x80 << 13, dir, v);
    t = __Func_8012038(*b22, v[0], v[2]);
    if (t == 0xff)
        goto face;
    if (__Func_8011f54(*b22, v[0], v[2]) - *(int *)(a + 0xc) > lim)
        goto face;
    v[0] = px;
    v[2] = pz;
    *(int *)(a + 0x30) = 0x80 << 10;
    *(int *)(a + 0x34) = 0x1999;
    { register int h3 __asm__("r3"); h3 = 0; *(short *)(a + 0x64) = h3; }
    __Actor_TravelTo(a, px, *(int *)(a + 0xc), pz);
    __Actor_SetAnim(a, 2);
    __Actor_SetAnimSpeed(a, 0x30);
    __Actor_WaitMovement(a);
    *(int *)(a + 0x6c) = (int)OvlFunc_921_2009f24;
    goto step;
face:
    *(short *)(a + 6) = dir;
    goto tail;
walk:
    if (__Func_8011f54(*b22, v[0], v[2]) - *(int *)(a + 0xc) > (0x80 << 12))
        goto settle;
    px = v[0];
    pz = v[2];
    *(int *)(a + 0x30) = 0x80 << 10;
    *(int *)(a + 0x34) = 0x1999;
    __Actor_TravelTo(a, v[0], v[1], v[2]);
    __Actor_WaitMovement(a);
    if (t != first)
        goto stop;
step:
    { register int q0 __asm__("r0"); register int q1 __asm__("r1");
      register int q2 __asm__("r2");
      q2 = (int)v; q0 = 0x80; q1 = dir; q0 <<= 13;
      __vec3_translate(q0, q1, (int *)q2); }
    t = __Func_8012038(*b22, v[0], v[2]);
    if (t != 0xff)
        goto walk;
settle:
    *(int *)(a + 0x30) = 0x80 << 10;
    *(int *)(a + 0x34) = 0x80 << 9;
    __Actor_TravelTo(a, px, *(int *)(a + 0xc), pz);
    __Actor_WaitMovement(a);
    __WaitFrames(2);
    goto top;
stop:
    *(int *)(a + 0x6c) = 0;
    f = a + 0x5a;
    { register int m3 __asm__("r3"); m3 = 1; m3 |= *f; *f = m3; }
    *(int *)(a + 0x34) = 0x80 << 7;
tail:
    __WaitFrames(0xa);
    __CutsceneEnd();
}
