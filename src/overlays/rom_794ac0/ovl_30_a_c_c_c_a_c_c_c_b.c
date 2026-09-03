/* OvlFunc_899_200a564  --  0x0200a564
 *
 * The tail of goldensun/asm/overlays/rom_794ac0/ovl_30_a_c_c_c_a_c_c_c.s; the
 * two functions ahead of it stay in _a.s. No data outside the target -- the
 * eight-entry jump table sits inside the function and gcc regenerates it.
 *
 * A per-sub-area script dispatch: two or three registrations per arm.
 *
 * THE FINDING IS ABOUT ldrsh ADDRESSING, and it runs OPPOSITE to the recorded
 * "the offset belongs in the load" lever. Thumb-1 has no immediate-offset
 * ldrsh, so the sign-extending load pattern takes a generic memory operand plus
 * a scratch and decides at output time: if the address is a PLUS of two
 * REGISTERS it emits one `ldrsh`, and otherwise it must materialise a zero into
 * the scratch and emit `mov rZ, #0` alongside it. Two extra instructions for a
 * bare-register address, none for a register pair.
 *
 * Measured in the .13.combine dumps of the two candidates: the loser carried
 * the offset in its own pseudo, giving a `(mem (plus reg reg))` and one
 * `ldrsh`; the winner folded the offset into an add, leaving `(mem (reg))` and
 * the `mov` + `ldrsh` pair. So:
 *
 *      ROM shows `add / mov rZ, #0 / ldrsh`  -> write the offset as a LITERAL
 *                                               in the address expression
 *      ROM shows a bare `ldrsh rX, [rY, rZ]` -> write the offset as a NAMED
 *                                               VARIABLE, keeping it a live
 *                                               pseudo so recog sees REG+REG
 *
 * The familiar `off = 0xb6; off <<= 1;` dance is the right lever for word
 * access and is exactly what BREAKS a signed halfword load. And note the
 * `mov / lsl` pair appears in the ROM either way, because Thumb `add` immediate
 * stops at 255 and gcc must build the offset in a register regardless -- so
 * that pair is NOT evidence of a named offset variable here.
 *
 * THE [cse] PREDICTION WAS INERT AGAIN -- fifth failure in six. The flag is
 * byte-identical to the default on both the winner and the loser. The repeats
 * it fired on are pooled symbol addresses in mutually exclusive SWITCH ARMS,
 * neither dominating the other, each deduplicated to one pool word anyway.
 * filtered.py should probably suppress [cse] when the repeats sit in different
 * arms of one jump-table dispatch.
 *
 * The three cross-jumped tails fall out of plain `break;` with no help.
 */
/* Cluster OvlFunc_899_200a564..OvlFunc_899_200a564 extracted from goldensun/asm/overlays/rom_794ac0/ovl_30_a_c_c_c_a_c_c_c.s. */
extern unsigned char *iwram_3001ebc;

extern void OvlFunc_899_200a6e4(unsigned int arg0, int arg1, unsigned int arg2,
                                unsigned int arg3);

extern unsigned char L5538[] __asm__(".L5538");
extern unsigned char L55b0[] __asm__(".L55b0");
extern unsigned char L55d8[] __asm__(".L55d8");
extern unsigned char L5600[] __asm__(".L5600");
extern unsigned char L56c8[] __asm__(".L56c8");
extern unsigned char L56f0[] __asm__(".L56f0");
extern unsigned char L5718[] __asm__(".L5718");
extern unsigned char L57a4[] __asm__(".L57a4");
extern unsigned char L57cc[] __asm__(".L57cc");
extern unsigned char L5894[] __asm__(".L5894");

extern unsigned char gScript_899__0200d560[];
extern unsigned char gScript_899__0200d650[];
extern unsigned char gScript_899__0200d678[];
extern unsigned char gScript_899__0200d768[];
extern unsigned char gScript_899__0200d830[];
extern unsigned char gScript_899__0200d858[];
extern unsigned char gScript_899__0200d8bc[];
extern unsigned char gScript_956__0200d808[];

void OvlFunc_899_200a564(void)
{
    short v;

    v = *(short *)(iwram_3001ebc + 0x16c);
    switch (v) {
    case 0xb:
        OvlFunc_899_200a6e4(0x18, 1, 2, (unsigned int)L55b0);
        OvlFunc_899_200a6e4(0x19, 3, 4, (unsigned int)gScript_899__0200d8bc);
        break;
    case 0xc:
        OvlFunc_899_200a6e4(0x18, 1, 4, (unsigned int)gScript_899__0200d678);
        OvlFunc_899_200a6e4(0x18, 2, 3, (unsigned int)L55d8);
        OvlFunc_899_200a6e4(0x19, 1, 3, (unsigned int)gScript_899__0200d830);
        break;
    case 0xd:
        OvlFunc_899_200a6e4(0x18, 2, 1, (unsigned int)L5538);
        OvlFunc_899_200a6e4(0x18, 3, 6, (unsigned int)L5718);
        OvlFunc_899_200a6e4(0x19, 2, 4, (unsigned int)L5894);
        break;
    case 0xe:
        OvlFunc_899_200a6e4(0x18, 3, 2, (unsigned int)L55b0);
        OvlFunc_899_200a6e4(0x19, 4, 3, (unsigned int)gScript_899__0200d858);
        break;
    case 0xf:
        OvlFunc_899_200a6e4(0x18, 4, 5, (unsigned int)L56c8);
        OvlFunc_899_200a6e4(0x19, 1, 2, (unsigned int)L57cc);
        break;
    case 0x10:
        OvlFunc_899_200a6e4(0x18, 4, 1, (unsigned int)gScript_899__0200d560);
        OvlFunc_899_200a6e4(0x18, 5, 6, (unsigned int)L56f0);
        OvlFunc_899_200a6e4(0x19, 3, 1, (unsigned int)L57a4);
        break;
    case 0x11:
        OvlFunc_899_200a6e4(0x18, 5, 4, (unsigned int)gScript_899__0200d650);
        OvlFunc_899_200a6e4(0x18, 6, 3, (unsigned int)L5600);
        OvlFunc_899_200a6e4(0x19, 4, 2, (unsigned int)gScript_956__0200d808);
        break;
    case 0x12:
        OvlFunc_899_200a6e4(0x18, 6, 5, (unsigned int)L56c8);
        OvlFunc_899_200a6e4(0x19, 2, 1, (unsigned int)gScript_899__0200d768);
        break;
    }
}
