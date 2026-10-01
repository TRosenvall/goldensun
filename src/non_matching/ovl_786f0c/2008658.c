/* OvlFunc_886_2008658 -- 0x02008658   (overlay 886, rom_786f0c)
 *
 * NON-MATCHING, 923 of 1029 encodings differ.
 *   (objcmp AT PRODUCTION FLAGS -- plain -O2, no Makefile row for this file.)
 *
 * SIZE AND COUNT ARE BOTH INEXACT, so the 923 is SATURATED and CANNOT RANK:
 *   size  ref 2664 bytes, ours 2652  (-12)
 *   count ref 1029 encodings, ours 1022  (-7)
 * Rank this function with aligncmp instead:
 *   aligned-equal 819 (79.6% of ref), 303 differing/ins/del in 211 hunks.
 * shimcount: clean, exit 0 -- PIN-FREE.  No `register asm`, no barrier, no
 * fakematch row needed.
 * datacheck on the reference: SILENT.  No data section, no label needs .global.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_786f0c/2008658.c \
 *     asm/overlays/rom_786f0c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_a.s \
 *     --func OvlFunc_886_2008658
 *
 * SPLIT SHAPE: NONE NEEDED.  ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_a.s holds this ONE
 * function and nothing else (`grep -c thumb_func_start` = 1), so landing is a
 * WHOLE-FILE conversion to
 *   src/overlays/rom_786f0c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_a.c
 * with NO tools/split_s.py run and NO `.global` requirements.  All four
 * non-library externs already resolve without touching another file:
 * ActorCmd_ARRAY_886__020092fc, gScript_886__02009310 and
 * gScript_886__02009400 are `.global` in ..._c.s, and OvlFunc_886_20090c0 is
 * `.global` in ..._b.s -- which is ALREADY LANDED as
 * src/overlays/rom_786f0c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_b.c.
 * The asm-label capture hazard does not arise: datacheck is silent, so there is
 * no `.L` extern to generate and grep for.
 *
 * ----------------------------------------------------------------------------
 * THE PROGRAM IS RIGHT, AND THE PROOF IS THE IMMEDIATE MULTISET -- A CHEAPER
 * AND STRICTER GATE THAN READING HUNKS AT THIS SIZE.
 * ----------------------------------------------------------------------------
 *
 * Reading the immediates in 211 differing hunks one by one is not tractable and
 * it is not necessary.  Compare the MULTISET of every `#imm` in the two
 * instruction streams instead -- one Counter per side, one subtraction.  Out of
 * 984 reference instructions only FOURTEEN immediate values differ in count,
 * and every one of them is ours SHORT, never ours over:
 *
 *   value   0x80 0xa0 0xc0 0xd0 0xe0 0x90   <- `mov` BASES
 *   ref       18    9    6    9    6    2
 *   ours       6    2    2    1    2    1
 *
 *   value    0x5  0x6  0x7  0x8  0x9  0x1   <- `lsl` SHIFT AMOUNTS
 *   ref        8   10   15   41   23   70
 *   ours       2    7    4   30   21   66
 *
 * (0x0 and 0xb are +1 each, both register-allocation artefacts.)  Every deficit
 * is one half of a `mov rX, #base` / `lsl rX, #n` PAIR, which is the signature
 * of a constant we build ONCE and copy where the reference rebuilds it.  No
 * immediate exists on one side and not the other, so no argument, slot, offset,
 * flag id or script pointer is wrong.  The mnemonic histogram says the same
 * thing in one line: `lsl` ref 81 / ours 45, `mov` ref 515 / ours 556.
 *
 * **ADOPT THIS GATE FOR THE BAND.**  It is strictly stronger than the
 * distinct-constant-SET check of band-800plus.md section 5 (which sees pooled
 * values only, and cannot see `mov`+`lsl` constants at all -- exactly where the
 * whole residue of this function lives) and it costs the same one command.  Run
 * both: the set check catches a wrong constant, the multiset check catches a
 * wrong COUNT of a right constant.
 *
 * THE RELOCATION SEQUENCE: 264 reference entries against our 268, and the
 * difference is THREE things, none of them a wrong call --
 *   * ONE extra iwram_3001ebc, because the reference keeps &iwram_3001ebc in r8
 *     across ~500 instructions and pools it once where we pool it twice;
 *   * the OvlFunc_886_20090c0 / ActorCmd_ARRAY_886__020092fc pool pair sits
 *     three calls earlier in the reference -- a pool DUMP POSITION, driven by
 *     the `.pool_aligned` the reference emits before `.Labc`;
 *   * nothing else.  All 251 call relocations are present in the reference's
 *     order.
 *
 * ----------------------------------------------------------------------------
 * A DUPLICATE-CALL BUG THE *CLOSER* FIGURE WAS HIDING -- READ THIS BEFORE
 * TRUSTING AN EXACT COUNT.
 * ----------------------------------------------------------------------------
 *
 * The first reconstruction measured ref 1029 / ours 1029 -- COUNT EXACT, size
 * +8.  It was a WRONG PROGRAM.  The three `__MapActor_GetActor(0)` guards
 * emitted the fetch TWICE each (once as a bare statement, once as the
 * assignment to the guard variable), six extra instructions -- and the function
 * is six instructions SHORT for an unrelated reason, so the two errors cancelled
 * in the count and left it reading exact.  The relocation sequence is what
 * caught it: three surplus __MapActor_GetActor entries, visible immediately in
 * the symbol diff.  Fixing it moved the figures AWAY from exact, to -12 / -7.
 *
 * The brief's warning that "a figure can improve for the WRONG REASON" is
 * usually read as being about a lucky spelling.  This is the sharper form:
 * **TWO defects of opposite sign can hold a count at exactly zero, so an EXACT
 * count is not by itself evidence of a correct program.**  The relocation
 * sequence and the immediate multiset are both independent of the count and
 * both caught it; check them even -- especially -- when the count looks perfect.
 *
 * ----------------------------------------------------------------------------
 * THE RESIDUE IS THE SAME ONE MECHANISM AS 959_200a7b0, WHICH MAKES IT A
 * POPULATION RESULT AND NOT A FUNCTION RESULT.
 * ----------------------------------------------------------------------------
 *
 * First differing encoding is at INDEX 1 and it is the high-save:
 *     ref   4647   mov r7, r8          -- ONE high register saved
 *     ours  465f   mov r7, fp          -- FOUR: fp, sl, r9, r8
 * Index 0 (`push {r5, r6, r7, lr}`, b5e0) MATCHES, so the low call-saved set is
 * already right.  What the reference parks, read out of its own .s:
 *     r5  ActorCmd_ARRAY_886__020092fc (4 uses) THEN gScript_886__02009400
 *         (3 uses) -- TWO DISJOINT RANGES OF ONE REGISTER, the batch-304
 *         reuse-to-inherit signature, and both come free from the symbols
 *     r6  the constant 0 (5 stores)
 *     r7  OvlFunc_886_20090c0, parked across ~380 instructions between
 *         __StartTask and __StopTask
 *     r8  &iwram_3001ebc
 * We park all four of those AND THREE MORE, all of them compiler-invented:
 *     fp = 0xd000 (0xd0 << 8)   r9 = 0x5000 (0xa0 << 7)   sl = 0x7000 (0xe0 << 7)
 * each a __Func_8092adc / __Func_809218c second argument used 5-10 times, which
 * the reference rebuilds with `mov`+`lsl` at every site.  Same mechanism on the
 * other target in this brief (there it is 0x4000, 0x8000, 0x102 and 0x5000) and
 * the same mechanism band-800plus.md section 2 traced on 2008c28.  **THREE
 * FUNCTIONS NOW, so section 8.3's open question is the band's only question for
 * this population:** which source shape denies cse pass 1 the commoning of a
 * repeated CONST_INT.
 *
 * ONE NEW DATUM THAT NARROWS IT.  The reference's very first call is
 * `__Func_80933f8(-1, -1, -1, 0)` and it builds -1 THREE SEPARATE TIMES --
 * `mov r0,#1 / mov r1,#1 / mov r2,#1 / neg r2,r2 / neg r1,r1 / neg r0,r0`, six
 * instructions, three identical CONST_INTs two instructions apart INSIDE ONE
 * CALL'S ARGUMENT SETUP.  We emit four: `mov r2,#1 / neg r2,r2 / mov r0,r2 /
 * mov r1,r2`.  So the original's cse declined to common two adjacent identical
 * constants in the same basic block, at a distance of two instructions.  **That
 * rules out every explanation that depends on DISTANCE, on call boundaries, or
 * on basic-block extent** -- including the "long block gives cse many
 * repetitions" framing, which is a good description of the symptom but cannot be
 * the cause.  What is left is the COST MODEL or the shape of the RTL cse sees.
 * Whoever takes section 8.3 next should go into `03.cse` after this four-
 * instruction sequence, not after a 300-instruction block: it is the smallest
 * reproduction of the blocker found so far.
 *
 * FLAGS: `-fno-gcse` and `-Os` are BYTE-IDENTICAL to the default (2652 / 1022 /
 * 819 aligned / 211 hunks).  `-fno-rerun-cse-after-loop` is WORSE (2696 / 1043)
 * and `-O1` is much worse (2692 / 1042, 67.2% aligned).  Screened with flagcmp,
 * SCREENING ONLY -- these numbers are not the claim line above.  **NO Makefile
 * row for this file**, and note the CSE_CFLAGS direction is now measured
 * NEGATIVE on a straight-line 800+ function, not merely inert.
 *
 * `-ffixed-r8..r11` was NOT tried and must not be, per the batch-307
 * retraction.  This function is in fact a second witness FOR that retraction:
 * its own reference carries the high-save prologue and parks &iwram_3001ebc in
 * r8, so the original toolchain plainly had the high bank available.
 *
 * ----------------------------------------------------------------------------
 * STRUCTURE, for whoever picks this up
 * ----------------------------------------------------------------------------
 *
 * 984 instructions, 6 branches, 6 labels, and a 28-byte frame -- `sub sp, #0x1c`.
 * THE FRAME IS NOT A SPILL AREA.  All seven words are written at ONE call site
 * (reference lines 422-432) and never read: it is the OUTGOING ARGUMENT BLOCK
 * for `__Func_80931ec`, which takes ELEVEN arguments -- four in r0-r3 and seven
 * on the stack.  The "spill-slot map is the declaration list" first move
 * therefore yields NOTHING here, and the tell that it will is that the stores
 * are contiguous, all to one call, and `add rX, sp` never appears.  Check for
 * that before sorting offsets.
 *
 * Control flow, all six branches:
 *   * THREE identical guards, `p = __MapActor_GetActor(0); if (p != 0)
 *     __MapActor_SetPos(N, *(int *)(p + 8), *(int *)(p + 0x10));` for N = 1, 2, 3.
 *     The fetch is REPEATED, once per guard -- lever "reproduce the ROM's number
 *     of accesses" -- and hoisting it to a single read is the bug described above.
 *   * ONE real conditional: `if (__Func_8091c7c(0, 0) == 1)
 *     *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;`
 *   * TWO `b` instructions (`b .Labc`, `b .Lf18`) that jump over an interposed
 *     literal pool to the immediately following label.  NOT edges; the code is
 *     straight-line across them.
 * Field offsets used: actor +8 and +0x10 (the x/y the SetPos guards copy),
 * +0x55 (cleared to 0 four times, three on actors 0x17/0x18/0x19 and once on
 * the __Func_8093554 return), and +0x23 as a bit field -- `p[0x23] &= 0xfe`
 * then later `p[0x23] |= 1` on actor 3.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char ActorCmd_ARRAY_886__020092fc[];
extern unsigned char gScript_886__02009310[];
extern unsigned char gScript_886__02009400[];
extern void OvlFunc_886_20090c0(void);

extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __PlaySound(int id);
extern void __StartTask(void (*f)(void), int n);
extern void __StopTask(void (*f)(void));
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_RunScript(int slot, unsigned char *s);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern int __Func_8091c7c(int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092a1c(int a, int b, unsigned char *c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80931ec(int a, int b, int c, int d, int e, int f,
                           int g, int h, int i, int j, int k);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern unsigned char *__Func_8093554(void);

void OvlFunc_886_2008658(void)
{
    unsigned char *p;

    __CutsceneStart();
    __Func_80933f8(-1, -1, -1, 0);
    __WaitFrames(1);
    __Func_8092b08(3, 1);
    __MapActor_SetSpeed(0, 0x6666, 0x3333);
    __MapActor_SetSpeed(1, 0x6666, 0x3333);
    __MapActor_SetSpeed(2, 0x6666, 0x3333);
    __MapActor_SetSpeed(3, 0x6666, 0x3333);
    __MapActor_SetAnim(8, 5);
    __Func_809218c(0, 0xca << 2, 0xfe << 1);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x17), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x18), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x19), 0);
    __MapActor_GetActor(0x17)[0x55] = 0;
    __MapActor_GetActor(0x18)[0x55] = 0;
    __MapActor_GetActor(0x19)[0x55] = 0;
    __StartTask(OvlFunc_886_20090c0, 0xc8 << 4);
    __WaitFrames(1);
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x20;
    __MapTransitionIn();
    __WaitMapTransition();
    __MapActor_WaitMovement(0);
    __MapActor_SetAnim(0, 1);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_SetPos(1, *(int *)(p + 8), *(int *)(p + 0x10));
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_SetPos(2, *(int *)(p + 8), *(int *)(p + 0x10));
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_SetPos(3, *(int *)(p + 8), *(int *)(p + 0x10));
    __Func_809218c(1, 0xc6 << 2, 0x80 << 2);
    __Func_809218c(2, 0xce << 2, 0xfc << 1);
    __Func_80921c4(3, 0x332, 0x83 << 2);
    __MapActor_SetAnim(1, 1);
    __MapActor_SetAnim(2, 1);
    __CutsceneWait(0xa);
    __Func_8092a1c(0, 0x1000a, ActorCmd_ARRAY_886__020092fc);
    __Func_8092a1c(1, 0x1000a, ActorCmd_ARRAY_886__020092fc);
    __Func_8092a1c(2, 0x1000a, ActorCmd_ARRAY_886__020092fc);
    __Func_8092a1c(3, 0x1000a, ActorCmd_ARRAY_886__020092fc);
    __CutsceneWait(0x96 << 1);
    __Func_8093554()[0x55] = 0;
    __Func_80933d4(0x1999, 0x333);
    __Func_80933f8(0x3120000, 0, 0xd7 << 17, 1);
    __CutsceneWait(0xf0);
    __MapActor_SetIdle(0xa);
    __MapActor_Emote(0xa, 0x81 << 1, 0x50);
    __Func_80921c4(0xa, 0x333, 0x195);
    __CutsceneWait(0x28);
    __MapActor_DoAnim(0xa, 4);
    __CutsceneWait(0x28);
    __Func_8092adc(0xa, 0xd0 << 8, 0x14);
    __MessageID(0x1c1e);
    __Func_8093040(0x900a, 0, 0x14);
    __MapActor_SetIdle(0);
    __MapActor_SetIdle(1);
    __MapActor_SetIdle(2);
    __MapActor_SetIdle(3);
    __MapActor_Emote(0xb, 0x80 << 1, 0x28);
    __Func_8093040(0x200b, 0, 0x14);
    __Func_80925cc(0xa, 2);
    __CutsceneWait(0x28);
    __Func_8093040(0x900a, 0, 0xa);
    __Func_8092adc(0xb, 0xa0 << 7, 0xa);
    __Func_8093040(0x200b, 0, 0x28);
    __Func_80925cc(0xa, 2);
    __CutsceneWait(0x14);
    __Func_8093040(0x900a, 0, 0x14);
    __MapActor_Emote(0, 0x81 << 1, 0x50);
    __MapActor_Emote(0xb, 0x83 << 1, 0x28);
    __Func_8093040(0x200b, 0, 0x28);
    __Func_809259c(0xa, 2);
    __MapActor_Emote(0xa, 0x81 << 1, 0x14);
    __MapActor_SetAnim(0xa, 4);
    __Func_8093040(0x900a, 0, 0xa);
    __Func_809259c(0xb, 1);
    __MapActor_DoAnim(0xb, 3);
    __CutsceneWait(0x14);
    __Func_809259c(0xa, 1);
    __MapActor_DoAnim(0xa, 4);
    __Func_809259c(0xb, 1);
    __MapActor_DoAnim(0xb, 3);
    __Func_809259c(0xa, 1);
    __MapActor_DoAnim(0xa, 4);
    __MapActor_Emote(9, 0x105, 0);
    __Func_80925cc(9, 1);
    __CutsceneWait(0x14);
    __Func_8092adc(9, 0x80 << 5, 0x28);
    __Func_80925cc(9, 2);
    __CutsceneWait(0x3c);
    __Func_80925cc(9, 3);
    __CutsceneWait(0x28);
    __Func_8093040(0x4009, 0, 0x28);
    __MapActor_SetAnim(0xb, 0);
    __Func_80925cc(0xb, 2);
    __Func_8093040(0x200b, 0, 0xa);
    __MapActor_DoAnim(9, 4);
    __Func_80925cc(9, 2);
    __Func_8093040(0x4009, 0, 0xa);
    __MapActor_Emote(0xa, 0x80 << 1, 0x14);
    __Func_8092adc(0xa, 0xa0 << 7, 0x28);
    __MapActor_DoAnim(0xa, 3);
    __Func_8093040(0x400a, 0, 0xa);
    __MapActor_DoAnim(9, 4);
    __Func_8092adc(9, 0xd0 << 8, 0xa);
    __MapActor_Jump(9, 2, 0);
    __MapActor_SetAnim(9, 4);
    __Func_8093040(0x4009, 0, 0xa);
    __MapActor_Emote(0xb, 0x101, 0);
    __MapActor_Emote(0xa, 0x101, 0x28);
    __Func_8092adc(0xa, 0xd0 << 8, 0x50);
    __Func_8092adc(0xa, 0xa0 << 7, 0x3c);
    __Func_809259c(0xa, 2);
    __Func_809259c(0xb, 2);
    __Func_80931ec(0xa, 0xb, 6, 6, 6, 0xb, 0xc, 1, 7, 1, 0);
    __CutsceneWait(0x14);
    __Func_80933d4(0x19999, 0x3333);
    __Func_80933f8(0x3090000, 0, 0xea << 17, 1);
    __Func_8093530();
    __CutsceneWait(0x28);
    __MapActor_DoAnim(1, 3);
    __Func_8093040(0x1001, 0, 0x14);
    __Func_80925cc(8, 2);
    __StopTask(OvlFunc_886_20090c0);
    __CutsceneWait(0x28);
    __MapActor_DoAnim(8, 6);
    __CutsceneWait(0x14);
    __Func_8093040(0x4008, 0, 0x14);
    __Func_80933f8(0x2ee0000, 0, 0x1c30000, 1);
    __CutsceneWait(0x14);
    __Func_8092adc(0xb, 0xa0 << 7, 0);
    __Func_8092adc(0xa, 0xa0 << 7, 0xa);
    __Func_8092adc(8, 0x80 << 5, 0x28);
    __MapActor_Emote(8, 0x80 << 1, 0x28);
    __Func_8092adc(8, 0xc0 << 6, 0x14);
    __Func_8092adc(8, 0x80 << 5, 0x14);
    __Func_8092adc(8, 0xc0 << 6, 0x28);
    __MapActor_DoAnim(8, 6);
    __CutsceneWait(0x3c);
    __MapActor_Jump(8, 6, 0);
    __Func_8093040(0x4008, 0, 0x14);
    __MapActor_SetSpeed(1, 0x19999, 0xcccc);
    __Func_80921c4(1, 0x315, 0x1d9);
    __Func_8092adc(1, 0xe0 << 7, 0x14);
    __MapActor_DoAnim(1, 3);
    __Func_8093040(0x4001, 0, 0xa);
    __Func_8092adc(8, 0x80 << 5, 0x14);
    __MapActor_DoAnim(8, 3);
    __Func_8092c40(0x4008, 0);
    __Func_8092adc(0xa, 0xa0 << 7, 0);
    __Func_8092adc(9, 0x80 << 5, 0);
    __Func_8092adc(1, 0xc0 << 6, 0);
    __Func_8092adc(2, 0xe0 << 7, 0);
    __Func_8092adc(3, 0xb0 << 8, 0);
    if (__Func_8091c7c(0, 0) == 1)
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    __Func_80933f8(0x3090000, 0, 0xd6 << 17, 1);
    __CutsceneWait(0x14);
    __Func_80925cc(0xa, 2);
    __ActorMessage(0xa, 0);
    __MapActor_DoAnim(0xb, 4);
    __CutsceneWait(0x14);
    __MessageID(0x1c33);
    __ActorMessage(0x200b, 0);
    __Func_80933f8(0x3090000, 0, 0xea << 17, 1);
    __CutsceneWait(0x14);
    __Func_8092adc(1, 0xd0 << 8, 0x14);
    __MapActor_DoAnim(1, 3);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(9, 4);
    __Func_8092adc(9, 0xd0 << 8, 0xa);
    __ActorMessage(0x4009, 0);
    __MapActor_DoAnim(8, 3);
    __ActorMessage(0x4008, 0);
    __Func_8092adc(1, 0xe0 << 7, 0xa);
    __MapActor_DoAnim(1, 3);
    __Func_8092adc(9, 0x80 << 5, 0xa);
    __MapActor_SetAnim(0xb, 3);
    __MapActor_SetAnim(0xa, 3);
    __MapActor_SetAnim(9, 3);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x14);
    __Func_8092adc(1, 0xc0 << 6, 0x14);
    __MapActor_Emote(1, 0x81 << 1, 0x50);
    __Func_8092adc(1, 0xe0 << 7, 0x14);
    __Func_8093040(0x4001, 0, 0x14);
    __Func_8092adc(1, 0xc0 << 6, 0xa);
    __Func_8092adc(0, 0, 0x28);
    __MapActor_SetAnim(0, 3);
    __MapActor_DoAnim(1, 3);
    __CutsceneWait(0x14);
    __Func_8092adc(0, 0x80 << 7, 0x14);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x14);
    __Func_8092adc(1, 0x80 << 5, 0);
    __Func_8092adc(0, 0xe0 << 8, 0);
    __MapActor_SetSpeed(2, 0x80 << 9, 0x80 << 8);
    __Func_80921c4(2, 0x333, 0x1e9);
    __Func_8092adc(2, 0xb0 << 8, 0x28);
    __Func_80925cc(2, 2);
    __Func_8093040(2, 0, 0x14);
    __MapActor_DoAnim(2, 3);
    __MapActor_SetAnim(8, 3);
    __MapActor_SetAnim(9, 3);
    __MapActor_SetAnim(0xa, 3);
    __MapActor_DoAnim(9, 3);
    p = __MapActor_GetActor(3);
    p[0x23] &= 0xfe;
    __Func_8092b08(3, 1);
    __MapActor_SetSpeed(3, 0x80 << 9, 0x80 << 8);
    __Func_80921c4(3, 0x31a, 0x82 << 2);
    __Func_8092adc(1, 0xa0 << 7, 0);
    __Func_8092adc(0, 0xa0 << 8, 0);
    __Func_80921c4(3, 0xc4 << 2, 0xf8 << 1);
    __Func_8092adc(3, 0x90 << 8, 0xa);
    p = __MapActor_GetActor(3);
    p[0x23] |= 1;
    __Func_8093040(3, 0, 0x14);
    __MapActor_SetAnim(8, 3);
    __MapActor_SetAnim(9, 3);
    __MapActor_SetAnim(0xa, 3);
    __MapActor_DoAnim(9, 3);
    __CutsceneWait(0x14);
    __Func_80933f8(0x3090000, 0, 0xd6 << 17, 1);
    __CutsceneWait(0x14);
    __MapActor_SetSpeed(0xb, 0x6666, 0x3333);
    __Func_80921c4(0xb, 0x343, 0xc2 << 1);
    __Func_8092adc(0xb, 0xa0 << 7, 0);
    __MapActor_Emote(0xb, 0x84 << 1, 0x28);
    __Func_8093040(0x200b, 0, 0x14);
    __Func_80933f8(0x3090000, 0, 0xea << 17, 1);
    __CutsceneWait(0x28);
    __Func_8092adc(2, 0xe0 << 7, 0);
    __Func_8092adc(3, 0xf0 << 8, 0x28);
    __Func_8092adc(2, 0x90 << 8, 0);
    __Func_8092adc(3, 0xd0 << 8, 0x14);
    __MapActor_SetAnim(2, 3);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x14);
    __Func_80925cc(0xa, 1);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0xa, 3);
    __Func_8093040(0xa, 0, 0x14);
    __Func_8092adc(0, 0xc0 << 8, 0);
    __Func_8092adc(1, 0xd0 << 8, 0);
    __Func_8092adc(2, 0xb0 << 8, 0);
    __Func_8092adc(3, 0xd0 << 8, 0x28);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(2, 3);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x14);
    __MapActor_SetSpeed(2, 0x80 << 9, 0x80 << 8);
    __MapActor_SetBehavior(1, gScript_886__02009400);
    __MapActor_SetBehavior(2, gScript_886__02009400);
    __MapActor_RunScript(3, gScript_886__02009400);
    __MapActor_SetBehavior(0xa, gScript_886__02009310);
    __Func_80921c4(0xb, 0x345, 0xbc << 1);
    __Func_8092adc(0xb, 0xd0 << 8, 0x14);
    __SetFlag(0x81d);
    __CutsceneEnd();
}
