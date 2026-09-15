/* SetTextColor (SetTextInk) -- 0x0801e71c, asm/rom_15000/rom_1de5c_c_a_b.s.
 *
 * r0 = colour. Masks to 4 bits and stores at [iwram_3001e8c] + 0xEAE, the ink
 * field Func_173ac defaults to 0x0F.
 *
 * WHOLE-FILE CONVERSION -- one function in the .s, no split, stage1.ld:447
 * verbatim with its asm/ prefix, no Makefile rule, no pins.
 *
 * THE POOLED 0xf IS NOT A SYMBOL, AND THAT IS THE RESULT WORTH KEEPING. The ROM
 * emits `ldr r2, .L1e72c` with `.word 0xf` -- a pooled value an eight-bit `mov`
 * could build, which is normally the tell for a named symbol and would send
 * someone to const.sym. It is the documented EXCEPTION instead: the constant is
 * an operand of a HALFWORD expression (the `and` feeds a `strh`), gcc narrows
 * the AND to HImode, and a HImode constant goes to the pool. gcc's text says
 * `ldrh r2, .L3`, which GAS folds to the same `ldr` encoding the ROM has.
 *
 * So const.sym gets NO _CONST_f entry. Its own bar requires that no literal
 * spelling reproduce the pool, and a plain `c & 0xf` does reproduce it -- this
 * is exactly the check its header asks for before adding an entry.
 *
 * THE SHAPE IS THE WHOLE PROBLEM, and it is two orderings:
 *
 *   * THE MASK MUST STAY A HALFWORD EXPRESSION. `c &= 0xf;` as its own
 *     statement makes the AND SImode, gcc builds 0xf with `mov r2, #0xf`, the
 *     pool word disappears and the function comes out 24 bytes against 28.
 *     Assigning through a `u16` local keeps it HImode.
 *   * THE BASE MUST BE NAMED BEFORE THE VALUE. With the base still spelled
 *     inline, gcc emits the address arithmetic first and the mask second; the
 *     ROM does the mask first. Naming the base, then the value, then storing,
 *     gives the ROM's order exactly.
 *
 * MEASURED: inline base with inline mask 4 differing; inline base with a u16
 * local 5; named base with inline mask 4; `unsigned short` parameter 11 and one
 * instruction long; `c &= 0xf` in any position 24 bytes.
 *
 * NOTE tools/tryc.py reports this function with its pool-placement warning,
 * because the reference keeps its literal pool inside the function body. That
 * warning is correct and unavoidable here; objcmp verifies the pool word and
 * `make compare` settles the placement.
 *
 * EXACT: 28 bytes, 11 encodings, 1 relocation, measured three times.
 */
#include "gba/types.h"

extern unsigned char *iwram_3001e8c;

void SetTextColor(int c)
{
    u8 *p;
    u16 v;

    p = iwram_3001e8c;
    v = c & 0xf;
    *(u16 *)(p + 0xeae) = v;
}
