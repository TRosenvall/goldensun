/* OvlFunc_924_200a648  --  0x0200a648, asm/overlays/rom_7ac2d8/ovl_22c4_c_c_c_a.s
 *
 * BLOCKER CLASS: loop reversal -- gcc counts DOWN where the ROM counts UP.
 * Status: 24 lines against 24, 9 differing, all of them consequences of the
 * one decision.
 *
 * WHAT IT DOES
 * Every eighth frame, rotates seven palette entries: BG palette slot 0x50 is
 * saved into 0x5e, then slots 0x52..0x5e shift down into 0x50..0x5c. A one-step
 * colour cycle.
 *
 * THE DIFFERENCE
 *      rom    mov r0, #0x0  ...  add r0, #0x1 / cmp r0, #0x6 / bls
 *      ours   mov r2, #0x6  ...  sub r2, #0x1 /              bne-ish
 *
 * The counter is dead outside the loop, so gcc's induction-variable pass
 * rewrites it as a countdown to zero, which needs no `cmp`. That is strictly
 * better code and it renames every register in the loop as a side effect --
 * which is why nine lines differ for one decision.
 *
 * WHAT WAS TRIED
 *   - `do { } while (i <= 6)` with `int i` and with `unsigned int i`:
 *     unsigned is one line better (9 rather than 10) and still reversed
 *   - `for (i = 0; i <= 6; i++)`: identical to the signed do/while
 *   - `-fno-strength-reduce`: BYTE-IDENTICAL. That is the informative one --
 *     the reversal is not strength reduction, so the obvious flag is the wrong
 *     flag and should not be tried again.
 *
 * WHAT WOULD REACH IT, if anything, is giving the counter a use that survives
 * the loop, because gcc only reverses a counter it can prove dead. Nothing in
 * the ROM's twenty-four instructions uses it after the loop, so any such use
 * would have to be invented -- which changes what the function does.
 *
 * The addresses are raw palette pointers, not symbols: 0x5000050 and 0x5000052
 * are BG palette entries and 0x500005e is the save slot. All three exceed the
 * immediate range and are genuinely pooled, so none of them is a pool tell.
 */

extern unsigned int iwram_3001e40;

void OvlFunc_924_200a648(void)
{
    unsigned short *dst;
    unsigned short *src;
    unsigned int i;

    if ((iwram_3001e40 & 7) != 0)
        return;
    dst = (unsigned short *)0x5000050;
    *(unsigned short *)0x500005e = *dst;
    src = (unsigned short *)0x5000052;
    i = 0;
    do {
        *dst = *src;
        i++;
        src++;
        dst++;
    } while (i <= 6);
}
