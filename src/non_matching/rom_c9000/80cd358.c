/* Task_BlitAnim_BG1Wide (0x080cd358) -- NON-MATCHING.
 * Blocker class: duplicate-constant CSE, in ONE arm of a switch.
 *
 * 85 lines against the ROM's 84. The switch's comparison tree, case 0, case 2
 * and case 3 are all byte-exact. Case 1 makes two calls with the same two
 * arguments:
 *
 *     copy((void *)0x6008000, gBuffer, 0xf0 << 7);
 *     fill(gBuffer, 0xf0 << 7, *(int *)(p + 0x7784));
 *
 * The ROM rebuilds `gBuffer` and `0xf0 << 7` for the second call. gcc shares
 * them, which needs two callee-saved registers it otherwise would not -- hence
 * `push {r5, r6, r7, lr}` against the ROM's `push {r5, lr}`, the base pointer
 * displaced from r5 to r7, and `bl _call_via_r4` where the ROM has
 * `_call_via_r3`.
 *
 * -fno-gcse and -fno-rerun-cse-after-loop are both inert, as batch 175
 * established for this class: it is cse.c's LOCAL constant sharing, and no flag
 * or spelling separates two uses of one literal.
 *
 * WHAT IS RIGHT, and is the reusable part:
 *
 *   THE SWITCH LOWERS EXACTLY. A plain `switch (x)` over cases 0..3 produces
 *   the ROM's `cmp #1 / beq / cmp #1 / bgt / cmp #0 / beq / b default` binary
 *   tree with no help at all. Do not try to hand-write the comparison chain.
 *
 *   THE INDIRECT CALLS ARE THE rom_e0524.c IDIOM. `ldr r3, =Func_8001af8 /
 *   bl _call_via_r3` comes from assigning the callee into a local of
 *   function-pointer type and calling through it; gcc-2.96 does not
 *   constant-propagate it back to a direct call. Two DIFFERENT pointer types
 *   are needed here (the two callees take different arguments), and the two
 *   locals stay separate.
 *
 *   `0xf0 << 7` IS 0x7800 AND THE ROM DERIVES IT FROM A NEARBY CONSTANT.
 *   Case 2 has `ldr r2, =0x7784 / add r2, #0x7c` where the source just says
 *   `0xf0 << 7`; gcc finds the derivation on its own. No lever needed.
 *
 * NEXT: nothing. One repeated constant in one arm.
 */
extern unsigned char *iwram_3001eec;
extern unsigned char gBuffer[];
extern void Func_8001af8(void *dst, void *src, int n);
extern void Func_80008d8(void *dst, int n, int v);
extern void BlitFade_Div2(void *src, void *dst, int n);
extern void BlitFade_Div4(void *src, void *dst, int n);
extern void BlitFade_Sub(void *src, int v, void *dst, int n);

typedef void (*CopyFn)(void *dst, void *src, int n);
typedef void (*FillFn)(void *dst, int n, int v);

void Task_BlitAnim_BG1Wide(void)
{
    unsigned char *p;
    CopyFn copy;
    FillFn fill;

    p = iwram_3001eec;
    if (*(int *)(p + 0x7824) != 1)
        return;
    switch (*(int *)(p + 0x7780)) {
    case 0:
        copy = Func_8001af8;
        copy((void *)0x6008000, gBuffer, 0xf0 << 7);
        break;
    case 1:
        copy = Func_8001af8;
        copy((void *)0x6008000, gBuffer, 0xf0 << 7);
        fill = Func_80008d8;
        fill(gBuffer, 0xf0 << 7, *(int *)(p + 0x7784));
        break;
    case 2:
        if (*(int *)(p + 0x7784) == 0x32)
            BlitFade_Div2(gBuffer, (void *)0x6008000, 0xf0 << 7);
        else
            BlitFade_Div4(gBuffer, (void *)0x6008000, 0xf0 << 7);
        break;
    case 3:
        BlitFade_Sub(gBuffer, *(int *)(p + 0x7784), (void *)0x6008000, 0xf0 << 7);
        break;
    }
    *(int *)(p + 0x7824) = 0;
}
