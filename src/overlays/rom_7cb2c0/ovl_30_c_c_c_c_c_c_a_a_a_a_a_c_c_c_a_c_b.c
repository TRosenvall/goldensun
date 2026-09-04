// fakematch
/* OvlFunc_945_200bdec  --  0x0200bdec
 *
 * Cut out of goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_c_a_c.s.
 *
 * A cutscene block: start, move an actor, speak a line, close.
 *
 * PARKED AT 2 OF 26. The residue was one instruction, and it is the
 * shape the pin was built for:
 *
 *     rom   mov r1, #0xa0 / mov r2, #0x28 / mov r0, #0x8 / lsl r1, #0x7
 *     ours  mov r1, #0xa0 / mov r2, #0x28 / lsl r1, #0x7  / mov r0, #0x8
 *
 * The ROM splits the build of the shifted argument AROUND the single-instruction
 * one; gcc finishes the shift first and sets r0 after. Pinning the argument
 * registers and assigning them in the ROM's order puts `mov r0` in the gap:
 *
 *     p1 = 0xa0;  p2 = 0x28;  p0 = 8;  p1 <<= 7;
 *
 * WHY THIS ONE FELL AND ITS NEIGHBOUR DID NOT. Both parks were written in the
 * same round and filed under one blocker name, "arg-interleave", but they are
 * two different problems and only one of them is a pin's business:
 *
 *   - HERE the misplaced `mov r0` has to land INSIDE another register's split
 *     build. That is moving the pinned register's OWN mov relative to other
 *     instructions, which is exactly what the pin's second knob does.
 *   - In src/non_matching/ovl_793768/2008e0c.c the two instructions are
 *     INDEPENDENT movs at a control-flow join, with no shift to sit inside.
 *     Six pin forms there are byte-identical to seven declaration spellings,
 *     because deciding which of two independent movs is emitted first is the
 *     post-allocation scheduler's business and no pin reaches it.
 *
 * Read the residue before reaching for the lever: `mov rX` against a `lsl` of
 * another register is reachable, `mov rX` against a bare `mov` is not.
 */

extern void __CutsceneStart(void);
extern void __MessageID(int id);
extern void __Func_809259c(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void OvlFunc_945_200c8e8(int a, int b, int c);

void OvlFunc_945_200bdec(void)
{
    __CutsceneStart();
    OvlFunc_945_200c8e8(0xf, 1, 1);
    {
        register int p0 __asm__("r0");
        register int p1 __asm__("r1");
        register int p2 __asm__("r2");
        p1 = 0xa0;
        p2 = 0x28;
        p0 = 8;
        p1 <<= 7;
        __Func_8092adc(p0, p1, p2);
    }
    __Func_809259c(8, 2);
    __MessageID(0x1e3d);
    __Func_8093040(8, 0, 0x14);
    OvlFunc_945_200c8e8(9, 0xb, 0);
}
