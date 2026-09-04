// fakematch
/* OvlFunc_945_200dca4  --  0x0200dca4
 *
 * Cut out of goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_c_a_c.s.
 *
 * A cutscene block: two actors placed and animated across several waits.
 *
 * PARKED AT 2 OF 43. The residue was one instruction, and it is the
 * shape the pin was built for:
 *
 *     rom   mov r1, #0xd0 / mov r0, #0x8  / lsl r1, #0x8
 *     ours  mov r1, #0xd0 / lsl r1, #0x8 / mov r0, #0x8
 *
 * The ROM splits the build of the shifted argument AROUND the single-instruction
 * one; gcc finishes the shift first and sets r0 after. Pinning the argument
 * registers and assigning them in the ROM's order puts `mov r0` in the gap:
 *
 *     p1 = 0xd0;  p0 = 8;  p1 <<= 8;
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
extern void __CutsceneWait(int n);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void OvlFunc_945_200c890(int a, int b, int c, int d);
extern void OvlFunc_945_200c8e8(int a, int b, int c);

void OvlFunc_945_200dca4(void)
{
    __CutsceneStart();
    OvlFunc_945_200c8e8(0xf, 1, 0);
    OvlFunc_945_200c890(9, 0xea << 1, 0x9a << 2, 0x80 << 8);
    OvlFunc_945_200c8e8(8, 1, 0x14);
    __Func_80925cc(9, 2);
    __CutsceneWait(0x14);
    {
        register int p0 __asm__("r0");
        register int p1 __asm__("r1");
        p1 = 0xd0;
        p0 = 8;
        p1 <<= 8;
        __Func_8092adc(p0, p1, 0x50);
    }
    __Func_8092adc(8, 0, 0x14);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x14);
    OvlFunc_945_200c8e8(9, 0x15, 0);
}
