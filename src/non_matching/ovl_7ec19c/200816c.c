/* OvlFunc_962_200816c  [ovl_7ec19c]  and its twin OvlFunc_967_2008234 [ovl_7f21b8]
 *
 * Source asm: goldensun/asm/overlays/rom_7ec19c/ovl_30_c_a_c_a.s
 *
 * NOT SPLIT. Both splits were made, both functions screened CLEAN, both failed
 * `make compare`, and both were reverted.
 *
 * A sanctum attendant with a QUADRANT facing test rather than the range test
 * the six elevated members use:
 *
 *     (u16)((facing + 0x2000) & ~0x3fff) == 0xc000
 *
 * THE C BELOW IS INSTRUCTION-FOR-INSTRUCTION CORRECT. tryc reports OK on 39
 * against 39 for both members. Two levers were needed to get there and both are
 * worth keeping:
 *
 *   1. the mask is a NAMED `int` local -- as a literal it narrows to a halfword
 *      pool entry where the ROM has `ldr r2, =0xffffc000`;
 *   2. the mask is assigned AFTER the addition -- assigned before it, gcc loads
 *      it into r2 early and builds 0x2000 in r1, where the ROM builds 0x2000 in
 *      r2 and then REUSES r2 for the mask. Four positions apart.
 *
 * Blocker: LITERAL POOL PLACEMENT -- and as of batch 80 the mechanism is read
 * out of the compiler rather than guessed at. It is still not reachable from C.
 *
 * WHAT THE ROM DOES. It splits its literals into TWO pools:
 *
 *     0x12  ldr r2, =0xffffc000      -> pool at 0x44
 *     0x28  ldr r0, =0x96f           -> pool at 0x48
 *     0x32  ldr r0, =0x262c          -> pool at 0x4c
 *     0x40  b .L1ca                  <- barrier; pool 1 dumped here
 *     0x50  ldr r0, =0x25d5          -> pool at 0x64, AFTER the function
 *
 * gcc places a pool at the last BARRIER within reach of the pool's first entry
 * (`arm_reorg`, arm.c:5500), and stops accumulating when a fix will not fit:
 *
 *     if (fix->address >= minipool_vector_head->max_address - fix->fix_size)
 *         return NULL;                       -- add_minipool_forward_ref
 *
 * `max_address` is the referencing insn's address plus its `pool_range`. For
 * the ROM to split here, the head entry -- 0xffffc000 at 0x12 -- must have a
 * max_address of at most 0x54, so that the 0x25d5 fix at 0x50 does not fit.
 * That means its `forwards` is about 64. From arm.md:
 *
 *     *thumb_movsi_insn            pool_range 1020
 *     *thumb_movhi_insn            pool_range   64
 *     *thumb_zero_extendhisi2      pool_range   60   (prints `ldr` for a pool
 *                                                     label, not `ldrh`)
 *     *thumb_movqi_insn            pool_range   32
 *
 * SO THE ROM'S MASK LOAD IS A HImode MOVE, not an SImode one -- and a HImode
 * CONST_INT of -0x4000 is emitted into the pool as the full word 0xffffc000,
 * which is exactly what is there. That also explains why the value looks
 * 32-bit: the pool entry is always four bytes wide whatever the mode.
 *
 * WHAT WAS TRIED THIS ROUND, all byte-compared against overlays/rom_7ec19c/
 * orig.bin rather than screened, because the screen cannot see pool distance:
 *
 *     int m = ~0x3fff       104 bytes, every instruction right, ONE pool
 *     int m = -0x4000       104, identical output
 *     unsigned m = 0xffffc000   104, identical output
 *     short m = -0x4000     104, identical output (promoted to int)
 *     (u16)(f & m) == (u16)0xc000   104, identical output
 *     literal ~0x3fff        96, mask folds to 0xc000 and the compare collapses
 *     (f & m) == 0xffffc000  96, same collapse
 *     u16 f, u16 m           96, HImode throughout -- 51 bytes differ
 *     u16 f, literal 0xc000  96, same
 *     u32 f, u16 m           96, same
 *     u16 f, short m        100, worse still
 *
 * The bind is that the mask must be 0xffffc000 to survive the `(u16)` compare,
 * and any C expression that holds that value is SImode at the tree level, which
 * gives pool_range 1020 and one pool at the end of the function. Every spelling
 * that makes the AND HImode narrows the mask to 0xc000 and loses the ROM's
 * `lsl #16 / cmp` pair with it. There is no reachable middle.
 *
 * The 104-versus-108 byte gap IS the whole blocker: our instructions are right
 * and our pool is in one piece where the ROM's is in two.
 *
 * NOT TO BE RETRIED without a new idea about the MODE of the mask. Retrying
 * spellings is exhausted; eleven are recorded above and in the batch-74 note.
 *
 * Both members are recorded here rather than in two files because the C is the
 * same apart from three ids: OvlFunc_967_2008234 uses 0x9a7, 0x28fc and 0x26f6.
 */
#include "gba/types.h"
#include "actor.h"

extern void __ActorMessage(int actor, int b);
extern Actor *__MapActor_GetActor(int slot);

void OvlFunc_962_200816c(int slot)
{
    Actor *a;
    u32 f;
    int m;

    a = __MapActor_GetActor(0);
    f = a->facing;
    f += 0x80 << 6;
    m = ~0x3fff;
    if ((u16)(f & m) == 0xc000) {
        __UI_Sanctum(slot);
    } else if (__GetFlag(0x96f)) {
        __MessageID(0x262c);
        __ActorMessage(slot, 0);
    } else {
        __MessageID(0x25d5);
        __ActorMessage(slot, 0);
    }
}
