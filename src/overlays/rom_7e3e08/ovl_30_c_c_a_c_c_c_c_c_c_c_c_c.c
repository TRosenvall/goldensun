/* OvlFunc_957_2008f10 -- MATCHING, 0 of 44 encodings.
 *
 * Pins: 0.  Devices: none.  Flag group: none (production flags).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_c_c_c.c \
 *     asm/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_c_c_c.s --whole
 *
 * SPLIT SHAPE: none needed.  The piece holds this function alone
 * (`grep -c '^\.thumb_func_start'` is 1) and tools/datacheck.py reports no
 * data section under it, so the hand .s is deleted and this .c takes its
 * place at the same link slot.
 *
 * WHY IT MATCHES, AND WHAT THE PARK HAD WRONG.
 *
 * The park (src/non_matching/ovl_7e3e08/2008f10.c) read this as the
 * "dead callee-saved register" shape and concluded the reference's fifth
 * callee-saved register is prologue bookkeeping "for a value the C has no way
 * to demand".  Its observations were right and its verdict was wrong: the
 * register is demandable, and the lever is WHERE the third constant is
 * computed, not whether it is named.
 *
 * find_reg (global.c:967) copies `used1` from `call_used_reg_set` whenever
 * `allocno[num].calls_crossed != 0` (global.c:992-995), so a pseudo whose live
 * range crosses a call cannot be given r0-r3 -- nor r4, which this build makes
 * call-clobbered with -fcall-used-r4.  The reference keeps `0x90 << 16` in r7
 * for that reason alone.  Computed after both calls, as the park wrote it, the
 * value crosses nothing, takes r3, and r7 never enters regs_ever_live -- which
 * is why the prologue came out one register short and every push/pop line
 * differed.  Named up here with the other two constants it crosses both calls,
 * takes the one callee-saved register left after r5 (the vector base) and r6
 * (the actor), and sched2 then sinks the `mov`/`lsl` pair down to its use,
 * which is where the reference computes it.
 *
 * Computing it BETWEEN the two calls (crossing only the second) also matches
 * byte-for-byte, so the requirement is "crosses a call", not "crosses both".
 *
 * The park's other finding is load-bearing and is kept: the first two
 * constants must also be named BEFORE the first call.  Written inline the
 * function comes out nine encodings SHORT, because nothing demands r8-r11 and
 * all of their save/restore bookkeeping is absent.
 */
#include "gba/types.h"
#include "actor.h"

extern struct Actor *__MapActor_GetActor(int slot);
extern void __vec3_translate(int a, int b, int *v);

void OvlFunc_957_2008f10(int slot, int b, int c)
{
    struct Actor *a;
    int v[3];
    int k1, k2, k3;

    k1 = 0xfc << 17;
    k2 = 0xc0 << 13;
    k3 = 0x90 << 16;
    a = __MapActor_GetActor(slot);
    v[0] = k1;
    v[2] = k2;
    __vec3_translate(b, c, v);
    a->pos.x = v[0];
    a->pos.y = v[2];
    a->pos.z = k3;
}
