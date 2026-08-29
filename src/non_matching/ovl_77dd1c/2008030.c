/* OvlFunc_882_2008030  [ovl_77dd1c]  --  0x02008030
 *
 * Source asm: goldensun/asm/overlays/rom_77dd1c/ovl_30_a_a.s
 *
 * Five of twenty-three, and two of the five are a link question rather than a
 * codegen one.
 *
 * 1. `bl _umodsi3_RAM` against our `bl __umodsi3`. gcc emits __umodsi3 for `%`
 *    and has no flag to rename it; overlay code calls the RAM-resident copy.
 *    This is the same situation batch 29 solved for division, and the fix is
 *    the same one line in the overlay's linker script:
 *
 *        __umodsi3 = _umodsi3_RAM;
 *
 *    src/overlays/rom_77dd1c/imports.s already exports umodsi3_RAM, so the stub
 *    is there. The alias is NOT added, because with the rest of the function
 *    still unmatched it would be an unused linker change with nothing to
 *    justify it; add it in the same commit that solves point 2.
 *
 * 2. THE ROM COPIES THE LOADED VALUE INTO A SECOND REGISTER:
 *
 *        rom    mov r2,#0 / ldrsh r3,[r5,r2] / mov r0,r3 / cmp r3,#0
 *        ours   mov r3,#0 / ldrsh r0,[r5,r3] /            cmp r0,#0
 *
 *    The ROM compares r3 and carries r0 to the join at the bottom; gcc loads
 *    straight into the register it will carry and drops the copy. Two source
 *    variables holding the same value -- one compared, one flowed -- does not
 *    reproduce it: gcc coalesces them, and the output is identical to the
 *    one-variable form.
 *
 * That copy is the whole remaining problem. It is one instruction the ROM has
 * and we do not, and everything after it follows from which register the value
 * ends up in.
 */
extern unsigned int __Random(void);

int OvlFunc_882_2008030(void *a)
{
    short *p;
    int v;

    p = (short *)((unsigned char *)a + 0x64);
    v = *p;
    if (v == 0) {
        *(unsigned short *)((unsigned char *)a + 6) = __Random();
        v = __Random() % 0x14 + 0x14;
        *p = v;
    }
    *p = v - 1;
    return 1;
}
