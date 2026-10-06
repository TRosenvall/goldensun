/* OvlFunc_882_200c5b8  [ovl_77dd1c]  --  0x0200c5b8
 *
 * NON-MATCHING, 36 of 31 encodings  (MEASURED batch 330, objcmp --func).
 *   THE LENGTH ARITHMETIC, DONE EXACTLY (batch 330):
 *     INSTRUCTION COUNT  ref 27, ours 34     POOL WORD COUNT  ref 3, ours 3
 *     one trailing alignment pad on the reference side, none on ours
 *     SIZE               ref 68 bytes, ours 80
 *   So the thirty-one and the thirty-seven in the encodings line are 27+3+1 and
 *   34+3+0.  The figure exceeds the reference length because the streams are
 *   SEVEN INSTRUCTIONS apart, so it is a MISALIGNMENT reading and not a
 *   distance.  The pad and the pool words account for only one entry of the
 *   gap; this is NOT the padding trap.  aligncmp puts fourteen encodings
 *   aligned-equal of thirty-one.  The reference is TWENTY-SEVEN INSTRUCTIONS,
 *   not the thirty this header used to say, and this body is THIRTY-FOUR.
 *
 *   The relocation difference is the same three THM_CALLs to the same symbol at
 *   shifted offsets -- a consequence of the length difference, not a separate
 *   blocker.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_77dd1c/200c5b8.c \
 *     asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_c_c_c_a.s --func OvlFunc_882_200c5b8
 *
 * Source asm: goldensun/asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_c_c_c_a.s
 *
 * Copies two sprite-flag bits (mask 0xc) from the player's sprite onto the
 * sprites of actors 0x16 and 8.
 *
 * Blocker: REGISTER PRESSURE -- gcc parks BOTH masks in callee-saved registers
 * where the ROM rematerialises the positive one.
 *
 *   rom    live across the calls: r6 = the source sprite, r5 = ~0xc.
 *          0xc is rebuilt with `mov r2,#0xc` / `mov r3,#0xc`, once per block.
 *   ours   live across the calls: the source sprite, 0xc AND ~0xc -- three
 *          values.
 *
 * That third value is what costs the seven instructions, and it costs them
 * twice over. This tree builds with `-fcall-used-r4`, so r4 is call-clobbered;
 * the third value goes to r8, which is a HIGH register, so the prologue grows
 * `mov r6,r8 / push {r6}` and the epilogue `pop {r3} / mov r8,r3`, and every
 * use of it needs a `mov` down into a low register first -- three of those.
 *
 * CONFIRMED IN BATCH 330, STRAIGHT OUT OF THE DUMPS (this is the strongest
 * evidence in the park and it is cheap to re-check with `-da`):
 *   * `.17.lreg` says "1 basic blocks, 2 edges" and tags EVERY pseudo
 *     "in block 0".  The three call-crossing pseudos are named there: the
 *     source pointer, "crosses 2 calls"; the negative mask, "crosses 1 call";
 *     and a third pseudo holding the positive mask, "crosses 1 call".
 *   * `.18.greg` says ";; 0 regs to allocate:" -- global allocation never runs
 *     on this function at all, every pseudo is local-alloc's.
 *   This settles the local-alloc argument below as a FACT about this function
 *   rather than an inference: `update_equiv_regs` needs REG_BASIC_BLOCK < 0,
 *   and with one basic block every pseudo has REG_BASIC_BLOCK == 0.
 *
 * SETTLED and worth keeping in any further attempt:
 *
 *   - MASK OPERAND ORDER. `m & q->flags`, not `q->flags & m` -- the ROM's
 *     combine is `and r3, r2` with the mask as destination. Same finding as
 *     src/non_matching/ovl_7b4558/20089f4.c, which is the same 0xc mask on the
 *     same +9 byte of the same sprite struct.
 *   - THE NEGATIVE MASK IS A NAMED int. `int m = ~0xc;` gives the ROM's
 *     `mov r5,#0xd / neg r5,r5`; written inline gcc narrows it to the byte
 *     immediate 0xf3 and the pair disappears.
 *   - THE SOURCE BYTE IS RE-READ per block. The ROM loads [r6,#9] twice, once
 *     in each block, which is what a plain `p->flags` on each side gives --
 *     the intervening call clobbers memory so gcc cannot lift it either.
 *
 * TRIED, all still spilling to a high register (figures are the old
 * misalignment readings; the measurement that matters is that none of them
 * changes the emitted shape at all):
 *   1. `int m = ~0xc;` named once, 0xc as a literal in both blocks
 *   2. the or-operands swapped, `(m & q->flags) | (0xc & p->flags)`
 *   3. `-0xd` written inline instead of a named m
 *   4. the `0xc & p->flags` term hoisted into a per-block temp
 *   5. that term cast to `unsigned char` to shorten its live range
 *   6. `--no-rerun-cse` (CSE_CFLAGS), on the theory that the hoist is the same
 *      post-loop CSE that batch 25 turned off
 *
 * MEASURED INERT, batch 330 -- four more spellings of the positive mask, on the
 * theory that two textually distinct 0xc rtxes might survive commoning:
 *   7. `~m` in both blocks instead of the literal
 *   8. `0xc` in one block and `0x0c` in the other
 *   9. a second named local `m2 = ~0xc` for the second block
 *  10. per-block named locals for BOTH masks, re-assigned before each use
 * All four are byte-for-byte identical to the body below.  Spelling the
 * constant is now a closed dimension: ten attempts, zero movement.
 *
 * None of them moves the constant. The lever this needs is one that makes gcc
 * REMATERIALISE a cheap constant rather than keep it live -- the exact inverse
 * of the constant-CSE class.
 *
 * Not a bitfield. A `unsigned b : 2` copy would emit the shift pair that turns
 * a bit range into a value and back; the ROM has no shifts at all, so this is
 * a straight masked merge.
 *
 * SETTLED, batch 42, by reading the compiler rather than probing it, and
 * re-confirmed from the dumps in batch 330 (see above).
 *
 * gcc-2.96 rebuilds a constant at its use instead of keeping it live in exactly
 * one place -- `update_equiv_regs` in local-alloc.c -- and only when BOTH of
 * these hold:
 *
 *     REG_N_REFS (regno) == 2        set once, used exactly once
 *     REG_BASIC_BLOCK (regno) < 0    the pseudo spans MORE THAN ONE basic block
 *
 * A straight-line function has one basic block, so the second condition can
 * never hold, whatever the C says -- it is a property of the control-flow graph
 * and not of the source, and the ROM's own function is branchless too, so no
 * source that matches can have a second block. The only other pass that could
 * do it is `combine`, and combine can only fold a constant into its consumer if
 * the target takes it as an immediate, which a two-instruction constant does
 * not, and which thumb's `and` never does.
 *
 * THE ONE DIMENSION THIS PARK HAS NEVER VARIED -- r7 (new, batch 330, OPEN).
 * REG_ALLOC_ORDER (arm.h:989-995) is 3,2,1,0,12,14,4,5,6,**7**,**8**,...  r7
 * comes BEFORE r8, so the third value went high because r7 was EXCLUDED, not
 * because it was ranked lower.  A synthetic thumb function needing seven
 * callee-saved values also skipped r7 and went straight to the high registers.
 * Against that: the tree holds 1,072 gcc-generated `push {r5, r6, r7, lr}`
 * prologues, and at least some of those r7s are the scratch the prologue uses
 * to save a high register rather than an allocated value -- the first one
 * inspected, Func_80dfddc in asm/rom_c9000, is exactly that case.  So whether
 * r7 is allocatable AS A VALUE under these flags is UNRESOLVED.  If it is, the
 * third mask fits in a low register and five of the seven extra instructions
 * disappear; if it is not, this function is a fakematch and the local-alloc
 * argument above is the final word.  That question is one `-da` run on a
 * four-callee-saved-value probe, and it is the next thing to do here -- not
 * another spelling of the mask.
 *
 * Until it is answered, register pinning -- a fakematch -- is the only known
 * way through. See docs/elevation.md and reports/fakematch-worklist.md.
 */

struct Spr { unsigned char pad_00[9]; unsigned char flags; };

extern void *__MapActor_GetActor(int slot);

#define SPRITE_OF(n) (*(struct Spr **)((unsigned char *)__MapActor_GetActor(n) + 0x50))

void OvlFunc_882_200c5b8(void)
{
    struct Spr *p, *q, *r;
    int m;

    p = SPRITE_OF(0);
    q = SPRITE_OF(0x16);
    m = ~0xc;
    q->flags = (0xc & p->flags) | (m & q->flags);
    r = SPRITE_OF(8);
    r->flags = (0xc & p->flags) | (m & r->flags);
}
