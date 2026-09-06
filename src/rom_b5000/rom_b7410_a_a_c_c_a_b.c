/* Func_80b7aac @ 0x080b7aac  [rom_b5000]  -- "SubmitCombatantSprite"
 *
 * Source asm: goldensun/asm/rom_b5000/rom_b7410_a_a_c_c_a.s (4 functions; this
 * is the LAST).  Landing is a TWO-way split of that .s -- the target is at the
 * end, so split_s.py writes no `_c` piece:
 *
 *     python3 tools/split_s.py asm/rom_b5000/rom_b7410_a_a_c_c_a.s Func_80b7aac
 *
 *     asm/rom_b5000/rom_b7410_a_a_c_c_a_a.s   Func_80b7738, Func_80b78e4,
 *                                             Func_80b7994
 *     src/rom_b5000/rom_b7410_a_a_c_c_a_b.c   <- this file
 *
 * Both suffixes are FREE: asm/rom_b5000 and src/rom_b5000 hold _a_a_c_c_a,
 * _a_a_c_c_b, _a_a_c_c_c_a and _a_a_c_c_c_b and nothing named _a_a_c_c_a_*.
 * tools/split_asm.py reports "carries data: no", "label exports: none needed";
 * the .s has no .word/.pool/.incbin/.section at all, and the three labels this
 * function defines (.Lb7ae0, .Lb7af6, .Lb7b0a) occur only inside its own body.
 *
 * stage1.ld names the object EXACTLY ONCE, in the rom_b5000 .text list between
 * `asm/rom_b5000/rom_b7410_a_a_c_b.o(.text)` and
 * `asm/rom_b5000/rom_b7410_a_a_c_c_b.o(.text)`; that one line becomes two, in
 * the order above.  NO .rodata/.data/.bss line mentions this object -- the only
 * rom_b7410 object with a .rodata line is rom_b7410_c_c_c_c_c.o.
 *
 * NO FLAG GROUP.  No explicit or wildcard Makefile rule matches
 * src/rom_b5000/rom_b7410_a_a_c_c_a_b.c, so the generic `asm/%.o: src/%.c` rule
 * with plain GCC296_CFLAGS is what builds it -- which is what objcmp used.
 *
 * VERDICT
 *   OK Func_80b7aac -- 132 bytes, 61 encodings and 5 relocations identical
 * (objcmp against asm/rom_b5000/rom_b7410_a_a_c_c_a.s and against the
 * post-split single-function extract; both green.)
 *
 * WHAT IT DOES.  Picks the animation for a combatant's battle sprite from the
 * unit record and submits it.  hp (+0x38, s16) zero means the unit is down, so
 * the animation is 4 or 5; alive-and-afflicted (+0x13c, +0x13b or +0x145 set)
 * picks 4 or 0; otherwise the default 1 stands.  Both choices turn on the same
 * byte at +0x12a being 1.  The animation speed is 0xe + (id & 3), which
 * staggers the four party slots.
 *
 * ---------------------------------------------------------------------------
 * TWO LEVERS.  Neither came from the template.
 *
 * The template (src/rom_c9000/rom_de974_c_c_c_c_c_c_a_b.c, Anim_CuttingEdge)
 * supplied only the call shape -- `_Actor_SetAnim(*GetBattleActor(x), n)` --
 * and its own headline (caching the actor in a local across three calls) is
 * WRONG here: this ROM re-calls GetBattleActor for the second use, so nothing
 * is carried.  What the template does give is a warning, which its own text
 * carries: it declares its callees `void` and pops r0.  This ROM pops r1.
 *
 * LEVER 1 -- `4 + (x == 1)`, not `5 - (x != 1)`.
 *
 * gcc canonicalises both to the ROM's `mov r3,#5 / sub r5,r3,r5`, so the two
 * spellings agree on the arithmetic and disagree on the CONSTANT.  Written
 * `5 - (u[0x12a] != 1)` the literal 1 in the comparison is a fresh constant and
 * gcc materialises it (`mov r3, #1`), then needs a copy to land the boolean in
 * anim's register:
 *
 *     rom    ldrb r3,[r3] / eor r3,r5 / neg r2,r3 / orr r2,r3 / lsr r5,r2,#31
 *     ours   ldrb r2,[r3] / mov r3,#1 / eor r2,r2,r3 / neg r3,r2 / orr r3,r3,r2
 *            / lsr r3,r3,#31 / mov r5,r3
 *
 * Written `4 + (u[0x12a] == 1)` gcc reassociates to `5 - (x^1 != 0)` ITSELF,
 * and the 1 it needs for the `eor` is then the same RTL constant as `anim = 1`
 * -- which is live in r5 on this edge, because this block is the sole successor
 * of the entry block and CSE's extended basic block reaches into it.  So the
 * ROM's `eor r3, r5` is not a hand-shared temporary; it is CSE finding the
 * initialiser.  Note the OTHER arm keeps its own `mov r2,#1`: `.Lb7ae0` has two
 * predecessors, is not in the extended block, and cannot see the constant.
 * That asymmetry between the two arms is the tell that the 1 is shared, and it
 * is reproduced without touching that arm.
 *
 * Writing `u[0x12a] != anim` to force the sharing by hand is much WORSE (38):
 * naming the value swaps r5 and r6 across the whole function.
 *
 * LEVER 2 -- `int` return type with NO `return` statement, and both callees
 * declared `void`.
 *
 * The ROM's `pop {r1} / bx r1` says the return type is non-void (arm.c
 * thumb_exit: VOIDmode offers {r0,r1,r2} and takes r0; size<=4 offers {r1,r2}
 * and takes r1).  That much is the recorded rule.  What is NOT obvious is that
 * writing the natural `return _Actor_SetAnimSpeed(...)` also moves an
 * instruction 40 bytes earlier, at the last call site:
 *
 *     rom    mov r1,#3 / and r1,r6 / ldr r0,[r0]   / add r1,#0xe / bl
 *     ours   mov r1,#3 / and r1,r6 / add r1,r1,#14 / ldr r0,[r0] / bl
 *
 * MECHANISM, read from `-fsched-verbose=8` on the candidate:
 *
 *   - The `ldr` (insn 142) and the `add` (insn 148) both feed the call and both
 *     have INSN_PRIORITY 65: arm_adjust_cost returns 1 for any data dependence
 *     whose consumer is a CALL_INSN, so the load's latency does not buy it
 *     anything.  They tie, and they are ready in the same cycle.
 *   - rank_for_schedule then falls through priority, register weight (skipped
 *     after reload), the last-scheduled-insn class (both 3) and lands on
 *     INSN_DEPEND count.  `add` has TWO dependents, `ldr` has ONE.
 *   - The second dependent is the epilogue: `(jump_insn ... (unspec_volatile
 *     [(return)]))` takes a data dependence on the last writer of whichever
 *     register thumb_exit will pop into.  Return int -> that is r1 -> the `add`
 *     wins.  Return void -> that is r0 -> the `ldr` wins and the tie falls
 *     through to INSN_LUID, which the `ldr` also wins.
 *
 * So the epilogue's SCRATCH REGISTER decides the argument-setup order at the
 * final call.  `int` with no `return` gets r1 for the pop while leaving the
 * last call `void`, so r0 is never set by the call, the `add`'s second
 * dependent never appears, and both halves come out at once.  Declared `void`
 * the body is exact and only the two epilogue halfwords differ (2); declared
 * `int` with `return f(...)` the epilogue is exact and only the two call-setup
 * halfwords differ (2).  Neither is a codegen blocker; they are the same lever
 * seen from its two ends.
 *
 * This is a REFINEMENT, not a new finding: "gcc pops its return address into r1
 * instead of r0 when the return type is non-void, whether or not anything is
 * returned" is in docs/elevation.md's "Codegen facts that decided matches", and
 * "`pop {r1} / bx r1` names a return value -- but `return x;` is not the
 * spelling" already prescribes `int` with no `return`.  What is new is the
 * REASON to prefer it here: that entry motivates the spelling by a spurious
 * `mov r0, #0`; on this function there is no spurious mov at all and the cost
 * of `return f(...)` is paid in the SCHEDULER, two instructions before the
 * call.  The dependent-count/LUID machinery is already documented under "The
 * return-type lever's mechanism", but there for the CALLEE's return type; the
 * caller's own return type reaching the same tie-break through thumb_exit is
 * the addition.
 *
 * MEASURED (rom 132 bytes / 61 encodings, objcmp against the original .s):
 *
 *   `5 - (x != 1)` in the else arm, `return f(...)`      63 enc, 25 differ
 *   the same with `x != anim` in both arms               63, 38
 *   `anim = 5; anim -= (x != 1);`                        59, 22
 *   `int t = (x != 1); anim = 5 - t;`                    61, 57
 *   `if (hp == 0) ... else if (...)` (arms swapped)      59, 49
 *   `4 + (x == 1)`, `return f(...)`                      61, 2   (the two call
 *                                                         -setup halfwords)
 *   the same + `0xe + (id & 3)`                          61, 2
 *   the same + `unsigned id`, `(id % 4) + 0xe`           61, 2
 *   the same + the actor named in a local                61, 2
 *   the same + `int *p = GetBattleActor(id);` first      61, 2   (moves the
 *                                                         `ldr` after the `add`
 *                                                         in RTL -- inert,
 *                                                         because INSN_DEPEND
 *                                                         is compared before
 *                                                         INSN_LUID)
 *   the same + the speed named BEFORE the call           63, 10
 *   the same + `(id & 3) + anim + 0xd`                   63, 8
 *   `4 + (x == 1)` declared `void`                       61, 2   (body exact;
 *                                                         `pop {r0} / bx r0`)
 *   `4 + (x == 1)`, `int`, no `return`                   MATCH
 *
 * Flag sweep on the 2-encoding candidate, for the record: -fno-schedule-insns2
 * fixes the second call site and BREAKS the first (it hoists `ldr r0,[r0]`
 * above `mov r1, r5`), so sched2 is on in the ROM; -fno-schedule-insns,
 * -fno-rerun-cse-after-loop, -fno-gcse and -fno-strength-reduce are all inert
 * here.  Nothing needed a Makefile rule.
 *
 * OTHER NOTES
 *
 *   * The underscore prefixes are the long-call veneers, not decoration:
 *     `_GetUnit` and `_Actor_SetAnim*` are called through them from this
 *     region, `GetBattleActor` is not.  The names must be spelled exactly as
 *     the ROM's `bl` operands or the relocations differ.
 *   * struct BattleActor is the one src/rom_b5000/rom_b7410_a_c_c_c.c already
 *     declares for this region (`void *sprite` at +0);
 *     `GetBattleActor(id)->sprite` and `*(int *)GetBattleActor(id)` are
 *     byte-identical here, so the struct spelling costs nothing.
 *   * The unit record is reached through `u8 *` with literal byte offsets, the
 *     house style for _GetUnit in this tree.  The +0x38 read is `s16` because
 *     the ROM uses `ldrsh` (thumb has no immediate-offset ldrsh, hence the
 *     `mov r1,#0x38` beside it -- that is not an interleave tell).
 *   * The `sub r1,#1` / `add r1,#0xa` walking the offset from 0x13c to 0x13b to
 *     0x145 is gcc's own arithmetic on the constant, not source arithmetic:
 *     three independent `u[K]` subscripts produce it.
 */
#include "gba/types.h"

struct BattleActor {
	void *sprite;
};

extern u8 *_GetUnit(s32 id);
extern struct BattleActor *GetBattleActor(s32 id);
extern void _Actor_SetAnim(void *sprite, s32 anim);
extern void _Actor_SetAnimSpeed(void *sprite, s32 speed);

/* Declared s32 with no `return` statement: the ROM pops its return address into
 * r1, which gcc-2.96 does only when r0 is reserved for a return value, and the
 * reservation is what puts the epilogue's dependence on r1 rather than r0.
 * Nothing is actually returned.  See the header. */
s32 Func_80b7aac(s32 id)
{
	u8 *u = _GetUnit(id);
	s32 anim = 1;

	if (*(s16 *)(u + 0x38) != 0) {
		if (u[0x13c] || u[0x13b] || u[0x145])
			anim = (u[0x12a] != 1) * 4;
	} else {
		anim = 4 + (u[0x12a] == 1);
	}
	_Actor_SetAnim(GetBattleActor(id)->sprite, anim);
	_Actor_SetAnimSpeed(GetBattleActor(id)->sprite, (id & 3) + 0xe);
}
