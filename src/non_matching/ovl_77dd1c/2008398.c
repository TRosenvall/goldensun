/* OvlFunc_882_2008398  [ovl_77dd1c] and two siblings
 *
 * Source asm: goldensun/asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_a.s
 * Siblings:   OvlFunc_882_20083cc, OvlFunc_882_2008400 -- same shape, two
 *             constants different each (0x4a/0x49/0x48 and 0xa/0xc/0xd).
 *
 * Blocker: ARGUMENT FILL ORDER. Seventeen instructions against seventeen,
 * one pair transposed:
 *
 *     rom    mov r1, #0x66 / ldr r2, =0x4b6
 *     ours   ldr r2, =0x4b6 / mov r1, #0x66
 *
 * The ROM builds the immediate argument before the pooled one; gcc emits the
 * pool load first. Naming both as locals does not reorder them -- it costs
 * two instructions instead (19 vs 17).
 *
 * Same class as Func_8078948 and LoadStatusIcon in the main ROM, where the
 * ROM likewise defers a register move that gcc does early. Now recognised by
 * tools/elevation_candidates.py as "arg-fill-order", which it was not when
 * this family was picked -- that is the fifth blocker added to the filter
 * after it cost a round rather than before.
 *
 * CHARACTERISED PROPERLY 2026-08-03, after two more failed attempts (0x4b6
 * declared as a symbol, with and without a prototype on __Func_809218c).
 *
 * The class is NOT "the ROM builds an immediate before a pooled argument".
 * That shape is everywhere -- 2242 sites in the remaining hand-written asm --
 * and gcc produces it happily. Eighteen sites in gcc's own honest output are
 * exactly that, e.g.
 *
 *     __Func_801776c(0x1528, 1)   ->   mov r1, #1 / ldr r0, .L3
 *
 * where the POOLED operand is the FIRST argument. gcc fills r0 last for an
 * implicitly declared callee, so the immediate lands first for free. Counting
 * the shape and calling it a blocker would have been wrong by two orders of
 * magnitude.
 *
 * What actually differs here is the order among the NON-r0 arguments:
 *
 *     rom    mov r1, #0x66 / ldr r2, =0x4b6 / mov r0, #0     r1 then r2
 *     ours   ldr r2, =0x4b6 / mov r1, #0x66 / mov r0, #0     r2 then r1
 *
 * Both fill r0 last, so the declaration lever -- which decides only where r0
 * goes -- cannot reach this, and testing confirms it does not. gcc orders the
 * remaining arguments by operand kind, pool first; the ROM orders them by
 * register number.
 *
 * No formulation has changed that ordering. Whatever does, it is not
 * declaration state, not symbol-ness of the pooled value, and not statement
 * order.
 */
extern void __PlaySound(int id);
extern void __Func_8010560(void *data, int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __CutsceneWait(int frames);
extern void OvlFunc_882_200815c(int arg);

extern unsigned char Data_578a[] __asm__(".L578a");

void OvlFunc_882_2008398(void)
{
    __PlaySound(0x9e);
    __Func_8010560(Data_578a, 0x23, 0x4a);
    __Func_809218c(0, 0x66, 0x4b6);
    __CutsceneWait(3);
    OvlFunc_882_200815c(0xa);
}
