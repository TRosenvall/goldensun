/*
 * Actor_SetAnimAndSpeed  --  asm/rom_9000/rom_c004_c_a_a_a_a_c_a_c.s
 *
 * BLOCKER: argument emission order, one adjacent pair. 51 lines against 51,
 * TWO differing:
 *
 *      rom   mov r1, r8 / ldr r0, [r5, #0x50]
 *      ours  ldr r0, [r5, #0x50] / mov r1, r8
 *
 * on the FIRST Sprite_SetAnim call only. Everything else matches: the switch,
 * both cases, the four-entry actor-array loop, the null check inside it, and
 * the epilogue.
 *
 * THE SHAPE THAT MAKES THIS INTERESTING. Look at all four call sites in the ROM:
 *
 *      case 1  SetAnim       mov r1, r8  / ldr r0, [r5, #0x50]   <- r1 first
 *      case 1  SetAnimSpeed  ldr r0, [r5, #0x50] / mov r1, r10   <- r0 first
 *      case 2  SetAnim       mov r1, r8  / mov r0, r5            <- r1 first
 *      case 2  SetAnimSpeed  mov r0, r5  / mov r1, r10           <- r0 first
 *
 * The order tracks the CALLEE, not the argument expressions: every SetAnim
 * takes r1 first and every SetAnimSpeed takes r0 first. Our output matches
 * three of the four and differs only where arg 0 is a memory load rather than
 * a register -- gcc hoists the load ahead of the register move there.
 *
 * TRIED AND REJECTED, measured:
 *
 *   * Reading the actor pointer into a local `s` before each call, so both
 *     calls take a register argument like case 2 does. WORSE -- 53 lines, 36
 *     differing: the local earns its own register and renumbers the frame.
 *     gcc must reload across the call anyway (the callee may write through the
 *     pointer), so the local buys nothing and costs allocation.
 *   * Naming the animation index in a local `a` assigned before the switch, to
 *     give it a birth point ahead of the load. NO CHANGE AT ALL, still exactly
 *     2 differing. The parameter already had a register; naming it added
 *     nothing for gcc to reorder.
 *
 * The remaining question is narrow and probably not about this function: what
 * makes gcc emit a register-to-register argument move BEFORE a memory load into
 * a lower-numbered argument register. Three of four sites already do it.
 */
extern void Sprite_SetAnim(void *s, int a);
extern void Sprite_SetAnimSpeed(void *s, int a);

void Actor_SetAnimAndSpeed(unsigned char *e, int anim, int speed)
{
    void **list;
    void *s;
    int i;

    if (e == 0)
        return;
    switch (*(unsigned char *)(e + 0x54) & 0xf) {
    case 1:
        Sprite_SetAnim(*(void **)(e + 0x50), anim);
        Sprite_SetAnimSpeed(*(void **)(e + 0x50), speed);
        break;
    case 2:
        list = *(void ***)(e + 0x50);
        for (i = 3; i >= 0; i--) {
            s = *list++;
            if (s != 0) {
                Sprite_SetAnim(s, anim);
                Sprite_SetAnimSpeed(s, speed);
            }
        }
        break;
    }
}
