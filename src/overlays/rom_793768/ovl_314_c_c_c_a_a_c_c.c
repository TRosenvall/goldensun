/* Cluster OvlFunc_898_2008e0c..OvlFunc_898_2008e0c extracted from goldensun/asm/overlays/rom_793768/ovl_314_c_c_c_a_a_c.s.
 *
 * Total .text for this TU = 120 bytes (= 0x78).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_793768/ovl_314_c_c_c_a_a_c_b.o and asm/overlays/rom_793768/ovl_314_c_c_c_a_b.o in
 * goldensun/overlays/rom_793768/overlay.ld.
 *
 * Was parked at 2 of 41 across batches 108, 194 and 264 and thirty-one measured
 * spellings. THE FIX IS ONE DECLARATION:
 *
 *     extern void __CutsceneEnd(void);
 *
 * The park called it with no declaration at all, so C89 gave it an implicit `int`
 * return. Nothing else in the body changed.
 *
 * WHY A RETURN TYPE DECIDES AN ARGUMENT PAIR. The two argument movs are equal
 * priority and both ready at t=0, so rank_for_schedule falls through to "prefer
 * the insn which has more later insns that depend on it". The tail's
 * `(unspec_volatile [(return)] 1)` takes a dependence on the last SET of every
 * register:
 *
 *   - an implicitly-int callee emits `*call_value_insn` carrying
 *     `(set (reg:SI 0 r0) (call ...))`, a real SET that displaces `mov r0, #0x13`
 *     as reg_last_sets[0]. The return then depends on `mov r1, #0` but not on
 *     `mov r0, #0x13` -- four dependents against three, and r1 is scheduled first.
 *   - a void callee emits a plain `*call_insn` that only CLOBBERS r0, and a clobber
 *     goes to reg_last_clobbers, which the UNSPEC_VOLATILE handler never walks. The
 *     counts tie at four, INSN_LUID breaks the tie in source order, and the ROM's
 *     pair falls out.
 *
 * So the ARGUMENT FILL ORDER class has a cheap new first check: THE RETURN TYPE OF
 * THE NEXT CALL IN THE SAME BASIC BLOCK. `extern int __CutsceneEnd(void);` is 2 of
 * 41 again, which confirms it is the return type and not the presence of a
 * declaration. Dropping `extern void __ActorMessage(int, int);` is also 2 -- the
 * same mechanism one call earlier -- while dropping __MapActor_SetAnim's is inert,
 * because it sits in a different basic block.
 *
 * AND THE PARK'S CENTRAL CLAIM WAS FALSE. It recorded "-fno-schedule-insns2 IS
 * INERT (still 2). The swap is therefore NOT sched2 ... That rules out the whole
 * adjacent-pair toolkit at a stroke." The flag is still 2 differing, but they are
 * two DIFFERENT instructions: with sched2 off the argument pair is CORRECT and the
 * diff moves to the prologue block, where the ROM is itself scheduled. It was sched2
 * all along. Counting differences without reading them is what cost three batches.
 *
 * It also explains why the pin family could never reach this (batch 194): a pin
 * binds a register, and the ready-list choice here is made on DEPENDENT COUNTS,
 * which a pin does not change.
 */
#include "gba/types.h"
#include "actor.h"

extern void __MapActor_SetAnim(int slot, int anim);
extern void __ActorMessage(int actor, int b);
extern Actor *__MapActor_GetActor(int slot);
extern void __CutsceneEnd(void);

void OvlFunc_898_2008e0c(void)
{
    Actor *a;
    u8 *p;

    a = __MapActor_GetActor(0x13);
    p = (u8 *)a + 0x5b;
    *p = 1;
    __CutsceneStart();
    if (__GetFlag(0x855) == 0) {
        __MessageID(0x1241);
        __MapActor_SetAnim(0x13, 0);
        __CutsceneWait(2);
    } else if (__GetFlag(0x858) != 0) {
        __MessageID(0x13ab);
    } else {
        __MessageID(0x134e);
    }
    __ActorMessage(0x13, 0);
    __CutsceneEnd();
    *p = 0;
}
