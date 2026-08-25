/* OvlFunc_882_200c5b8  [ovl_77dd1c]  --  0x0200c5b8
 *
 * Source asm: goldensun/asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_c_c_c_a.s
 *
 * Copies two sprite-flag bits (mask 0xc) from the player's sprite onto the
 * sprites of actors 0x16 and 8. Thirty instructions in the ROM; the best
 * attempt here is thirty-six.
 *
 * Blocker: REGISTER PRESSURE -- gcc parks BOTH masks in callee-saved registers
 * where the ROM rematerialises the positive one.
 *
 *   rom    live across the calls: r6 = the source sprite, r5 = ~0xc.
 *          0xc is rebuilt with `mov r2,#0xc` / `mov r3,#0xc`, once per block.
 *   ours   live across the calls: the source sprite, 0xc AND ~0xc -- three
 *          values.
 *
 * That third value is what costs the six instructions, and it costs them
 * twice over. This tree builds with `-fcall-used-r4`, so r4 is call-clobbered
 * and only r5 and r6 are cheaply available; the third value goes to r8, which
 * is a HIGH register, so the prologue grows `mov r6,r8 / push {r6}` and the
 * epilogue `pop {r3} / mov r8,r3`, and every use of it needs a `mov` down into
 * a low register first.
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
 * TRIED, all still spilling to r8:
 *   1. `int m = ~0xc;` named once, 0xc as a literal in both blocks   36 vs 30
 *   2. the or-operands swapped, `(m & q->flags) | (0xc & p->flags)`  37 vs 30
 *   3. `-0xd` written inline instead of a named m                    36 vs 30
 *   4. the `0xc & p->flags` term hoisted into a per-block temp       38 vs 30
 *   5. that term cast to `unsigned char` to shorten its live range   37 vs 30
 *   6. `--no-rerun-cse` (CSE_CFLAGS), on the theory that the hoist   36 vs 30
 *      is the same post-loop CSE that batch 25 turned off
 *
 * None of them moves the constant. The lever this needs is one that makes gcc
 * REMATERIALISE a cheap constant rather than keep it live -- the exact inverse
 * of the constant-CSE class, where the problem is gcc hoisting a value the ROM
 * also hoists but paying a push/pop the ROM does not. No such lever is known
 * here yet, and if one is found this is the function to test it on: the
 * diagnosis is unambiguous and everything else about the translation matches.
 *
 * Not a bitfield. A `unsigned b : 2` copy would emit the shift pair that turns
 * a bit range into a value and back; the ROM has no shifts at all, so this is
 * a straight masked merge.
 *
 * SETTLED, batch 42, by reading the compiler rather than probing it.
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
 * and not of the source. The only other pass that could do it is `combine`, and
 * combine can only fold a constant into its consumer if the target takes it as
 * an immediate, which a two-instruction constant does not.
 *
 * So this is NOT waiting on a construct that has not been found. In plain C it
 * is unreachable, and register pinning -- a fakematch -- is the only way
 * through. See docs/elevation.md and reports/fakematch-worklist.md.
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
