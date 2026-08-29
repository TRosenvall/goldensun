/* Cluster OvlFunc_936_20083d8..OvlFunc_936_20083d8 extracted from goldensun/asm/overlays/rom_7c097c/ovl_30_c_c_c_a_a_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed -- the .o
 * keeps its name and its slot in goldensun/overlays/rom_7c097c/overlay.ld is
 * unchanged.
 *
 * Another facing-gated interaction: stand in the right arc and something
 * happens, otherwise you get a line of dialogue.
 *
 * THE RANGE CHECK IS SHIFTED, NOT MASKED, and that is the detail worth having:
 *
 *     ldr r2, =0xffff9fff / ldrh r3, [r0, #6] / add r3, r2
 *     ldr r2, =0x7ffe0000 / lsl r3, #16 / cmp r3, r2 / bhi .L0
 *
 * 0xffff9fff is -0x6001, so the value tested is `(u16)(facing - 0x6001)`
 * against 0x7ffe. gcc does NOT emit `lsl #16 / lsr #16` to narrow it and then
 * compare against 0x7ffe -- it shifts ONCE and compares against 0x7ffe0000,
 * the constant pre-shifted to match. Writing the cast form
 *
 *     if ((u16)(f - 0x6001) <= 0x7ffe)
 *
 * produces exactly that. The pre-shifted pool constant is gcc's doing, not
 * something the source has to spell out, so a `0x7ffe0000` appearing in a
 * comparison is a tell for a narrowing cast rather than for a literal that
 * large.
 *
 * Compare src/overlays/rom_7c5974/ovl_30_c_c_a_c_c_c_c_a.c, which has the same
 * facing test written WITHOUT a cast -- `f - 0xa001 <= 0x3ffe` on a u32 -- and
 * needs no shift at all. The difference is only whether the original narrowed
 * before comparing; the ROM shows which one it was.
 *
 * Everything else is a faithful transcription and needed no lever: no
 * declaration was required, and the branch polarity falls out of writing the
 * in-range case as the `if` body, which is where the ROM's fall-through goes.
 */
#include "gba/types.h"
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);

void OvlFunc_936_20083d8(void)
{
    Actor *a;
    u32 f;

    a = __MapActor_GetActor(0);
    f = a->facing;
    if ((u16)(f - 0x6001) <= 0x7ffe) {
        __Func_80b0278(0x17, 0x17);
    } else {
        __CutsceneStart();
        __MessageID(0x1ad1);
        __Func_8093054(0x17, 0);
        __CutsceneEnd();
    }
}
