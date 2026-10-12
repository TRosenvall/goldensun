/* OvlFunc_881_200a4a8 -- NON-MATCHING, 14 differing STREAM encodings of 270.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch_elev/b258/rich/final/ovl_30_c_a_c_c_a_c_a_a_a_c_c.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 270 / ours 270 stream encodings; 187/187 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_77a7c8/200a4a8.c \
 *     asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_c_c.s --func OvlFunc_881_200a4a8
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
// fakematch
/* OvlFunc_881_200a4a8  --  0x0200a4a8   ** PARKED at 14 of 270 encodings **
 *   [asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_c_c.s, the ONLY function
 *    in that .s -- one `.thumb_func_start`, no .section/.data/.bss/.word, so
 *    tools/asmfacts.py says WHOLE / convert directly and no split is needed.]
 *
 * VERDICT, re-measured three times on this exact file (and three more on the
 * byte-identical `_CONST_2` spelling):
 *
 *   XX ENCODINGS differ in 14 place(s) (ref 270, ours 270)
 *      first at index 173: ref 2201  ours 2301
 *   XX RELOCATIONS differ   <- ONE ours-only entry, nothing else:
 *        ('000002bc', 'R_ARM_ABS32', '_AREA_02')
 *
 * SIZE IS EXACT (704 bytes), the encoding COUNT is exact (270), and every one
 * of the ref's 67 relocations is present at the SAME OFFSET with the same name.
 * The only relocation difference is the documented phantom for a linker symbol
 * standing where the disassembly shows a resolved literal -- see
 * src/overlays/rom_77a7c8/ovl_30_c_c_a_c_a.c, which landed with exactly this
 * shape for `_CONST_2`.  `make compare` is the authority on that one word.
 *
 * LANDING (nothing needs changing):
 *   target   src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_c_c.c
 *   ld line  overlays/rom_77a7c8/overlay.ld:52 already reads
 *            `asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_c_c.o(.text)`
 *            and the cross-dir rule `asm/%.o: src/%.c` builds it from src/.
 *            The line stays VERBATIM.  That basename is UNIQUE in the tree.
 *   flags    tryc.makefile_flags() = set(); the only rom_77a7c8 rule in the
 *            Makefile names ovl_30_c_a_c_c_a_c_c_c_c.o literally, so the tree
 *            default -O2 -mthumb -mthumb-interwork -fcall-used-r4 applies and
 *            objcmp prints no `(built with: ...)` line.
 *   symbols  `.L679c` is a 4-byte `.lcomm` DEFINED in
 *            asm/overlays/rom_77a7c8/ovl_30_c_c_c_c_c_c.s and `.global` there,
 *            so this file only CONSUMES it -- nothing to export.  The
 *            `extern int L679c __asm__(".L679c")` idiom is already used by two
 *            landed file-mates (ovl_30_c_a_c_c_a_c_a_b.c and
 *            ovl_30_c_a_c_c_a_c_a_a_c_b.c).  gScript_881__0200cf20 likewise
 *            lives in ovl_30_c_c_c_c_c_c.s; gState and _AREA_02 are absolutes.
 *
 * ============================ WHAT CLOSED IT ============================
 *
 * | step                                                     | differing |
 * |----------------------------------------------------------|-----------|
 * | plain transcription, no pins                              263  (+16) |
 * | + 35 pin blocks in the ROM's own fill order               165  (+12) |
 * | + the gState tail rewritten (see below) and `m = 1`        ... |
 * |   moved after its __MapActor_GetActor call                  33  (=)  |
 * | + `__asm__ volatile ("" : "+r" (n))` on the 0xfe mask       19  (=)  |
 * | + that barrier moved out of the first mask site             14  (=)  |
 *
 * THE POOL SPLIT WAS A LENGTH SYMPTOM, NOT A POOL PROBLEM.  The 165-differing
 * candidate dumped its literal pool at the `b .L25de` barrier in the middle of
 * the function and a second time before the epilogue, where the ROM has ONE
 * pool at the end.  Nothing about the pool was touched: fixing the gState tail
 * (+3 instructions) and dropping a redundant pool word took the function from
 * 716 bytes to 704 and the split vanished on its own, taking 132 shifted
 * `ldr rN, [pc, #imm]` encodings with it.  NEW and cheap to check: a candidate
 * with a mid-function pool dump the ROM does not have is first of all TOO LONG
 * -- gcc dumps at the last barrier it can reach, so a dozen extra bytes past
 * the ROM's own pool distance is enough to force it.  Do not reach for a
 * control-flow lever; measure the length.
 *
 * THE gState TAIL NEEDED THREE SEPARATE THINGS, and each is a recorded rule
 * firing on one statement pair:
 *   1. `unsigned int g = (unsigned int)gState;` with LITERAL offsets
 *      (`g + (0xe2 << 1)`) is what produces the ROM's `mov/lsl/add rD, rB, rO`
 *      and the REBUILT second offset.  A named offset variable reassigned
 *      (`o = 0xe2; o <<= 1; ... o = 0xe3; o <<= 1;`) collapses to
 *      `strh rV, [rB, rO]` + `add rO, #2` -- the register-offset addressing
 *      mode and a use_related_value, two defects from one spelling.
 *   2. `*(short *)(...) = 0x4e` as a LITERAL is `ldrh r3, .L8+4` against a
 *      `.word 78`: gcc pools a HImode constant.  `int k = 0x4e;` then storing
 *      `k` gives the ROM's `mov r3, #0x4e`.  The store has to take an
 *      INT-valued operand for the `mov` to appear at all.
 *   3. The `2` at gState+0x1c4 is the pool tell.  Same store, same mode, and
 *      the ROM writes `ldr r2, =2` for it while writing `mov r3, #0x4e` four
 *      bytes later -- one immediate and one pooled in ONE statement pair, which
 *      is the internal control const.sym's header asks for.  `(int)&_AREA_02`
 *      reproduces it exactly.
 *
 * `_AREA_02` RATHER THAN `_CONST_2`, AND THE TWO ARE BYTE-IDENTICAL.  Both are
 * absolute 2 (area.sym:222, const.sym:46), both are INCLUDEd by stage1.ld, and
 * swapping one for the other leaves all 270 encodings and the relocation OFFSET
 * unchanged -- only the relocation's NAME moves.  The field is chosen on
 * evidence rather than convenience: gState+0x1c4 and gState+0x1c6 are written
 * as a PAIR here (2, 0x4e) and again in src/non_matching/ovl_7e636c/2008df0.c
 * (0x88, 0x1e), four bytes above the area halfword at gState+0x1C0 that
 * area.sym documents, so the first of the pair reads as a destination area id.
 * `_CONST_2` is already identified in const.sym as an actor flag bit at +0x64,
 * which this is not.  A wrong NAME costs nothing (const.sym's own header) and a
 * wrong VALUE fails `make compare`, so this is a naming judgement, recorded.
 *
 * ======================= THE TWO RESIDUE CLUSTERS =======================
 *
 * CLUSTER 1 -- 4 encodings, a cprop REMATERIALISATION at the first `|= 1`.
 *
 *     rom   bl GetActor / mov r2,#1 / add r0,#0x5a / ldrb r3,[r0] /
 *           mov r8,r2 / mov r2,r8 / orr r3,r2 / strb r3,[r0]
 *     ours  bl GetActor / mov r3,#1 / add r0,#0x5a / mov r8,r3 /
 *           ldrb r3,[r0] / mov r2,#1 / orr r3,r2 / strb r3,[r0]
 *
 * Same instruction COUNT, same `m` in r8, same second site (`mov r2, r8 /
 * orr r2, r3 / strb r2 / mov r8, r2` -- byte-exact, including the dead
 * write-back).  The ROM reads `m` out of r8 at the FIRST use; we rematerialise
 * the constant.  That is cprop: `m = 1` reaches the use as a known constant and
 * the use is a SOURCE only (`*p |= m`), so it is replaced by the literal, while
 * the second site survives because there `m` is the DESTINATION (`m |= *p`).
 * For the ROM's form, `m` must not be a known constant at the first use, and
 * nothing written in C makes it one: MEASURED WORSE or INERT --
 *     `*p |= 1` literal at both sites                252  (-16 bytes)
 *     `u = p[0x5a]; m = 1; p[0x5a] = u | m;`           63
 *     `p[0x5a] = m | p[0x5a];`                         14  (inert)
 *     `__asm__ volatile ("" : "+r" (m))` after m = 1  158  (+20 bytes)
 *     `volatile int m`                                 33
 *     `{ int t; t = m; asm("+r"(t)); *p |= t; }`      158  (+20)
 *     `{ register int t __asm__("r2"); t = m; ... }`    14  (inert -- cprop
 *       folds straight through the copy, exactly as the recorded
 *       "a constant-valued pin is folded by cprop and truly inert")
 * Every spelling that makes `m` opaque forces it OUT of r8 (an opaque value
 * gains references, gains priority and takes a low register), which costs 20
 * bytes.  The discriminator for the class: THE VALUE MUST LIVE IN A HIGH
 * REGISTER AND BE OPAQUE AT THE SAME TIME, and `register ... __asm__("r8")` is
 * forbidden.  Floor for this cluster: 4.
 *
 * CLUSTER 2 -- 10 encodings, the gState tail's register roles and one
 * scheduling slot.  With the three fixes above the tail is the ROM's eleven
 * instructions in the ROM's order except that ours assigns
 * offset->r3 value->r3 address->r2 where the ROM has offset->r0 value->r2
 * address->r3, and our `ldr` of the pooled 2 lands after the `add` instead of
 * before the `lsl`.  Naming the value (`w = (int)&_AREA_02;` then storing `w`)
 * makes the FIRST store BYTE-EXACT -- `ldr r1,=gState / mov r0,#0xe2 /
 * ldr r2,=2 / lsl r0,#1 / add r3,r1,r0 / strh r2,[r3]` -- and at the same time
 * turns the SECOND offset into `add r0, r0, #2`, a use_related_value, losing
 * two instructions and four bytes.  The two halves are in TENSION: seven
 * spellings of the pair were measured and every one that fixes the first store
 * breaks the second.
 *     barrier between the stores (asm memory clobber / do-while(0))     14 / 14
 *     `w` named, with or without the barrier, 6 variants       59-62  (-4 bytes)
 *     `k = 0x4e` assigned BEFORE `w`                                21  (=)
 *     value pinned to r2                                     77  (-4 bytes)
 *     offset pinned to r0                                    80  (-4 bytes)
 * Floor for this cluster: 10 on this base, 5 if the related-value half can be
 * blocked -- and the lever for THAT is a way to stop cse relating 0x1c6 to a
 * live 0x1c4, which this round did not find.
 *
 * ==================== THE ALLOCATION WALL, AND HOW IT FELL ====================
 *
 * THE BIGGEST SINGLE STEP (33 -> 19) WAS TWO EXTRA REFERENCES ON ONE LOCAL, AND
 * THE REASON IS A FORMULA, NOT A SEARCH.  The second half of this function is
 * one basic block holding three values that need call-saved registers: the
 * rebuilt `&.L679c` (14 loads through it), the 0xfe mask `n`, and the `1` mask
 * `m`.  The ROM puts them in r6, r5 and r8; plain C puts them in r5, r6 and r8
 * -- a pure r5/r6 rename worth 19 of the 33.  It is a LOCAL-allocation order
 * question, and gcc-2.96 `local-alloc.c` prints the tie-break in one macro:
 *
 *     #define QTY_CMP_PRI(q) ((int) (((double) (floor_log2 (qty[q].n_refs)
 *                   * qty[q].n_refs * qty[q].size) / (qty[q].death - qty[q].birth))
 *                   * 10000))
 *
 * With 15 references over ~111 instructions the address scores 3*15/111 = 4054;
 * with 5 references over ~33 the mask scores 2*5/33 = 3030, so the address is
 * allocated first and takes r5.  To flip it the mask needs SEVEN references:
 * 2*7/33 = 4242.  `__asm__ volatile ("" : "+r" (n))` adds exactly two -- one
 * use and one set -- and the whole rename closes in one line, all 29
 * `ldr r0, [r6]` correct and `mov r5, #0xfe` where the ROM has it.
 *
 *   > A REGISTER-ROLE SWAP BETWEEN TWO LOCAL QUANTITIES IS ARITHMETIC.  Count
 *   > the references and the instruction span of each, evaluate
 *   > floor_log2(refs)*refs/span, and if the ROM's winner is the loser you need
 *   > to move one of the two numbers.  `"+r"` on the value adds TWO references
 *   > and nothing else -- no instruction, no placement.  An input-only
 *   > `"" : : "r" (n)` adds only ONE and is NOT enough: it measures 100 here
 *   > against the 19 of the in-out form, which is the cleanest confirmation
 *   > the mechanism is the reference count and not the barrier.
 *
 * THE BARRIER IS ALSO A SCHEDULING FENCE, SO ITS PLACEMENT IS A SECOND LEVER.
 * At `n`'s definition it nails `mov r5, #0xfe` before the GetActor call (19).
 * After the `&=` statement it frees that and perturbs the next call's fill (17).
 * Anywhere in the dead middle of `n`'s live range -- before either
 * `__CutsceneWait`, before or inside the second mask site -- it is worth 14, and
 * five such placements TIE at 14.  So: put the reference-adding barrier where
 * the schedule does not care.
 *
 * ROUTES TO THE SAME RENAME THAT ARE LOCAL MINIMA (both measured, both
 * instructive):
 *   `register int n __asm__("r5")`   closes the rename COMPLETELY -- all 29
 *       loads on r6 -- and then makes the first `&=` one instruction SHORT:
 *       `ldrb r3,[r0] / and r3, r5` against the ROM's `ldrb r2,[r0] /
 *       mov r3,r5 / and r3,r2`.  With `n` a hard register gcc commutes the
 *       thumb `and` so the dying loaded byte becomes the tied operand and no
 *       copy is needed; with `n` a pseudo the mask is the tied operand and the
 *       copy appears.  Six spellings of that statement (`n & p[0x5a]`,
 *       `p[0x5a] & n`, a loaded temporary, a `(unsigned char)` cast, an r3 pin,
 *       an r3 pin plus an asm) are ALL byte-identical, so the pin is the cause
 *       and the site is unreachable.  124 differing, 700 bytes.
 *   `unsigned char n` / `short n` / `unsigned short n`  reach 124 and -4 bytes
 *       too, by a DIFFERENT and worse route (n lands in r8, m in r6).  The
 *       equal number is a coincidence; read the diff, not the count.
 *
 * MEASURED WORSE / INERT on the allocation axis (all against 33 on the base
 * before the barrier):
 *   -fno-schedule-insns2                          49  REGRESSES -> sched2 is
 *       already producing the ROM's order and ALIAS IS THE WRONG AXIS.  Run on
 *       the final file too: 49 again.
 *   -fno-gcse / -fno-rerun-cse-after-loop / -fno-strength-reduce /
 *     -fno-expensive-optimizations / -fno-cse-follow-jumps / -fno-force-mem /
 *     -fno-peephole / -fno-caller-saves / -fno-function-cse /
 *     -fno-thread-jumps / -ffixed-r7                     33 each (INERT)
 *   -fno-cse-skip-blocks                                229
 *   one `int` for the 0x3000 and the 0xfe (the recorded "reuse n" lever)  251
 *   one shared `unsigned char *p` for the four GetActor sites            129
 *   one NAMED `int *ap` for the second half's address                     33
 *   one named `int *ap` for the WHOLE function                          256
 *       (gcc commons it to ONE pool load; the ROM's two `ldr rN, =.L679c`
 *        are two pseudos and no single variable reproduces them, with or
 *        without an `"+r"` barrier on the pointer)
 *   four declaration orders of {v, n, m, k}                              33
 *   `volatile` on the .L679c access                                      33
 *   0xfe written as a literal at both sites instead of a local           33
 *       (gcse commons the two literals into the same pseudo; the named local
 *        is a spelling, not a lever)
 *   0xfe AND 1 both written as literals                                 104
 *
 * PIN SET.  All 35 blocks are kept and transcribed from the ROM's own register
 * fill order.  A single-drop sweep over all 35 on the final base says only
 * SEVEN are load-bearing: 0, 1, 13 and 27 are eviction pins and cost 235, 236,
 * 183 and 259 when dropped (the four sites that rebuild 0x19999, 0x1768, 0xd48
 * and 0x178c/0xd48 -- without them gcc commons those pool constants into r8 and
 * r10 and the push mask widens), 16, 20 and 30 are ordering pins and cost 2
 * each.  The other 28 are inert scaffolding; they are kept because they record
 * the ROM's fill order at every site, which is the information a later pass
 * needs, and they cost nothing.
 *
 * Harness: scratch_elev/b258/rich/t1 -- dd.py (normalising side-by-side diff),
 * nd.py (normalised difference count), gen.py (pin-drop sweep, unpins a block
 * by inlining its q-values), minimise_c8.log (the sweep output), and the
 * e_*.c / c[1-9].c ladder.  scratch_elev/b258/rich/q.sh and qf.sh run objcmp in
 * the container, qf.sh with extra flags through ocf.py.
 */
extern int L679c __asm__(".L679c");
extern int _AREA_02;
extern unsigned char gState[];
extern unsigned char gScript_881__0200cf20[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __MessageID(int id);
extern void __ActorMessage(int actor, int b);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_SetBehavior(int slot, void *s);
extern void __Func_808c44c(void);
extern void __Func_808c4c0(void);
extern void __Func_808f1c0(int a, int b);
extern void __Func_8091a58(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __Func_80936a0(int a, int b);
extern void OvlFunc_881_200a768(int a);

#define R0 register int q0 __asm__("r0")
#define R1 register int q1 __asm__("r1")
#define R2 register int q2 __asm__("r2")
#define R3 register int q3 __asm__("r3")

#define A  (*(int *)&L679c)

void OvlFunc_881_200a4a8(void)
{
    unsigned char *a0, *a1, *a2, *a3, *a4;
    unsigned int g;
    int v, n, m, k;

    __CutsceneStart();
    A = 0x37;
    OvlFunc_881_200a768(0x37);
    __MapTransitionIn();
    __WaitMapTransition();
    __Func_808c44c();
    a0 = __MapActor_GetActor(0);
    if (a0 != 0) {
        R0; R1; R2;
        q0 = A; q1 = *(int *)(a0 + 8); q2 = *(int *)(a0 + 0x10);
        __MapActor_SetPos(q0, q1, q2);
    }
    { R0; R1; R2; q0 = A; q1 = 0x19999; q2 = 0xcccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    { R0; R1; R2; q0 = A; q1 = 0x1768; q2 = 0xd78;
      __Func_80921c4(q0, q1, q2); }
    { R0; R1; R2; q0 = A; q1 = 0; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { R0; R1; R2; q1 = 0x80; q2 = 0x3c; q0 = A; q1 <<= 1;
      __MapActor_Emote(q0, q1, q2); }
    { R0; R1; q0 = A; q1 = 2;
      __Func_809259c(q0, q1); }
    __MessageID(0x263c);
    { R0; R1; R2; q0 = A | (0x80 << 5); q1 = 0; q2 = 0xa;
      __Func_8093040(q0, q1, q2); }
    { R0; R1; R2; q2 = 0; q0 = 0; q1 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_808c4c0();
    { R0; R1; q0 = 0x16666; q1 = 0xa;
      __Func_80936a0(q0, q1); }
    { R0; R1; q0 = 0x80; q1 = 0x80; q0 <<= 12; q1 <<= 9;
      __Func_80933d4(q0, q1); }
    { R0; R1; R2; R3; q1 = 1; q3 = 1; q2 = 0xd680000; q1 = -q1; q0 = 0x17880000;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x28);
    __Func_808c44c();
    { R0; R1; q1 = 0x81; q0 = A; q1 <<= 1;
      __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x3c);
    { R0; R1; q0 = A; q1 = 1;
      __Func_80925cc(q0, q1); }
    v = 0xc0 << 6;
    { R0; R1; R2; q0 = A | v; q1 = 0; q2 = 0x28;
      __Func_8093040(q0, q1, q2); }
    { R0; R1; R2; q0 = A; q1 = 0x1768; q2 = 0xd48;
      __Func_80921c4(q0, q1, q2); }
    { R0; R1; R2; q0 = A; q1 = 0x1794; q2 = 0xd48;
      __Func_80921c4(q0, q1, q2); }
    { R0; R1; R2; q0 = A; q1 = v; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { R0; R1; R2; q2 = 0x6666; q0 = 0; q1 = 0xcccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    { R0; R1; q1 = (int)gScript_881__0200cf20; q0 = 0;
      __MapActor_SetBehavior(q0, (void *)q1); }
    __CutsceneWait(0x14);
    { R0; R1; q0 = A; q1 = 1;
      __Func_80925cc(q0, q1); }
    { R0; R1; R2; q0 = A; q1 = 0; q2 = 0xa;
      __Func_8093040(q0, q1, q2); }
    while (*(short *)(__MapActor_GetActor(0) + 0x64) == 0)
        __WaitFrames(1);
    { R0; R1; R2; q1 = 0x80; q0 = 0; q1 <<= 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { R0; R1; R2; q1 = 0x83; q0 = A; q2 = 0; q1 <<= 1;
      __MapActor_Emote(q0, q1, q2); }
    { R0; R1; q0 = A; q1 = 2;
      __Func_80925cc(q0, q1); }
    { R0; R1; R2; q0 = A; q1 = 0; q2 = 0xa;
      __Func_8093040(q0, q1, q2); }
    { R0; R1; R2; q1 = 0x80; q0 = A; q2 = 0xa; q1 <<= 8;
      __Func_8092adc(q0, q1, q2); }
    { R0; R1; q0 = A; q1 = 0;
      __ActorMessage(q0, q1); }
    { R0; R1; R2; q1 = 0; q2 = 0x14; q0 = 0;
      __Func_8092adc(q0, q1, q2); }
    n = 0xfe;
    a1 = __MapActor_GetActor(A);
    a1[0x5a] &= n;
    { R0; R1; R2; q1 = 0x178c; q2 = 0xd48; q0 = A;
      __Func_80921c4(q0, q1, q2); }
    __asm__ volatile ("" : "+r" (n));
    __CutsceneWait(1);
    a2 = __MapActor_GetActor(A);
    m = 1;
    a2[0x5a] |= m;
    __CutsceneWait(0x14);
    a3 = __MapActor_GetActor(A);
    n &= a3[0x5a];
    a3[0x5a] = n;
    { R0; R1; R2; q1 = 0x1794; q2 = 0xd48; q0 = A;
      __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(1);
    a4 = __MapActor_GetActor(A);
    m |= a4[0x5a];
    a4[0x5a] = m;
    { R0; R1; q1 = 3; q0 = 0xf2;
      __Func_808f1c0(q0, q1); }
    { R0; R1; q1 = 0; q0 = 0xf2;
      __Func_8091a58(q0, q1); }
    { R0; R1; q0 = A; q1 = 1;
      __Func_80925cc(q0, q1); }
    __MessageID(0x2642);
    { R0; R1; R2; q0 = A; q1 = 0; q2 = 0xa;
      __Func_8093040(q0, q1, q2); }
    { R0; R1; R2; q1 = 0xc0; q2 = 0; q0 = A; q1 <<= 6;
      __Func_8092adc(q0, q1, q2); }
    __Func_808c4c0();
    { R0; R1; q0 = 0x80; q1 = 0xa; q0 <<= 9;
      __Func_80936a0(q0, q1); }
    __CutsceneWait(0x14);
    __SetFlag(0x8d << 2);
    __SetFlag(0x9bf);
    g = (unsigned int)gState;
    *(short *)(g + (0xe2 << 1)) = (int)&_AREA_02;
    k = 0x4e;
    *(short *)(g + (0xe3 << 1)) = k;
    __CutsceneEnd();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_881_200a4a8 -- 0x0200a4a8,
 * asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_c_c.s
 *
 * FOURTEEN differing of 270, at EXACT size (704 bytes) and exact encoding
 * count. Candidate: scratch_elev/b258/rich/final/ovl_30_c_a_c_c_a_c_a_a_a_c_c.c.
 * Ladder: 263 -> 165 (35 ROM-order pin blocks) -> 33 (gState tail plus `m = 1`
 * placement) -> 19 -> 14.
 *
 * THE RELOCATION LINE IS ONE OURS-ONLY ENTRY AND NOTHING ELSE --
 * ('000002bc','R_ARM_ABS32','_AREA_02'), the documented phantom. Every other
 * relocation matches by offset and name. That is NOT the residue; the 14
 * encodings are.
 *
 * TWO BLOCKERS IN TENSION, and the floors are separate:
 *   floor 4  -- a cprop rematerialisation needing a value to be
 *               high-register-resident AND opaque at the same time. The
 *               obvious device is `__asm__("r8")`, which is forbidden.
 *   floor 10 -- the gState tail's two halves pull against each other: naming
 *               the value fixes store 1 and triggers `use_related_value` on
 *               store 2.
 *
 * ALIAS IS THE WRONG AXIS: `-fno-schedule-insns2` REGRESSES, 14 -> 49. Eleven
 * other optimisation flags are inert.
 *
 * A MID-FUNCTION POOL DUMP THE ROM LACKS WAS A LENGTH SYMPTOM, NOT A POOL
 * PROBLEM. 132 of the 165 differing at that stage were shifted `ldr [pc]`
 * entries, and fixing a TWELVE-BYTE SHORTFALL ELSEWHERE dissolved all of them.
 * Do not chase a spurious pool directly -- measure the length first.
 *
 * `.L679c` is defined and .global in ovl_30_c_c_c_c_c_c.s, consumed only, so
 * nothing needs exporting. asmfacts.py says WHOLE -- convert directly;
 * makefile_flags() is empty; overlay.ld:52 already names the asm/ path and MUST
 * STAY VERBATIM.
 *)

*/
#endif
