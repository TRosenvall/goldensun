/* ActorCmd_Loop @ 0x0800d710 -- asm/rom_9000/rom_d654_a_c_a_a_a_c.s
 *
 * NON-MATCHING, 24 of 38 encodings (MEASURED batch 324, brief G).
 *   SIZE 80 bytes against 80, 38 encodings against 38 -- so unlike the previous
 *   park body this IS a distance, and relocations agree (rel=ok reported by
 *   tools/sweep_variants.py; objcmp's RELOCDIFF line is the one-instruction
 *   shift below).
 *   PREVIOUS PARK: 27 of 38 with ref 38 / ours 36 and 80 bytes against 76.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_9000/rom_d710.c \
 *     asm/rom_9000/rom_d654_a_c_a_a_a_c.s --func ActorCmd_Loop
 *
 * THE RESIDUE IS ONE INSTRUCTION.  Every encoding from the ROM's index 14 to
 * its index 36 is byte-identical to ours at index 13..35; the whole positional
 * figure of 24 is the one-slot shift caused by a single missing instruction at
 * ROM index 13:
 *
 *     rom    beq .Ld742 / mov r0, r5 / add r0, #0x5d / ldrb r2, [r0] / ...
 *     ours   beq .Ld742 /             add r0, #0x5d  / ldrb r3, [r0] / ...
 *
 * WHAT THE PREVIOUS PARK GOT RIGHT, AND WHAT IT GOT WRONG.  It identified both
 * halves correctly -- a re-read of the counter byte, and an inverted branch
 * polarity -- and concluded "both halves are individually solvable; getting
 * them at the same time is what is open."  That conclusion is REFUTED: they are
 * solved at the same time by two independent edits.
 *
 *   1. CACHE THE INCREMENTED BYTE IN A u8 LOCAL.  `*counter = c = *counter + 1;`
 *      removes the re-read AND supplies the ROM's zero-extension pair
 *      `lsl r2,#24 / lsr r2,#24` -- which the park did not account for at all.
 *      A `u8` local is not known zero-extended after `add r2,#1`, so comparing
 *      it as an int forces the mask; the re-read version got the zero extension
 *      free from `ldrb` and was therefore SHORTER, which is why the park's
 *      figure hid a length deficit.
 *   2. PUT THE JUMP PATH IN THE FALL-THROUGH WITH `goto reset`.  Writing the
 *      reset as the `if` body (the park's shape) emits the reset block FIRST
 *      and inverts the branch to `blt`.  Moving the reset to a trailing label
 *      and testing `c >= (s16)count` gives the ROM's `bge .Ld74a` with the
 *      Actor_FindScriptMarker block in between.  Basic-block order is source
 *      order in gcc-2.96, so this is a source-order fact, not scheduling.
 *
 * THE REMAINING CAUSE, NAMED: `mov r0, r5` IS DELETED AS A CSELIB NO-OP SET.
 * greg puts `actor` in r5 (it crosses the call) and the counter pointer in r0;
 * `u.c.15.regmove` has `(set (reg/v:SI 43) (plus (reg/v:SI 32) (const_int 93)))`
 * and thumb's `*thumb_addsi3` has no three-operand form for an 8-bit immediate,
 * so reload must emit `mov r0,r5` then `add r0,#93`.  By `u.c.18.greg` the insn
 * already reads `(set (reg r0) (plus (reg r0) 93))` -- the copy is gone.
 * `reload1.c:7874 reload_cse_simplify` calls `reload_cse_noop_set_p` on every
 * set and deletes it when cselib says the destination already holds the source;
 * the entry `mov r5, r0` taught cselib exactly that, and nothing between them
 * writes r0.  cselib forgets everything only at a CODE_LABEL, a volatile asm or
 * a setjmp (`simplify-rtx.c:3180-3188`), and there is no CODE_LABEL between the
 * two in this layout.
 *
 * So the open question is precise: WHY DID THE ORIGINAL BUILD'S cselib NOT KNOW
 * r0 == r5 AT THAT POINT?  The ROM's own stream writes r0 nowhere between its
 * index 1 and index 13 either, so a plain "r0 was clobbered" answer does not
 * survive inspection -- which makes a CODE_LABEL present at reload time and
 * removed later the most likely answer, and that is the thing to look for next.
 *
 * MEASURED INERT (flags, all on this body; sched2 is NOT inert):
 *   -fno-expensive-optimizations .... 24 (inert; so the second
 *                                     reload_cse_regs_1 pass is not the cause)
 *   -fno-cse-follow-jumps ........... 24 (inert)
 *   -fno-schedule-insns2 ............ 27 (WORSE -- sched2 is doing real work
 *                                     here; it is what puts `strb` before the
 *                                     two shift pairs)
 *
 * MEASURED WORSE / INERT (bodies):
 *   reset as the `if` body, c cached ............... 25
 *   `u8 *counter` hoisted above the operand decode . 26
 *   `(u8 *)actor + 0x5d` instead of &->scriptLoop .. 25 (inert)
 *   `c = ++actor->scriptLoop` ...................... 25 (inert)
 *   no pointer local, three direct field accesses .. 25 (inert)
 *   a separate `Actor *a = actor;` ................. 25 (inert; copy
 *                                     propagation rewrites the uses back)
 *   `lim = (s16)count` named before the compare .... 25 (inert)
 *   jump path as the THEN arm (to force a CODE_LABEL
 *     on the bump block) .......................... 30, +8 bytes (WORSE:
 *                                     it duplicates the call site)
 *
 * A DEVICE, kept only as a figure about the blocker: adding a spurious
 * `if (count == 0) goto bump;` at the tail puts a real CODE_LABEL ahead of the
 * bump block and the `mov r0,r5` REAPPEARS -- 24 of 38 at +4 bytes. That
 * confirms the cselib mechanism above and is not a result; the extra compare is
 * not in the ROM.
 */
#include "actor.h"

extern u16 Actor_FindScriptMarker(Actor *actor, void *label);

/* Two operands: an iteration count and a target label.
 *
 * A count of 0xFFFF loops forever and always jumps. Otherwise the counter at
 * +0x5D is bumped and compared against the count; still lower and control
 * jumps back to the label, otherwise the counter resets and the cursor steps
 * past all three words. Always returns 1.
 */
s32 ActorCmd_Loop(Actor *actor)
{
    s32 *operand = (s32 *)((u8 *)actor->script + (s16)actor->scriptPos * 4 + 4);
    u32 count = *operand++;
    void *label = (void *)*operand;
    u8 *counter;
    u8 c;

    if (count != 0xffff) {
        counter = &actor->scriptLoop;
        *counter = c = *counter + 1;
        if (c >= (s16)count)
            goto reset;
    }
    actor->scriptPos = Actor_FindScriptMarker(actor, label);
    return 1;
reset:
    *counter = 0;
    actor->scriptPos += 3;
    return 1;
}
