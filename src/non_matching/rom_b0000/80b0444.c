/* Debug_TestEquipAndStatus -- NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_b0000/rom_b0070_a_a_c_a_a_c.s
 * Best screen: 44 instructions against the ROM's 45.
 *
 * BLOCKER CLASS: constant reuse -- gcc is CLEVERER than the original build, and
 * this is the sharpest instance of that in the tree.
 *
 * The function writes 0x1c to gState+0x11c. The offset 0x11c is built as
 * `mov r2, #0x8e / lsl r2, #1`, so r2 holds 0x11c at the point of the store --
 * and 0x11c truncated to a byte IS 0x1c. gcc notices and stores r2:
 *
 *     rom    mov r2, #0x8e / lsl r2, #1 / add r3, r2 / mov r2, #0x1c / strb r2, [r3]
 *     ours   mov r2, #0x8e / lsl r2, #1 / add r3, r2 / strb r2, [r3]
 *
 * One instruction shorter and correct. There is nothing wrong with the C; the
 * two values genuinely coincide and no spelling can un-coincide them, because
 * the offset and the stored value are both fixed by the ROM.
 *
 * FLAGS RULED OUT: -fno-rerun-cse-after-loop, -fno-gcse and
 * -fno-cse-follow-jumps all give byte-identical output. This is the same
 * disposition catalogued in src/non_matching/overlays/constant_reuse.c, where
 * eleven flags were ruled out for three other functions, and it is now the
 * fourth member -- but the FIRST where the reused value is a truncation of an
 * address offset rather than a repeat of the same constant.
 *
 * Everything after that instruction is one position out and therefore reads as
 * 38 differing; the body itself screens clean.
 */
extern unsigned char gState[];
extern int _GiveItemTo(int unit, int item);
extern void _EquipItem(int unit, int slot);
extern unsigned char *_GetUnit(int id);
extern void Func_80b0278(int a, int b);

int Debug_TestEquipAndStatus(void)
{
    unsigned char *g;
    int off;
    int one;

    g = gState;
    *(int *)(g + 0x10) = 0x30d40;
    g += 0x8e << 1;
    *g = 0x1c;
    _EquipItem(1, _GiveItemTo(1, 0x48d));
    _EquipItem(0, _GiveItemTo(0, 0x40b));
    _GiveItemTo(2, 0xe7);
    off = 0x131;
    one = 1;
    _GetUnit(3)[off] = one;
    _GetUnit(5)[off] = one;
    _GetUnit(2)[0xa0 << 1] = one;
    Func_80b0278(1, 0x1e);
    return 0;
}
