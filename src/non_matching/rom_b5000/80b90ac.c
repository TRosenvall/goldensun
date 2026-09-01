/* Func_80b90ac (0x080b90ac) -- NON-MATCHING.
 * Blocker class: the ROM copies a buffer address into a second callee-saved
 * register inside a guard, and gcc coalesces the copy away.
 *
 * THIRD instance of this exact wall. The full measurement table lives in
 * src/non_matching/rom_b5000/80c1014.c -- that function has the same
 * `Func_80b6c08(3, buf)` opener, the same guarded countdown loop, and the same
 * residue -- and src/non_matching/ovl_77dd1c/2009498.c states it from the other
 * side. Not re-derived here.
 *
 * 33 lines against the ROM's 34:
 *
 *     rom    mov r5, sp ... (guard) ... mov r7, r5   then ldrh r0, [r6, r7]
 *     ours   mov r7, sp ... no copy ...              then ldrh r0, [r6, r7]
 *
 * Measured: a second local `p = base;` assigned inside the guard -- the shape
 * the ROM's `mov r7, r5` reads as -- is byte-identical to not writing it, 33
 * lines and 25 differing either way. That is the same result 80c1014 got from
 * three spellings including one where the call argument and the loop base share
 * no variable at all.
 *
 * WHAT IS NEW HERE, and worth having: the copy goes to r7, a LOW callee-saved
 * register, where 80c1014's went to r8. So this is not about the high-register
 * bank -- gcc will not emit a redundant address copy into any callee-saved
 * register, whatever the pressure.
 *
 * A SECOND, SEPARATE DIFFERENCE that the base copy hides: the ROM re-reads the
 * halfword BEFORE the byte store (`ldrh / strb`), ours after (`strb / ldrh`).
 * The read-twice spelling is right -- both streams have two `ldrh` -- only the
 * order differs, and it is downstream of the allocation.
 *
 * NEXT: nothing source-level. Three specimens now agree.
 */
extern int Func_80b6c08(int a, short *buf);
extern unsigned char *_GetUnit(int id);
extern void _CalcStats(int id);

void Func_80b90ac(void)
{
    short buf[14];
    short *base;
    int count;
    int n;
    int off;
    int z;

    base = buf;
    count = Func_80b6c08(3, base);
    if (count > 0) {
        off = 0;
        z = 0;
        n = count;
        do {
            _GetUnit(*(unsigned short *)(off + (int)base))[0x12b] = z;
            _CalcStats(*(unsigned short *)(off + (int)base));
            n--;
            off += 2;
        } while (n != 0);
    }
}
