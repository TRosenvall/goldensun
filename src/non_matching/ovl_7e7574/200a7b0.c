/* OvlFunc_959_200a7b0 -- 0x0200a7b0   (overlay 959, rom_7e7574)
 *
 * NON-MATCHING, 753 of 835 encodings differ.
 *   (objcmp AT PRODUCTION FLAGS -- plain -O2, no Makefile row for this file.)
 *
 * SIZE AND COUNT ARE BOTH INEXACT, so the 753 is SATURATED and CANNOT RANK:
 *   size  ref 2212 bytes, ours 2208  (-4)
 *   count ref 835 encodings, ours 834  (-1)
 * Rank this function with aligncmp instead:
 *   aligned-equal 688 (82.4% of ref), 208 differing/ins/del in 136 hunks.
 * shimcount: clean, exit 0 -- PIN-FREE.  No `register asm`, no barrier, no
 * fakematch row needed.
 * datacheck on the reference: SILENT.  No data section, no label needs .global.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7e7574/200a7b0.c \
 *     asm/overlays/rom_7e7574/ovl_9dc_c_c_a_a_a_c_c.s \
 *     --func OvlFunc_959_200a7b0
 *
 * SPLIT SHAPE: THREE-WAY, confirmed by `tools/split_s.py --dry-run`.  The .s
 * holds FOUR functions (OvlFunc_959_200a69c, OvlFunc_959_200a718,
 * OvlFunc_959_200a7b0, OvlFunc_959_200b054) and the split is
 *     _a.s  2 functions, 125 lines    (200a69c, 200a718)
 *     _b.s  1 function,  831 lines    (200a7b0 -- THIS ONE)
 *     _c.s  1 function,  2157 lines   (200b054)
 * rewriting overlays/rom_7e7574/overlay.ld, with the target landing as
 *   src/overlays/rom_7e7574/ovl_9dc_c_c_a_a_a_c_c_b.c
 * `.global` REQUIREMENTS: NONE.  datacheck.py on the file is silent, so the
 * asm-label capture hazard does not arise here -- there is no `.L` extern to
 * generate and grep for.  The two siblings this function calls across the
 * split boundary (OvlFunc_959_200a69c, OvlFunc_959_200a718) are already
 * `.global` in the reference .s, as are OvlFunc_959_200a52c and
 * OvlFunc_959_200a5f8 in their own files.
 *
 * ----------------------------------------------------------------------------
 * THE PROGRAM IS RIGHT, AND THE EVIDENCE IS THE TWO CHEAP CHECKS, NOT THE 753.
 * ----------------------------------------------------------------------------
 *
 * (A) THE RELOCATION SEQUENCE MATCHES ENTRY FOR ENTRY -- all 251 R_ARM_THM_CALL
 *     entries in the reference's order, plus iwram_3001ebc and gState twice
 *     each, with only pool DUMP POSITIONS differing.
 *
 * (B) THE DISTINCT-CONSTANT SET MATCHES EXACTLY.  Ours against the reference's
 *     `ldr rX, =` set:
 *       0x103 0x105 0x22b 0x2464 0x247d 0x247e 0x301 0x3333 0x5999 0x6666
 *       0x942 0xb333 0xcccc  +  gState  iwram_3001ebc
 *     Nothing in one and not the other EXCEPT the reference's bare 0xa3 where
 *     ours has `_AREA_a3`.  That is the relocation-FORM non-residue and the
 *     defect is in the REFERENCE, by the tree's own rule: gcc-2.96 never pools
 *     a constant it can build with an eight-bit `mov`, and 0xa3 fits in eight
 *     bits, so `ldr r0, =0xa3` where `mov r0, #0xa3` would do proves the
 *     operand was a SYMBOL in the original.  `_AREA_a3 = 0xa3;` is already
 *     defined in area.sym (line 179) -- an absolute symbol emits no bytes, so
 *     the link is byte-identical.  Precedent: the __Func_8091f90 call in
 *     src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_a_b.c takes
 *     `(int) (&_AREA_4d)` in exactly this position.
 *
 * ----------------------------------------------------------------------------
 * WHAT THE CONSTANT-SET CHECK BOUGHT: 52 BYTES AND 3 ENCODINGS IN ONE EDIT,
 * AND IT IS A NEW RESULT FOR THE STRAIGHT-LINE 800+ POPULATION.
 * ----------------------------------------------------------------------------
 *
 * The first reconstruction -- every message id spelled as its own literal --
 * measured +56 size / +6 count, and the constant-set diff said why in one
 * command: the REFERENCE pools THREE message ids (0x2464, 0x247d, 0x247e)
 * while OURS pooled TWENTY-SEVEN (0x2464..0x247b, 0x247d..0x247f).  Twenty-
 * three of our pool words had no counterpart at all.
 *
 * The reference's shape is the tell, and it is a THUMB IMMEDIATE-WIDTH tell:
 *     0x2465..0x246b  ->  `add r0, r5, #k`          (3-bit imm3 form, k <= 7)
 *     0x246c..0x247b  ->  `mov r0, r5` + `add r0, #k` (8-bit imm8 form, k > 7)
 * One register holds the BASE and every dialogue id is base + k.  A plain
 * literal cannot produce that: cse.c's related-value machinery
 * (`use_related_value`) is SYMBOL-based, so it never rewrites one CONST_INT as
 * another plus an offset.  **The base was a SOURCE LOCAL.**  Naming it took
 *     +56 / +6   ->   +4 / +3
 * and brought the distinct-constant set into exact agreement.
 *
 * GENERALISATION FOR THE BAND: in a straight-line cutscene, a RUN OF
 * CONSECUTIVE IDS IS A NAMED BASE PLUS LITERAL OFFSETS, and the screen is
 * `add rD, rN, #k` / `mov rD, rN` + `add rD, #k` against a pooled base.  This
 * is lever 4 ("the offset as a named local") applied to the ID SPACE rather
 * than to a structure offset, and at 800+ instructions it is worth an order of
 * magnitude more than it is lower down, because the run is longer.  Note it is
 * the OPPOSITE of the batch-307 reading: there the parked constants were cse's
 * invention and naming them was inert; here the parked base is the SOURCE's
 * and not naming it costs 23 pool words.
 *
 * ----------------------------------------------------------------------------
 * REUSE-TO-INHERIT MEASURED **POSITIVE** HERE, WHICH REVERSES THE BATCH-307
 * READING -- AND THE PRECONDITION IS WHY.
 * ----------------------------------------------------------------------------
 *
 * The two arms need two bases (0x247d in the then-arm, 0x2464 in the else-arm)
 * and the reference serves both from r5 -- `push {r5, lr}`, ONE call-saved
 * register.  Two declared variables against one declared variable:
 *
 *   | candidate                     | size | count | aligned | hunks |
 *   |-------------------------------|------|-------|---------|-------|
 *   | all ids literal               |  +56 |    +6 |  (n/m)  |  n/m  |
 *   | two bases, one per arm (v2)   |   +4 |    +3 |  82.3%  |  137  |
 *   | ONE base, both arms (v3) HERE |   -4 |    -1 |  82.4%  |  136  |
 *
 * band-800plus.md records reuse-to-inherit as "measured WORSE" at 800+, on the
 * grounds that its first precondition -- count already exact -- fails up here.
 * That reading needs narrowing, not reversing: on 2009f3c the count was +10
 * when the merge was tried and the merge removed a quantity the figures were
 * still arguing about.  Here the count was +3 when the merge was tried, and the
 * merge went to -1 AND gained an aligned point AND lost a hunk -- it moved in
 * the same direction on every instrument.  **The precondition is not "exact",
 * it is "close enough that the set is no longer in dispute."**  The two
 * variables are in DISJOINT arms, so this is reuse in its cleanest form: one
 * quantity, two disjoint live ranges, which is exactly what the reference's
 * single r5 is.
 *
 * ----------------------------------------------------------------------------
 * THE WHOLE REMAINING RESIDUE IS ONE THING: THE HIGH-SAVE PROLOGUE.
 * ----------------------------------------------------------------------------
 *
 * First differing encoding is at INDEX 0 and it is the push mask:
 *     ref   b520   push {r5, lr}              -- ONE call-saved register
 *     ours  b5e0   push {r5, r6, r7, lr}  + mov r7,fp / mov r6,sl / mov r5,r9 /
 *                  push {r5,r6,r7} / mov r7,r8 / push {r7}
 * Six prologue instructions and a matching epilogue, i.e. ~12 encodings, which
 * is why the count can sit at -1 while 753 encodings differ: the commoning
 * SAVES about as many instructions as the save/restore costs.  Everything after
 * index 0 is displaced by it.
 *
 * WHAT WE PARK THAT THE REFERENCE DOES NOT, read out of the generated .s:
 *     r8 = 0x4000 (0x80<<7), and `add r8, r8, r3` derives 0x5000 from it
 *     sl = 0x8000 (0x80<<8)
 *     fp = 0x102  (0x81<<1)
 *     r9 = a fourth speed/offset constant
 *     r7 = 0x942  (the save-flag id, hoisted to the top of the else-arm)
 *     r5 = the message base  <- the ONLY one the reference also parks
 * The reference REBUILDS every one of those at every site: `mov r1, #0x80`
 * twice in a row, two instructions apart, inside one call's argument setup,
 * un-commoned.  So cse pass 1 is doing to us exactly what band-800plus.md
 * section 2 describes, and the open question in that document's section 8.3 --
 * which source shape denies cse1 the cross-call commoning of a repeated
 * CONST_INT -- is NOT answered here.  What IS newly bounded is the size of the
 * prize: on this function it is worth ~12 encodings and the last 4 bytes.
 *
 * FLAGS: FOUR MORE GROUPS MEASURE BYTE-IDENTICAL, EXTENDING THE BATCH-307 LIST
 * FROM ONE FUNCTION TO TWO.  Screened with flagcmp (SCREENING ONLY -- these
 * numbers are not the claim line above):
 *     -fno-gcse | -fno-rerun-cse-after-loop | -Os | -fno-cse-follow-jumps
 * all four give size 2208, count 834, aligned 688 / 82.4%, 136 hunks -- the
 * SAME OBJECT as the default.  **NO Makefile row should be written for this
 * file**, and the CSE_CFLAGS precondition is absent anyway: the one save-flag
 * id read more than once (0x942) is read in two DIFFERENT arms, so neither use
 * dominates the other.
 *
 * `-ffixed-r8..r11` was NOT tried and must not be, per the batch-307
 * retraction: it masks the commoning excess by forcing rematerialisation and is
 * not REG_ALLOC_ORDER evidence.
 *
 * ----------------------------------------------------------------------------
 * STRUCTURE, for whoever picks this up
 * ----------------------------------------------------------------------------
 *
 * 810 instructions, 5 branches, 6 labels, ZERO high-register mentions in the
 * reference.  Five basic blocks:
 *     entry      if (__GetFlag(0x301)) return;   SetFlag, CutsceneStart
 *     then-arm   lines 18-187 of the reference, 49 statements
 *     else-arm   lines 192-585, 125 statements
 *     else-tail  lines 590-817, 71 statements  (reached by a `b` over a pool)
 *     join       __CutsceneEnd(); return
 * Two reference artefacts that are NOT residue and cost nothing to reproduce:
 *   * the early `return` compiles to `bl .L302e @far jump`, not `b` -- Thumb-1's
 *     unconditional `b` is +/-2 KB and the epilogue is ~1.7 KB away, so gcc
 *     emits the `bl` long form.  Ours does the same, unprompted.
 *   * `b .L2ddc` immediately before `.L2ddc:` is a jump OVER an interposed
 *     literal pool, not a real edge.  The else-arm and else-tail are one
 *     straight-line region in the source.
 * The two arms end with the SAME five statements (the iwram_3001ebc store, the
 * gState[0x22b] write, __Func_8091f90, __Func_8091eb0) -- duplicated in the
 * source, not a shared tail; the reference emits both copies.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char *iwram_3001ebc;
extern int _AREA_a3;

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __PlaySound(int id);
extern void __SetCameraTarget(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_801776c(int a, int b);
extern void __Func_8091eb0(int a, int b);
extern void __Func_8091f90(int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_8093500(int a, int b);
extern void __Func_8093530(void);
extern void OvlFunc_959_200a52c(void);
extern void OvlFunc_959_200a5f8(void);
extern void OvlFunc_959_200a69c(void);
extern void OvlFunc_959_200a718(void);

void OvlFunc_959_200a7b0(void)
{
    unsigned char *gs;
    int m;

    if (__GetFlag(0x301) != 0)
        return;
    __SetFlag(0x9c << 2);
    __CutsceneStart();
    if (__GetFlag(0x942) != 0) {
        m = 0x247d;
        __MapActor_SetSpeed(0, 0x80 << 8, 0x80 << 7);
        __Func_80921c4(0, 0xe4 << 1, 0xd8);
        __Func_809280c(0, 0xc, 0);
        OvlFunc_959_200a52c();
        __MapActor_Emote(0xc, 0x80 << 1, 0x3c);
        __Func_8092adc(0xc, 0x80 << 8, 0);
        __MapActor_Jump(0xc, 4, 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xc), 1);
        __CutsceneWait(0x1e);
        __MapActor_SetSpeed(2, 0xb333, 0x5999);
        __Func_809218c(2, 0xe8 << 1, 0xc0);
        __MapActor_WaitMovement(2);
        __CutsceneWait(0x1e);
        __Func_8092adc(0, 0x80 << 7, 0);
        __Func_8092adc(2, 0x80 << 7, 0);
        __Func_8092adc(1, 0x80 << 7, 0);
        __Func_8092adc(3, 0x80 << 7, 0);
        __MapActor_SetPos(0xd, 0xe4 << 17, 0xa0 << 17);
        __Func_80933d4(0x80 << 10, 0x80 << 7);
        __MessageID(m);
        __ActorMessage(0xd, 0);
        __Func_809218c(0xd, 0xe5 << 1, 0x88 << 1);
        __MapActor_WaitMovement(0xd);
        __Func_8092adc(0xd, 0xa0 << 7, 0);
        __CutsceneWait(0x28);
        __Func_809228c(0xd, -8, 8);
        __MapActor_WaitMovement(0xd);
        __CutsceneWait(0x3c);
        __PlaySound(0x9b);
        __Func_801776c(m + 1, 1);
        __Func_809228c(0xd, 8, -8);
        OvlFunc_959_200a5f8();
        __CutsceneWait(0x78);
        __Func_809259c(0, 2);
        __Func_809259c(2, 2);
        __Func_809259c(1, 2);
        __Func_809259c(3, 2);
        __CutsceneWait(0x14);
        OvlFunc_959_200a718();
        __Func_809280c(0xd, 0, 0);
        __MessageID(m + 2);
        __ActorMessage(0xd, 0);
        OvlFunc_959_200a69c();
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x200;
        __CutsceneWait(1);
        gs = (unsigned char *)&gState;
        gs[0x22b] = 3;
        __Func_8091f90((int) (&_AREA_a3), 4);
        __Func_8091eb0(0x62, 4);
    } else {
        m = 0x2464;
        __MapActor_SetAnim(0, 1);
        __PlaySound(0x11);
        __CutsceneWait(0x1e);
        __MessageID(m);
        __ActorMessage(0xc, 0);
        __Func_8092848(0, 0xc, 0);
        __CutsceneWait(0x8c);
        __Func_8092adc(0xc, 0x80 << 8, 0);
        __MapActor_Jump(0xc, 4, 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xc), 1);
        __MessageID(m + 0x1);
        __ActorMessage(0xc, 0);
        __MapActor_SetSpeed(0, 0x80 << 8, 0x80 << 7);
        __Func_80921c4(0, 0xe4 << 1, 0xd8);
        __Func_809280c(0, 0xc, 0);
        OvlFunc_959_200a52c();
        __MapActor_SetSpeed(2, 0xb333, 0x5999);
        __Func_809218c(2, 0xe8 << 1, 0xc0);
        __MapActor_WaitMovement(2);
        __CutsceneWait(0x1e);
        __MessageID(m + 0x2);
        __ActorMessage(2, 0);
        __MapActor_Emote(0xc, 0x80 << 1, 0);
        __CutsceneWait(0x6e);
        __PlaySound(0x3c);
        __MessageID(m + 0x3);
        __ActorMessage(0xc, 0);
        __CutsceneWait(0x1e);
        __MapActor_DoAnim(2, 3);
        __CutsceneWait(0x14);
        __MapActor_SetAnim(2, 1);
        __Func_809259c(0xc, 1);
        __CutsceneWait(0x14);
        __MapActor_SetSpeed(0xc, 0x6666, 0x3333);
        __Func_809218c(0xc, 0x82 << 2, 0xd0);
        __MapActor_WaitMovement(0xc);
        __MapActor_SetAnim(0xc, 1);
        __CutsceneWait(0x14);
        __Func_8092adc(0xc, 0xb0 << 8, 0);
        __CutsceneWait(0x1e);
        __Func_8092adc(0xc, 0xa0 << 7, 0);
        __CutsceneWait(0x1e);
        __Func_809280c(0xc, 2, 0);
        __CutsceneWait(0x14);
        __MessageID(m + 0x4);
        __ActorMessage(0xc, 0);
        __CutsceneWait(0x28);
        __MapActor_DoAnim(2, 3);
        __CutsceneWait(0x14);
        __MapActor_Emote(0xc, 0x84 << 1, 0);
        __CutsceneWait(0x78);
        __MessageID(m + 0x5);
        __ActorMessage(0xc, 0);
        __CutsceneWait(0x19);
        __MapActor_DoAnim(2, 3);
        __CutsceneWait(0x1e);
        __MapActor_DoAnim(0xc, 3);
        __CutsceneWait(0x28);
        __Func_809218c(2, 0xf0 << 1, 0xc8);
        __MapActor_WaitMovement(2);
        __Func_8092848(2, 0xc, 0);
        __CutsceneWait(0x3c);
        __MessageID(m + 0x6);
        __ActorMessage(2, 0);
        __CutsceneWait(0x14);
        __MapActor_SetAnim(0xc, 4);
        __CutsceneWait(0x50);
        __MessageID(m + 0x7);
        __ActorMessage(0xc, 0);
        __MapActor_SetPos(0xd, 0xe4 << 17, 0xa0 << 17);
        __PlaySound(0x13);
        __MessageID(m + 0x8);
        __ActorMessage(0xd, 0);
        __Func_809280c(0, 0xd, 0);
        __Func_809280c(2, 0xd, 0);
        __Func_809280c(1, 0xd, 0);
        __CutsceneWait(5);
        __Func_8092adc(3, 0x80 << 7, 0);
        __Func_809280c(0xc, 0xd, 0);
        __CutsceneWait(0x1e);
        __PlaySound(0x3d);
        __Func_80933d4(0x80 << 10, 0x80 << 7);
        __Func_8093500(0xd, 1);
        __Func_8093530();
        __MapActor_SetSpeed(0xd, 0xcccc, 0x6666);
        __Func_809218c(0xd, 0xe4 << 1, 0x98 << 1);
        __SetCameraTarget(0xd, 1);
        __MapActor_WaitMovement(0xd);
        __SetCameraTarget(1, 1);
        __MapActor_Emote(0, 0x81 << 1, 0);
        __MapActor_Emote(2, 0x81 << 1, 0);
        __MapActor_Emote(1, 0x81 << 1, 0);
        __MapActor_Emote(3, 0x81 << 1, 0);
        __MapActor_Emote(0xc, 0x81 << 1, 0);
        __CutsceneWait(0x3c);
        __Func_809280c(0xc, 0xd, 0);
        __Func_809259c(0xc, 2);
        __CutsceneWait(0x3c);
        __MessageID(m + 0x9);
        __ActorMessage(0xc, 0);
        __Func_809280c(0xd, 0xc, 0);
        __MessageID(m + 0xa);
        __ActorMessage(0xd, 0);
        __CutsceneWait(0x3c);
        __Func_809280c(0xd, 2, 0);
        __CutsceneWait(0x1e);
        __MessageID(m + 0xb);
        __ActorMessage(0xd, 0);
        __Func_8092848(3, 2, 0);
        __Func_8092848(0, 1, 0);
        __CutsceneWait(0x3c);
        __Func_809280c(0, 0xd, 0);
        __Func_809280c(2, 0xd, 0);
        __Func_809280c(1, 0xd, 0);
        __Func_809280c(3, 0xd, 0);
        __Func_809259c(0xd, 1);
        __CutsceneWait(0x3c);
        __MessageID(m + 0xc);
        __ActorMessage(0xd, 0);
        __MapActor_Emote(1, 0x103, 0);
        __CutsceneWait(0x3c);
        __MapActor_SetAnim(0xd, 4);
        __MessageID(m + 0xd);
        __ActorMessage(0xd, 0);
        __Func_809218c(1, 0xe4 << 1, 0xf8);
        __MapActor_WaitMovement(1);
        __Func_8092adc(1, 0x80 << 7, 0);
        __MessageID(m + 0xe);
        __ActorMessage(1, 0);
        __Func_809218c(2, 0xec << 1, 0xd8);
        __MapActor_WaitMovement(2);
        __Func_8092adc(2, 0x80 << 7, 0);
        __CutsceneWait(0xa);
        __MessageID(m + 0xf);
        __ActorMessage(2, 0);
        __MapActor_Emote(0xc, 0x105, 0);
        __CutsceneWait(0x3c);
        __MessageID(m + 0x10);
        __ActorMessage(0xc, 0);
        __Func_809218c(3, 0xdc << 1, 0xd8);
        __MapActor_WaitMovement(3);
        __Func_809280c(3, 0xd, 0);
        __MapActor_DoAnim(3, 3);
        __CutsceneWait(0xa);
        __MessageID(m + 0x11);
        __ActorMessage(3, 0);
        __MapActor_SetAnim(0xd, 4);
        __MessageID(m + 0x12);
        __ActorMessage(0xd, 0);
        __MapActor_Emote(0, 0x81 << 1, 0);
        __MapActor_Emote(1, 0x81 << 1, 0);
        __MapActor_Emote(3, 0x81 << 1, 0);
        __MapActor_Emote(2, 0x81 << 1, 0);
        __MapActor_Emote(0xd, 0x84 << 1, 0);
        __CutsceneWait(0x3c);
        __MessageID(m + 0x13);
        __ActorMessage(0xd, 0);
        __CutsceneWait(0x14);
        __MapActor_Emote(1, 0x103, 0);
        __CutsceneWait(0x3c);
        __MessageID(m + 0x14);
        __ActorMessage(1, 0);
        __Func_809259c(0xd, 1);
        __CutsceneWait(0x3c);
        __MessageID(m + 0x15);
        __ActorMessage(0xd, 0);
        __Func_809218c(0xd, 0xe4 << 1, 0x8c << 1);
        __MapActor_WaitMovement(0xd);
        __Func_8092adc(0xd, 0xa0 << 7, 0);
        __CutsceneWait(0x50);
        __Func_809228c(0xd, -8, 8);
        __MapActor_WaitMovement(0xd);
        __CutsceneWait(0x3c);
        __PlaySound(0x9b);
        __Func_801776c(0x247e, 1);
        __Func_809228c(0xd, 8, -8);
        __Func_809280c(0xd, 0xb, 0);
        OvlFunc_959_200a5f8();
        __PlaySound(0x34);
        __MessageID(m + 0x17);
        __ActorMessage(0xd, 0);
        __CutsceneWait(0x3c);
        __Func_809280c(0, 0xb, 0);
        __Func_809280c(1, 0xb, 0);
        __Func_809280c(2, 0xb, 0);
        __Func_809280c(3, 0xb, 0);
        __Func_809280c(0xc, 0xb, 0);
        OvlFunc_959_200a718();
        OvlFunc_959_200a69c();
        __SetFlag(0x942);
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x200;
        __CutsceneWait(1);
        gs = (unsigned char *)&gState;
        gs[0x22b] = 3;
        __Func_8091f90((int) (&_AREA_a3), 4);
        __Func_8091eb0(0x62, 4);
    }
    __CutsceneEnd();
}
