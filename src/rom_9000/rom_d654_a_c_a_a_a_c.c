/* ActorCmd_Loop @ 0x0800d710 -- asm/rom_9000/rom_d654_a_c_a_a_a_c.s
 *
 * MATCHING.  0 differing encodings of 38, NO PINS, NO DEVICES, NO FLAGS.
 *   80 bytes against 80, 37 instructions against 37, 1 relocation identical.
 *   --whole agrees: "OK whole file -- 80 bytes, 38 encodings and 1 relocations identical".
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_9000/rom_d654_a_c_a_a_a_c.c asm/rom_9000/rom_d654_a_c_a_a_a_c.s --func ActorCmd_Loop
 *
 *   SPLIT SHAPE: none needed.  The reference holds exactly one
 *   .thumb_func_start (ActorCmd_Loop) and datacheck.py on it is CLEAN
 *   (exit 0, no output -- no data section, no exports).  stage1.ld:177 names
 *   asm/rom_9000/rom_d654_a_c_a_a_a_c.o(.text), so the install path is
 *   src/rom_9000/rom_d654_a_c_a_a_a_c.c with no split_s.py run.
 *
 * HOW THIS LANDED, and it is two levers crossed.
 *
 * LEVER 1 (batch 326, brief G) -- THE 0xffff ARM HAS ITS OWN COPY OF THE CALL.
 * The ROM's label `.Ld742` sits AFTER the second `mov r0, r5`, which no
 * single-call shape can produce.  Writing the call twice, 0xffff arm first,
 * reproduces it because the three passes run in this order:
 *   1. reload emits `mov r0,r5` in BOTH arms;
 *   2. `reload_cse_regs` (toplev.c:3369) -> `reload_cse_simplify`
 *      (reload1.c:7874) deletes the 0xffff arm's copy, because cselib still
 *      knows r0 == r5 from the entry `mov r5,r0` and forgets only at a
 *      CODE_LABEL / volatile asm / setjmp (simplify-rtx.c:3180-3188) -- while a
 *      CODE_LABEL protects the counter arm's copy;
 *   3. cross-jumping (toplev.c:3515) merges the two arms' common tail
 *      `bl / strh / b`, stopping short of the surviving `mov r0,r5`;
 *   4. `jump_optimize` collapses the remainder to `beq .Ld742`.
 * So the park's open question -- "why did the original build's cselib not know
 * r0 == r5?" -- had the wrong subject.  cselib DID know, in the arm that got
 * deleted; the copy that survives is the one a CODE_LABEL protected.
 *
 * LEVER 2 (this batch) -- `c = ++*counter;` INSTEAD OF `*counter = c = *counter + 1;`.
 * That closed lever 1's remaining run of five, indices 15..19:
 *     rom   ldrb r2,[r0] / add r2,#1 / strb r2,[r0] / lsl r3,r4,#16 / lsl r2,#24
 *     ours  ldrb r3,[r0] / add r3,#1 / lsl r2,r3,#24 / strb r3,[r0] / lsl r3,r4,#16
 * The cause is the ORDER THE SOURCE GENERATES, not the scheduler.  In
 * `u.c.15.regmove` for the embedded-assignment body the zero-extend pair
 * (insns 76, 77) is emitted BEFORE the store (insn 80), because C evaluates
 * `c = *counter + 1` -- truncation to u8 and the widening back out -- before the
 * outer store to `*counter`.  Three pseudos result: 54 the byte, 55 the SImode
 * sum that the `strb` stores, 56 the shifted temp.  55 and 56 then get different
 * hard registers (r3 and r2), so the `lsl` carries a register move, has no WAR
 * anti-dependence against the `strb`, and sched2 is free to hoist it.
 * `c = ++*counter;` makes the increment-and-store the whole statement and the
 * widening a property of the USE in the compare, so the store is emitted first
 * and the zero-extend runs IN PLACE on the stored register -- `lsl r2,#24`, the
 * two-operand form, which is what the ROM has.
 *
 * SO THE PARK'S "FIX THE REGISTER SHARING AND THE SCHEDULE FOLLOWS" IS RIGHT
 * ABOUT THE EFFECT AND WRONG ABOUT THE REACH: the sharing is not an allocator
 * outcome to be steered, it is downstream of which statement emits the store.
 *
 * TWO FIGURE CORRECTIONS TO THE RETIRED PARK, both worth keeping:
 *   * its "24 of 38 ... this IS a distance" was NOT a distance.  The ROM is
 *     37 instructions + 1 pool word; that body was 36 instructions + a
 *     mid-stream `.short 0` pad + 1 pool word.  Equal bytes, equal encodings,
 *     ONE INSTRUCTION APART.  That is a fourth variant of the padding trap, and
 *     objcmp now catches it (it prints `INSTRUCTION COUNT ref 38, ours 37`).
 *   * its whole "MEASURED WORSE / INERT" list was measured on the single-call
 *     body and does not transfer.  `c = ++*counter` is recorded there as
 *     "25 (inert)"; crossed with lever 1 it is EXACTLY ZERO.
 *
 * ALSO BYTE-IDENTICAL, measured this batch (kept because they show the lever is
 * about STATEMENT STRUCTURE, not about the type of `c`):
 *   `u32 c; *counter = c = *counter + 1; if ((u8)c < (s16)count)` ...... 0
 *   `int c; *counter = c = *counter + 1; if ((u8)c < (s16)count)` ...... 0
 * Both need an explicit `(u8)` at the compare to put the widening on the use;
 * `c = ++*counter` gets there with no cast, so it ships.
 *
 * MEASURED, all with lever 1 in place (so these are crosses, not singles):
 *   `u8 c; c = *counter + 1; *counter = c;` (split store) ......... 4
 *   the same with block-scope declarations .......................... 4
 *   declaration order `u8 c; u8 *counter;` .......................... 5 (inert)
 *   `actor->scriptLoop = c = *counter + 1;` ......................... 5 (inert)
 *   `lim = (s16)count` named before the compare ..................... 5 (inert)
 *   `(u8 *)actor + 0x5d` instead of &->scriptLoop ................... 5 (inert)
 *   `if ((s16)count > c)` (compare operands swapped) ................ 7
 */
#include "actor.h"

extern u16 Actor_FindScriptMarker(Actor *actor, void *label);

s32 ActorCmd_Loop(Actor *actor)
{
    s32 *operand = (s32 *)((u8 *)actor->script + (s16)actor->scriptPos * 4 + 4);
    u32 count = *operand++;
    void *label = (void *)*operand;
    u8 *counter;
    u8 c;

    if (count == 0xffff) {
        actor->scriptPos = Actor_FindScriptMarker(actor, label);
    } else {
        counter = &actor->scriptLoop;
        c = ++*counter;
        if (c < (s16)count) {
            actor->scriptPos = Actor_FindScriptMarker(actor, label);
        } else {
            *counter = 0;
            actor->scriptPos += 3;
        }
    }
    return 1;
}
