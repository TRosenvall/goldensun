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
