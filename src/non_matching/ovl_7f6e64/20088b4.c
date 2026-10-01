/* OvlFunc_969_20088b4  --  0x020088b4  --  PARK
 *
 * NON-MATCHING, 724 of 962  (tools/objcmp.py, PRODUCTION FLAGS:
 *   -O2 -mthumb -mthumb-interwork -mcpu=arm7tdmi -fno-builtin -nostdinc
 *   -ffreestanding -fcall-used-r4 -Iinclude -- the tree default for this path,
 *   NO Makefile row needed and none should be written)
 *
 * SIZE   ref 2508 bytes, ours 2500  --  INEXACT by 8 (ours SHORT)
 * COUNT  ref 962 encodings, ours 962  --  EXACT
 * RELOC  ref 257, ours 258.  The names and their SEQUENCE are otherwise
 *        identical entry for entry; the one extra is R_ARM_ABS32 `_AREA_00`,
 *        the relocation-FORM non-residue (see the pooled-zero note below).
 * ALIGNCMP (tools/aligncmp.py, separately): 826 aligned-equal of 962 = 85.9%,
 *        198 differing/ins/del in 122 hunks.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7f6e64/20088b4.c \
 *     asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_a_c_a.s --func OvlFunc_969_20088b4
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/ovl_7f6e64/20088b4.c \
 *     asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_a_c_a.s OvlFunc_969_20088b4
 *
 * SPLIT SHAPE: NONE.  `grep -c thumb_func_start` on the reference is 1, so this
 * is a WHOLE-FILE conversion to src/overlays/rom_7f6e64/ovl_314_c_a_c_c_a_c_a.c
 * when it lands.  tools/datacheck.py is SILENT on the reference, so no `.global`
 * list is required and the asm-label capture hazard does not arise -- generated
 * labels are .L3..L8 and the reference's own are .La6c/.La88/.Ledc/.L11d6, no
 * collision.  ONE linker row names the object: overlays/rom_7f6e64/overlay.ld:38.
 * tools/shimcount.py: 30 register pins over 15 sites (14 PIN3, 1 PIN2).
 *
 * ================= LEVERS THAT PAID, IN ORDER, WITH FIGURES ================
 * Baseline (plain literals, no names, no pins): -4 / +1 / 81.6% / 161 hunks.
 *
 * 1. THE DECLARATION-INITIALISED CONSTANT, on the two __MapActor_SetSpeed
 *    sites that share 0x10000 and 0x8000.  `int sx = 0x80 << 9;` AT ITS
 *    DECLARATION took size from -4 to EXACT and aligned 81.6% -> 82.4%.
 * 2. PINS AT THE SITES THE REFERENCE REBUILDS.  Pinning the two SetSpeed
 *    sites: 82.4% -> 82.8%.  Adding the three 0x244 sites and the two 0x1ec
 *    sites: -> 84.2%, 145 hunks.  (These five are mov+lsl constants our build
 *    commons into a callee-saved register and the reference rebuilds.)
 * 3. THE NAMED ZERO, THE `strh` CARRIER AND 0x4000 AS INTS, plus pins on the
 *    0xcccc/0x6666 pair: 84.2% -> 84.3%, 145 -> 135 hunks, and the two BOGUS
 *    pool words 0xffff8000 and 0x00004000 disappeared.  `h = 0x80 << 8;`
 *    `*(short *)(b + 6) = h;` is band-800plus.md's lever 3 verbatim.
 *    NOTE this LOST the size exactness of step 1 (0 -> -16): step 1's
 *    exactness WAS two cancelling defects -- two bogus pool words paying for
 *    eight bytes of real absence.  Rung 4 of the brief's FIGURES THAT LIE.
 * 4. THE HELD-QUANTITY SET, with the TWO-STEP COMPUTED FORM where a one-set
 *    constant had been rematerialised: 84.3% -> 85.8%, 126 hunks.
 * 5. PINS ON THE FOUR GENUINELY DUPLICATED POOL VALUES (0x212 x2, 0x202 x2,
 *    0xa014 x2, the third 0x6666): 85.8% -> 86.1%, 121 hunks.
 * 6. THE POOLED ZERO IS A SYMBOL, and this is the step that bought the count.
 *    `z2 = (int)&_AREA_00;` -> COUNT EXACT at 962, and the distinct pool-word
 *    set went exact at 32 = 32.  Cost 5 aligned points of nothing (86.1% ->
 *    85.9%) and is kept because count and pool content are the structural axes.
 *
 * ================= THE TELL FOR STEP 6, STATED AS A RULE ==================
 * The reference loads its zero with a FULL-WORD `ldr r2, .La6c` against an
 * explicit `.word 0`.  `*thumb_movsi_insn` decides pool-vs-mov on the VALUE,
 * and 0 is eight-bit-movable, so gcc would emit `mov` and could NEVER pool it.
 * A pooled, eight-bit-movable word in a reference is therefore a SYMBOL, which
 * is area.sym's criterion firing correctly.  `_CONST_0` measures BYTE-IDENTICAL
 * to `_AREA_00` here (same value, so the same encodings); only the relocation
 * NAME differs, and neither is in the reference, which carries the bare literal.
 * That is the relocation-FORM non-residue the overlay facts already call benign.
 * This does NOT contradict src/non_matching/ovl_7892c8/200b1b8.c, where the
 * refuted spelling was for a zero the reference built with `mov`.
 *
 * ================= WHICH MECHANISM DOMINATED, AND THE PROOF ===============
 * THE STRAIGHT-LINE cse1-COMMONS-CONSTANTS-ACROSS-CALLS BLOCKER DOMINATES, and
 * it dominates in BOTH directions at once -- which is the thing this function
 * adds to docs/band-800plus.md.  The proof is one command, the POOLED-CONSTANT
 * MULTISET, and it is better evidence than any hunk:
 *
 *     ref:   0x105 x8   0x103 x7   0x101 x7   0x6666 x3   0xcccc x2
 *            0xa014 x2  0x212 x2   0x202 x2   everything else x1
 *     ours (before pins): every value x1 except 0x6666 x2
 *     `mov rlo, rhigh` reuse copies:  ref 29, ours 42
 *
 * So cse pass 1 in OUR build commons 22 pool loads of three __MapActor_Emote
 * ids into callee-saved registers that the reference RELOADS at every site; and
 * at the same time `update_equiv_regs` gives OUR one-set constant pseudos a
 * REG_EQUIV, so reload REMATERIALISES six quantities the reference HOLDS
 * (0x4000 in r9, 0xd000 in r11, 0xb000 in r10, 0x5000 in r6, 0x8000 in r7,
 * 0x2013 in r8).  Two opposite symptoms, one pass.  The levers are therefore
 * also opposite, and this is the batch's finding:
 *
 *   * WHERE THE REFERENCE RELOADS AT EVERY SITE -> PIN.  A pin's destination is
 *     a call-clobbered hard register, so it rebuilds per site by construction.
 *   * WHERE THE REFERENCE HOLDS THE VALUE -> TWO SETS, i.e. the TWO-STEP
 *     COMPUTED FORM `q4000 = 0x80; q4000 <<= 7;`.  THE GATE IS REG_N_SETS:
 *     a one-set constant pseudo gets a REG_EQUIV and reload rematerialises it,
 *     which is exactly what a plain `int q4000 = 0x80 << 7;` did -- naming it
 *     was INERT at all four __Func_8092adc sites (ref `mov r1, r9`, ours
 *     `movs r1,#128 / lsls r1,#7`).  The second set removes the REG_EQUIV and
 *     the pseudo must then win or lose a register on its own priority.
 *
 * WHAT RULES OUT THE ALTERNATIVES.
 *   * NOT register pressure in general: -ffixed-r8..r11 is a RETRACTED theory
 *     (band-800plus.md section 7) and this reference is a sixth witness against
 *     it -- it parks constants in r8, r9, r10 and r11 itself.
 *   * NOT the pool-ordering machinery: the DISTINCT pool-word set is EXACT
 *     (32 = 32, counting -0x180000/-0x200000/_AREA_00 as the print and
 *     relocation forms of 0xffe80000/0xffe00000/0x0).  Nothing is missing or
 *     spare; only WHERE the two dumps fall differs.
 *   * NOT a flag: no CSE_CFLAGS row and no -fno-rerun-cse-after-loop was tried
 *     and none should be.  THIS FUNCTION HAS NO LOOP (one conditional, the
 *     `bne .L11d6` at the tail, over 8 instructions; the `b .La88` and
 *     `b .Ledc` are POOL SKIPS and register state carries across them), so
 *     lever 5's precondition -- a save-flag id used two or three times with one
 *     use dominating another -- is absent, and the twelve settings measured in
 *     batch 307 against this exact mechanism all missed it.
 *   * NOT declaration order and NOT region scoping: both are documented inert
 *     for a value cse1 invented, and nothing here contradicts that.
 *
 * ================= THE 8 BYTES, ATTRIBUTED ================================
 * With the count exact and the pool CONTENT exact, the whole -8 is the literal
 * pool SPLIT.  `thumb_reorg` put the reference's first dump at encoding index
 * 177 (7 words: the zero, 0x2450000, 0x245, 0x212, 0x213, 0x209, 0x203, with a
 * `.short 0` pad) and ours at index 473 (23 words), so the reference duplicates
 * two more values across its two dumps than ours does.  88 of our 198 differing
 * entries are `ldr rX, [pc, #N]` with a different N -- pure placement, not
 * content.  The remaining 110 are 40 `mov` + 35 `lsls` + 24 `movs` + 6 `adds`,
 * i.e. the commoning residue above.  THE POOL SPLIT CANNOT BE EXPECTED TO MATCH
 * BEFORE THE INSTRUCTION STREAM DOES; it is a consequence, not a cause.
 *
 * ================= PROBES MEASURED AND REJECTED, WITH FIGURES =============
 * Carry the frame as a column: this function has NO `sub sp` AT ALL, no
 * `mov rX, sp`, no `add rX, sp` -- zero frame on both sides, so no slot map
 * and nothing to sort.  All five rows below are frame-correct.
 *
 *   candidate                                     size  count  aligned  hunks
 *   plain literals (baseline)                      -4    +1    81.6%    161
 *   named long-lived constants, no pins            -12    -1    80.1%    154
 *   decl-initialised sx/sy only                      0    +3    82.4%    159
 *   + pins at SetSpeed / 0x244 / 0x1ec               0    +3    84.2%    145
 *   + named zero, h, q4000, pinned 0xcccc pair     -16    -2    84.3%    135
 *   + held set with two-step form                  -12    -1    85.8%    126
 *   + pins on the four duplicated pool values      -16    -3    86.1%    121
 *   THIS FILE (+ pooled zero as _AREA_00)           -8     0    85.9%    122
 *
 *   PINS AT ALL 22 POOLED-ID __MapActor_Emote SITES -- REJECTED, AND THE
 *   REASON IS WORTH THE ROW.  They FIX the mechanism: reuse copies 42 -> 26
 *   against the reference's 29, and total pool loads 35 -> 56.  They still
 *   measure WORSE both ways: ascending q0..q2 fills give -12 / -4 / 83.8% /
 *   125, and the reference's own per-site register-write order (extracted site
 *   by site and transcribed as the pin order) gives -8 / -2 / 84.0% / 130 on
 *   top of the named set, and -4 / -1 / 84.1% / 126 on top of this file.  So
 *   this is a case where the mechanism-level evidence and the two instruments
 *   DISAGREE and the mechanism loses: 22 pins buy the right pool loads and pay
 *   for them with 22 sites of wrong instruction order.  The brief's own warning
 *   reads correctly here -- the ROM's register-write order is NOT the source
 *   order, it is sched2's.  Anyone retrying this should attack the FILL ORDER,
 *   not re-litigate the pins; the four pin groups that DID pay are in the file.
 *
 *   THE BLANKET OUT-OF-RANGE PIN PASS -- MEASURED, AND IT IS A GENUINE
 *   TRADE-OFF, NOT A LOSS.  Pinning EVERY site with a literal argument outside
 *   0..255 (61 further sites on top of this file, ascending fills) gives
 *   -8 / -3 / 87.2% / 90 HUNKS.  That is 1.3 aligned points better and THIRTY-TWO
 *   FEWER HUNKS than this file, and 169 differing against 198 -- but it costs the
 *   COUNT exactness this file has, so by the discipline's own order (both axes,
 *   then size-and-count, then aligned) this file still ranks first: 8 + 0 against
 *   8 + 3.  The row is kept because the same pass took this batch's
 *   OvlFunc_969_200cbec from -28/-14 to BOTH AXES EXACT in one step, so the
 *   difference between the two functions is the finding, not the pass.
 *
 *   AND IT EXPOSED A LIMIT OF PINS THAT IS WORTH RECORDING ON ITS OWN.
 *   A PIN FORCES THE REGISTER, NOT THE REBUILD.  At the 0x5000 site the
 *   reference builds the value TWICE in interleaved instructions
 *   (`mov r6,#0xa0 / mov r1,#0xa0 / lsl r6,#7 / mov r0,#0x14 / lsl r1,#7`) --
 *   two independent RTL sets cse did not merge.  Pinning our site still yields
 *   `adds r1, r5, #0`, because cse replaces the pin's own `q1 = 0x5000` with the
 *   already-commoned pseudo and the pin merely copies it into r1.  So a pin
 *   cannot undo commoning for a value that is ALSO a named long-lived quantity
 *   in the same function; the two levers CONFLICT at exactly those sites, and
 *   that conflict is why this function stays mixed where 200cbec went exact.
 *
 *   `int one` instead of `unsigned char one`: -16 / -2 / 83.8% / 130.  WORSE.
 *   `char` is unsigned here and the narrow type is what scores AND is right.
 *
 *   `*p |= one` / `*p &= 0xfe` / `*a |= 2` instead of the spelled-out
 *   `*p = *p | one` forms: BYTE-IDENTICAL.  Inert, therefore UNTESTED as a
 *   lever, not disproved -- the three read-modify-write byte sites still carry
 *   a register-choice defect (ref `ldrb r2 / movs r3,#2`, ours `ldrb r3 /
 *   movs r2,#2`; 4 encodings) that neither spelling nor operand order moves.
 *   That defect is the FIRST differing index (37) and is the best next lead.
 *
 *   `(int)&_CONST_0` for the pooled zero: BYTE-IDENTICAL to `_AREA_00`.
 *   Either name works; `_AREA_00` is kept because area.sym's criterion is the
 *   one that fired.
 *
 * ================= THE TRANSCRIPTION ITSELF ===============================
 * 923 reference instructions, 255 calls over 40 callees, THREE branches to a
 * label of which TWO ARE POOL SKIPS.  Reading either skip as control flow
 * would cost argument fills, so the whole body is written straight-line.
 * One arithmetic error found and fixed by measurement: 0x91 << 18 is 0x2440000
 * and I first wrote 0x24400000, which gcc built with `lsls #22` and aligncmp
 * caught at ref index 50 in the first hunk list.
 *
 * Three overlay-local callees are declared extern and NOT defined here --
 * OvlFunc_969_2008894, _20088a8 and _2009280 -- this file converts only
 * 20088b4.  `one` is shared across the two `|=` sites because the reference
 * keeps 1 in r5 across both; that sharing is the reference's, not an invention.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern unsigned char *__Func_8093554(void);
extern unsigned char *iwram_3001ebc;
extern unsigned char gScript_969__0200dfc4[];
extern int _AREA_00;

extern void OvlFunc_969_2008894(int a);
extern void OvlFunc_969_20088a8(int a, int b);
extern void OvlFunc_969_2009280(int a, int b);

extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __CutsceneEnd(void);
extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __Func_800fe9c(void);
extern int  __Func_8091c7c(int a, int b);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092504(int a);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __MapActor_DoAnim(int a, int b);
extern void __MapActor_Emote(int a, int b, int c);
extern void __MapActor_Jump(int a, int b, int c);
extern void __MapActor_RunScript(int a, unsigned char *s);
extern void __MapActor_SetAnim(int a, int b);
extern void __MapActor_SetBehavior(int a, unsigned char *s);
extern void __MapActor_SetPos(int a, int x, int z);
extern void __MapActor_SetSpeed(int a, int x, int z);
extern void __MapActor_TravelTo(int a, int x, int z);
extern void __MapActor_WaitMovement(int a);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __MessageID(int id);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern void __WaitMapTransition(void);


#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_969_20088b4(void)
{
    unsigned char *a;
    unsigned char *b;
    unsigned char *p;
    unsigned char *s;
    unsigned char one;
    int z;
    int z2;
    int h;
    int q4000;
    int qb000;
    int qd000;
    int q5000;
    int m4013;
    int m2014;
    int m2013;
    int m8015;
    int ma014;
    int m8001;

    a = __MapActor_GetActor(0x12);
    __CutsceneStart();
    OvlFunc_969_2009280(1, 0);
    OvlFunc_969_2009280(2, 0);
    OvlFunc_969_2009280(3, 0);
    __Func_80933f8(-1, -1, -1, 0);
    __WaitFrames(1);
    z = 0;
    a[0x55] = z;
    a += 0x23;
    *a = 2 | *a;
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x12), 0);
    __Func_8092b08(0x12, 1);
    __MapActor_SetPos(0x12, 0x2440000, 0x1520000);
    b = __MapActor_GetActor(0);
    s = b + 0x55;
    *s = z;
    __Func_8092b08(0, 1);
    __MapActor_SetPos(0, 0x2450000, 0x1200000);
    __WaitFrames(1);
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    { PIN3; q1 = 0x80 << 9; q2 = 0x80 << 8; q0 = 0x12; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x80 << 9; q2 = 0x80 << 8; q0 = 0; __MapActor_SetSpeed(q0, q1, q2); }
    p = __MapActor_GetActor(0) + 0x5a;
    *p = 0xfe & *p;
    { PIN3; q1 = 0x91 << 2; q0 = 0x12; q2 = 0xdd; __MapActor_TravelTo(q0, q1, q2); }
    __Func_8092158(0, 0x245, 0xab);
    { PIN3; q0 = 0x12; q1 = 0x212; q2 = 0xd3; __MapActor_TravelTo(q0, q1, q2); }
    __Func_8092158(0, 0x213, 0xa1);
    __MapActor_TravelTo(0x12, 0x208, 0xbf);
    __Func_8092158(0, 0x209, 0x8d);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x12), 1);
    __MapActor_TravelTo(0x12, 0x203, 0xab);
    __Func_8092158(0, 0x204, 0x79);
    __PlaySound(0x120);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0, 6);
    __Func_8092504(0);
    *(int *)(b + 8) = 0x2040000;
    *(int *)(b + 0xc) = 0x80000;
    *(int *)(b + 0x10) = 0x940000;
    h = 0x80 << 8;
    *(short *)(b + 6) = h;
    *s = 3;
    z2 = (int)&_AREA_00;
    __PlaySound(0x98);
    *(int *)(b + 0x28) = 0x40000;
    __PlaySound(0x98);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 1);
    __Func_8092158(0, 0x1f8, 0x94);
    __CutsceneWait(0xa);
    p = __MapActor_GetActor(0) + 0x5a;
    one = 1;
    *p = *p | one;
    q4000 = 0x80;
    q4000 <<= 7;
    *(int *)(b + 0xc) = 0xffe00000;
    *(short *)(b + 6) = q4000;
    __CutsceneWait(0x14);
    __PlaySound(0x134);
    __Func_8092158(0x12, 0x20c, 0xbf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x12), 0);
    { PIN3; q0 = 0x12; q1 = 0x212; q2 = 0xd3; __Func_8092158(q0, q1, q2); }
    { PIN3; q1 = 0x91 << 2; q0 = 0x12; q2 = 0xdd; __Func_8092158(q0, q1, q2); }
    { PIN3; q1 = 0x91 << 2; q2 = 0xa9 << 1; q0 = 0x12; __MapActor_TravelTo(q0, q1, q2); }
    p = __MapActor_GetActor(0) + 0x23;
    *p = one | *p;
    { PIN3; q2 = 0x6666; q1 = 0xcccc; q0 = 0; __MapActor_SetSpeed(q0, q1, q2); }
    OvlFunc_969_2009280(1, 1);
    OvlFunc_969_2009280(2, 1);
    OvlFunc_969_2009280(3, 1);
    { PIN3; q1 = 0xf6 << 1; q0 = 0; q2 = 0xa4; __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x202; q2 = 0xa4; __Func_809218c(q0, q1, q2); }
    { PIN3; q1 = 0xf6 << 1; q0 = 2; q2 = 0x8c; __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x202; q2 = 0x8c; __Func_80921c4(q0, q1, q2); }
    __MapActor_SetAnim(0, 1);
    __MapActor_SetAnim(1, 1);
    __MapActor_SetAnim(2, 1);
    __Func_8092adc(0, q4000, 0);
    __Func_8092adc(1, q4000, 0);
    __Func_8092adc(2, q4000, 0);
    __Func_8092adc(3, q4000, 0);
    __MapActor_WaitMovement(0x12);
    __MapActor_SetPos(0x12, 0, 0);
    m4013 = 0x4013;
    __PlaySound(0x121);
    __MessageID(0x2757);
    OvlFunc_969_2008894(m4013);
    __Func_8092adc(0, h, 0);
    __Func_8092adc(1, h, 0);
    __Func_8092adc(2, h, 0);
    OvlFunc_969_20088a8(3, h);
    __Func_8093554()[0x55] = z2;
    __Func_80933d4(0x4cccc, 0x9999);
    __Func_80933f8(0x1300000, 0x200000, 0x9e0000, 1);
    __Func_8093530();
    __CutsceneWait(0x14);
    qd000 = 0xd0;
    qd000 <<= 8;
    m2014 = 0x2014;
    OvlFunc_969_20088a8(0x14, qd000);
    __Func_80925cc(0x14, 1);
    __PlaySound(0x3d);
    OvlFunc_969_2008894(m2014);
    __MapActor_SetAnim(0x13, 4);
    OvlFunc_969_2008894(m4013);
    qb000 = 0xb0;
    qb000 <<= 8;
    OvlFunc_969_20088a8(0x14, qb000);
    __MapActor_Emote(0x14, 0x105, 0x28);
    OvlFunc_969_2008894(m2014);
    OvlFunc_969_2008894(0x15);
    __MapActor_Emote(0x13, 0x100, 0);
    __MapActor_Emote(0x14, 0x100, 0x14);
    __Func_8092adc(6, 0x3000, 0);
    __Func_8092adc(0x13, 0x5000, 0);
    __Func_8092adc(0x14, 0x5000, 0x14);
    __Func_80933d4(0x19999, 0x3333);
    __Func_80933f8(0x1260000, -1, 0xc20000, 1);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x15; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(0x15, 0x110, 0xc8);
    __Func_80925cc(0x14, 1);
    __CutsceneWait(0x14);
    OvlFunc_969_2008894(m2014);
    __MapActor_Emote(0x13, 0x103, 0x14);
    OvlFunc_969_2008894(0x13);
    __MapActor_DoAnim(0x15, 3);
    OvlFunc_969_2008894(0x15);
    __MapActor_Emote(0x14, 0x101, 0x28);
    OvlFunc_969_2008894(m2014);
    __MapActor_DoAnim(0x15, 4);
    OvlFunc_969_2008894(0x15);
    __MapActor_Emote(0x13, 0x101, 0x3c);
    __Func_8093040(0x13, 0, 0x28);
    __MapActor_Emote(0x13, 0x106, 0x28);
    OvlFunc_969_20088a8(0x13, h);
    m2013 = 0x2013;
    OvlFunc_969_2008894(m2013);
    __Func_8092adc(6, 0, 0);
    __MapActor_Emote(0x15, 0x103, 0x28);
    OvlFunc_969_2008894(0x15);
    __Func_8092adc(0x13, 0x3000, 0);
    __Func_8092adc(0x14, qb000, 0x28);
    __Func_80925cc(0x15, 1);
    OvlFunc_969_2008894(0x15);
    q5000 = 0xa0;
    q5000 <<= 7;
    __Func_8092adc(0x14, 0x5000, 0);
    OvlFunc_969_20088a8(0x13, q5000);
    __MapActor_Emote(0x13, 0x108, 0x14);
    OvlFunc_969_2008894(m2013);
    __MapActor_Emote(0x15, 0x103, 0x14);
    __Func_8093040(0x15, 0, 0x14);
    __Func_8092adc(0x14, h, 0x28);
    __MapActor_DoAnim(0x14, 4);
    __Func_8093040(m2014, 0, 0x28);
    __MapActor_DoAnim(0x15, 3);
    __Func_8093040(0x15, 0, 0x14);
    __Func_8092adc(0x14, q5000, 0x14);
    __Func_809259c(0x15, 2);
    OvlFunc_969_2008894(0x15);
    __MapActor_Emote(0x14, 0x105, 0);
    __MapActor_Emote(0x13, 0x105, 0x50);
    __Func_809259c(0x15, 2);
    OvlFunc_969_2008894(0x15);
    __MapActor_Emote(0x13, 0x101, 0x3c);
    OvlFunc_969_2008894(m2013);
    __MapActor_DoAnim(0x15, 3);
    OvlFunc_969_2008894(0x15);
    __Func_809259c(0x13, 1);
    __Func_80925cc(0x14, 1);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x15, 4);
    __Func_8093040(0x15, 0, 0x14);
    __MapActor_Emote(0x14, 0x105, 0x3c);
    __Func_8093040(m2014, 0, 0x14);
    OvlFunc_969_20088a8(0x15, qb000);
    OvlFunc_969_2008894(0x15);
    OvlFunc_969_20088a8(6, 0x3000);
    __Func_80925cc(6, 2);
    __CutsceneWait(0x14);
    OvlFunc_969_20088a8(0x15, qd000);
    __MapActor_DoAnim(0x13, 4);
    OvlFunc_969_2008894(m2013);
    __Func_8092adc(6, 0, 0);
    __Func_80925cc(0x15, 1);
    OvlFunc_969_2008894(0x15);
    __MapActor_DoAnim(0x14, 3);
    OvlFunc_969_2008894(m2014);
    __MapActor_DoAnim(0x15, 4);
    OvlFunc_969_2008894(0x15);
    __MapActor_Emote(0x14, 0x101, 0);
    __MapActor_Emote(0x13, 0x101, 0x50);
    __Func_8092adc(0x13, h, 0);
    __Func_8092adc(0x14, h, 0);
    { PIN2; q0 = 0x6666; q1 = 0xccc; __Func_80933d4(q0, q1); }
    __Func_80933f8(0x1260000, -1, 0xb40000, 1);
    __Func_80921c4(0x15, 0x106, 0xb0);
    m8015 = 0x8015;
    __Func_8092adc(0x15, h, 0x28);
    __Func_8092adc(0x15, 0, 0x14);
    __Func_809259c(0x15, 2);
    OvlFunc_969_2008894(m8015);
    __MapActor_Emote(0x13, 0x100, 0x14);
    OvlFunc_969_2008894(m2013);
    __Func_809259c(0x15, 2);
    OvlFunc_969_2008894(m8015);
    __MapActor_Emote(0x14, 0x103, 0x28);
    { PIN3; q0 = 0xa014; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    __MapActor_Emote(0x15, 0x105, 0x14);
    OvlFunc_969_2008894(m8015);
    __MapActor_Emote(0x13, 0x103, 0x14);
    OvlFunc_969_2008894(m2013);
    __MapActor_Emote(0x15, 0x101, 0x28);
    ma014 = 0xa014;
    OvlFunc_969_2008894(m8015);
    __MapActor_DoAnim(0x14, 4);
    OvlFunc_969_2008894(ma014);
    __MapActor_DoAnim(0x13, 3);
    OvlFunc_969_2008894(m2013);
    __MapActor_Emote(0x15, 0x103, 0x3c);
    __Func_8092adc(0x15, h, 0x14);
    __Func_8093040(0xa015, 0, 0x28);
    __MapActor_Emote(6, 0x105, 0x78);
    __MapActor_Emote(0x14, 0x105, 0x3c);
    OvlFunc_969_2008894(ma014);
    __Func_8092adc(0x15, 0, 0x28);
    __MapActor_SetAnim(0x13, 3);
    OvlFunc_969_2008894(m2013);
    __Func_80925cc(0x15, 1);
    __Func_8093040(m8015, 0, 0x14);
    OvlFunc_969_2008894(ma014);
    __MapActor_Emote(0x15, 0x100, 0x28);
    __MapActor_SetAnim(0x13, 4);
    OvlFunc_969_2008894(m2013);
    __MapActor_Emote(6, 0x105, 0x28);
    __MapActor_Emote(0x14, 0x108, 0x28);
    OvlFunc_969_2008894(ma014);
    __MapActor_Emote(0x13, 0x103, 0x14);
    OvlFunc_969_2008894(m2013);
    __Func_80925cc(0x15, 1);
    __CutsceneWait(0x14);
    __Func_809259c(0x14, 2);
    OvlFunc_969_2008894(ma014);
    __MapActor_SetAnim(0x13, 4);
    OvlFunc_969_2008894(m2013);
    *(int *)(iwram_3001ebc + 0x1c0) = 0x202;
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_80933f8(0x1f80000, 0xffe80000, 0xa80000, 0);
    __WaitFrames(1);
    __Func_800fe9c();
    __WaitFrames(1);
    m8001 = 0x8001;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    __Func_80925cc(1, 1);
    OvlFunc_969_2008894(m8001);
    __MapActor_Emote(3, 0x101, 0x28);
    OvlFunc_969_2008894(3);
    __MapActor_DoAnim(2, 3);
    __Func_8093040(0x1002, 0, 0x28);
    __MapActor_Jump(1, 2, 0x14);
    OvlFunc_969_2008894(m8001);
    __Func_8092c40(m8001, 0);
    __Func_8092adc(0, 0, 0);
    __Func_8092adc(2, q4000, 0);
    __Func_8092adc(3, 0x2000, 0);
    if (__Func_8091c7c(0, 0) == 1)
        *(unsigned short *)(iwram_3001ebc + 0x1d8) += 1;
    __CutsceneWait(0x14);
    OvlFunc_969_2008894(1);
    __Func_8092adc(2, 0, 0);
    __Func_8092adc(3, h, 0x14);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(2, 3);
    __MapActor_DoAnim(3, 3);
    __MapActor_SetBehavior(1, gScript_969__0200dfc4);
    __MapActor_SetBehavior(2, gScript_969__0200dfc4);
    __MapActor_RunScript(3, gScript_969__0200dfc4);
    __CutsceneWait(0x14);
    __CutsceneEnd();
}
