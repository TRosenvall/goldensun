/* OvlFunc_956_20081b4  [ovl_7e0928]
 * Source asm: goldensun/asm/overlays/rom_7e0928/ovl_30_a_c_c_a_a.s
 *
 * Blocker: INTERLEAVED ARGUMENT SET-UP again, but note the direction. Seven
 * instructions against seven:
 *
 *     rom    mov r1, #0xc8 / lsl r1, #4 / ldr r0, =OvlFunc_956_200804c
 *     ours   mov r1, #0xc8 / ldr r0, =OvlFunc_956_200804c / lsl r1, #4
 *
 * This is the MIRROR of the usual case. Everywhere else in this corpus the
 * ROM interleaves and gcc emits the pair contiguously; here the ROM is
 * contiguous and gcc interleaves. Same two instructions, opposite preference.
 *
 * Taken with Func_80167ac -- where the ROM reuses a constant and gcc does not,
 * the reverse of the constant_reuse family -- that makes two independent
 * places where the divergence runs the opposite way from the documented
 * pattern. Whatever governs these choices is not a single global disposition
 * of one compiler versus the other, and any explanation that only accounts for
 * one direction is incomplete.
 */
extern void __StartTask(void *task, int priority);
extern void OvlFunc_956_200804c(void);

/* Starts the overlay's main task at priority 0xC80. */
void OvlFunc_956_20081b4(void)
{
    __StartTask(OvlFunc_956_200804c, 0xc8 << 4);
}
