/* Func_800c570  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_9000/rom_c004_c_a_c_a.s
 * Best screen: 1 instruction in disagreeing regions, of 21 (rom 21, ours 20).
 *
 * BLOCKER CLASS: gcc propagates a value the guard proved.
 *
 * The single missing instruction is the mask:
 *
 *      rom   ldrb r3, [r3] / cmp r3, #0x1 / bne .Lexit
 *            ...
 *            mov r3, #0x1 / and r1, r3          <- 1 materialised again
 *      ours  ldrb r3, [r3] / cmp r3, #0x1 / bne .Lexit
 *            ...
 *                           and r4, r3          <- reuses the byte's register
 *
 * Past the guard the loaded byte IS 1, so gcc reuses the register that already
 * holds it instead of building the constant a second time. The inference is
 * legitimate and there is no spelling that unteaches it: the guard tests
 * against 1 and the mask is 1, and any rewriting of either keeps them equal.
 *
 * Same family as src/non_matching/rom_b5000/80c23c0.c and the mask-narrowing
 * parks -- our output is SHORTER because the optimiser proved something.
 *
 * THREE SPELLINGS WERE NEEDED to get from 15 of 21 down to 1, and they are the
 * reason this park is worth reading:
 *
 *   1. `f &= 1;` ON THE PARAMETER, not through a copy. `m = f; m &= 1;` emits
 *      a real `mov r2, r1` that the ROM does not have -- the ROM ANDs the
 *      incoming argument register in place.
 *   2. `a = *(unsigned char **)(a + 0x50);` -- ASSIGNING BACK INTO THE
 *      PARAMETER. The ROM reuses r0 for the loaded pointer (`ldr r0, [r0, #0x50]`);
 *      a fresh local gets a fresh register and, here, a callee-saved one.
 *   3. The mask built as `n = 3; n = -n;` rather than `n = -3`, matching
 *      `mov r3, #0x3 / neg r3, r3`.
 *
 * (2) is the general one: when the ROM's load is destructive on an argument
 * register, assign back into the parameter rather than introducing a local.
 */
void Func_800c570(unsigned char *a, int f)
{
    unsigned char *p;
    int v;
    int n;

    if (a == 0)
        return;
    p = a;
    p += 0x54;
    if (*p != 1)
        return;
    a = *(unsigned char **)(a + 0x50);
    f &= 1;
    v = a[0x1d];
    n = 3;
    n = -n;
    f <<= 1;
    n &= v;
    n |= f;
    a[0x1d] = n;
}
