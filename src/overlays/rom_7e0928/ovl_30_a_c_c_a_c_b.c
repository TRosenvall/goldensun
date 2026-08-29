/* Cluster OvlFunc_956_20081b4..OvlFunc_956_20081b4 extracted from goldensun/asm/overlays/rom_7e0928/ovl_30_a_c_c_a_c.s.
 *
 * Split out of that .s; the _c part stays as assembly and keeps its slot in
 * goldensun/overlays/rom_7e0928/overlay.ld, so the ROM layout does not move.
 *
 * Registers OvlFunc_956_200804c as a task at priority 0xc80.
 *
 * THE LOCAL IS LOAD-BEARING. Written as
 *
 *     __StartTask(OvlFunc_956_200804c, 0xc8 << 4);
 *
 * gcc-2.96 splits the constant's mov/lsl pair around the function-pointer
 * pool load, where the ROM keeps the pair contiguous:
 *
 *     rom    mov r1, #0xc8 / lsl r1, #4 / ldr r0, =OvlFunc_956_200804c
 *     ours   mov r1, #0xc8 / ldr r0, =OvlFunc_956_200804c / lsl r1, #4
 *
 * Assigning the shifted value to a local FIRST makes gcc finish building it
 * before it starts on the next argument. Spelling the same value as a single
 * constant (0xc80) does not work, and neither does hoisting the function
 * pointer into a local instead -- it has to be this operand.
 */
extern void OvlFunc_956_200804c(void);
extern void __StartTask(void (*fn)(void), int prio);

void OvlFunc_956_20081b4(void)
{
    int prio = 0xc8 << 4;

    __StartTask(OvlFunc_956_200804c, prio);
}
