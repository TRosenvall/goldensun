/* Func_80b10cc  --  0x080b10cc, cut from
 * goldensun/asm/rom_b0000/rom_b0070_a_a_c_c_a_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_b0000/rom_b0070_a_a_c_c_a_c_a.o and
 * asm/rom_b0000/rom_b0070_a_a_c_c_a_c_c.o in goldensun/stage1.ld.
 *
 * DrawRowText: emit a row's label and its number, skipping both if the row
 * count is zero.
 *
 * DECLARE gState AS A STRUCT, NOT AS BYTES. With
 *
 *     extern unsigned char gState[];
 *     ... *(int *)(gState + 0x10) ...
 *
 * gcc folds the offset into the pool entry -- `ldr r3, =gState+16 / ldr r0,
 * [r3]` -- where the ROM loads the base and uses the instruction's own offset:
 * `ldr r3, =gState / ldr r0, [r3, #0x10]`. Two differences of 25, and both go
 * away with a real member access. The byte-array spelling is convenient for
 * reaching an unknown offset and it is the wrong tool whenever the ROM keeps
 * base and offset apart; docs/elevation.md has the general rule under "Name the
 * pointer to move a load's base and offset".
 */
typedef struct { unsigned char pad00[0x10]; int f10; unsigned char pad14[0x2ac]; } GlobalState;
struct X { unsigned char pad00[0xc]; int f0c; };

extern unsigned char iwram_3001f2c[];
extern GlobalState gState;
extern void _Func_801e7c0(int a, int b, int c, int d);
extern void _Func_801ea08(int a, int b, int c, int d, int e);

void Func_80b10cc(void)
{
    int v;

    v = (*(struct X **)iwram_3001f2c)->f0c;
    if (v != 0) {
        _Func_801e7c0(0xc8a, v, 0, 0);
        _Func_801ea08(gState.f10, 6, v, 0x20, 8);
    }
}
