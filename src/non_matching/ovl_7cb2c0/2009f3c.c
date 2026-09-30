/* OvlFunc_945_2009f3c -- 0x02009f3c   (overlay 945, rom_7cb2c0)
 *
 * NON-MATCHING, 696 of 840 encodings differ.
 *   (objcmp AT PRODUCTION FLAGS -- plain -O2, no Makefile row for this file.)
 *
 * SIZE AND COUNT ARE BOTH INEXACT, so the 696 is SATURATED and CANNOT RANK,
 * but only just -- this is the closest either axis has come in this band:
 *   size  ref 2204 bytes, ours 2208  (+4, ONE pool word)
 *   count ref 840 encodings, ours 841  (+1)
 * aligncmp, which IS the ranking instrument here:
 *   aligned-equal 653 (77.7% of ref), 279 differing/ins/del in 166 hunks.
 * shimcount: clean, exit 0 -- PIN-FREE.  Nothing for fakematch.txt.
 * datacheck on the reference: SILENT.  No data section, no label needs .global.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7cb2c0/2009f3c.c \
 *     asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_a_a_c.s \
 *     --func OvlFunc_945_2009f3c
 *
 * SPLIT SHAPE: NONE NEEDED, and this is split_s.py's own verdict, not a guess:
 *   python3 tools/split_s.py --dry-run \
 *     asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_a_a_c.s \
 *     OvlFunc_945_2009f3c
 *   -> "holds only OvlFunc_945_2009f3c and no data; convert it directly, no
 *      split needed"
 * Landing is therefore a WHOLE-FILE conversion to
 *   src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_a_a_c.c
 * with ZERO `.global` requirements -- the reference pools no symbols at all,
 * only numeric literals, so there is nothing to export and nothing to import
 * beyond the plain externs below.
 *
 * ----------------------------------------------------------------------------
 * THIS FUNCTION IS THE CONTROL THAT EXPLAINS THE WHOLE BAND, AND IT RETRACTS
 * AN ATTRIBUTION MADE IN THIS SAME BATCH.
 * ----------------------------------------------------------------------------
 *
 * THE REFERENCE ITSELF USES THE THUMB HIGH REGISTER BANK.  Its prologue is
 *     push {r5, r6, r7, lr} / mov r7, r11 / mov r6, r10 / mov r5, r9
 *     push {r5, r6, r7}     / mov r7, r8  / push {r7}
 * and its body carries 45 mentions of r8/r9/r10/r11, in exactly the shape
 *     ldr r3, =0x100b / mov r9, r3 / mov r0, r9      ... mov r0, r9  (x8)
 *     mov r3, #0xb0 / lsl r3, #8 / mov fp, r3 / mov r1, fp
 * -- a constant computed once, parked in a high register, and copied down to
 * an argument register at each use.  So gcc-2.96 AS CONFIGURED HERE DOES emit
 * that prologue and the ROM DOES contain it.  Tree-wide: 467 hand-written .s
 * files carry the `mov r7, fp` high-save prologue and 210 OF THEM ALREADY HAVE
 * A LANDED .c SIBLING, so the shape is demonstrably reachable from C.
 *
 * THAT RETRACTS THE -ffixed READING IN src/non_matching/ovl_77a7c8/2008c28.c.
 * That park (same batch, same band) found -ffixed-r8..r11 worth +28 size and
 * +14 count, and floated it as a possible claim about the original toolchain's
 * Thumb register pool.  THE CLAIM IS WRONG AND THIS FUNCTION DISPROVES IT: the
 * high bank is in the pool.  -ffixed there is a MASKING flag -- it forces
 * reload to rematerialise commoned constants and so happens to cancel an
 * excess of them -- not a configuration difference.  2008c28's park has been
 * annotated in place.  A correction that leaves the original claim standing
 * has not landed, so read that park's section (4) as struck.
 *
 * ----------------------------------------------------------------------------
 * WHAT THE RESIDUE ACTUALLY IS: WE AND THE ROM COMMON A DIFFERENT SET OF
 * CONSTANTS.  THE SET IS THE QUANTITY THAT MATTERS, NOT THE SPELLING.
 * ----------------------------------------------------------------------------
 *
 * The reference parks SEVEN constants in callee-saved registers for the whole
 * function, and the seventh register carries TWO DISJOINT RANGES:
 *     r5  0x400b, later 0x8008      r6  0x100c        r7  0xc0 << 6 (0x3000)
 *     r8  0xd0 << 8 (0xd000)        r9  0x100b        r10 0xa0 << 7 (0x5000)
 *     r11 0xb0 << 8 (0xb000)
 * It REMATERIALISES everything else, including the first occurrence of several
 * of those same values.
 *
 * A plain-literal candidate (all constants written as literals, no locals)
 * lets cse.c pass 1 choose, and it chooses DIFFERENTLY -- it parks 0x80 << 8,
 * which the reference always rematerialises, and rematerialises 0x3000,
 * 0xb000 and 0x100b, which the reference parks.  The hunks read it directly:
 *     ours 2780 movs r7,#128 / 023f lsls r7,r7,#8   <- we park 0x8000
 *     ref  2180 movs r1,#128 / 0209 lsls r1,r1,#8   <- reference rematerialises
 *     ref  23b0 movs r3,#176 / 021b lsls r3,r3,#8 / 469b mov fp,r3 / 4659 mov r1,fp
 *     ours 21b0 movs r1,#176 / 0209 lsls r1,r1,#8   <- and the reverse
 * Because the two sets are nearly disjoint, the count runs +10 and the size
 * +20: every constant one side parks and the other does not is a two-
 * instruction swing.
 *
 * ----------------------------------------------------------------------------
 * THE LADDER, IN ORDER, WITH FIGURES -- INCLUDING THREE MEASURED NEGATIVES
 * ----------------------------------------------------------------------------
 *
 *   #   shape                                       size   count  aligned  hunks
 *   v1  all constants as literals                   +20    +10    80.0%    144
 *   v3  the ROM's seven NAMED, each assigned just
 *       before the ROM's own parking point          +20    +10    80.0%    144
 *   v2  the eight NAMED and all assigned at the
 *       TOP of the function          <-- THIS BODY   +4     +1    77.7%    166
 *   v5  v2 with declaration AND assignment order
 *       permuted to the ROM's r5..r11 order          +4     +1    77.7%    166
 *   v4  v2 but 0x400b/0x8008 MERGED into one
 *       variable (the batch-304 reuse lever)         +8     +3    75.0%    169
 *
 * RANKED PER THE DISCIPLINE -- size-and-count first, aligned second, never the
 * raw count -- v2 IS THE PARKED BODY.  It is 16 bytes and 9 encodings closer
 * than v1 on the primary axis.  Note honestly that v1 is 2.3 points BETTER on
 * aligncmp, which is the ranking instrument while both axes are inexact; the
 * two metrics genuinely disagree here and v2 was chosen because +4/+1 is one
 * pool word from a true distance and +20/+10 is not.  v1 is kept as
 * scratch_elev/b307g/f3c_v1.c if anyone wants to re-rank.
 *
 * NEGATIVE 1 -- NAMING A CONSTANT gcc WOULD COMMON ANYWAY IS BYTE-IDENTICAL
 *   TO LEAVING IT A LITERAL.  v3 names the reference's seven constants and
 *   places each assignment at the reference's own parking point, the textbook
 *   application of "the offset as a named local" and of one-variable-per-
 *   region.  It compiles to THE SAME OBJECT as v1 -- same size, same count,
 *   same 672 aligned, same 144 hunks.  The named-local lever has NO PURCHASE
 *   on a value cse1 already commons; all it can do is change WHICH values are
 *   commoned, and that only works when the naming changes the SET (v2), not
 *   when it agrees with cse's existing choice (v3).
 *
 * NEGATIVE 2 -- DECLARATION ORDER IS INERT.  v5 is v2 with declarations and
 *   assignments reordered to the reference's own r5..r11 register order, and
 *   it is byte-identical to v2.  Consistent with batch 305: if the values go
 *   through LOCAL-alloc, allocno_compare never ranked them and declaration
 *   order cannot reach them.
 *
 * NEGATIVE 3 -- THE REUSE LEVER MEASURED WORSE, and the precondition that
 *   fails is instructive.  The reference's r5 carries 0x400b and then 0x8008
 *   in two disjoint ranges, which is batch 304's signature exactly, so merging
 *   them into one declared variable looks mandatory.  It is not: v4 is +8/+3
 *   against v2's +4/+1 and loses 2.7 aligned points.  The precondition that
 *   fails is the FIRST one -- the count is not already exact.  Reuse places a
 *   value in a register another range already won; here we are still arguing
 *   about how many registers there should be, and merging removes one
 *   quantity from a set whose SIZE is what the size and count are measuring.
 *   Re-try the merge only once v2's remaining pool word is gone.
 *
 * ----------------------------------------------------------------------------
 * THE BLOCKER, ATTRIBUTED, AND WHAT RULES THE ALTERNATIVES OUT
 * ----------------------------------------------------------------------------
 * cse.c PASS 1 (dump 03.cse) picks which repeated constants become
 * call-crossing pseudos, and its choice is not reachable from the source once
 * the set of long-lived quantities is fixed.  From the -da dumps on the
 * sibling 2008c28: 00.rtl holds N independent (set (reg) (const_int K)) insns,
 * one per use site, and 03.cse leaves ONE real set plus (set (reg hard)
 * (reg pseudo)) copies carrying (expr_list:REG_EQUAL (const_int K)).  A call
 * does not invalidate cse's constant table, and this function is 807
 * instructions of straight-line script in TWO basic blocks (one branch, at the
 * pool dump), so cse1's window is effectively the entire function.
 *
 * NOT A FLAG.  cse pass 1 is unconditional at -O1, -O2 and -Os.  Measured on
 * the sibling and inert or worse on all of them: -fno-rerun-cse-after-loop,
 * -fno-gcse, both together, -fno-cse-follow-jumps, -fno-cse-skip-blocks,
 * -fno-expensive-optimizations, -fno-force-mem, -fno-caller-saves,
 * -fno-omit-frame-pointer, -O1, -Os.  The two -fcse-* switches only tune cse's
 * reach ACROSS blocks and the commoning here is INTRA-block, which is why
 * both are inert.  NO CSE_CFLAGS ROW SHOULD BE WRITTEN for this file.
 *
 * NOT sched2 and not any ordering pass: the residue is a SIZE and COUNT gap,
 * which no reordering can produce.  (sched1 does not run in this build.)
 *
 * NEXT STEP.  The remaining +4 bytes is ONE POOL WORD and the +1 is one
 * instruction; find it before touching anything else, by diffing the .word
 * list of the generated .s against the reference's `ldr rX, =` set.  On the
 * sibling 2008c28 that exact reading found a POOLED ZERO -- gcc-2.96 puts the
 * 0 of a `*(short *)p = 0` in the literal pool because *thumb_movhi_insn has
 * no immediate form -- and an int carrier (`z = 0; *p = z;`) was worth 16
 * bytes and 5 encodings.  This function has no halfword store, so the word
 * here is a different one.
 */
extern void __ActorMessage(int a, int b);
extern void __CutsceneWait(int n);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern void __PlayMapMusic(void);
extern void __PlaySound(int id);
extern void __SetFlag(int id);

extern void OvlFunc_945_200c86c(int a);
extern void OvlFunc_945_200c880(int a, int b);
extern void OvlFunc_945_200c8ac(int a, int b, int c, int d);

void OvlFunc_945_2009f3c(void)
{
    int k3000;
    int kd000;
    int kb000;
    int k5000;
    int k100b;
    int k100c;
    int k400b;
    int k8008;

    k3000 = 0xc0 << 6;
    kd000 = 0xd0 << 8;
    kb000 = 0xb0 << 8;
    k5000 = 0xa0 << 7;
    k100b = 0x100b;
    k100c = 0x100c;
    k400b = 0x400b;
    k8008 = 0x8008;
    __PlaySound(0x1c);
    __Func_80933d4(0x26666, 0x4ccc);
    OvlFunc_945_200c8ac(0xe4 << 17, -1, 0xa2 << 18, 0x10000014);
    __Func_80925cc(9, 1);
    __MessageID(0x1d93);
    OvlFunc_945_200c86c(9);
    __Func_8092adc(0, kd000, 0);
    __Func_8092adc(0xa, kd000, 0);
    __Func_8092adc(0xb, 0, 0);
    __Func_8092adc(0xc, k3000, 0);
    __Func_8092adc(0xd, 0x80 << 8, 0x28);
    __MapActor_Emote(9, 0x103, 0x28);
    __Func_809259c(9, 2);
    OvlFunc_945_200c86c(9);
    __Func_8092adc(0xc, 0, 0);
    __Func_8092adc(0xb, kd000, 0);
    __Func_8092adc(0xd, kd000, 0x14);
    __Func_80925cc(0xb, 1);
    OvlFunc_945_200c86c(k100b);
    __MapActor_Emote(0xd, 0x81 << 1, 0x14);
    __Func_809259c(0xd, 2);
    OvlFunc_945_200c86c(0xd);
    __MapActor_Emote(9, 0x105, 0x3c);
    OvlFunc_945_200c86c(9);
    __MapActor_Emote(0xc, 0x82 << 1, 0x14);
    OvlFunc_945_200c86c(0x900c);
    __Func_80925cc(8, 1);
    __MapActor_SetAnim(8, 3);
    OvlFunc_945_200c86c(8);
    OvlFunc_945_200c880(0xc, k3000);
    OvlFunc_945_200c86c(0x900c);
    OvlFunc_945_200c880(0xb, kb000);
    __MapActor_DoAnim(0xb, 3);
    __CutsceneWait(0xa);
    __Func_80925cc(0xd, 1);
    __MapActor_DoAnim(0xd, 3);
    OvlFunc_945_200c86c(0xd);
    __Func_8092adc(0xd, 0x80 << 8, 0);
    __Func_8092adc(0xc, k5000, 0);
    OvlFunc_945_200c880(0xb, k5000);
    __MapActor_SetSpeed(0xd, 0x6666, 0x3333);
    __MapActor_SetSpeed(0xc, 0xcccc, 0x6666);
    __Func_809218c(0xc, 0xde << 1, 0xa7 << 2);
    __Func_80921c4(0xd, 0xec << 1, 0xa7 << 2);
    __MapActor_WaitMovement(0xc);
    __MapActor_SetAnim(0xc, 1);
    __CutsceneWait(0x50);
    OvlFunc_945_200c880(0xc, kd000);
    __MapActor_Emote(0xc, 0x101, 0x3c);
    __Func_80925cc(0xb, 1);
    __CutsceneWait(0x14);
    __Func_8093040(k400b, 0, 0x28);
    __Func_80925cc(0xb, 2);
    __Func_8092adc(0xb, kd000, 0);
    OvlFunc_945_200c86c(k100b);
    __Func_8092adc(0xc, kd000, 0);
    __MapActor_Emote(9, 0x101, 0x3c);
    __MapActor_SetAnim(0xb, 4);
    __CutsceneWait(0x14);
    OvlFunc_945_200c86c(k100b);
    __MapActor_SetAnim(9, 3);
    OvlFunc_945_200c86c(9);
    __Func_8092adc(0xd, kd000, 0);
    __MapActor_Surprise(0xd, 0x81 << 1);
    __MapActor_Jump(0xd, 2, 0x14);
    OvlFunc_945_200c86c(0xd);
    __MapActor_DoAnim(9, 3);
    OvlFunc_945_200c86c(9);
    OvlFunc_945_200c880(0xb, kd000);
    __Func_80925cc(0xc, 1);
    OvlFunc_945_200c86c(k100c);
    __MapActor_Emote(8, 0x105, 0x28);
    __MapActor_SetAnim(8, 3);
    OvlFunc_945_200c86c(8);
    __MapActor_Emote(0xd, 0x81 << 1, 0x28);
    __MapActor_Jump(0xd, 4, 0);
    OvlFunc_945_200c86c(0xd);
    __MapActor_SetAnim(9, 3);
    OvlFunc_945_200c86c(9);
    __Func_80925cc(0xb, 1);
    OvlFunc_945_200c86c(k100b);
    __MapActor_Emote(8, 0x81 << 1, 0x28);
    __ActorMessage(8, 0);
    __Func_809259c(0xb, 2);
    __Func_8093040(k100b, 0, 0x28);
    __MapActor_Emote(9, 0x80 << 1, 0);
    __Func_8092adc(9, k5000, 0x14);
    __Func_809259c(9, 2);
    __Func_8093040(9, 0, 0x14);
    __MapActor_SetAnim(0xb, 3);
    __CutsceneWait(0x14);
    __MapActor_Emote(0xc, 0x80 << 1, 0x28);
    __Func_809259c(0xc, 2);
    OvlFunc_945_200c86c(k100c);
    __Func_8092adc(0xb, k5000, 0x14);
    OvlFunc_945_200c86c(k400b);
    __Func_809259c(0xc, 2);
    OvlFunc_945_200c86c(k100c);
    __MapActor_DoAnim(0xb, 3);
    __Func_80925cc(0xb, 1);
    OvlFunc_945_200c86c(k400b);
    __MapActor_Emote(0xc, 0x81 << 1, 0x3c);
    __Func_80925cc(9, 1);
    OvlFunc_945_200c86c(9);
    __MapActor_Emote(0xb, 0x101, 0x28);
    __Func_8092adc(0xb, kd000, 0x14);
    __MapActor_SetAnim(9, 3);
    OvlFunc_945_200c86c(9);
    __MapActor_Emote(0xb, 0x103, 0x14);
    __Func_809259c(0xb, 2);
    OvlFunc_945_200c86c(k100b);
    __MapActor_Emote(9, 0x84 << 1, 0x28);
    OvlFunc_945_200c86c(9);
    __Func_80925cc(8, 1);
    __MapActor_SetAnim(8, 3);
    OvlFunc_945_200c86c(8);
    __Func_8092adc(9, kd000, 0x28);
    OvlFunc_945_200c86c(9);
    __Func_80925cc(0xc, 1);
    __CutsceneWait(0x14);
    OvlFunc_945_200c86c(k100c);
    __Func_8092adc(0xb, k5000, 0);
    __Func_8092adc(9, k5000, 0);
    __Func_8092adc(0xd, 0x80 << 8, 0);
    __Func_8092adc(0, 0xc0 << 8, 0);
    __Func_8092adc(0xa, kb000, 0x28);
    __Func_80925cc(0xb, 1);
    OvlFunc_945_200c86c(k400b);
    __MapActor_SetAnim(0xc, 3);
    __Func_8093040(k100c, 0, 0x14);
    __Func_80925cc(9, 2);
    OvlFunc_945_200c86c(9);
    __MapActor_Emote(0xc, 0x84 << 1, 0x28);
    __MapActor_SetAnim(0xc, 3);
    OvlFunc_945_200c86c(k100c);
    __MapActor_DoAnim(8, 3);
    OvlFunc_945_200c86c(8);
    __Func_8092adc(8, 0x80 << 8, 0x14);
    __PlaySound(0x13);
    __Func_809259c(8, 2);
    __MapActor_Emote(8, 0x80 << 1, 0x50);
    OvlFunc_945_200c86c(k8008);
    __MapActor_Emote(0xc, 0x101, 0);
    __MapActor_Emote(0xb, 0x101, 0);
    __MapActor_Emote(0xd, 0x101, 0);
    __MapActor_Emote(0xa, 0x101, 0);
    __MapActor_Emote(0, 0x101, 0x28);
    __Func_8092adc(0xc, kd000, 0);
    __Func_8092adc(0xb, kd000, 0);
    __Func_8092adc(0xd, kb000, 0);
    __Func_8092adc(0xa, kb000, 0);
    __Func_8092adc(0, 0xc0 << 8, 0x28);
    __MapActor_Surprise(8, 0x81 << 1);
    __MapActor_Jump(8, 4, 0x28);
    __Func_809259c(8, 2);
    __ActorMessage(k8008, 0);
    __MapActor_SetSpeed(8, 0x19999, 0xcccc);
    __Func_80921c4(8, 0x1db, 0x256);
    __Func_8092adc(8, 0x80 << 8, 0);
    __MapActor_SetSpeed(9, 0x80 << 9, 0x80 << 8);
    __Func_80921c4(9, 0xe7 << 1, 0x26a);
    OvlFunc_945_200c880(9, kb000);
    __MapActor_Emote(9, 0x80 << 1, 0x28);
    __Func_809259c(9, 2);
    OvlFunc_945_200c86c(0x8009);
    __MapActor_Emote(0xb, 0x101, 0x3c);
    OvlFunc_945_200c86c(0xb);
    __MapActor_Emote(0xc, 0x81 << 1, 0x14);
    OvlFunc_945_200c86c(k100c);
    __MapActor_Emote(8, 0x103, 0x14);
    __MapActor_Jump(8, 4, 0);
    __Func_8092adc(8, k5000, 0x14);
    OvlFunc_945_200c86c(8);
    __PlaySound(0x1c);
    __Func_809259c(8, 3);
    OvlFunc_945_200c86c(8);
    __MapActor_Emote(0xd, 0x101, 0x3c);
    OvlFunc_945_200c86c(0xd);
    OvlFunc_945_200c880(8, k3000);
    __MapActor_DoAnim(8, 4);
    OvlFunc_945_200c86c(8);
    __Func_80921c4(0xc, 0xde << 1, 0x9d << 2);
    OvlFunc_945_200c880(0xc, kd000);
    OvlFunc_945_200c86c(0x900c);
    __Func_8092adc(8, k5000, 0x14);
    __MapActor_DoAnim(8, 3);
    OvlFunc_945_200c86c(8);
    __MapActor_Emote(0xb, 0x81 << 1, 0x3c);
    OvlFunc_945_200c86c(k100b);
    __MapActor_Emote(0xd, 0x107, 0x28);
    __Func_809259c(0xd, 2);
    OvlFunc_945_200c86c(0xd);
    OvlFunc_945_200c880(9, k3000);
    __MapActor_DoAnim(9, 4);
    OvlFunc_945_200c86c(0x1009);
    OvlFunc_945_200c880(0xc, 0);
    __Func_80925cc(8, 1);
    OvlFunc_945_200c86c(8);
    __Func_8092adc(0xb, 0, 0);
    __MapActor_Emote(0xc, 0x105, 0);
    __MapActor_Emote(9, 0x105, 0x3c);
    __Func_80933d4(0x13333, 0x2666);
    OvlFunc_945_200c8ac(0xe8 << 17, -1, 0xaa << 18, 0x80 << 21);
    __Func_80925cc(0xa, 1);
    OvlFunc_945_200c880(0xa, 0);
    OvlFunc_945_200c86c(0xa);
    __Func_8092adc(0, 0, 0);
    __MapActor_Emote(0xa, 0x81 << 1, 0x28);
    OvlFunc_945_200c86c(0xa);
    __Func_8092adc(0xa, 0x80 << 8, 0x14);
    __MapActor_Emote(0xa, 0x80 << 1, 0);
    __MapActor_Jump(0xa, 4, 0x28);
    OvlFunc_945_200c86c(0xa);
    __Func_80925cc(0xa, 1);
    OvlFunc_945_200c86c(0xa);
    __MapActor_SetSpeed(0xd, 0x80 << 9, 0x80 << 8);
    __Func_809218c(0xd, 0xdb << 1, 0x293);
    __Func_8092adc(8, 0x80 << 8, 0);
    __Func_8092adc(9, kb000, 0);
    __Func_8092adc(0xc, k3000, 0);
    __Func_8092adc(0xb, kb000, 0);
    __PlaySound(0x11);
    __MapActor_SetSpeed(0xa, 0x80 << 9, 0x80 << 8);
    __Func_80921c4(0xa, 0xf4 << 1, 0x2ae);
    __Func_8092adc(0xa, kb000, 0);
    __MapActor_WaitMovement(0xd);
    __MapActor_SetAnim(0xd, 1);
    __Func_8092adc(0xd, kd000, 0);
    __PlayMapMusic();
    __SetFlag(0x921);
}
