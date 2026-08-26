/* OvlFunc_959_2008e80  --  0x02008e80, asm/overlays/rom_7e7574/ovl_9dc_a_c_c_a_a_c_c_a.s
 *
 * Source asm: goldensun/asm/overlays/rom_7e7574/ovl_9dc_a_c_c_a_a_c_c_a.s
 *
 * BLOCKER CLASS: constant CSE, in the direction the tree has not seen before --
 * gcc SHARES two identical arguments where the ROM builds both.
 * Status: 37 lines against 38, 17 differing.
 *
 *     rom    mov r0, #0xc0 / mov r1, #0xc0 / lsl r0, #10 / lsl r1, #10
 *     ours   mov r1, #0xc0 / lsl r1, #0xa / mov r0, r1
 *
 * `__Func_8012330(0xc0 << 10, 0xc0 << 10, 0x80 << 9)` passes the same value in
 * r0 and r1. gcc computes it once and copies; the ROM computes it twice. Every
 * documented lever for this points the other way -- the constant-CSE blocker in
 * pick_candidates.py is about gcc hoisting a POOL LOAD into a callee-saved
 * register, and -fno-gcse, -fno-cse-follow-jumps and -fno-rerun-cse-after-loop
 * are the flags for it. Measured here: -fno-gcse 17, -fno-cse-follow-jumps 17,
 * -fno-rerun-cse-after-loop 22 (worse), -O1 16. None of them stops a copy that
 * costs one instruction less than rebuilding.
 *
 * The rest of the function reads clean and is left in place: the -1 held in r6
 * across the compare and both later arguments falls out of writing `-1` in all
 * three places, and the `short` at base+0x16c is read once and used for both
 * `v - 0x28` and `v + 0x330`.
 *
 * Batch 83 solved the mirror of this for STACK arguments -- naming them keeps
 * two pseudos alive. It does not apply: these are register arguments, and
 * naming them does not stop gcc noticing they are equal.
 */
extern unsigned char iwram_3001ebc[];
extern int __CheckPartyItem(int item);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void OvlFunc_959_2008e30(int n);
extern void __Func_8012330(int a, int b, int c);

void OvlFunc_959_2008e80(void)
{
    char *base;
    short v;

    base = *(char **)iwram_3001ebc;
    if (__CheckPartyItem(0xea) != -1) {
        v = *(short *)(base + (0xb6 << 1));
        OvlFunc_959_2008e30(v - 0x28);
        __PlaySound(0x9d);
        __Func_8012330(0xc0 << 10, 0xc0 << 10, 0x80 << 9);
        __Func_8012330(-1, -1, 0xe666);
        __SetFlag(v + (0xcc << 2));
    }
}
