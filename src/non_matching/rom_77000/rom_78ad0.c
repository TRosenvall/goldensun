/* Func_8078ad0 @ 0x08078ad0
 *
 * Source asm: goldensun/asm/rom_77000/rom_78a8c_c_a.s
 *
 * NOT SPLIT. The .s still holds all three of its functions and the linker
 * script is untouched.
 *
 * Looks an id up in a 0x200-entry byte table and, if the entry is non-zero,
 * forwards it (minus one) to Func_8078aa0. Fifteen instructions against
 * fifteen, and FOURTEEN OF THEM ARE IDENTICAL IN ORDER AND OPERANDS.
 *
 * Blocker: REGISTER BIRTH ORDER, and it is as small as this class gets. The
 * result register is r4 in the ROM and r3 in ours:
 *
 *     rom    mov r4, #0x0 ... mov r4, r0 ... mov r0, r4
 *     ours   mov r3, #0x0 ... mov r3, r0 ... mov r0, r3
 *
 * Nothing else differs. Both are scratch registers here -- r4 is caller-saved
 * in this tree (`-fcall-used-r4`), which the ROM confirms by using it without
 * pushing it -- so this is a pure allocation preference. After the `ldrb` both
 * r3 and r4 are free and gcc reaches for r3 first.
 *
 * TRIED, all six byte-identical to each other:
 *   1. the result initialised before the mask vs after
 *   2. early `return 0` instead of a result variable
 *   3. `if (v)` vs `if (v != 0)`
 *   4. the result declared before and after the other locals
 *   5. the mask as a named u32 local
 *   6. the mask as an inline literal
 *
 * ONE THING WORTH TAKING FROM (5) AND (6). The narrow_constant lever --
 * naming a mask so gcc emits a 32-bit pooled constant rather than narrowing it
 * -- IS NOT NEEDED HERE. The plain literal `id & 0x1ff` already gives
 * `ldr r3, =0x1ff`, because `id` is a u32 and the result indexes an array, so
 * gcc has no width to narrow to. The lever is specific to masking a value gcc
 * knows is narrower than a word, such as a u16 struct field -- see
 * src/non_matching/rom_15000/rom_1c154.c. Reaching for it by reflex on any
 * pooled mask would be wrong.
 *
 * The table is a `.L` data label in another object, bound with a gcc asm-label
 * so C can name it; that part reproduces exactly.
 */
#include "gba/types.h"

extern u8 L7b490[] __asm__(".L7b490");
extern s32 Func_8078aa0(s32 n);

s32 Func_8078ad0(u32 id)
{
    u32 v;
    s32 r;

    v = L7b490[id & 0x1ff];
    r = 0;
    if (v != 0)
        r = Func_8078aa0(v - 1);
    return r;
}
