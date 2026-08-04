/* Func_8092b54 @ 0x08092b54 -- asm/rom_8a000/rom_92950_a_c_c.s
 *
 * NOT SPLIT. The .s still holds both its functions and the linker script is
 * untouched; the split was made, screened, and reverted.
 *
 * Copies two fields of one map actor's sprite onto another's: the byte at
 * 0x1C wholesale, and the low ten bits of the halfword at 0x08 merged into the
 * destination's existing value.
 *
 * Blocker: REGISTER BIRTH ORDER. Twenty-five instructions against twenty-seven,
 * and every instruction is otherwise the right one. Three values are live
 * across the second MapActor_GetActor call -- the destination slot, the copied
 * byte, and the copied bits -- so three callee-saved registers are needed, and
 * both sides pick r5, r6 and r8. They disagree about WHICH:
 *
 *     rom    r8 = dst slot,  r6 = byte,     r5 = bits
 *     ours   r6 = dst slot,  r8 = byte,     r5 = bits
 *
 * r8 is a high register, so reaching it costs a `mov` at each end. Putting the
 * byte there instead of the slot costs exactly the two extra instructions:
 *
 *     rom    mov r8, r0 ... mov r0, r8
 *     ours   mov r6, r0 ... mov r0, r6 / mov r8, r2
 *
 * TRIED, all still 27 or worse:
 *   1. the byte as u8 and as s32 (no change -- ldrb either way, it is the
 *      allocation that differs, not the load)
 *   2. the two reads swapped, so `bits` is born before the byte
 *   3. the destination store hoisted to just after the pointer is computed,
 *      to shorten the byte's live range -- this is worse, 28, because it
 *      forces the merge to be recomputed after the store
 *
 * The shape of the question: gcc gives r8 to the LONGER-lived value (the byte,
 * live from the first call to the final strb) and the ROM gives it to the
 * SHORTER one (the slot, live from entry to the second call). Nothing tried so
 * far reverses that preference, and it is not obviously reachable from the C --
 * neither value is named in a way gcc is free to reorder.
 *
 * The struct below is a local minimum, not a claim. include/actor.h has
 * `void *sprite` at 0x50 and no sprite struct at all, so 0x08 and 0x1C are
 * named for what this function does with them and nothing more.
 */
#include "gba/types.h"
#include "actor.h"

struct Sprite {
    u8  pad_00[8];
    u16 attr;           /* low ten bits are the part copied */
    u8  pad_0a[0x12];
    u8  kind;           /* 0x1C */
};

extern Actor *MapActor_GetActor(s32 slot);

void Func_8092b54(s32 dst, s32 src)
{
    struct Sprite *sp;
    struct Sprite *dp;
    u8 v;
    s32 bits;
    s32 merged;

    sp = MapActor_GetActor(src)->sprite;
    v = sp->kind;
    bits = sp->attr;
    dp = MapActor_GetActor(dst)->sprite;
    merged = (dp->attr & ~0x3ff) | (bits & 0x3ff);
    dp->kind = v;
    dp->attr = merged;
}
