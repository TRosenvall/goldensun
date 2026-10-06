/* Cluster OvlFunc_930_2009060..OvlFunc_930_2009060 extracted from goldensun/asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_c_c_c.s.
 *
 * MATCHES.  0 differing encodings of 23; whole-file clean, 48 bytes, and the
 * one relocation identical.  PINS: 0 (no inline asm, no shim, no per-file flag).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_c_c_c.c asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_c_c_c.s --whole
 *
 * SPLIT SHAPE: none.  datacheck.py prints nothing for the reference and the .s
 * holds exactly one .thumb_func_start, so the file is already one TU.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_c_c_b.o and
 * asm/overlays/rom_7b7f1c/ovl_30_c_c_b.o in
 * goldensun/overlays/rom_7b7f1c/overlay.ld.
 *
 * WHAT IT DOES.  The reference's own comment says it adjusts slot 0 directly
 * and touches +0xc.  With include/actor.h that reads: compare the player's
 * pos.y (0x0c) against this actor's, and set or clear Actor.flags bit 1 --
 * the bit actor.h documents as shifting the sprite by -0x140.0000.  So it is a
 * depth test driving the sprite-offset flag.
 *
 * ================= HOW THE PARK'S 11 CAME APART =================
 *
 * The park read "register allocation, plus operand order on the ORR", and
 * concluded "That is allocation, not spelling: r0 holds the incoming parameter
 * and gcc has no reason to move it."  The observation was exact and the verdict
 * was wrong.  Instruction counts agreed (22 and 22) and stream lengths agreed
 * (23 and 23), so that figure WAS a true distance -- a cascade, not a phase
 * shift -- and it decomposed into three facts and then into two source changes:
 *
 *   pop {r1} / bx r1   against ours pop {r0} / bx r0            2 encodings
 *   the pointer walked in r1 against ours in r0                 6 encodings
 *   orr r3,r2 and and r3,r2 against ours orr r2,r3 / and r2,r3,
 *     and the strb that inherits it                             3 encodings
 *
 * 1. THE RETURN TYPE IS NOT void.  thumb_exit (arm.c:8290-8320) takes
 *    size = GET_MODE_SIZE (mode) from DECL_RESULT and only
 *    `size == 0 && mode == VOIDmode` offers
 *    ARG_REGISTER(1)|ARG_REGISTER(2)|ARG_REGISTER(3) -- r0, r1, r2.  A
 *    `size <= 4` return offers ARG_REGISTER(2)|ARG_REGISTER(3) -- r1 and r2
 *    only.  So `pop {r1}` is a DECLARATION, not an allocation accident.  `int`
 *    with no return statement at all is the spelling (docs/elevation.md,
 *    "`pop {r1} / bx r1` names a return value -- but `return x;` is not the
 *    spelling").  That alone took the park's body from eleven to nine.
 *
 * 2. THE BODY IS A STRUCT MEMBER READ-MODIFY-WRITE.  `actor->flags |= 2` in
 *    one arm and `&= 0xfd` in the other, with the store written inside each arm
 *    and merged by cross-jumping, gives the ROM's
 *        mov r1, r5 / add r1, #0x23 / ldrb r2, [r1] / mov r3, #2 / orr r3, r2
 *    and the single `strb r3, [r1]` tail.  The pointer register and the ORR's
 *    operand order are the SAME fact: both follow from the aggregate access.
 *
 * WHY THE POINTER WANTED r0 -- the part worth keeping.  Measured in .18.greg on
 * the park's own body: `;; 34 preferences: 0`.  The pointer pseudo inherits a
 * preference for r0 from the PARAMETER pseudo, because expand_preferences
 * (global.c:828-868) merges preferences between two allocnos when one dies in
 * an insn that sets the other and they do not conflict -- which is exactly what
 * `p = a` is.  prune_preferences (global.c:889-910) would have stripped every
 * call-used register from that set, but only
 * `if (allocno[num].calls_crossed != 0)`, and the pointer is created after the
 * call, so it keeps r0.  find_reg then applies the preference twice, at
 * global.c:1067-1090 (copy preferences) and :1103 (plain preferences).
 * Independently, r1 was unreachable in find_reg's pass 0:
 * `IOR_COMPL_HARD_REG_SET (used, regs_used_so_far)` (global.c:1013) excludes
 * every register not already used somewhere in the function, and nothing else
 * in the park's output used r1.  So r1 only becomes reachable once the body
 * itself puts a value there.
 *
 * MEASURED INERT at 9, all with the int return type in place: `p = a + 0x23`;
 * a parameter typed `unsigned char *` with no local `a`; `p = &a[0x23]`;
 * `v = t | 2` / `v = t & 0xfd` (operand reversal); a second unused parameter.
 * MEASURED WORSE at 11: `v = 2; v |= t;` -- which does tie the destination to
 * the constant, the way the ROM has it, but then swaps t and the constant
 * between r2 and r3.  MEASURED LONGER: the store inside each arm with a plain
 * pointer, 24 instructions of 22; `a[0x23] |= 2` on a plain
 * `unsigned char *`, 23 of 22.  Only the aggregate member access is right.
 * Three spellings of it reach 0 -- this one, a `void *` parameter cast to
 * `Actor *`, and `&= ~2` for `&= 0xfd` -- so the lever is the member access,
 * not the constant or the parameter type.
 */
#include "gba/types.h"
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);

int OvlFunc_930_2009060(Actor *actor)
{
    if (__MapActor_GetActor(0)->pos.y > actor->pos.y)
        actor->flags |= 2;
    else
        actor->flags &= 0xfd;
}
