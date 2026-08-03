/* OvlFunc_899_2008428  [ovl_794ac0]
 * Source asm: goldensun/asm/overlays/rom_794ac0/ovl_30_a_c_a_a_c_c_c_a_a.s
 *
 * Blocker: INTERLEAVED ARGUMENT SET-UP, the class documented in
 * src/non_matching/overlays/interleaved_arg_setup.c. The ROM splits the
 * shifted constant's mov/lsl around the slot argument:
 *
 *     rom    mov r1, #0x80 / mov r0, #0xf / lsl r1, #8 / mov r2, #0
 *
 * ONE MORE FORMULATION RULED OUT. Their matched OvlFunc_942_20082dc produces
 * exactly this shape from a local assigned EARLY in the function and used
 * late:
 *
 *     int new_var;
 *     new_var = 0x80 << 7;      // before several calls
 *     ... __Func_8092adc(0xc, new_var, 0);
 *
 * Applied here it is strictly worse: the local becomes live across three
 * calls, so it takes a callee-saved register and the function grows from
 * fourteen instructions to sixteen, including a push/pop pair the ROM does
 * not have. Whatever makes their version work is not the early assignment on
 * its own -- theirs sits inside two nested ifs, so the value is live over a
 * much shorter stretch of straight-line code.
 *
 * That is now nine formulations tried against this class across three
 * functions, and none reproduces it.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void OvlFunc_899_20083bc(int slot);
extern void __Func_8092adc(int slot, int angle, int speed);

/* Cutscene: line 0x1253, then turn slot 15 to 0x8000. */
void OvlFunc_899_2008428(void)
{
    __CutsceneStart();
    __MessageID(0x1253);
    OvlFunc_899_20083bc(0xf);
    __Func_8092adc(0xf, 0x80 << 8, 0);
    __CutsceneEnd();
}
